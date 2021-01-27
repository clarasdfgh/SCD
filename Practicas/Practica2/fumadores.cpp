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

const int num_items = 3;

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
// Monitor hoare Estanco
//----------------------------------------------------------------------

class Estanco : public HoareMonitor
{
private:
int     mostrador;
CondVar estanquero,
        fumadores[num_items];

public:
  Estanco( );
  void obtenerIngrediente( int ing );
  void ponerIngrediente( int ing );
  void esperarRecogidaIngrediente( );
};

// -----------------------------------------------------------------------------
Estanco::Estanco( ){
  mostrador = -1;     //mostrador vacío

  for (int i = 0; i < num_items; i++){
    fumadores[i] = newCondVar();
  }
    estanquero = newCondVar();
}

// -----------------------------------------------------------------------------
void Estanco::obtenerIngrediente( int ing ){
  while(mostrador != ing){
    fumadores[ing].wait();
  }

  mtx.lock();
  cout << "Fumador " << ing << "  : toma ingrediente del mostrador" << endl;
  mtx.unlock();

  mostrador = -1;
  estanquero.signal();
}

// -----------------------------------------------------------------------------
void Estanco::ponerIngrediente( int ing ){
  esperarRecogidaIngrediente();

  mtx.lock();
  cout << "Estanquero : ingrediente " << ing << " puesto en el mostrador" << endl;
  mtx.unlock();

  mostrador = ing;
  fumadores[ing].signal();
}

// -----------------------------------------------------------------------------
void Estanco::esperarRecogidaIngrediente( ){
  while(mostrador != -1){
    estanquero.wait();
  }
}

//-------------------------------------------------------------------------
// Función que simula la acción de producir un ingrediente, como un retardo
// aleatorio de la hebra (devuelve número de ingrediente producido)

int producir_ingrediente()
{
   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_produ( aleatorio<10,100>() );

   // informa de que comienza a producir
   cout << "Estanquero : empieza a producir ingrediente (" << duracion_produ.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_produ' milisegundos
   this_thread::sleep_for( duracion_produ );

   const int num_ingrediente = aleatorio<0,num_items-1>() ;

   // informa de que ha terminado de producir
   cout << "Estanquero : termina de producir ingrediente " << num_ingrediente << endl;

   return num_ingrediente ;
}

//----------------------------------------------------------------------
// Función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero( MRef<Estanco> monitor )
{
    while( true ){
      int ingrediente = producir_ingrediente();

      monitor->ponerIngrediente(ingrediente);
      monitor->esperarRecogidaIngrediente();
    }
}

//-------------------------------------------------------------------------
// Función que simula la acción de fumar, como un retardo aleatoria de la hebra

void fumar( int num_fumador )
{

   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

   // informa de que comienza a fumar

    cout << "Fumador " << num_fumador << "  :"
          << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );

   // informa de que ha terminado de fumar

    cout << "Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl;

}

//----------------------------------------------------------------------
// Función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( MRef<Estanco> monitor, int num_fumador )
{
   while( true )
   {
     monitor->obtenerIngrediente(num_fumador);
     fumar(num_fumador);
   }
}

//----------------------------------------------------------------------

int main()
{
  cout << "------------------------------------------" << endl
       << " Problema de los fumadores con monitor SU " << endl
       << "------------------------------------------" << endl
       << flush ;

  MRef<Estanco> estanco = Create<Estanco>( );
  thread estanquero;
  thread fumadores[num_items];

  for (int i = 0; i < num_items; i++){
    fumadores[i] = thread (funcion_hebra_fumador, estanco, i);
  }
    estanquero   = thread (funcion_hebra_estanquero, estanco);


  for (int i = 0; i < num_items; i++){
    fumadores[i].join();
  }
    estanquero.join();


  cout << endl << endl;
}
