#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

//**********************************************************************
// variables compartidas

const int num_items = 3;

Semaphore mostrador = 1;
std::vector<Semaphore> fumador;


mutex 		mtx;


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
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero(  )
{
    while( true ){
      sem_wait( mostrador );
        int i = producir_ingrediente();

        mtx.lock();
        cout << "Estanquero : ingrediente " << i << " puesto en el mostrador" << endl;
        mtx.unlock();

      sem_signal( fumador[i] );
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
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( int num_fumador )
{
   while( true )
   {
     sem_wait( fumador[num_fumador] );

     mtx.lock();
     cout << "Fumador " << num_fumador << "  : toma ingrediente del mostrador" << endl;
     mtx.unlock();

     sem_signal( mostrador );

     fumar(num_fumador);
   }
}

//----------------------------------------------------------------------

int main()
{
  cout << "---------------------------" << endl
       << " Problema de los fumadores " << endl
       << "---------------------------" << endl
       << flush ;


  for (int i = 0; i < num_items; i++){
    fumador.push_back(0);
  }

  thread estanquero;
  thread fumadores[num_items];

  for (int i = 0; i < num_items; i++){
    fumadores[i] = thread (funcion_hebra_fumador, i);
  }

    estanquero = thread (funcion_hebra_estanquero);


  for (int i = 0; i < num_items; i++){
    fumadores[i].join();
  }

    estanquero.join();

  cout << endl << endl;
}
