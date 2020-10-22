# SCD Práctica 1: Problemas del productor-consumidor y de los fumadores

###### Clara Mª Romero Lara, SCD 20-21

## 1. Problema del productor-consumidor

#### Variables

Las variables compartidas han sido las siguientes:

- El `buffer`, un vector de 10 elementos que sostiene los datos producidos antes de que se consuman
- El índice que marca el elemento actual del buffer,`n`
- Un cerrojo `mtx` para la sección crítica



#### Semáforos

Hemos empleado dos semáforos: uno de `escritura` y otro de `lectura`. Sirven para que no se pueda producir una interfoliación incorrecta (leer el mismo dato varias veces, leer en orden incorrecto...)

- `Escritura` está inicializado al tamaño del buffer, 10. Esto quiere decir que, como el buffer empieza vacío, puede producir hasta 10 veces sin obstáculos.
  - Usa sem_wait antes de introducir un nuevo dato en el buffer
  - Usa sem_signal tras consumir el dato

- `Lectura` está inicializado a 0 porque al inicio del programa no hay elementos en el buffer que pueda consumir.
  - Usa sem_wait antes de consumir un dato del buffer
  - Usa sem_signal tras producir e insertar un nuevo dato en el buffer



### Versión LIFO

En esta versión *Last In, First Out* el dato consumido es siempre el índice `n`. Cuando se consume se reduce su valor en uno.

#### Código fuente

```c++
#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

//**********************************************************************
// variables compartidas

const int num_items = 40 ,   // número de items
	        tam_vec   = 10 ;   // tamaño del buffer

unsigned  cont_prod[num_items] = {0}, // contadores de verificación: producidos
          cont_cons[num_items] = {0}; // contadores de verificación: consumidos

Semaphore escritura = tam_vec,
					lectura   = 0;

int 		  buffer[tam_vec];
int				n = 0;

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

//**********************************************************************
// funciones comunes a las dos soluciones (fifo y lifo)
//----------------------------------------------------------------------

int producir_dato()
{
   static int contador = 0 ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "producido: " << contador << endl << flush ;

   cont_prod[contador] ++ ;
   return contador++ ;
}
//----------------------------------------------------------------------

void consumir_dato( unsigned dato )
{
   assert( dato < num_items );
   cont_cons[dato] ++ ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "\t\t\tconsumido: " << dato << endl << endl ;

}


//----------------------------------------------------------------------

void test_contadores()
{
   bool ok = true ;
   cout << "comprobando contadores ...." ;
   for( unsigned i = 0 ; i < num_items ; i++ )
   {  if ( cont_prod[i] != 1 )
      {  cout << "error: valor " << i << " producido " << cont_prod[i] << " veces." << endl ;
         ok = false ;
      }
      if ( cont_cons[i] != 1 )
      {  cout << "error: valor " << i << " consumido " << cont_cons[i] << " veces" << endl ;
         ok = false ;
      }
   }
   if (ok)
      cout << endl << flush << "solución (aparentemente) correcta." << endl << flush ;
}

//----------------------------------------------------------------------

void  funcion_hebra_productora(  )
{
   for( unsigned i = 0 ; i < num_items ; i++ )
   {
      int dato = producir_dato() ;
			sem_wait( escritura );				//Esperamos al semáforo de escritura

			mtx.lock();							//Sección crítica, bloqueamos el cerrojo
			buffer[n] = dato;
			n++;

			cout << "\tinsertado: " << dato << endl;

			mtx.unlock();

			sem_signal( lectura );

	  }
}

//----------------------------------------------------------------------

void funcion_hebra_consumidora(  )
{
   for( unsigned i = 0 ; i < num_items ; i++ )
   {
      int dato ;
			sem_wait( lectura );

			mtx.lock();
			n--;
			dato = buffer[n];

			cout << "\t\tretirado:  " << dato << endl;

			mtx.unlock();

      consumir_dato( dato ) ;
			sem_signal ( escritura );

    }
}
//----------------------------------------------------------------------

int main()
{
   cout << "---------------------------------------------------------" << endl
        << "Problema de los productores-consumidores (solución LIFO)." << endl
        << "---------------------------------------------------------" << endl
        << flush ;

   thread hebra_productora ( funcion_hebra_productora ),
          hebra_consumidora( funcion_hebra_consumidora );

   hebra_productora.join() ;
   hebra_consumidora.join() ;

   test_contadores();
}

```



