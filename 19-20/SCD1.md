 # 1-INTRODUCCIÓN



> 1-Conceptos básicos, motivación
>
> 2-Modelo abstracto, consideraciones sobre el hardware
>
> 3-Exclusión mutua, sincronización
>
> 4-Propiedades de los sist concurrentes
>
> 5-Verficación de sist concurrentes




## 1-Conceptos básicos, motivación
### Conceptos básicos

- Programa secuencial: declaraciones de datos + conjunto de instrucciones sobre estos que se ejecutan en secuencia
- Programa concurrente: conjunto de programas secuenciales ordinarios que se pueden ejecutar lógicamente en paralelo
- Proceso: ejecución de un programa secuencial
- Concurrencia: potencial de ejecución paralela, solapamiento real/virtual de varias actividades en el tiempo
- Programación concurrente (PC): conjunto de notaciones y técnicas de programación usadas para expresar paralelismo potencial y resolver problemas de sincronización y comunicación
  - Es independiente de la implementación del paralelismo, es un concepto abstracto
- Programación paralela: acelera la resolución de problemas mediante el aprovechamiento de la capacidad de procesamiento en paralelo del hardware
- Programación distribuida: hace que varios componentes software en distintos ordenadores trabajen juntos
- Programación en tiempo real: sistemas que están funcionando continuamente, recibiendo E/S desde hardware (sist reactivos) en los que se trabaja con restricciones muy estrictas a la respuesta temporal (sist tiempo real)
  - Programa correcto a nivel lógico + paralelo + correcto a nivel temporal
  - Mala respuesta a tiempo > Respuesta correcta tarde

###  Motivación

¿Por qué? Eficiencia y calidad. Permite aprovechar mejor los recursos del hardware.

En sist de un solo procesador: cuando la tarea que tiene el control necesita E/S cede el control a otra para evitar espera ociosa del procesador; sist multiusuario.

En sist de varios procesadores: permite repartir las tareas entre procesadores; minimiza tiempo de ejecución.

Muchos programas se entienden mejor en términos de muchos procesos secuenciales ejecutándose concurrentemente antes que como un único programa secuencial (ej. servidor web para reserva de vuelos, cada cliente es un proceso con restricciones comunes)


## 2-Modelo abstracto, consideraciones sobre el hardware
### Consideraciones sobre el hardware
#### Modelos de arquitectura para la programación concurrente
##### Mecanismos de implementación de la concurrencia

![1568361473476](/home/clara/.config/Typora/typora-user-images/1568361473476.png)

- Dependen de la arquitectura
- Consideran una máquina virtual que representa un sistema (multiprocesador o distribuido) , proporcionando una base homogénea para la ejecución de procesos concurrentes
- El tipo de paralelismo afecta a la eficiencia pero **no a la corrección**

#### Concurrencia en sist monoprocesador:

- Multiprogramación: el SO gestiona cómo los múltiples procesos se reparten los ciclos de CPU
- Mejor aprovechamiento de la CPU
- Multiusuario
- Permite el diseño concurrente
- Sincronización y comunicación mediante variables compartidas
*Ej. SO antiguos: Windows 95*


#### Concurrencia en sist multiprocesadores de memoria compartida:
Un procesador puede compartir o no físicamente la memoria, pero el espacio de direcciones es compartido. 
- La interacción entre procesos se puede implementar mediante variables compartidas.
*Ej. Ordenador con CPU multicore*

#### Concurrencia de sist distribuidos: 

- No existe memoria común, cada uno tiene su propio espacio de direcciones
- Los procesadores interactúan con una red de interconexión (paso de msjs)
- PC: además de la concurrencia trata con problemas como tratamiento de errores, transparencia, heterogeneidad
*Ej. Internet, clusters de ordenadores*

## Modelo abstracto de concurrencia:

