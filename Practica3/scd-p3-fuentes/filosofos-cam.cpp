// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Práctica 3. Implementación de algoritmos distribuidos con MPI
//
// Archivo: filosofos-plantilla.cpp
// Implementación del problema de los filosofos con camarero.
// Plantilla para completar.
//
// Clara M Romero Lara -- SCD 20-21
// -----------------------------------------------------------------------------


#include <mpi.h>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <iostream>

using namespace std;
using namespace std::this_thread ;
using namespace std::chrono ;

const int
   num_filosofos = 5 ,
   num_procesos  = 2*num_filosofos+1,
   num_asientos  = num_filosofos - 1,
   id_camarero   = num_procesos - 1;

int tag_sentado   = 1,
    tag_levantado = 2;


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

// ---------------------------------------------------------------------

void funcion_filosofos( int id )
{
  int id_ten_izq = (id+1)              % num_procesos, //id. tenedor izq.
      id_ten_der = (id+num_procesos-1) % num_procesos, //id. tenedor der.
      valor = 0;

  MPI_Status estado ;

  while ( true )
  {
    cout << "Filosofo " << id << " solicita al camarero sentarse" <<endl;
    MPI_Ssend( &valor, 1, MPI_INT, id_camarero, tag_sentado, MPI_COMM_WORLD);

    MPI_Recv ( &valor, 1, MPI_INT, id_camarero, tag_sentado, MPI_COMM_WORLD, &estado);
    cout << "Camarero sienta al filosofo " <<id << " a la mesa" <<endl;

    cout <<"Filosofo " <<id << " solicita tenedor izq." <<id_ten_izq <<endl;
    MPI_Ssend( &valor, 1, MPI_INT, id_ten_izq, 0, MPI_COMM_WORLD);

    cout << "Filosofo " << id <<" solicita tenedor der." <<id_ten_der <<endl;
    MPI_Ssend( &valor, 1, MPI_INT, id_ten_der, 0, MPI_COMM_WORLD);

    cout << endl << "Filosofo " <<id <<" comienza a comer" << endl << endl ;
    sleep_for( milliseconds( aleatorio<50,100>() ) );

    cout << "Filosofo " <<id <<" suelta tenedor izq. " <<id_ten_izq <<endl;
    MPI_Ssend( &valor, 1, MPI_INT, id_ten_izq, 0, MPI_COMM_WORLD);

    cout<< "Filosofo " <<id <<" suelta tenedor der. " <<id_ten_der <<endl;
    MPI_Ssend( &valor, 1, MPI_INT, id_ten_izq, 0, MPI_COMM_WORLD);

    cout << endl << "Filosofo " << id << " comienza a pensar" << endl << endl;
    sleep_for( milliseconds( aleatorio<100,500>() ) );

    cout << "Filosofo " << id << " solicita levantarse de la mesa" << endl;
    MPI_Ssend( &valor, 1, MPI_INT, id_camarero, tag_levantado, MPI_COMM_WORLD);
 }
}
// ---------------------------------------------------------------------

void funcion_tenedores( int id )
{
  int valor, id_filosofo ;  // valor recibido, identificador del filosofo
  MPI_Status estado ;       // metadatos de las dos recepciones

  while ( true )
  {
     MPI_Recv( &valor, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &estado);
     id_filosofo = estado.MPI_SOURCE;

     cout <<"\tTenedor " <<id <<" ha sido cogido por filo. " <<id_filosofo <<endl;


     MPI_Recv( &valor, 1, MPI_INT, id_filosofo, 0, MPI_COMM_WORLD, &estado);
     cout <<"\tTenedor "<< id<< " ha sido liberado por filo. " <<id_filosofo <<endl ;
  }
}
// ---------------------------------------------------------------------

void funcion_camarero( )
{
  int num_ocupados = 0,
      tag_actual,
      valor;

  MPI_Status estado ;       // metadatos de las dos recepciones

  while ( true )
  {
    if(num_ocupados < num_asientos){
      tag_actual = MPI_ANY_TAG;
    } else{
      tag_actual = tag_levantado;
    }

    MPI_Recv(&valor, 1, MPI_INT, MPI_ANY_SOURCE, tag_actual, MPI_COMM_WORLD, &estado);

    if(estado.MPI_TAG == tag_sentado){
      num_ocupados++;

      MPI_Ssend(&valor, 1, MPI_INT, estado.MPI_SOURCE, tag_sentado, MPI_COMM_WORLD);
      cout << "Filosofo " << estado.MPI_SOURCE << " se sienta a la mesa" << endl;

    } else if(estado.MPI_TAG == tag_levantado){
      num_ocupados--;
      cout << "Filosofo " << estado.MPI_SOURCE << " abandona la mesa" << endl;
    }
  }
}
// ---------------------------------------------------------------------

int main( int argc, char** argv )
{
   int id_propio, num_procesos_actual ;

   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &id_propio );
   MPI_Comm_size( MPI_COMM_WORLD, &num_procesos_actual );


   if ( num_procesos == num_procesos_actual )
   {
      // ejecutar la función correspondiente a 'id_propio'
      if (  id_propio == id_camarero)
        funcion_camarero();
      else if ( id_propio % 2 == 0 )     // si es par
         funcion_filosofos( id_propio ); //   es un filosofo
      else                               // si es impar
         funcion_tenedores( id_propio ); //   es un tenedor
   }
   else
   {
      if ( id_propio == 0 ) // solo el primero escribe error, indep. del rol
      { cout << "el número de procesos esperados es:    " << num_procesos << endl
             << "el número de procesos en ejecución es: " << num_procesos_actual << endl
             << "(programa abortado)" << endl ;
      }
   }

   MPI_Finalize( );
   return 0;
}

// ---------------------------------------------------------------------