##### Traza de la ejecución

*La traza ha sido recortada para mejor formato de esta documentación, pero incluye la salida de `test_contadores()` al final*

> `---------------------------------------------------------
> Problema de los productores-consumidores (solución LIFO).
> ---------------------------------------------------------`
> producido: 0
> 	insertado: 0
> 		retirado:  0
> 			consumido: 0
>
> producido: 1
> 	insertado: 1
> 		retirado:  1
> 			consumido: 1
>
> producido: 2
> 	insertado: 2
> 		retirado:  2
> producido: 3
> 	insertado: 3
> 			consumido: 2
>
> ​		retirado:  3
> producido: 4
> ​	insertado: 4
> ​			consumido: 3
>
> ​		retirado:  4
> ​			consumido: 4
>
> producido: 5
> 	insertado: 5
> 		retirado:  5
> producido: 6
> 	insertado: 6
> 			consumido: 5
>
> `---La traza ha sido recortada aquí ---`
>
> producido: 39
> 	insertado: 39
> 			consumido: 38
>
> ​		retirado:  39
> ​			consumido: 39
>
> comprobando contadores ....
> solución (aparentemente) correcta.



### Versión FIFO

En esta versión *First In, First Out* el dato consumido es siempre el elemento 0 del buffer. Cuando se consume, desplazamos todos los elementos posteriores del buffer y reducimos `n` en uno.

#### Código fuente

```c++
#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

//**********************************************************************
// variables compartidas

const int num_items = 40 ,   // número de items
	        tam_vec   = 10 ;   // tamaño del buffer

unsigned  cont_prod[num_items] = {0}, // contadores de verificación: producidos
          cont_cons[num_items] = {0}; // contadores de verificación: consumidos

Semaphore escritura = tam_vec,
					lectura   = 0;

int 		  buffer[tam_vec];
int				n = 0;

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

//**********************************************************************
// funciones comunes a las dos soluciones (fifo y lifo)
//----------------------------------------------------------------------

int producir_dato()
{
   static int contador = 0 ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "producido: " << contador << endl << flush ;

   cont_prod[contador] ++ ;
   return contador++ ;
}
//----------------------------------------------------------------------

void consumir_dato( unsigned dato )
{
   assert( dato < num_items );
   cont_cons[dato] ++ ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "\t\t\t\t\t\tconsumido: " << dato << endl << endl ;

}


//----------------------------------------------------------------------

void test_contadores()
{
   bool ok = true ;
   cout << "comprobando contadores ...." ;
   for( unsigned i = 0 ; i < num_items ; i++ )
   {  if ( cont_prod[i] != 1 )
      {  cout << "error: valor " << i << " producido " << cont_prod[i] << " veces." << endl ;
         ok = false ;
      }
      if ( cont_cons[i] != 1 )
      {  cout << "error: valor " << i << " consumido " << cont_cons[i] << " veces" << endl ;
         ok = false ;
      }
   }
   if (ok)
      cout << endl << flush << "solución (aparentemente) correcta." << endl << flush ;
}

//----------------------------------------------------------------------

void  funcion_hebra_productora(  )
{
   for( unsigned i = 0 ; i < num_items ; i++ )
   {
      int dato = producir_dato() ;
			sem_wait( escritura );				//Esperamos al semáforo de escritura

			mtx.lock();										//Sección crítica, bloqueamos el cerrojo
			buffer[n] = dato;
			n++;

			cout << "\t\tinsertado: " << dato << endl;

			mtx.unlock();

			sem_signal( lectura );

	  }
}

//----------------------------------------------------------------------

void funcion_hebra_consumidora(  )
{
   for( unsigned i = 0 ; i < num_items ; i++ )
   {
      int dato ;
			sem_wait( lectura );

			mtx.lock();

			dato = buffer[0];

			n--;

			for(int i = 0; i < n; i++){
				buffer[i] = buffer[i+1];
			}

			cout << "\t\t\t\tretirado:  " << dato << endl;

			mtx.unlock();

      consumir_dato( dato ) ;
			sem_signal ( escritura );

    }
}
//----------------------------------------------------------------------

int main()
{
   cout << "---------------------------------------------------------" << endl
        << "Problema de los productores-consumidores (solución FIFO)." << endl
        << "---------------------------------------------------------" << endl
        << flush ;

   thread hebra_productora ( funcion_hebra_productora ),
          hebra_consumidora( funcion_hebra_consumidora );

   hebra_productora.join() ;
   hebra_consumidora.join() ;

   test_contadores();
}

```



