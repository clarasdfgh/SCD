# SCD Práctica 2: Problemas de los fumadores y de los lectores-escritores

###### Clara Mª Romero Lara, SCD 20-21



## 1. Problema de los fumadores

#### Código fuente

```c++
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

```



#### Traza de ejecución

> ```
> ------------------------------------------
>  Problema de los fumadores con monitor SU 
> ------------------------------------------
> Estanquero : empieza a producir ingrediente (98 milisegundos)
> Estanquero : termina de producir ingrediente 2
> Estanquero : ingrediente 2 puesto en el mostrador
> Fumador 2  : toma ingrediente del mostrador
> Estanquero : empieza a producir ingrediente (79 milisegundos)
> Fumador 2  : empieza a fumar (31 milisegundos)
> Fumador 2  : termina de fumar, comienza espera de ingrediente.
> Estanquero : termina de producir ingrediente 0
> Estanquero : ingrediente 0 puesto en el mostrador
> Fumador 0  : toma ingrediente del mostrador
> Fumador 0  : empieza a fumar (64 milisegundos)
> Estanquero : empieza a producir ingrediente (67 milisegundos)
> Fumador 0  : termina de fumar, comienza espera de ingrediente.
> Estanquero : termina de producir ingrediente 0
> Estanquero : ingrediente 0 puesto en el mostrador
> Fumador 0  : toma ingrediente del mostrador
> Fumador 0  : empieza a fumar (24 milisegundos)
> Estanquero : empieza a producir ingrediente (26 milisegundos)
> Fumador 0  : termina de fumar, comienza espera de ingrediente.
> Estanquero : termina de producir ingrediente 0
> Estanquero : ingrediente 0 puesto en el mostrador
> Fumador 0  : toma ingrediente del mostrador
> Fumador 0  : empieza a fumar (195 milisegundos)
> Estanquero : empieza a producir ingrediente (99 milisegundos)
> Estanquero : termina de producir ingrediente 2
> Estanquero : ingrediente 2 puesto en el mostrador
> Fumador 2  : toma ingrediente del mostrador
> Fumador 2  : empieza a fumar (174 milisegundos)
> Estanquero : empieza a producir ingrediente (49 milisegundos)
> Estanquero : termina de producir ingrediente 1
> Estanquero : ingrediente 1 puesto en el mostrador
> Fumador 1  : toma ingrediente del mostrador
> Fumador 1  : empieza a fumar (48 milisegundos)
> Estanquero : empieza a producir ingrediente (22 milisegundos)
> Estanquero : termina de producir ingrediente 0
> Estanquero : ingrediente 0 puesto en el mostrador
> Fumador 0  : termina de fumar, comienza espera de ingrediente.
> Fumador 0  : toma ingrediente del mostrador
> Estanquero : empieza a producir ingrediente (89 milisegundos)
> Fumador 0  : empieza a fumar (65 milisegundos)
> Fumador 1  : termina de fumar, comienza espera de ingrediente.
> Fumador 0  : termina de fumar, comienza espera de ingrediente.
> Fumador 2  : termina de fumar, comienza espera de ingrediente.
> ---
> ```

#### 

## 2. Problema de los lectores-escritores

#### Código fuente

```c++
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

const int num_lectores   = 3,
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

}

// -----------------------------------------------------------------------------
void Lect_escr::fin_escritura( ){
  escrib = false;

  if (lectura.get_nwt() != 0){    //Función de CondVar que comprueba cuántas hebras hay esperando en ella
    lectura.signal();
  } else{
    escritura.signal();
  }
}


//-------------------------------------------------------------------------
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

//----------------------------------------------------------------------
// Función que ejecuta la hebra del escritor

void funcion_hebra_escritor( MRef<Lect_escr> monitor, int n_escritor )
{
    while( true ){
      chrono::milliseconds espera( aleatorio<10,90>() );
      this_thread::sleep_for( espera );

      monitor->ini_escritura();
      escribir(n_escritor);
      monitor->fin_escritura();
    }
}

//-------------------------------------------------------------------------
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


//----------------------------------------------------------------------
// Función que ejecuta la hebra del lector

void  funcion_hebra_lector( MRef<Lect_escr> monitor, int n_lector )
{
   while( true )
   {
     chrono::milliseconds espera( aleatorio<90,200>() );
     this_thread::sleep_for( espera );

     monitor->ini_lectura();
     leer(n_lector);
     monitor->fin_lectura();
   }
}

//----------------------------------------------------------------------

int main()
{
  cout << "----------------------------------------------------" << endl
       << " Problema de los lectores-escritores con monitor SU " << endl
       << "----------------------------------------------------" << endl
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

```



#### Traza de ejecución

> ```
> ----------------------------------------------------
>  Problema de los lectores-escritores con monitor SU 
> ----------------------------------------------------
> Escritor 1 : empieza a escribir (40 milisegundos)
> Escritor 1 : termina de escribir 
> Escritor 0 : empieza a escribir (64 milisegundos)
> Escritor 0 : termina de escribir 
> Lector   2 : empieza a leer (65 milisegundos)
> Lector   1 : empieza a leer (30 milisegundos)
> Lector   1 : termina de leer 
> Lector   0 : empieza a leer (11 milisegundos)
> Lector   2 : termina de leer 
> Lector   0 : termina de leer 
> Escritor 2 : empieza a escribir (80 milisegundos)
> Escritor 2 : termina de escribir 
> Lector   1 : empieza a leer (20 milisegundos)
> Lector   1 : termina de leer 
> Escritor 1 : empieza a escribir (31 milisegundos)
> Escritor 1 : termina de escribir 
> Lector   2 : empieza a leer (63 milisegundos)
> Lector   0 : empieza a leer (82 milisegundos)
> Lector   1 : empieza a leer (13 milisegundos)
> Lector   2 : termina de leer 
> Lector   1 : termina de leer 
> Lector   0 : termina de leer 
> Escritor 0 : empieza a escribir (21 milisegundos)
> Escritor 0 : termina de escribir 
> Escritor 2 : empieza a escribir (71 milisegundos)
> Escritor 2 : termina de escribir 
> Lector   1 : empieza a leer (98 milisegundos)
> Lector   2 : empieza a leer (52 milisegundos)
> Lector   0 : empieza a leer (70 milisegundos)
> Lector   2 : termina de leer 
> ---
> ```