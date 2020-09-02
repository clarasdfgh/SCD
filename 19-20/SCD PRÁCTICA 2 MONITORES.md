# SCD PRÁCTICA 2: MONITORES

###### Clara María Romero Lara



## Fumadores

#### Código fuente

```
#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "HoareMonitor.h"

using namespace std ;
using namespace HM ;

mutex mtx; //candado de escritura en pantalla

const int num_fum = 3;

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

//----------------------------------------------------------------------
// Monitor SU Estanco

class Estanco : public HoareMonitor
{
private:
  int suministro;
  CondVar fumador[num_fum] ;
  CondVar estanquero ;

public:                       // constructor y métodos públicos
  Estanco(  ) ;            // constructor
  void  ponerIngrediente( int ingrediente );
  void  obtenerIngrediente( int n_fumador );
  void  esperarRecogida();
} ;


Estanco::Estanco(  )
{
  suministro = -1;

  estanquero = newCondVar();

  for (int i = 0; i<num_fum; ++i)
    fumador[i] = newCondVar();
}

void Estanco::ponerIngrediente( int ingrediente )
{
  suministro = ingrediente;
  fumador[ingrediente].signal();
}

void Estanco::esperarRecogida()
{
  if( suministro != -1 )
    estanquero.wait();
}

void Estanco::obtenerIngrediente( int n_fumador )
{
  if( suministro != n_fumador )
    fumador[n_fumador].wait();
  suministro = -1;
  estanquero.signal();
}

//----------------------------------------------------------------------
// Funciones

int producir ()
{
  int producto = aleatorio<0,num_fum-1>();
  mtx.lock();
  cout << "Estanquero: pone ingrediente " << producto << endl;
  mtx.unlock();
  return producto;

}


void fumar( int num_fumador )
{

  // calcular milisegundos aleatorios de duración de la acción de fumar)
  chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

  // informa de que comienza a fumar
  mtx.lock();
  cout << "Fumador " << num_fumador << "  :"
       << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;
  mtx.unlock();

  // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
  this_thread::sleep_for( duracion_fumar );

  // informa de que ha terminado de fumar
  mtx.lock();
  cout << "Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl;
  mtx.unlock();

}

//----------------------------------------------------------------------
// Hebras

void funcion_hebra_estanquero( MRef<Estanco> monitor )
{
  while ( true )
    {
      int ingrediente = producir();
      monitor->ponerIngrediente(ingrediente);
      monitor->esperarRecogida();
    }
}


void  funcion_hebra_fumador(  MRef<Estanco> monitor,  int num_fumador )
{
  while( true )
    {
      monitor->obtenerIngrediente(  num_fumador );
      cout << "Fumador " << num_fumador << " retira el producto." << endl;
      fumar(num_fumador);
    }
}

//----------------------------------------------------------------------

int main()
{
  cout << "===========================" << endl
       << " Problema de los fumadores " << endl
       << "---------------------------" << endl;

  auto monitor = Create<Estanco>( );

  thread hebra_fumador[num_fum];

  for ( int i = 0; i < num_fum; ++i )
    hebra_fumador[i] = thread ( funcion_hebra_fumador, monitor, i );


  thread hebra_estanquero( funcion_hebra_estanquero , monitor );

  for ( int i = 0; i < num_fum; ++i )
    hebra_fumador[i].join();

  hebra_estanquero.join();

}
```



#### Extracto ejecución

> ```
> ===========================
>  Problema de los fumadores 
> ===========================
> 
> Estanquero: pone ingrediente 1
> Fumador 1 retira el producto.
> Fumador 1  : empieza a fumar (37 milisegundos)
> Estanquero: pone ingrediente 1
> Fumador 1  : termina de fumar, comienza espera de ingrediente.
> Estanquero: pone ingrediente 0
> Fumador 1 retira el producto.
> Fumador 1  : empieza a fumar (116 milisegundos)
> Fumador 0 retira el producto.
> Fumador 0  : empieza a fumar (33 milisegundos)
> Estanquero: pone ingrediente 2
> Fumador 2 retira el producto.
> Estanquero: pone ingrediente 1
> Fumador 2  : empieza a fumar (114 milisegundos)
> Fumador 0  : termina de fumar, comienza espera de ingrediente.
> Fumador 2  : termina de fumar, comienza espera de ingrediente.
> Fumador 1  : termina de fumar, comienza espera de ingrediente.
> (...)
> ```



## Barbero

#### Código fuente

