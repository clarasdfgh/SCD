# SCD Práctica 2.1: Problema de los lectores-escritores sin inanición

###### Clara Mª Romero Lara, SCD 20-21



## 1. Problema de los lectores-escritores

#### Problema

En la versión previa entregada, **se intentaba mitigar el problema de la inanición dándole a los lectores un periodo de tiempo de espera mayor que el de los escritores**. Cómo tenían prioridad, la salida del programa solía tender a la inanición, dejando a las hebras escritoras sin ninguna oportunidad.

A continuación, la salida del código igualando los tiempos de espera de lectores y escritores:

>```
>----------------------------------------------------
> Problema de los lectores-escritores con monitor SU 
>----------------------------------------------------
>Lector   1 : empieza a leer (100 milisegundos)
>Lector   0 : empieza a leer (13 milisegundos)
>Lector   2 : empieza a leer (89 milisegundos)
>Lector   0 : termina de leer 
>Lector   3 : empieza a leer (12 milisegundos)
>Lector   3 : termina de leer 
>Lector   1 : termina de leer 
>Lector   3 : empieza a leer (60 milisegundos)
>Lector   0 : empieza a leer (94 milisegundos)
>Lector   2 : termina de leer 
>Lector   3 : termina de leer 
>Lector   1 : empieza a leer (52 milisegundos)
>Lector   0 : termina de leer 
>Lector   2 : empieza a leer (75 milisegundos)
>Lector   3 : empieza a leer (63 milisegundos)
>Lector   1 : termina de leer 
>Lector   0 : empieza a leer (60 milisegundos)
>Lector   3 : termina de leer 
>Lector   1 : empieza a leer (17 milisegundos)
>Lector   2 : termina de leer 
>Lector   1 : termina de leer 
>Lector   0 : termina de leer 
>Escritor 3 : empieza a escribir (31 milisegundos)
>Escritor 3 : termina de escribir 
>Lector   0 : empieza a leer (61 milisegundos)
>Lector   3 : empieza a leer (65 milisegundos)
>Lector   1 : empieza a leer (20 milisegundos)
>```



Es obvio que los lectores dominan las hebras. 



### Solución

En la función `fin_escritura()` es en la que se decide si se dará paso a los lectores o a los escritores. La función antigua favorecía a los lectores:

```c++
void Lect_escr::fin_escritura( ){
  escrib = false;

if (lectura.get_nwt() != 0)
  {
    lectura.signal();
  } else{
    escritura.signal();
  }
}

```



En la nueva función, en cuanto hay al menos un escritor esperando acceder, se limita el número de lectores a 3.

```c++
void Lect_escr::fin_escritura( ){
  escrib = false;

  if(escritura.get_nwt() > 0 && n_lec >= 3){
    if(n_lec > 3){
      escritura.signal();
    } else{
      lectura.signal();
    }
  } else{
    lectura.signal();
  }

}

```



La traza de ejecución con la nueva función:

> ```
> ----------------------------------------------------
>  Problema de los lectores-escritores con monitor SU 
> ----------------------------------------------------
> Lector   3 : empieza a leer (86 milisegundos)
> Lector   1 : empieza a leer (20 milisegundos)
> Lector   2 : empieza a leer (81 milisegundos)
> Lector   1 : termina de leer 
> Lector   0 : empieza a leer (18 milisegundos)
> Lector   3 : termina de leer 
> Lector   0 : termina de leer 
> Lector   2 : termina de leer 
> Escritor 0 : empieza a escribir (98 milisegundos)
> Escritor 0 : termina de escribir 
> Lector   2 : empieza a leer (84 milisegundos)
> Lector   1 : empieza a leer (20 milisegundos)
> Lector   0 : empieza a leer (37 milisegundos)
> Lector   3 : empieza a leer (29 milisegundos)
> Lector   1 : termina de leer 
> Lector   3 : termina de leer 
> Lector   0 : termina de leer 
> Lector   2 : termina de leer 
> Escritor 2 : empieza a escribir (100 milisegundos)
> Escritor 2 : termina de escribir 
> Lector   0 : empieza a leer (41 milisegundos)
> Lector   3 : empieza a leer (91 milisegundos)
> Lector   2 : empieza a leer (40 milisegundos)
> Lector   1 : empieza a leer (58 milisegundos)
> Lector   2 : termina de leer 
> Lector   0 : termina de leer 
> Lector   1 : termina de leer 
> Lector   3 : termina de leer 
> Escritor 1 : empieza a escribir (16 milisegundos)
> Escritor 1 : termina de escribir 
> Lector   1 : empieza a leer (94 milisegundos)
> Lector   0 : empieza a leer (74 milisegundos)
> Lector   2 : empieza a leer (26 milisegundos)
> Lector   2 : termina de leer 
> Lector   2 : empieza a leer (57 milisegundos)
> Lector   3 : empieza a leer (79 milisegundos)
> ```