- **Sentencia atómica** (indivisible): SIEMPRE se ejecuta de principio a fin sin verse afectada durante su ejecución por otras sentencias en ejecución de otros procesos del programa
  - No se ve afectada cuando el funcionamiento de esta instrucción no depende de las instrucciones de otros procesos
  - El resultado no se verá nunca afectado por otras instrucciones ejecutadas concurrentemente: su resultado puede verse predecido desde el inicio de su ejecución
*Ej de sentencias atómicas: leer una celda de memoria y cargar su valor en un registro, incrementar el valor de un registro, operaciones aritméticas entre registros, escribir el valor de un registro en la memoria*

- **Sentencias no atómicas**: las más comunes en cualquier lenguaje de alto nivel
*Una sentencia tan simple como x := x+1 no es atómica, porque al compilarla se lee x, carga x en registro, aumenta el valor de x, escribe el nuevo valor en el registro... El resultado depende de la variable x, que podría cambiar si otro proceso está operando simultáneamente sobre ella.*

#### Interfoliación de sentencias atómicas

Si tenemos dos procesos secuenciales, A y B, compuestos por las sentencias atómicas A1, A2, A3, B1, B2, B3; la interfoliación es cualquiera de las mezclas posibles de las sentencias atómicas de A y B.

| Proceso A              | A1, A2, A3             |
| ---------------------- | ---------------------- |
| **Proceso B**          | **B1, B2, B3**         |
| Programa concurrente C | A1, A2, A3, B1, B2, B3 |
| Programa concurrente C | B1, B2, B3, A1, A2, A3 |
| Programa concurrente C | A1, B1, A2, B2, A3, B3 |

*etcétera...*

La interfoliación es secuencial, respeta el orden (A2 no puede ir antes que A1)

Las sentencias atómicas se ordenan en función del instante en el que acaban (que es cuando tienen efecto)

#### Abstracción
El modelo basado en el estudio de todas las posibles secuencias de ejecución entralazadas de los procesos constituye la abstracción. Se consideran únicamente las características relevantes que determinan el resultado del cálculo; y permite simplificar el análisis y diseño de programas concurrentes.

Los detalles no relevantes se ignoran:

- Áreas de memoria asignadas a los procesos

- Los registros particulares que se están usando

- El costo de los cambios de contexto entre procesos

- La política del SO relativa a asignación de CPU

- Las diferencias entre entornos multiprocesador o monoprocesador

  *Remember: el tipo de paralelismo afecta a la eficiencia pero no a la corrección*

##### Independencia del estado de ejecución
>**El entrelazamiento preserva la consistencia**: el resultado de una instrucción individual sobre un dato no depende de las circunstancias de la ejecución

##### Velocidad de ejecución, hipótesis del progreso infinito
>**Progreso finito**: no se puede hacer ninguna suposición acerca de la velocidad absoluta o relativa de la ejecución de procesos, excepto que esta es mayor que 0

> **Punto de vista global**: durante la ejecución de un programa concurrente, en cualquier momento existirá al menos 1 proceso preparado; es decir: eventualmente se permitirá la ejecución de algún proceso

> **Punto de vista local**: cuando el proceso concreto de un programa concurrente comienza la ejecución de una sentencia, completará la ejecución de esta en un intervalo de tiempo finito

##### Estados e historias de ejecución de un programa concurrente
>**Estado de un programa concurrente**: valores de las variables del programa en un momento dado. Incluye variables declaradas explícitamente y variables con información de estado oculta (contador del programa, registros...)

>**Historia o traza de un programa concurrente**: secuencia de estados producida por una secuencia concreta de interfoliación

### Notación para expresar ejecución concurrente

- Sistemas estáticos: 
  - Número de procesos fijado en el src del programa
  - Los procesos se activan al lanzar el programa
- Sistemas dinámicos:
  - Número variable de procesos/hebras que se activan en cualquier momento de la ejecución

## Exclusión mutua, sincronización

Según el modelo abstracto, los procesos concurrentes ejecutan instr atómicas de forma que (en principio) el entremezclado en el tiempo de las instr es totalmente arbitrario. Esto NO QUIERE DECIR QUE SEAN INDEPENDIENTES ENTRE SÍ, ya que hay combinaciones no válidas.