##### Traza de la ejecución

*La traza ha sido recortada para mejor formato de esta documentación, pero incluye la salida de `test_contadores()` al final*

> `---------------------------------------------------------
> Problema de los productores-consumidores (solución FIFO).
> ---------------------------------------------------------`
> producido: 0
> 		insertado: 0
> 				retirado:  0
> producido: 1
> 		insertado: 1
> producido: 2
> 		insertado: 2
> 						consumido: 0
>
> ​				retirado:  1
> producido: 3
> ​		insertado: 3
> ​						consumido: 1
>
> ​				retirado:  2
> producido: 4
> ​		insertado: 4
> ​						consumido: 2
>
> ​				retirado:  3
> producido: 5
> ​		insertado: 5
> ​						consumido: 3
>
> ​				retirado:  4
> ​						consumido: 4
>
> ​				retirado:  5
> ​						consumido: 5
>
> ` ---La traza ha sido recortada aquí ---`
>
> producido: 39
> 		insertado: 39
> 						consumido: 36
>
> ​				retirado:  37
> ​						consumido: 37
>
> ​				retirado:  38
> ​						consumido: 38
>
> ​				retirado:  39
> ​						consumido: 39
>
> comprobando contadores ....
> solución (aparentemente) correcta.



## 2. Problema de los fumadores

### Semáforos

Hemos empleado un total de 4 semáforos, 3 de ellos contenidos en un vector de la STL.

- `Mostrador`, según lo describe el problema, sólo puede sujetar un ingrediente. El mostrador empieza vacío, así que se puede inicializar en 1 para que se produzca el elemento sin obstáculos al principio del programa.
  - Usa sem_wait antes de producirse el ingrediente
  - Usa sem_signal cuando el fumador recoge un ingrediente del mostrador
- El vector de semáforos `Fumador` tiene 3 elementos, uno por cada fumador y su correspondiente ingrediente. Se inicializa (en el main) a 0, porque el mostrador está vacío y no pueden consumir.
  - Usa sem_wait con argumento `i` antes de tomar el ingrediente `i` del mostrador
  - Usa sem_signal con `i` como argumento cuando se produce el ingrediente adecuado para el fumador `i`

También hemos empleado un cerrojo `mtx` para una salida de texto ininterrumpida.

### Código fuente

```c++
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

```



#### Traza de la ejecución

> `---------------------------
>  Problema de los fumadores 
> ---------------------------`
>
> Estanquero : empieza a producir ingrediente (43 milisegundos)
> Estanquero : termina de producir ingrediente 0
> Estanquero : ingrediente 0 puesto en el mostrador
> Fumador 0  : toma ingrediente del mostrador
> Fumador 0  : empieza a fumar (184 milisegundos)
> Estanquero : empieza a producir ingrediente (36 milisegundos)
> Estanquero : termina de producir ingrediente 1
> Estanquero : ingrediente 1 puesto en el mostrador
> Fumador 1  : toma ingrediente del mostrador
> Fumador 1  : empieza a fumar (97 milisegundos)
> Estanquero : empieza a producir ingrediente (90 milisegundos)
> Estanquero : termina de producir ingrediente 0
> Estanquero : ingrediente 0 puesto en el mostrador
> Fumador 1  : termina de fumar, comienza espera de ingrediente.
> Fumador 0  : termina de fumar, comienza espera de ingrediente.
> Fumador 0  : toma ingrediente del mostrador
> Fumador 0  : empieza a fumar (131 milisegundos)
> Estanquero : empieza a producir ingrediente (10 milisegundos)
> Estanquero : termina de producir ingrediente 2
> Estanquero : ingrediente 2 puesto en el mostrador
> Fumador 2  : toma ingrediente del mostrador
> Fumador 2  : empieza a fumar (53 milisegundos)
> Estanquero : empieza a producir ingrediente (18 milisegundos)
> Estanquero : termina de producir ingrediente 2
> Estanquero : ingrediente 2 puesto en el mostrador
> Fumador 2  : termina de fumar, comienza espera de ingrediente.
> Fumador 2  : toma ingrediente del mostrador
> Fumador 2  : empieza a fumar (71 milisegundos)
>
> `...`

