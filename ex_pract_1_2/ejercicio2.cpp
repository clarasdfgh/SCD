#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "HoareMonitor.h"
#include "Semaphore.h"

using namespace std ;
using namespace HM;


//**********************************************************************
// variables compartidas

const int num_lectores   = 5,
          num_escritores = 3;

mutex	mtx;  //Mutex para la salida por pantalla


//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}


//**********************************************************************
// Monitor hoare lect_escr
//----------------------------------------------------------------------

class Lect_escr : public HoareMonitor
{
private:
int     n_lec;
bool    escrib;
CondVar lectura,
        escritura;
int     contador;

public:
  Lect_escr( );
  void ini_lectura( );
  void fin_lectura( );
  void ini_escritura( );
  void fin_escritura( );
};

// -----------------------------------------------------------------------------
Lect_escr::Lect_escr( ){
  n_lec = 0;
  escrib = false;

  lectura = newCondVar();
  escritura = newCondVar();
}

// -----------------------------------------------------------------------------
void Lect_escr::ini_lectura( ){
  if (escrib){
    lectura.wait();
  }

  n_lec++;
  lectura.signal();

}

// -----------------------------------------------------------------------------
void Lect_escr::fin_lectura( ){
  n_lec--;

  if (n_lec == 0){
    escritura.signal();
  }
}

// -----------------------------------------------------------------------------
void Lect_escr::ini_escritura( ){
  if (escrib || n_lec > 0){
    escritura.wait();
  }

  escrib = true;
  if(contador == 0){
    contador++;
  } else{
    contador = 0;
  }
}

// -----------------------------------------------------------------------------
void Lect_escr::fin_escritura( ){
  escrib = false;

  if (contador > 0){
    escritura.signal();
  } else{
    lectura.signal();
  }

  /*if(escritura.get_nwt() > 0 && n_lec >= 3){
    if(n_lec > 3){
      escritura.signal();
    } else{
      lectura.signal();
    }
  } else{
    lectura.signal();
  }*/

  /*if (lectura.get_nwt() != 0) //Función de CondVar que comprueba cuántas hebras hay esperando en ella
  {
    lectura.signal();
  } else{
    escritura.signal();
  }*/
}


//------------------------------------------------------------------------------
// Función que simula la acción de escribir, como un retardo
// aleatorio de la hebra

void escribir( int n_escritor )
{
   // calcular milisegundos aleatorios de duración de la acción de escribir
   chrono::milliseconds duracion_escr( aleatorio<10,100>() );

   // informa de que comienza a escribir
   mtx.lock();
   cout << "Escritor " << n_escritor << " : empieza a escribir (" << duracion_escr.count() << " milisegundos)" << endl;
   mtx.unlock();

   // espera bloqueada un tiempo igual a ''duracion_escr' milisegundos
   this_thread::sleep_for( duracion_escr );

   // informa de que ha terminado de escribir
   mtx.lock();
   cout << "Escritor " << n_escritor << " : termina de escribir " << endl;
   mtx.unlock();
}

//------------------------------------------------------------------------------
// Función que ejecuta la hebra del escritor

void funcion_hebra_escritor( MRef<Lect_escr> monitor, int n_escritor )
{
    while( true ){
      chrono::milliseconds espera( aleatorio<10,100>() );
      this_thread::sleep_for( espera );

      monitor->ini_escritura();
      escribir(n_escritor);
      monitor->fin_escritura();
    }
}

//------------------------------------------------------------------------------
// Función que simula la acción de leer, como un retardo
// aleatorio de la hebra

void leer( int n_lector )
{
   // calcular milisegundos aleatorios de duración de la acción de leer
   chrono::milliseconds duracion_lect( aleatorio<10,100>() );

   // informa de que comienza a leer
   mtx.lock();
   cout << "Lector   " << n_lector << " : empieza a leer (" << duracion_lect.count() << " milisegundos)" << endl;
   mtx.unlock();

   // espera bloqueada un tiempo igual a ''duracion_lect' milisegundos
   this_thread::sleep_for( duracion_lect );

   // informa de que ha terminado de leer
   mtx.lock();
   cout << "Lector   " << n_lector << " : termina de leer " << endl;
   mtx.unlock();
}


//------------------------------------------------------------------------------
// Función que ejecuta la hebra del lector

void  funcion_hebra_lector( MRef<Lect_escr> monitor, int n_lector )
{
   while( true )
   {
     chrono::milliseconds espera( aleatorio<100,120>() );
     this_thread::sleep_for( espera );

     monitor->ini_lectura();
     leer(n_lector);
     monitor->fin_lectura();
   }
}

//------------------------------------------------------------------------------

int main()
{
  cout << "---------------------------------------------------------------------------" << endl
       << " Problema de los lectores-escritores con monitor SU -- examen, ejercicio 2 " << endl
       << "---------------------------------------------------------------------------" << endl
       << flush ;

  MRef<Lect_escr> monitor = Create<Lect_escr>( );
  thread lectores[num_lectores];
  thread escritores[num_escritores];


  for (int i = 0; i < num_lectores; i++){
    lectores[i] = thread (funcion_hebra_lector, monitor, i);
  }

  for (int i = 0; i < num_escritores; i++){
    escritores[i] = thread (funcion_hebra_escritor, monitor, i);
  }


  for (int i = 0; i < num_lectores; i++){
    lectores[i].join();
  }

  for (int i = 0; i < num_escritores; i++){
    escritores[i].join();
  }

  cout << endl << endl;
}