```
#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "HoareMonitor.h"
#include "Semaphore.h"

using namespace std ;
using namespace HM ;

mutex mtx; //candado de escritura en pantalla

const int NUM_CLIENTES = 3;

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

//----------------------------------------------------------------------
// Funciones

void EsperarFueraBarberia(int num_cliente){
   chrono::milliseconds espera_fuera( aleatorio<20,200>() );


   mtx.lock();
   cout << "Cliente " << num_cliente << "  :"
         << " sale de la barberia (" << espera_fuera.count() << " milisegundos)" << endl;
   mtx.unlock();

   this_thread::sleep_for( espera_fuera );

   mtx.lock();
   cout << "Cliente " << num_cliente << "  : termina la espera." << endl;
   mtx.unlock();
}


void cortarPeloACliente(){
   chrono::milliseconds cortar_pelo( aleatorio<20,200>() );


   mtx.lock();
   cout << "El barbero comienza a cortar el pelo al cliente  " << endl;
   mtx.unlock();

   this_thread::sleep_for( cortar_pelo );

   mtx.lock();
   cout << "El barbero acaba de cortar el pelo al cliente " << endl;
   mtx.unlock();
}

//----------------------------------------------------------------------
// Monitor barbería

class Barberia : public HoareMonitor{
private:

   CondVar barbero;
   CondVar silla;
   CondVar cola_espera;

public:
   Barberia();
   void cortarPelo(int n_cliente);
   void siguienteCliente();
   void finCliente();
};

Barberia::Barberia(){
   barbero = newCondVar();
   silla = newCondVar();
   cola_espera = newCondVar();
}

void Barberia::cortarPelo(int n_cliente){
   cout << "Entra cliente " << n_cliente << endl;
   if (cola_espera.empty()){
      if (barbero.empty()){
         cout << "Cliente " << n_cliente << " espera a que el barbero acabe" << endl;
         cola_espera.wait();
      }else{
         cout << "Cliente " << n_cliente << " despierta al barbero" << endl;
         barbero.signal();

      }

   }else{
      cout << "El cliente " << n_cliente << " se pone a la cola" << endl;
      cola_espera.wait();
   }
   cout << "Cliente " << n_cliente << " se pone en la silla " << endl;
   silla.wait();

}

void Barberia::siguienteCliente(){
   if (cola_espera.empty()){
      cout << "No hay clientes, el barbero se va a dormir" << endl;
      barbero.wait();
   }else{
      cout << "Avisa al siguiente cliente" << endl;
      cola_espera.signal();
   }
}

void Barberia::finCliente(){
   cout << "Barbero acaba con el cliente de la silla "<< endl;
   silla.signal();
}


//----------------------------------------------------------------------
// Funciones hebra

void funcion_hebra_barbero( MRef<Barberia> barberia )
{


   while (true){
      barberia->siguienteCliente();
      cortarPeloACliente();
      barberia->finCliente();

   }
}

void  funcion_hebra_cliente( MRef<Barberia> barberia, int num_cliente )
{
   while( true )
   {

      barberia->cortarPelo(num_cliente);
      EsperarFueraBarberia(num_cliente);

   }
}


int main()
{
  cout << "=========================" << endl
       << " Problema de la barbería " << endl
       << "-------------------------" << endl;

   MRef<Barberia> barberia = Create<Barberia> ();
   thread barbero;
   barbero = thread (funcion_hebra_barbero, barberia);

   thread clientes[NUM_CLIENTES];
   for (int i = 0; i < NUM_CLIENTES; i++)
      clientes[i] = thread (funcion_hebra_cliente, barberia, i);

   barbero.join();


   for (int i = 0; i < NUM_CLIENTES; i++)
      clientes[i].join();

}
```



#### Extracto ejecución

> ```
> =========================
>  Problema de la barbería 
> =========================
> 
> No hay clientes, el barbero se va a dormir
> Entra cliente 0
> Cliente 0 despierta al barbero
> El barbero comienza a cortar el pelo al cliente  
> Cliente 0 se pone en la silla 
> Entra cliente 1
> Cliente 1 espera a que el barbero acabe
> Entra cliente 2
> El cliente 2 se pone a la cola
> El barbero acaba de cortar el pelo al cliente 
> Barbero acaba con el cliente de la silla 
> Cliente 0  : sale de la barberia (188 milisegundos)
> Avisa al siguiente cliente
> Cliente 1 se pone en la silla 
> El barbero comienza a cortar el pelo al cliente  
> El barbero acaba de cortar el pelo al cliente 
> Barbero acaba con el cliente de la silla 
> Cliente 1  : sale de la barberia (134 milisegundos)
> Avisa al siguiente cliente
> (...)
> ```

