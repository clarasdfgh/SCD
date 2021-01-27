# SCD Seminario 1: Cálculo concurrente de una integral

###### Clara Mª Romero Lara, SCD 20-21

## 1. Descripción del problema

El cálculo del valor *I* de la integral de una función *f* de variable real (entre 0 y 1, por ejemplo ) y valores reales positivos se puede calcular evaluando la función *f* en un conjunto de *m* puntos uniformemente espaciados en el intervalo [0, 1], y aproximando *I* como la media de todos esos valores: 

![image-20201002125913500](/home/clara/.config/Typora/typora-user-images/image-20201002125913500.png)

## 2. Código secuencial 

Se nos proporciona la función secuencial que resuelve este problema:

```c++
calcular_integral_secuencial(  )
{
   double suma = 0.0 ;                        // inicializar suma
   for( long j = 0 ; j < m ; j++ )            // para cada $j$ entre $0$ y $m-1$:
   {  const double xj = double(j+0.5)/m ;     //      calcular $x_j$
      suma += f( xj );                        //      añadir $f(x_j)$ a la suma actual
   }
   return suma/m ;                            // devolver valor promedio de $f$
}
```

Donde f(x) es una función cuya integral entre 0 y 1 conocemos que es π.

El main dado contiene la salida de resultados y medición de tiempo.

## 3. Resolución del problema

Existen dos formas de repartir las hebras en este problema: de forma contigua o entrelazada.

Hemos escogido la forma contigua: cada una de las 4 hebras ejecuta un número n/4 de cálculos, de los que luego obtenemos el promedio y juntamos con el uso de `future.get()` 

#### Declaración de hebras

La función `calcular_integral_concurrente()` no hace sino declarar un vector de 4 (número de hebras escogido previamente) *future*, lanzarlo con la `funcion_hebra()`, y recopilar los resultados utilizado `get()` en `suma`.

```c++
double calcular_integral_concurrente( )
{
  double suma = 0.0;
  future<double> futuro[n];

  for(int i=0; i<n; i++){
      futuro[i] = async(launch::async,funcion_hebra,i);   //Creamos las hebras(futuros)
  }

  for(int i=0; i<n; i++){                   //Sumatorio de los resultados de las hebras
      suma += futuro[i].get();
  }

  return suma;
}
```



#### Función hebra

La función hebra calcula la integral de misma forma que la función secuencial, con la diferencia de que reparte los cálculos entre las 4 hebras; asigna a cada una el intervalo correspondiente dividiendo el número de muestras entre el número de hebras, y multiplicando por el índice de la hebra actual.

```c++
double funcion_hebra( long i )
{
  double suma = 0.0 ;
  for( long j = i * (m/n) ; j < (i+1) * (m/n) ; j++ )  //Hemos elegido la forma contigua
  {  const double xj = double(j+0.5)/m ;
     suma += f( xj );
  }
  return suma/m ;
}
```