Existe una condición de sincronización cuando existe una restricción sobre el orden en el que se pueden mezclar las instrucciones de distintos procesos

Un caso particular es la exclusión mutua, secuencias finitas de instr que deben ejecutarse de principio a fin por un único proceso, sin que otro proceso las ejecute a la vez

### Exclusión mutua

	- Sección crítica: conjunto de secuencias de instr consecutivas que definen un proceso
 - Exclusión mutua: sucede cuando los procesos funcionan correctamente si en un momento dado solo hay como maximo un proceso ejecutando la sección crítica
   	- El solapamiento de las instr debe ser tal que la sección crítica se ejecuta de principio a fin sin que ningún otro proceso la ejecute 

### Condición de sincronización

> No son correctas todas las posibles interfoliaciones de las secuencias de instr atómicas de los procesos

Se suele aplicar cuando en un punto del proceso se debe esperar a que se cumpla una condición global. La condición de sincronización se ve aplicada en el productor-consumidor:

 - Un proceso prod-cons solo funcionará correctamente cuando el orden en el que se entremezclan las sentencias Lectura y Escritura es correcto
    - **E, L, E, L...** es correcto, el consumidor no lee hasta que el productor no genera un nuevo valor de x; y el productor no escribe un nuevo valor hasta que el anterior se haya leído
      	- L, E, L, E... es incorrecto, se lee un valor vacío
      	- E, L, E, E, L... es incorrecto, no se lee un valor
      	- E, L, E, L, L... es incorrecto, un valor se lee dos veces

## Propiedades de los sist concurrentes

Propiedad: atributo del programa que es cierto para todas las secuencias de interfoliación

### Propiedades de seguridad *(safety)*

> Condiciones que deben cumplirse a cada instante, del tipo "Nunca pasará nada malo"

Se requieren en las especificaciones estáticas del programa, y son facilmente demostrables. Suelen restringir las interfoliaciones

- Exclusión mutua: 2 procesos nunca entrelazan ciertas secuencias de operaciones
- Ausencia de interbloqueo *(deadlock freedom)*: nunca ocurrirá que los procesos se encuentren esperando algo que no sucederá
- Propiedad de seguridad en Productor-Consumidor: el consumidor consumirá todos los datos en orden que genere el productor 

### Propiedades de vivacidad *(liveness)*

> Condiciones que deben cumplirse eventualmente, del tipo "Realmente sucede algo bueno"

Son propiedades dinámicas, más difíciles de probar

- Ausencia de inanición *(starvation freedom)*: un proceso no puede ser pospuesto indefinidamente, en algún momento avanzará

 - Equidad *(fairness)* : un proceso que desee progresar debe hacerlo con relativa justicia respecto a los demás
   	- Este aspecto está más ligado a la implementación y no siempre se cumple correctamente

## Verficación de sist concurrentes

¿Cómo demostrar que un programa cumple una determinada propiedad?

> **Posibilidad**: realizar varias ejecuciones del programa y comprobar que se verfique la propiedad
>
> *Problema*: número limitado de interfoliaciones, no demuestra la existencia de casos indeseables

#### Enfoque operacional *(análisis exhaustivo)*

> Se comprueba la corrección de todas las posibles historias
>
> *Problema*: utilidad limitada, no es aplicable a concurrencias complejas ya que las interfoliaciones crecen de manera exponencial



#### Enfoque axiomático

Se define un sistema lógico formal que permite establecer propiedades del programa en base a unos axiomas y reglas de inferencia. Se usan fórmulas lógicas y las sentencias atómicas se usan como predicados

 - Menor complejidad: el trabajo que conlleva la prueba de correción es proporcional al número de sentencias

 - Invariante global: un predicado qye referencia a variables globales, y será cierto para cualquier asignación de los procesos

   > ej. Productor-Consumidor invariante global:
   >
   > ​	consumidos <= producidos <= consumidos+1

