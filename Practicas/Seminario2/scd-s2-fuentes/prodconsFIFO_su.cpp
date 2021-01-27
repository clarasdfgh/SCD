// -----------------------------------------------------------------------------
// Productor-Consumidor FIFO con monitores de semántica SU
//
// Clara Mª Romero Lara, SCD 20-21
// -----------------------------------------------------------------------------


#include <iostream>
#include <iomanip>
#include <cassert>
#include <thread>
#include <random>
#include "HoareMonitor.h"
#include "Semaphore.h"

using namespace std ;
using namespace HM;

constexpr int
   num_items  = 40,      // número de items a producir/consumir
   prods      = 5,
   cons       = 5;

mutex
   mtx ;                 // mutex de escritura en pantalla

unsigned
   cont_prod[num_items], // contadores de verificación: producidos
   cont_cons[num_items], // contadores de verificación: consumidos
   buffer_[prods];

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
// funciones comunes a las dos soluciones (fifo y FIFO)
//----------------------------------------------------------------------

int producir_dato( int i )
{
  if(buffer_[i] < (num_items / prods)){
     static int contador = 0 ;
     this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

     mtx.lock();
     cout << "producido: " << contador << endl << flush ;
     buffer_[i]++;
     mtx.unlock();

     cont_prod[contador] ++ ;
     return contador++;
   }
}

//----------------------------------------------------------------------

void consumir_dato( unsigned dato )
{
   if ( num_items <= dato )
   {
      cout << " dato === " << dato << ", num_items == " << num_items << endl ;
      assert( dato < num_items );
   }
   cont_cons[dato] ++ ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
   mtx.lock();
   cout << "                  consumido: " << dato << endl ;
   mtx.unlock();
}
//----------------------------------------------------------------------

void ini_contadores()
{
   for( unsigned i = 0 ; i < num_items ; i++ )
   {  cont_prod[i] = 0 ;
      cont_cons[i] = 0 ;
   }
}

//----------------------------------------------------------------------

void test_contadores()
{
   bool ok = true ;
   cout << "comprobando contadores ...." << flush ;

   for( unsigned i = 0 ; i < num_items ; i++ )
   {
      if ( cont_prod[i] != 1 )
      {
         cout << "error: valor " << i << " producido " << cont_prod[i] << " veces." << endl ;
         ok = false ;
      }
      if ( cont_cons[i] != 1 )
      {
         cout << "error: valor " << i << " consumido " << cont_cons[i] << " veces" << endl ;
         ok = false ;
      }
   }
   if (ok)
      cout << endl << flush << "solución (aparentemente) correcta." << endl << flush ;
}

// *****************************************************************************
// clase para monitor buffer, version FIFO, semántica SU, 5 prod. y 5 cons.

class ProdConsSUFIFO : public HoareMonitor
{
 private:
 static const int           // constantes:
   num_celdas_total = 10;   //  núm. de entradas del buffer
 int                        // variables permanentes
   buffer[num_celdas_total],//  buffer de tamaño fijo, con los datos
   primera_libre,           //  indice de celda de la próxima inserción
   primera_ocupada,
   num_celdas_ocupadas;

 CondVar         // colas condicion:
   ocupadas,                //  cola donde espera el consumidor (n>0)
   libres ;                 //  cola donde espera el productor  (n<num_celdas_total)

 public:                    // constructor y métodos públicos
   ProdConsSUFIFO(  ) ;        // constructor
   int  leer();                // extraer un valor (sentencia L) (consumidor)
   void escribir( int valor ); // insertar un valor (sentencia E) (productor)
} ;
// -----------------------------------------------------------------------------

ProdConsSUFIFO::ProdConsSUFIFO(  )
{
   primera_libre   = 0;
   primera_ocupada = 0;
   num_celdas_ocupadas = 0;

   ocupadas = newCondVar();
   libres   = newCondVar();
}
// -----------------------------------------------------------------------------
// función llamada por el consumidor para extraer un dato

int ProdConsSUFIFO::leer(  )
{
   // esperar bloqueado hasta que 0 < num_celdas_ocupadas
   while ( primera_libre == 0 ){
      ocupadas.wait( );
   }

   // hacer la operación de lectura, actualizando estado del monitor
   assert( 0 < primera_libre  );
   const int valor = buffer[0] ;
   primera_libre-- ;
   num_celdas_ocupadas--;

   for(int i = 0; i < num_celdas_ocupadas; i++){
     buffer[i] = buffer[i+1];
   }

   // señalar al productor que hay un hueco libre, por si está esperando
   libres.signal();

   // devolver valor
   return valor ;
}

// -----------------------------------------------------------------------------

void ProdConsSUFIFO::escribir( int valor )
{
   // esperar bloqueado hasta que num_celdas_ocupadas < num_celdas_total
   while ( num_celdas_ocupadas == num_celdas_total )
      libres.wait( );

   //cout << "escribir: ocup == " << num_celdas_ocupadas << ", total == " << num_celdas_total << endl ;
   assert( num_celdas_ocupadas < num_celdas_total );

   // hacer la operación de inserción, actualizando estado del monitor
   buffer[primera_libre] = valor ;
   primera_libre = (primera_libre + 1) % num_celdas_total;
   num_celdas_ocupadas++ ;

   // señalar al consumidor que ya hay una celda ocupada (por si esta esperando)
   ocupadas.signal();
}
// *****************************************************************************
// funciones de hebras

void funcion_hebra_productora( MRef<ProdConsSUFIFO> monitor, int n )
{
   for( unsigned i = 0 ; i < num_items/prods ; i++ )
   {
      int valor = producir_dato( n ) ;
      monitor->escribir( valor );
   }
}
// -----------------------------------------------------------------------------

void funcion_hebra_consumidora( MRef<ProdConsSUFIFO> monitor, int n )
{
   for( unsigned i = 0 ; i < num_items/cons ; i++ )
   {
      int valor = monitor->leer();
      consumir_dato( valor ) ;
   }
}
// -----------------------------------------------------------------------------

int main()
{
   cout << "---------------------------------------------------------------------------------" << endl
        << "Problema de los productores-consumidores (5 prod/cons, Monitor SU, buffer FIFO). " << endl
        << "---------------------------------------------------------------------------------" << endl
        << flush ;

   MRef<ProdConsSUFIFO> monitor = Create<ProdConsSUFIFO>( );

   thread hebra_productora[prods],
          hebra_consumidora[cons];

   for (int i = 0; i < prods; i++){
     buffer_[i] = 0;
   }

   for (int i = 0; i < prods; i++){   //Si el numero de prod/cons fuera distinto habría que inicializar en varios bucles
     hebra_productora[i]  = thread( funcion_hebra_productora, monitor,  i );
     hebra_consumidora[i] = thread( funcion_hebra_consumidora, monitor, i );
   }

   for(int i = 0; i < cons; i++){
     hebra_productora[i].join();
     hebra_consumidora[i].join();
   }

   // comprobar que cada item se ha producido y consumido exactamente una vez
   test_contadores() ;
}