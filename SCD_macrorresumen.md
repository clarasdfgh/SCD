# SCD Tema 1: Introducción

> 1. Conceptos básicos y abstracción
> 2. Modelo abstracto y consideraciones sobre el hardware
> 3. Exclusión mutua y sincronización
> 4. Propiedades de los sistemas concurrentes
> 5. Verificación de programas concurrentes

### 1. Conceptos básicos y abstracción

#### CONCEPTOS BÁSICOS

- **Programa secuencial:** declaraciones de datos + conjunto de instrucciones sobre dichos datos que deben ejecutarse en secuencia

- **Programa concurrente:** conjunto de programas secuenciales que se pueden ejecutar lógicamente en paralelo

- **Proceso:** ejecución de un programa secuencial

- **Concurrencia:** describe el potencial para ejecución paralela, es decir, el solapamiento real o virtual de varias actividades en el tiempo

- **Programación concurrente:** conjunto de notaciones y técnicas usadas para expresar paralelismo potencial y resolver problemas de sincronización y comunicación

  Esta es independiente de la implementación del paralelismo, es una *abstracción*.

- **Programación paralela:** su principal objetivo es acelerar la resolución de problemas concretos mediante el aprovechamiento de la capacidad de procesamiento paralelo del hardware disponible

- **Programación distribuida:** su principal objetivo es hacer que varios componentes software localizados en *distintos ordenadores* trabajen juntos

- **Programación en tiempo real:** se centra en la programación de sistemas que están funcionando continuamente, recibiendo E/S desde componentes hardware (*sistemas reactivos*), en los que se trabaja con restricciones muy estrictas de respuesta temporal (*sistemas de tiempo real*).

#### MOTIVACIÓN

Aún siendo más compleja que la programación secuencial, la programación concurrente es necesaria por sus mejoras en **eficiencia** y **calidad**.

##### Mejora en eficiencia:

La PC nos permite aprovechar al máximo los recursos hardware disponibles:

En **sistemas monoprocesador** cuando una tarea entra en espera de E/S se cede el control a la siguiente, evitando la espera ociosa del procesador. También permite que varios usuarios usen el sistema de forma interactiva, siendo el precursor de los actuales SO multiusuario.

En **sistemas multiprocesador** se reparten las tareas entre procesadores, reduciendo el tiempo de ejecución. Es fundamental para acelerar cómputos numéricos complejos.

##### Mejora en calidad:

Muchos programas se entienden mejor en términos de varios procesos secuenciales ejecutándose concurrentemente: por ejemplo, un servidor web de reserva de vuelos. Es más natural considerar cada petición de usuario como un proceso e implementar políticas que impidan conflictos entre ellos (cómo no permitir que se supere un límite de reservas de vuelo).

### 2. Modelo abstracto y consideraciones sobre el hardware

### CONSIDERACIONES HARDWARE

#### Modelos de arquitectura para programación concurrente

Dependen fuertemente de la arquitectura. Se considera una máquina virtual que representa un sistema (multiprocesador o distribuido), proporcionando una base homogénea para ejecución de procesos concurrentes. 

Este tipo de paralelismo debe afectar a la eficiencia, y no a la corrección.

##### Concurrencia en sistemas monoprocesador (Paralelismo virtual):

![image-20210110111310714](/home/clara/.config/Typora/typora-user-images/image-20210110111310714.png)

- **Multiprogramación:** el SO gestiona cómo múltiples procesos se reparten los ciclos de CPU.
  - Mejor aprovechamiento de la CPU
  - Servicio interactivo a varios usuarios
  - Permite usar soluciones de diseño concurrentes
  - Sincronización y comunicación mediante variables compartidas

##### Concurrencia multiprocesadores de memoria compartida (Paralelismo real/híbrido):

![image-20210110111727727](/home/clara/.config/Typora/typora-user-images/image-20210110111727727.png)

![image-20210110111858289](/home/clara/.config/Typora/typora-user-images/image-20210110111858289.png)

- Los procesadores pueden compartir o no *físicamente* la misma memoria, pero comparten un espacio de direcciones

  - La interacción entre procesos se puede implementar mediante variables alojadas en direcciones del espacio compartido

    Un ejemplo son los ordenadores con procesadores multicore

##### Concurrencia en sistemas distribuidos (Paralelismo real/híbrido):

![image-20210110112252154](/home/clara/.config/Typora/typora-user-images/image-20210110112252154.png)

![image-20210110112306369](/home/clara/.config/Typora/typora-user-images/image-20210110112306369.png)

- No existe la memoria común: cada procesador cuenta con su propio espacio de direcciones. Los procesadores interactuan transfiriéndose datos a través de redes de interconexión (paso de mensajes)

- **Programación distribuida:** además de la concurrencia, se toma en cuenta el tratamiento de fallos, transparencia, heterogeneidad...

  Ejemplo: clústers de ordenadores, internet o intranet

### MODELO ABSTRACTO DE CONCURRENCIA

##### Secuencias atómicas y no atómicas:

> **Sentencia atómica (indivisible):** 
>
> Una sentencia o instrucción de un proceso en un programa concurrente es atómica si siempre se ejecuta de principio a fin sin verse afectada (durante su ejecución) por otras sentencias en ejecución de otros procesos del programa.

- No se verán afectadas cuando el funcionamiento de dicha instrucción **no dependa nunca** de cómo se estén ejecutando otras instrucciones.

- El funcionamiento de una instrucción se define por su efecto en el *estado de ejecución* del programa cuando acaba. El estado de ejecución está formado por los valores de las variables y registros de todos los procesos

- Algunas instrucciones atómicas podrían ser:

  - Leer una celda de memoria (variable **x**) y cargar su valor en ese momento en un registro (**r**) del procesador
  - Incrementar el valor de un registro **r** (y otras operaciones aritméticas)
  - Escribir el valor de un registro **r** en una celda de memoria

  El resultado de estas instrucciones **nunca** dependerá de otras instrucciones que se estén ejecutando concurrentemente. Al finalizar, la celda de memoria o registro sobre el que se trabaje tomará un valor concreto predecible siempre a partir del estado de inicio (**estado determinado**)

- La mayoría de las sentencias en lenguajes de alto nivel **no son atómicas:** algo tan simple como un incremento `x := x+1` se compondría de:

  - Leer el valor **x** y cargarlo en un registro **r**
  - Incrementar en 1 el valor almacenado en **r**
  - Escribir el valor de **r** en la variable **x**

  El resultado almacenado en **x** depende de que haya o no otras secuencias concurrentes que escriban sobre **x**: existe un cierto **grado de indeterminación**, no se puede predecir el estado final a partir del inicial.

  ###### Interfoliación de secuencias atómicas

  Si tenemos dos procesos secuenciales, A y B, compuestos por las sentencias atómicas A1, A2, A3, B1, B2, B3; la interfoliación es cualquiera de las mezclas posibles de las sentencias atómicas de A y B.

  | Proceso A                  | A1, A2, A3             |
  | -------------------------- | ---------------------- |
  | **Proceso B**              | **B1, B2, B3**         |
  | Programa concurrente C (1) | A1, A2, A3, B1, B2, B3 |
  | Programa concurrente C (2) | B1, B2, B3, A1, A2, A3 |
  | Programa concurrente C (3) | A1, B1, A2, B2, A3, B3 |

  *etcétera...*

  La interfoliación es secuencial: respeta el orden (A2 no puede ir antes que A1)

  Las sentencias atómicas se ordenan en función del instante en el que acaban (que es cuando tienen efecto)

  > **Cálculo de posibles interfoliaciones:**
  >
  > - El Proceso P1 ejecuta n1 instrucciones atómicas
  > - El Proceso P2 ejecuta n2 instrucciones atómicas
  >
  > Existen tantas posibles interfoliaciones de P1 y P2 como posibles subconjuntos de n1 elementos del conjunto {1, 2, 3... n1+n2}. El número de interfoliaciones sería este coeficiente binomial:
  >
  > ![image-20210110115808336](/home/clara/.config/Typora/typora-user-images/image-20210110115808336.png)
  >
  > O, con k procesos:
  >
  > ![image-20210110115825179](/home/clara/.config/Typora/typora-user-images/image-20210110115825179.png)
  >
  > Por ejemplo, para los procesos A y B de la tabla anterior existirían 20 posibles interfoliaciones

##### Abstracción:

El modelo basado en el estudio de todas las posibles secuencias de ejecución entrelazadas de los procesos constituye una abstracción. Se consideran únicamente las características relevantes que determinan el resultado del cálculo; y permite simplificar el análisis y diseño de programas concurrentes.

Los detalles no relevantes se ignoran:

- Áreas de memoria asignadas a los procesos

- Los registros particulares que se están usando

- El costo de los cambios de contexto entre procesos

- La política del SO relativa a asignación de CPU

- Las diferencias entre entornos multiprocesador o monoprocesador

  *Remember: el tipo de paralelismo afecta a la eficiencia pero no a la corrección*

##### Independencia del entorno de ejecución:

> **El entrelazamiento preserva la consistencia:** 
>
> El resultado de una instrucción individual sobre un dato no depende de las circunstancias de la ejecución.

Supongamos un programa P que se compone de dos instrucciones atómicas I0, I1 que se ejecutan concurrentemente. Entonces:

- Si I0 e I1 no acceden a la misma celda de memoria o registro, el orden de ejecución no afecta al resultado final. Es determinado.
- Si I0 e I1 si acceden a la misma celda, entonces la única suposición razonable es que el valor final de ésta sea el que meta I0, o I1, dependiendo de cuál accediera primero. Jamás este resultado será uno distinto al que corresponda a estas dos instrucciones atómicas. Es determinado.

En caso contrario sería imposible razonar acerca de la corrección de programas concurrentes.

##### La propiedad de Consistencia Secuencial Estricta:

> **Consistencia secuencial estricta:**
>
> Una instrucción atómica de lectura de una variable siempre leerá el valor escrito en dicha variable por la última instrucción atómica previa de escritura en esa variable.

De ahora en adelante supondremos que esta propiedad se cumple siempre:

- Implica que un valor escrito en de forma atómica es **inmediatamente** legible o visible para todos los procesos sin retardo alguno
- Esto hace más sencillo el análisis de comportamiento de las interfoliaciones de R/W en variables compartidas

##### Velocidad de ejecución, hipótesis del progreso finito:

> **Progreso Finito:**
>
> No se puede hacer ninguna suposición acerca de las velocidades absolutas/relativas de ejecución de los procesos, salvo que es mayor que cero.

Un programa concurrente se entiende en base a sus componentes (procesos) y sus interacciones, sin tomar en cuenta el entorno de ejecución. Si hiciéramos susposiciones temporales (por ejemplo, *un disco suele ser más lento que una CPU*) sería más difícil detectar y corregir fallos; y la corrección dependería de la configuración de ejecución, que puede cambiar.

Si se cumple la hipótesis de progreso finito, la velocidad de ejecución de cada proceso será no nula, y esto implica: 

> > **Punto de vista global:** 
> >
> > Durante la ejecución de un programa concurrente, en cualquier momento existirá al menos 1 proceso preparado, es decir, even- tualmente se permitirá la ejecución de algún proceso.
>
> > **Punto de vista local:** 
> >
> > Cuando un proceso concreto de un programa concurrente comienza la ejecución de una sentencia, completará la ejecución de la sentencia en un intervalo de tiempo finito.

##### Estados e historias de ejecución de un programa concurrente:

> **Estado de un programa concurrente:** 
>
> Valores de las variables del programa en un momento dado. Incluyen variables declaradas explícitamente y variables con información de estado oculta (contador del programa, registros, ...)

Un programa concurrente comienza su ejecución en un estado inicial y los procesos van modificando el estado conforme se van ejecutando sus sentencias atómicas (producen transiciones entre dos estados de forma indivisible).

> **Historia o traza de un programa concurrente:**
>
>  Secuencia de estados s0 → s1 → . . . → sn , producida por una secuencia concreta de interfoliación.

#### NOTACIÓN PARA EXPRESAR EJECUCIÓN CONCURRENTE

Distinguimos dos tipos de sistemas concurrentes, en función de la posibilidades para especificar cuales son sus procesos:

- Sistemas Estáticos:
  - Número de procesos fijado en el fuente del programa
  - Los procesos se activan al lanzar el programa
  - Ejemplo: Message Passing Interface (MPI-1)
- Sistemas Dinámicos:
  - Número variable de procesos/hebras que se pueden activar en cualquier momento de la ejecución.
  - Ejemplos: OpenMP, PThreads, Java Threads, MPI-2

##### Grafo de sincronización:

![image-20210110122656244](/home/clara/.config/Typora/typora-user-images/image-20210110122656244.png)

El Grafo de Sincronización es un Grafo Dirigido Acíclico (DAG) donde cada nodo representa una secuencia de sentencias del programa.

- Dadas dos actividades, A y B, una flecha desde A hacia B significa que B no puede comenzar su ejecución hasta que A haya finalizado
- Muestra las restricciones de precedencia que determinan cuándo una actividad puede empezar en un programa

##### Definición estática de procesos:

![image-20210110123140365](/home/clara/.config/Typora/typora-user-images/image-20210110123140365.png)

El número de procesos (arbitrario) y el código que ejecutan no cambian entre ejecuciones. Cada proceso se asocia con su identificador y su código mediante la palabra clave process.

El programa acaba cuando acaban todos los procesos. Las vars. compartidas se inicializan antes de que comienzen los procesos.

##### Definición estática de vectores de procesos:

![image-20210110123132853](/home/clara/.config/Typora/typora-user-images/image-20210110123132853.png)

Se pueden usar definiciones estáticas de grupos de procesos similares que solo se diferencia en el valor de una constante (vectores de procesos).

- En cada caso, a y b se traducen por dos constantes concretas (el valor de a será típicamente 0 o 1)
- El número total de procesos será `b−a + 1` (se supone que a <= b)

##### Creación de procesos no estructurada con fork-join:

![image-20210110123043416](/home/clara/.config/Typora/typora-user-images/image-20210110123043416.png)

- **fork:** sentencia que especifica que la rutina nombrada puede comenzar su ejecución, al mismo tiempo que comienza la sentencia siguiente (bifurcación)
- **join:** sentencia que espera la terminación de la rutina nombrada, antes de comenzar la sentencia siguiente (unión)

**Ventajas:** práctica y potente, creación dinámica.

**Inconvenientes:** no estructuración, difícil comprensión de los programas.

##### Creación de procesos no estructurada con cobegin-coend:

![image-20210110123031677](/home/clara/.config/Typora/typora-user-images/image-20210110123031677.png)

Las sentencias en un bloque delimitado por cobegin-coend comienzan su ejecución todas ellas a la vez: en el coend se espera a que se terminen todas las sentencias.

Hace explícito qué rutinas van a ejecutarse concurrentemente.

**Ventajas:** impone estructura 1 entrada, 1 salida ⇒ más fácil de entender. 

**Inconvenientes:** menor potencia expresiva que fork-join.

### 3. Exclusión mutua y sincronización

Según el modelo abstracto, los procesos concurrentes ejecutan instrucciones atómicas de forma que (en principio) el entremezclado en el tiempo de las instrucciones es totalmente arbitrario. Esto NO QUIERE DECIR QUE SEAN INDEPENDIENTES ENTRE SÍ, ya que hay combinaciones no válidas.

- Se dice que hay una **condición de sincronización** cuando esto ocurre, es decir, que hay alguna restricción sobre el orden en el que se pueden mezclar las instrucciones de distintos procesos
- Un caso particular es la **exclusión mutua**, son secuencias finitas de instrucciones que deben ejecutarse de principio a fin por un único proceso, sin que a la vez otro proceso las esté ejecutando tambien

#### EXCLUSIÓN MUTUA

La restricción se refiere a una o varias secuencias de instrucciones consecutivas que aparecen en el texto de uno o varios procesos.

- Al conjunto de dichas secuencias de instrucciones se le denomina **sección crítica** (SC)
- Ocurre **exclusión mutua** (EM) cuando los procesos solo funcionan correctamente si, en cada instante de tiempo, hay como mucho uno de ellos ejecutando cualquier instrucción de la sección crítica

Es decir, el solapamiento de las instrucciones debe ser tal que cada secuencia de instrucciones de la SC se ejecuta como mucho por un proceso de principio a fin, sin que (durante ese tiempo) otros procesos ejecuten ninguna de esas instrucciones ni otras de la misma SC.

El ejemplo típico de EM ocurre en procesos con memoria compartida que acceden para leer y modificar variables o estructuras de datos comunes usando operaciones no atómicas (es decir, compuestas de varias instrucciones máquina o elementales que pueden solaparse con otras secuencias). Veamos un ejemplo sencillo que usa una variable entera **x** en memoria compartida y operaciones aritméticas elementales:

- La SC está formada por todas las secuencias de instrucciones máquina implicadas, por ejemplo, en `x:=x+1` 
- Veamos cómo, si se dan interfoliaciones que no respetan la EM de la SC, el resultado será indeterminado
  - Aquí, el término indeterminado indica que para cada valor inicial de x, existe un conjunto de posibles valores de x al finalizar. El valor concreto que toma x depende la interfolicación concreta que ocurre.

Las instrucciones implicadas en `x:=x+1` serían:

1. `load r <- x` : cargar el valor de la variable **x** en un registro **r** de la CPU
2. `add r,1` : incrementar en una unidad el valor del registro **r**
3. `store r -> x` : guardar el valor del registro **r** en la posición de memoria de la variable **x**

Suponemos que inicialmente x vale 0 y ambos procesos ejecutan la asignación, puede haber varias secuencias de interfoliación, aquí vemos dos:

![image-20210110130219266](/home/clara/.config/Typora/typora-user-images/image-20210110130219266.png)

Partiendo de x=0, al final **x** puede valer 1 o 2, dependiendo de la interfoliación.

#### CONDICIÓN DE SINCRONIZACIÓN

> No son correctas todas las posibles interfoliaciones de las secuencias de instrucciones atómicas de los procesos

Se suele aplicar cuando en un punto del proceso se debe esperar a que se cumpla una condición global. La condición de sincronización se ve aplicada en el productor-consumidor:

Un proceso prod-cons solo funcionará correctamente cuando el orden en el que se entremezclan las sentencias Lectura y Escritura es correcto:

- **E, L, E, L...** es correcto, el consumidor no lee hasta que el productor no genera un nuevo valor de x; y el productor no escribe un nuevo valor hasta que el anterior se haya leído
   - L, E, L, E...     es incorrecto, se lee un valor vacío
   - E, L, E, E, L... es incorrecto, no se lee un valor
   - E, L, E, L, L... es incorrecto, un valor se lee dos veces

### 4. Propiedades de los sistemas concurrentes

**Propiedad:** atributo del programa que es cierto para todas las secuencias de interfoliación

#### PROPIEDADES DE SEGURIDAD *(safety)*

> Condiciones que deben cumplirse a cada instante, del tipo "Nunca pasará nada malo"

Se requieren en las especificaciones estáticas del programa, y son facilmente demostrables. Suelen restringir las interfoliaciones. Ejemplos:

- **Exclusión mutua:** 2 procesos nunca entrelazan ciertas secuencias de operaciones
- **Ausencia de interbloqueo *(deadlock freedom)*:** nunca ocurrirá que los procesos se encuentren esperando algo que no sucederá
- **Propiedad de seguridad en Productor-Consumidor:** el consumidor consumirá todos los datos en orden que genere el productor 

#### PROPIEDADES DE VIVACIDAD *(liveness)*

> Condiciones que deben cumplirse eventualmente, del tipo "Realmente sucederá algo bueno"

Son propiedades dinámicas, más difíciles de probar

- **Ausencia de inanición *(starvation freedom)*:** un proceso no puede ser pospuesto indefinidamente, en algún momento avanzará

 - **Equidad *(fairness)* :** un proceso que desee progresar debe hacerlo con relativa justicia respecto a los demás
   - Este aspecto está más ligado a la implementación y no siempre se cumple correctamente, existen grados

### 5. Verificación de programas concurrentes

¿Cómo demostrar que un programa cumple una determinada propiedad?

> **Posibilidad**: realizar varias ejecuciones del programa y comprobar que se verfique la propiedad
>
> *Problema*: número limitado de interfoliaciones, no demuestra la existencia de casos indeseables

#### ENFOQUE OPERACIONAL *(análisis exhaustivo)*

Se comprueba la corrección de todas las posibles historias

> *Problema*: utilidad limitada, no es aplicable a concurrencias complejas ya que las interfoliaciones crecen de manera exponencial

#### ENFOQUE AXIOMÁTICO

Se define un sistema lógico formal que permite establecer propiedades del programa en base a unos axiomas y reglas de inferencia. Se usan fórmulas lógicas y las sentencias atómicas se usan como predicados

 - Menor complejidad: el trabajo que conlleva la prueba de correción es proporcional al número de sentencias

 - Invariante global: un predicado que referencia a variables globales, y será cierto para cualquier asignación de los procesos

   > ej. Productor-Consumidor invariante global:
   >
   > ​	consumidos <= producidos <= consumidos+1



## 

# SCD Tema 2: Sincronización en memoria compartida

> 1. Introducción a la sincronización en memoria compartida
> 2. Soluciones software con espera ocupada para EM
> 3. Soluciones hardware con espera ocupada (cerrojos) para EM
> 4. Semáforos para sincronización 
> 5. Monitores como mecanismo de alto nivel

### 1. Introducción a la sincronización en memoria compartida

En este tema estudiaremos soluciones de exclusión mutua y sincronización basadas en el uso de memoria compartida entre los procesos involucrados. Estas soluciones se dividen dos categorías:

-  Soluciones de bajo nivel con espera ocupada, con bucles para realizar las esperas
-  Soluciones de alto nivel, que parten de las anteriores. La sincronización se consigue bloqueando el proceso que tenga que esperar
   -  Semáforos
   -  Regiones críticas
   -  Monitores

Cuanto un proceso tiene que esperar, entra en un bucle indefinido del que no sale hasta que se cumplan las condiciones necesarias. A esto se le denomina **espera ocupada**. Existen soluciones software y hardware, además de alto nivel:

- **Software**: se usan operaciones estándar sencillas de lectura y escritura de datos simples (típicamente valores lógicos o enteros) en la memoria compartida
- **Hardware**, cerrojos: basadas en la existencia de instrucciones máquina específicas dentro del repertorio de instrucciones de los procesadores involucrados
- **Alto nivel**: suelen dar errores porque producen algoritmos innecesariamente complicados que afectan a la eficiencia de uso de la CPU (por los bucles).  Ofrecen interfaces de acceso a las estructuras de datos y se usa **bloqueo de procesos** en vez de espera ocupada.
  - *Semáforos:* se construyen directamente sobre las soluciones de bajo nivel, usando servicios del SO para bloqueo y reactivación de procesos
  - *Regiones críticas condicionales:* se pueden implementar sobre los semáforos
  - *Monitores:* de más alto nivel, se implementan en lenguajes OOP

### 2. Soluciones software con espera ocupada para EM

En esta sección veremos diversas soluciones para lograr exclusión mutua en una sección crítica **usando variables compartidas entre los procesos** o hebras involucrados. Estos algoritmos usan dichas variables para hacer espera ocupada cuando sea necesario en el protocolo de entrada.  

Los algoritmos que resuelven este problema no son triviales, y menos para más de dos procesos. En la actualidad se conocen distintas soluciones con distintas propiedades. Veremos estos dos algoritmos:

- Algoritmo de Dekker, 2 procesos
- Algoritmo de Peterson, para 2 o más procesos

Previamente a esos algoritmos, veremos la estructura de los procesos con secciónes críticas y las propiedades que deben cumplir los algoritmos.

#### ESTRUCTURA DE LOS PROCESOS CON SC

Para analizar las soluciones a EM asumimos que un proceso que incluya un bloque considerado como sección crítica (**SC**) tendrá dicho bloque estructurado en tres etapas:

1. **Protocolo de entrada, PE:** una serie de instrucciones que incluyen posiblemente espera, en los casos en los que no se pueda conceder acceso a la sección crítica
2. **Sección crítica, SC:** instrucciones que solo pueden ser ejecutadas por un proceso como mucho
3. **Protocolo de salida, PS:** instrucciones que permiten que otros procesos puedan conocer que este proceso ha terminado la sección crítica

Todas las sentencias que no forman parte de ninguna de estas tres etapas se denominan Resto de Sentencias (**RS**) .

##### Acceso repetitivo a las secciones críticas

En general, un proceso puede contener más de una sección crítica, y cada sección crítica puede estar desglosada en varios bloques de código separados en el texto del proceso. Para simplificar el análisis, suponemos, sin pérdida de generalidad, que: 

- Cada proceso tiene una única sección crítica (SC). 
- La SC es un único bloque contiguo de instrucciones. 
- Se ejecuta un bucle infinito, con dos pasos: 
  - **Sección crítica** (con el PE antes y el PS después) 
  - **Resto de sentencias:** se emplea un tiempo arbitrario no acotado, e incluso el proceso puede finalizar en esta sección, de forma prevista o imprevista. 

Es el caso más general: no se supone nada acerca de cuantas veces un proceso puede intentar entrar en una SC.

##### Diagrama de estado de un proceso

![image-20210111173206871](/home/clara/.config/Typora/typora-user-images/image-20210111173206871.png)

El proceso únicamente puede terminar cuando se encuentra en RS

El código del PE introduce esperas (mediante bucles) para aseguar exclusión mútua en la SC.

Para que se puedan implementar soluciones correctas al problema de EM, es necesario suponer que **los procesos siempre terminan una sección crítica y emplean un intervalo de tiempo finito desde que la comienzan hasta que la terminan.** Es decir, *durante el tiempo en que un proceso se encuentra en una sección crítica nunca*:

- Finaliza o aborta
  - Es finalizado o abortado externamente
- Entra en un bucle infinito
- Es bloqueado o suspendido indefinidamente de forma externa

Es deseable que el tiempo empleado en SC sea lo menos posible.

#### PROPIEDADES PARA EM

Para que un algoritmo para EM sea correcto, se deben cumplir cada una de estas tres **propiedades mínimas**: 

1. Exclusión mutua 
2. Progreso 
3. Espera limitada 

Además, hay **propiedades deseables** adicionales que también deberían cumplirse, si bien consideramos correctos algoritmos que no las cumplen : 

- Eficiencia 
- Equidad 

##### Exclusión mutua

> En cada instante de tiempo, y para cada sección crítica existente, habrá como mucho un proceso ejecutando alguna sentencia de dicha región crítica

Es la propiedad fundamental para el problema de la sección crítica.

En esta sección veremos soluciones de memoria compartida que permiten un único proceso en una sección crítica. 

Si bien esta es la propiedad fundamental, no puede conseguirse de cualquier forma, y para ello se establecen las otras dos condiciones mínimas que vemos a continuación.

##### Progreso

Consideremos una SC en un instante en el cual no hay ningún proceso ejecutándola, pero sí hay procesos en el PE compitiendo por entrar a la SC. La propiedad de progreso establece:

> **Un algoritmo de EM debe estar diseñado de forma tal que:** 
>
> 1. Después de un intervalo de tiempo finito desde que ingresó el primer proceso al PE, uno de los procesos en el mismo podrá acceder a la SC. 
> 2. La selección del proceso anterior es completamente independiente del comportamiento de los procesos que durante todo ese intervalo no han estado en SC ni han intentado acceder.

Cuando la condición 1 no se da, se dice que ocurre un **interbloqueo**, ya que todos los procesos en el PE quedan en espera ocupada indefinidamente sin que ninguno pueda avanzar.

##### Espera limitada

Supongamos que un proceso emplea un intervalo de tiempo en el PE intentando acceder a una SC. Durante ese intervalo de tiempo, cualquier otro proceso activo **puede entrar un número arbitrario de veces n a ese mismo PE** y lograr acceso a la SC (incluyendo la posibilidad de que n = 0). La propiedad de espera limitada establece que: 

> Un algoritmo de exclusión mutua debe estar diseñado de forma que **n** nunca será superior a un valor máximo determinado. 

Esto implica que las esperas en el PE siempre serán finitas (suponiendo que los procesos emplean un tiempo finito en la SC).

##### Propiedades deseables: eficiencia y equidad

- Eficiencia: los protocolos de entrada y salida deben emplear poco tiempo de procesamiento (excluyendo las esperas ocupadas del PE), y las variables compartidas deben usar poca cantidad de memoria.
- Equidad: en los casos en que haya varios procesos compitiendo por acceder a una SC (de forma repetida en el tiempo), no debería existir la posibilidad de que sistemáticamente se perjudique a algunos y se beneficie a otros.

#### REFINAMIENTO SUCESIVO DE DIJKSTRA

#### ALGORITMO DE DEKKER

#### ALGORITMO DE PETERSON

### 3. Soluciones hardware con espera ocupada (cerrojos) para EM



### 4. Semáforos para sincronización 

Son un mecanismo que soluciona o aminora los problemas de las soluciones a bajo nivel.

- No usa espera ocupada, sino bloqueo de procesos (más eficiente)
- Útil para exclusión mutua en problemas sencillos
- Se puede utilizar en sincronización, pero puede resultar en esquemas de uso complejos
- Se implementan mediante instancias de una estructura de datos a la que se accede por subprogramas específicos por seguridad

##### Bloqueo y desbloqueo de procesos

Los semáforos exigen que los procesos en espera no ocupen CPU, lo cual implica:

- Un proceso en ejecución tiene que poder solicitar quedar bloqueado
- Un proceso bloqueado no puede ejecutar instr en CPU
- Un proceso de ejecución debe poder solicitar que se desbloquee (reanude) otro proceso bloqueado
- Deben poder existir simultáneamente varios procesos bloqueados
- Cada petición de bloqueo o desbloqueo debe referirse a alguuno de estos conjuntos

#### ESTRUCTURA Y OPERACIONES CON SEMÁFOROS

Un semáforo es una instancia de una estr de datos (registro) que contiene un conjunto de procesos bloqueados (esperando al semáforo); y un valor natural entero no negativo (valor del semáforo)

Estas estructuras residen en memoria compartida; y se deben inicializar al principio de cada programa de forma que:

- El conjunto de procesos asociados esté vacío
- Se indica el valor inicial del semáforo

##### Operaciones sobre semáforos

A parte de la inicialización, los semáforos cuentan con dos operaciones básicas que se pueden realizar sobre una variable u objeto tipo semáforo:

- sem_wait(s):
  - Si el valor de s es 0, bloqueamos el proceso, que será reanudado cuando su valor sea 1
  - Decrementar el semáforo en una unidad
- sem_signal(s):
  - Incrementar el valor de s en una unidad
  - Si hay procesos esperando en s, reanudar o despertar a uno de ellos (y pone su valor a 0 al salir)

Este diseño implica que el valor del semáforo nunca será negativo, ya que antes de decrementar se espera a que el valor sea 1. Los únicos procesos que esperan son los que tienen valor 0.

##### Invariante de un semáforo

Dado un semáforo s, en un instante del tiempo cualquiera t su valor Vt será el valor inicial V0 más el número de llaadas a sem_signal completadas ns, menos el número de llamadas a sem_wait completadas nw. Este valor nunca es negativo.

`Vt = V0 + ns - nw >= 0`

##### Implementación de sem_wait, sem_signal

```pseudocode
procedure sem_wait( s :semaphore);
begin
	if s.valor == 0 then	
		bloquearme( s.procesos ) ;
	s.valor := s.valor - 1 ;
end 
```

```pseudocode
procedure sem_signal( s :semaphore);
begin
	s.valor := s.valor + 1;	
	if not vacio( s.procesos ) then
		desbloquear_uno( s.procesos ) ;
end 
```

Estas operaciones se ejecutan en EM, no puede haber dos procesos distintos ejecutando instrucciones en un mismo semáforo

##### Limitaciones de los semáforos

Son muy útiles para problemas de exclusión mutua y sincronización sencilla, pero los problemas más complejos de sincronización son más difíciles de verificar; y un programa erróneo o malintencionado puede provocar estados de bloqueo indefinido o incorrectos.

![image-20210111184225786](/home/clara/.config/Typora/typora-user-images/image-20210111184225786.png)

#### USO DE SEMÁFOROS: PATRONES SENCILLOS

### 5. Monitores como mecanismo de alto nivel

#### FUNCIONAMIENTO DE LOS MONITORES

#### SINCRONIZACIÓN EN MONITORES

#### VERIFICACIÓN DE MONITORES

#### PATRONES DE SOLUCIÓN CON MONITORES

#### EL PROBLEMA DE LOS LECTORES-ESCRITORES

#### SEMÁNTICA DE LAS SEÑALES DE LOS MONITORES

#### COLAS DE PRIORIDAD

#### IMPLEMENTACIÓN DE MONITORES



## 

# SCD Tema 3: Sistemas basados en paso de mensajes

> 1. Mecanismos básicos en sistemas basados en paso de mensajes 
> 2. Paradigmas de interacción de procesos en programas distribuidos 
> 3. Mecanismos de alto nivel en sistemas distribuidos

### 1. Mecanismos básicos en sistemas basados en paso de mensajes 

En el capítulo anterior vimos sistemas multiprocesador con memoria compartida. Estos sistemas son más fáciles de programar gracias a las variables compartidas y mecanismos como cerrojos, semáforos y monitores. Sin embargo, la implementación es más costosa y su escalabilidad se ve limitada por el hardware: el acceso a memoria común es un cuello de botella.

Los sistemas distribuidos son un conjunto de procesos que se transmiten datos a través de una red. Esto facilita la distribución de datos y recursos y soluciona el problema de la escalabilidad y de coste. Sin embargo, es más difícil de programar, y los mecanismos vistos anteriormente no aplican.

##### Necesidad de una notación de programación distribuida

Se usan tres notaciones:

- Lenguajes tradicionales (de memoria compartida):
  - Operadores de asignación (cambio del estado interno de la máquina)
  - Estructuración (como secuencia, repetición, alternación...) 
- Envío y recepción (entorno externo):
  - Tan importantes como la asignación
  - Permiten comunicar procesos que se ejecutan en paralelo
- Paso de mensajes: 
  - Abstracción (oculta hardware con una red de interconexión)
  - Portabilidad (se puede implementar eficientemente en cualquier arquitectura)
  - No requiere de mecanismos para asegurar la exclusión mutua

#### VISTA LÓGICA, ARQUITECTURA Y EJECUCIÓN

![image-20210110182508295](/home/clara/.config/Typora/typora-user-images/image-20210110182508295.png)

Existen **n** procesos, cada uno con su propio espacio de direcciones. Los procesos se comunican mediante paso de mensajes.

- En un mismo procesador pueden residir múltiples procesos
  - Por motivos de eficiencia, se suele adoptar la política de alojar un único proceso en cada procesador disponible
- Cada interacción **requiere de la cooperación de los 2 procesos:** el propietario de los datos (*emisor*) debe intervenir aunque no haya conexión lógica con el evento tratado en el *receptor*

##### Estructura de un programa de paso de mensajes

Diseñar un código diferente para cada proceso puede ser complejo. Una solución es el estilo **SPMD (Single Program Multiple Data)**: todos los procesos ejecutan un mismo código fuente, pero cada uno procesa datos distintos o ejecutar flujos de control distintos.

![image-20200121191628132](/home/clara/.config/Typora/typora-user-images/image-20200121191628132.png)

```pseudocode
process Proceso[ n_proc : 0..1 ]; 
	var dato : integer ; 
	
begin 
	if n_proc == 0 then begin {si soy 0} 
		dato := Produce(); 
		send( dato, Proceso[1]); 
	end else begin {si soy 1} 
		receive( dato, Proceso[0] ); 
		Consume( dato ); 
	end 
end
```

Otra opción es el estilo **MPMD (Multiple Program Multiple Data)**: cada proceso ejecuta el mismo o distintos programas de un conjunto de ejecutables, y cada proceso puede usar datos diferentes

![image-20200121192433107](/home/clara/.config/Typora/typora-user-images/image-20200121192433107.png)

```pseudocode
process ProcesoA ; 
	var var_orig : integer ; 
begin 
	var_orig := Produce(); 
	send( var_orig, ProcesoB ); 
end
```

```pseudocode
process ProcesoB ; 
	var var_dest : integer ; 
begin 
	receive( var_dest, ProcesoA ); 
	Consume( var_dest ); 
end
```

#### PRIMITIVAS BÁSICAS DE PASO DE MENSAJES

El paso de un mensaje entre dos procesos constituye una transferencia de una secuencia finita de bytes

![image-20210110183649183](/home/clara/.config/Typora/typora-user-images/image-20210110183649183.png)

- Se leen de una variable del proceso emisor (var_orig) y se escriben en una variable del proceso receptor (var_dest). Ambas variables son del mismo tipo
- Se transfieren a través de una red de interconexión
- Implica sincronización: los bytes acaban de recibirse después de iniciado el envío
- El efecto neto final es equivalente a una hipotética asignación var_dest = var_orig, si se pudiera hacer

El proceso emisor realiza el envío invocando a **send**, y el receptor invocando a **receive**. Su sintaxis es la siguiente:

```pseudocode
send(variable_origen, identificador_proceso_destino)
receive(variable_destino, identificador_proceso_origen)
```

Cada proceso nombra explícitamente al otro, indicando su nombre de proceso como id

```pseudocode
process P1;
	var var_orig: integer;
begin
	var_orig := Produce();
	send(var_orig, P2);
end
```

```pseudocode
process P2;
	var var_dest: integer;
begin
	receive(var_dest, P1);
	Consume(var_dest);
end
```

#### Esquemas de identificación de la comunicación

¿Cómo identifica el emisor al receptor del mensaje y viceversa? existen dos posibilidades:

- **Denominación directa estática**

  El emisor identifica explícitamente al receptor y viceversa. Para esto se usan identificadores de procesos. Cada id es un valor entero biunívocamente asociado a un proceso en tiempo de compilación (estáticamente)

  ```pseudocode
  process P1;
  	var var_orig: integer;
  begin
  	var_orig := Produce();
  	send(var_orig, P2);
  end
  ```

  ```pseudocode
  process P2;
  	var var_dest: integer;
  begin
  	receive(var_dest, P1);
  	Consume(var_dest);
  end
  ```

  **Ventajas:** no hay retardo para establecer la identificación (los símbolos P1, P2 se traducen en 2 números enteros)

  **Inconvenientes:** cambios en la identificación requieren recompilar el código, y sólo permite comunicación 1-1 (en pares)

  Existen otras posibilidades: los esquemas asímetricos, en los que el emisor identifica un receptor pero el receptor no marca un emisor específico. El receptor acepta de cualquier emisor, y cuando recibe el mensaje puede llegar a saber su emisor si su id está en los metadatos. Incluso podemos definir subgrupos de emisores y aceptar sólo mensajes de estos mismos.

- **Denominación indirecta**

  Los mensajes se depositan en almacenes intermedios (buzones) que son accesibles desde todos los procesos. El emisor nombra a qué buzón envía y el receptor de qué buzón quiere recibir.

  ```pseudocode
  var buzon: channel of integer;
  ```

  ```pseudocode
  process P1;
  	var var_orig: integer;
  begin
  	var_orig := Produce();
  	send(var_orig, buzon);
  end
  ```

  ```pseudocode
  process P2;
  	var var_dest: integer;
  begin
  	receive(var_dest, buzon);
  	Consume(var_dest);
  end
  ```

  El uso de buzones da mayor flexibilidad, permite comunicación entre múltiples receptores y emisores. Existen tres tipos: canales (1-1), puertos (n-1) y generales (n-n).

  Un mensaje enviado a un buzón general permanece en dicho buzón hasta que todos los procesos receptores potenciales lo hayan leído (cada envío es un broadcast)

#### Declaración estática vs. dinámica

Los identificadores de proceso suelen ser valores enteros, asignados biunívocamente a un proceso del programa concurrente. Se pueden gestionar

- **Estaticamente:** en el código fuente se asocia explícitamente el entero al proceso
  - Muy eficiente en tiempo, pero cambios en la estructura del programa requieren adaptar el código y recompilarlo
- **Dinámicamente:** el id numérico se calcula en tiempo de ejecución cuando es necesario
  - Es menos eficiente al ser más complejo, pero el código será válido aunque cambie el número de procesos de cada tipo

#### Comportamiento de las operaciones de paso de mensajes

Consideremos esta trasmisión de mensaje:

```pseudocode
process Emisor;
	var var_orig: integer = 100;
begin
	send(var_orig, Receptor);
	var_orig = 0;
end
```

```pseudocode
process Receptor;
	var var_dest: integer = 1;
begin
	receive(var_dest, Emisor);
	imprime(var_dest);
end
```

El comportamiento esperado del send es que el valor recibido en var_dest sea el que tenía var_orig antes de enviarse, o sea, 100. Si se garantiza esto, se habla de comportamiento **seguro**. 

Si pudiera llegar a imprimirse 0 o 1, el comportamiento es **no seguro**, y se considera incorrecto, aunque existen situaciones en las que podríamos llegar a querer utilizar operaciones no seguras.

#### Instantes críticos en el lado del emisor

Para poder transmitir un mensaje el sistema de paso de mensajes (SPM) en el lado del emisor da los siguientes pasos:

- Fin de registro de la **solicitud de envío (SE)**: después de iniciada la llamada a send, el SPM registra los id de ambos procesos, la dirección y el tamaño de la variable de origen
- **Inicio de lectura (IL)**: el SPM lee el primer byte de todos los que forman el valor de la variable de origen
- **Fin de lectura (FL)**: el SPM lee el último byte de todos los que forman el valor de la variable de origen

#### Instantes críticos en el lado del receptor

En el lado del receptor, el SPM da estos pasos:

- Fin de registro de la **solicitud de recepción (SR)**: después de indicar receive, el SPM registra los id de procesos y la direccion y tamaño de la variable de destino
- Fin de **emparejamiento (EM)**: el SPM espera hasta que haya registrado una solicitud de envío que case con la de recepción anterior, y entonces se emparejan ambas
- **Inicio de escritura (IE)**: el SPM escribe en destino el primer byte recibido
- **Fin de escritura (FE)**: el SPM escribe en destino el último byte recibido

#### Sincronización en el SPM para la transmisión

La transmisión de un mensaje supone sincronización:

- El emparejamiento solo puede completarse depués de registrada la solicitud de envío, SE < EM
- Antes de que se escriba el primer byte en el receptor se debe haber comenzado la lectura en el emisor, IL < IE
- Antes de que se acaben de escribir los datos en el receptor se deben haber acabado de leer en el emisor, FL < FE
- Por transitividad, se cumple IL < FE
- No tienen orden las siguientes acciones:
  - SE, SR y EM, IL/FL
- Se puede iniciar la lectura IL antes de que ocurra el emparejamiento EM. Si esto se hace
  - El SPM debe almacenar temporalmente algunos o todos los bytes de la variable origen en alguna zona de memoria del lado del emisor
  - Esa zona es el **almacén temporal** de datos

![image-20200123103943188](/home/clara/.config/Typora/typora-user-images/image-20200123103943188.png)

#### Mensajes en tránsito y memoria

Por la hipótesis de progreso finito, el intervalo de tiempo entre la solicitud de envío y el fin de la escritura tiene una duración no predecible. Entre SE y FE se dice que el mensaje está **en tránsito.**

- El SPM necesita memoria temporal para todos los mensajes en tránsito que gestione en un momento dado
- La cantidad de memoria dependetrá de diversos detalles (tamaño, número, velocidad de transmisión, políticas de envío...)
- Dicha memoria puede estar ubicada en emisor y/o en receptor y/o en nodos intermedios, si hay
- En un momento dado, el SPM puede detectar que no tiene suficiente memoria para el almacenamiento temporal de datos durante el envío
  - Debe retrasar la lectura en el emisor hasta asegurarse de tener memoria

#### Seguridad de las operaciones de paso de mensajes

Ya hemos visto que las operaciones pueden ser no seguras: el valor que el emisor pretendía enviar podría no ser el mismo que el receptor recibe:

- **Operación de envío-recepción segura:** se puede garantizar a priori que el valor de var_orig antes del envío (antes de SE) coincidirá con el valor de var_dest tras la recepción (después de FE)
- **Operación de envío-recepción insegura:** en dos casos:
  - Envío inseguro: ocurre cuando es posible modificar el valor de var_orig entre SE y FL (y podría enviarse un valor distinto al valor de SE)
  - Recepción insegura: ocurre cuando se puede acceder a var_dest entre SR y FE, si se lee antes de recibirlo totalmente o se modifica tras haberlo recibido parcialmente

#### Tipos de operaciones de paso de mensajes

**Operaciones seguras**: devuelven el control cuando se garantiza la seguridad: send no espera a la recepción, pero receive sí espera. Existen dos mecanismos de paso de mensajes seguro:

- Envío y recepción síncronos
- Envío asíncrono seguro

**Operaciones inseguras**: devuelven el control inmediatamente tras hacerse la solicitud. El programador debe asegurarse de que no se alteran las variables mientras el mensaje está en tránsito, existen sentencias adicionales para comprobar el estado de la operación

#### Operaciones síncronas

```
s_send(var_orig, id_proceso_receptor);
```

Realiza el envío de datos y espera bloqueado hasta que los datos hayan terminado de leerse en el emisor y se haya emparejado e iniciado el receive en receptor, es decir, no termina hasta que FL y EM ocurran

```
receive(var_dest, id_proceso_emisor);
```

Espera bloqueado hasta que el emisor envía un mensaje con destino al proceso receptor y hasta que hayan terminado de escribirse los datos en la var_dest, es decir, no termina hasta que ocurra FE

![image-20200123175309075](/home/clara/.config/Typora/typora-user-images/image-20200123175309075.png)

Exige cita entre emisor y receptor: la operación s_send no devuelve el control hasta que el receive correspondiente sea alcanzado por el receptor. El intercambio del mensaje constituye un punto de sincronización entre emisor y receptor. El emisor podrá hacer aserciones acerca del estado del receptor

![image-20200123222902044](/home/clara/.config/Typora/typora-user-images/image-20200123222902044.png)

Las operaciones síncronas son fáciles de implementar, pero poco flexibles. Se puede dar sobrecarga por espera ociosa si el send/receive se inician de forma muy diferida y además genera interbloqueo

#### Envío asíncrono seguro

```
send(variable, id_proceso_receptor)
```

Inicia el envío de datos designados y espera bloqueado hasta que hayan terminado de copiarse todos los datos de variable a un lugar seguro. Tras la copia de los datos designados, devuelve el control sin que tengan que haberse recibido los datos en el receptor: por tanto, devuelve el control tras FL. Se usa junto a recepción síncrona (receive())

![image-20200123223736621](/home/clara/.config/Typora/typora-user-images/image-20200123223736621.png)

![image-20200123223805251](/home/clara/.config/Typora/typora-user-images/image-20200123223805251.png)

> El fin de send no depende de la actividad del receptor. Puede ocurrir antes, después o durante la recepción.

Las ventajas son que el uso del send lleva a menos tiempos de espera que s_send, ya que no hay que esperar al emparejamiento: es más eficiente en tiempo y preferible cuando el emisor no tiene que esperar recepción

Un posible inconveniente sería que hay que tener en cuenta que send requiere memoria de almacenamiento temporal, la cual puede llegar a crecer mucho. Si no hubiera memoria suficiente, el SPM podría tener que retrasar el IL del emisor. También existe posibilidad de interbloqueo.

#### Operaciones inseguras

Ya que las operaciones seguras son menos eficientes en tiempo y memoria, podemos mirar operaciones de **inicio de envío o recepción:**

- Estas operaciones devuelven el control antes de que sea seguro modificar o leer los datos
- Deben existir sentencias de chequeo de estado: indican si los datos pueden alterarse o leerse sin comprometer la seguridad
- Una vez iniciada la operacion, el usuario puede realizar cualquier cómputo que no dependa de la finalización de la operación y, cuando sea necesario, comprobará su estado

#### Paso de mensajes asíncrono inseguro

```pseudocode
i_send(var_orig, id_proceso_receptor, var_resguardo)
```

Indica el SPM que comience una operación de envía al receptor: se registra la solicitud de envío (SE) y acaba. No espera a FL ni a nada del receptor. Var_resguardo permite consultar después el estado del envío.

```pseudocode
i_receive(var_dest, id_proceso_emisor, var_resguardo)
```

Indica al SPM que se inicie una recepción de un mensaje del emisor: se registra la SR, no espera FE ni ninguna acción del emisor. Var_resguardo permite consultar después el estado de la recepción

![image-20200123225243845](/home/clara/.config/Typora/typora-user-images/image-20200123225243845.png) 

En general, las operaciones asíncronas tardan mucho menos que las síncronas, ya que simplemente suponen el registro de la SE o SR

#### Esperar al final de operaciones asíncronas

Cuando el proceso hace i_send o i_receive puede continuar trabajando hasta que llega un momento ene l que debe esperar a que termine la operación. Se dispone de estos dos procedimientos:

```pseudocode
wait_send(var_resguardo);
wait_recv(var_resguardo);
```

En ambos casos, se invoca a proceso emisor/receptor, y lo bloquea hasta que la operación de envío/recepción asociada a var_resguardo ha llegado al instante FL/FE (momento en el cual vuelve a ser seguro usar la variable)

![image-20200123230005032](/home/clara/.config/Typora/typora-user-images/image-20200123230005032.png)

> wait_send no espera emparejamiento: es seguro, pero no síncrono

Estas operaciones permiten a los procesos emisor y receptor hacer trabajo útil concurrentemente con la operación de envío o recepción. Mejora el tiempo de espera ociosa para poder aprovecharlo, pero hay que reestructurar el programa.

#### Consulta de estado de operaciones asíncronas

```pseudocode
test_send(var_resguardo);
test_recv(var_resguardo);
```

Funciones lógicas que se invocan por el emisor/receptor. Si el envío asociado a var_resguardo ha llegado a FL/FE, devuelve true, si no, false.

#### Espera ocupada de múltiples procesos

Usando las operaciones i_receive junto con las de test, se usa espera ocupada de forma que:

- Se espera un mensaje cualquiera proveniente de varios emisores
- Tras recibir el primero de los mensajes se ejecuta una acción, independientemente de cual sea el emisor de ese mensaje
- Entre la recepción del primer mensaje y la acción el retraso es normalmente pequeño

Sin embargo, con las primitivas vistas, no es posible cumplir estos requisitos usando esperabloqueada, es inevitable seleccionar de antemano qué emisor quedemos esperar recibir en primer lugar, y este emisor no coincide necesariamente con el emisor del primer mensaje que podría recibirse

#### ESPERA SELECTIVA

La espera selectiva es una operación que permite espera bloqueada de múltiples emisores. Se usan las palabras clave `select` y `when`. El ejemplo anterior puede implementarse de esta forma:

```pseudocode
process Emisor1 ;
	var a : integer:= 100; 
begin 
	send( a, Receptor); 
end 

process Emisor2 ; 
	var b : integer:= 200; 
begin 
	send( b, Receptor); 
end
```

```pseudocode
process Receptor ; 
	var b1, b2 : integer ; 
	r1, r2 : boolean := false ; 
begin 
	while not r1 or not r2 do begin 
		
		select 
			when receive( b1, Emisor1 ) do 
				r1 := true ; 
				print("recibido de 1 : ", b1 );
            when receive( b2, Emisor2 ) do 
            	r2 := true ; 
            	print("recibido de 2 : ", b2 ); 
          end { select }
      end { while }
end { process }
```

##### Productor-consumidor distribuido

Consideramos una solución muy simple, en la que el productor usa s_send:

```pseudocode
process Productor ; 
begin 
	while true do begin 
		v := Produce(); 
		s_send( v, Consumidor ); 
	end 
end
```

```pseudocode
process Consumidor ; 
begin 
	while true do begin 
		receive( v, Productor); 
		Consume(v);
	end 
end
```

Produce y Consume pueden tardar tiempos distintos: si usáramos send, el SPM tendría que alojar memoria para almacenamiento temporal, y la cantidad de memoria podría llegar a crecer indefinidamente. El **problema** es que al usar s_send se pueden producir esperas ociosas largas.

Para reducir las esperas, vamos a introducir un **proceso intermedio `buffer`** que acepte las peticiones de Productor y Consumidor:

Consideramos una solución muy simple, en la que el productor usa s_send:

```pseudocode
process Productor ; 
begin 
	while true do begin 
		v := Produce(); 
		s_send( v, Buffer ); 
	end 
end
```

```pseudocode
process Consumidor ; 
begin 
	while true do begin 
		s_send(s,Buff); 
		receive(v,Buff);
		Consume(v)
	end 
end
```

```pseudocode
process Buffer;
begin
	while true do begin 
		receive(v,Prod); 
		receive(s,Cons); 
		s_send(v,Cons);
	end 
end
```

**Problema**: el proceso intermedio se bloquea por turnos para esperar bien al emisor, bien al receptor, pero nunca a los dos a la vez. No estamos solucionando las esperas excesivas.

Para solucionarlo usamos **espera selectiva** en el proceso intermedio: ahora este puede esperar a ambos procesos a la vez. Para reducir las esperas, usamos un vector FIFO de datos pendientes de lectura:



### 2. Paradigmas de interacción de procesos en programas distribuidos

Definen un esquema de interacción entre procesos y una estructura de control que aparece en muchos programas. Unos pocos paradigmas se utilizan repetidamente para desarrollar muchos programas distribuidos.

#### MAESTRO-ESCLAVO

//todo

#### ITERACIÓN SÍNCRONA

//todo

#### ENCAUZAMIENTO (*PIPELINING*)

//todo

### 3. Mecanismos de alto nivel en sistemas distribuidos

//todo

#### PARADIGMA CLIENTE-SERVIDOR

//todo

#### LLAMADA A PROCEDIMIENTO (RPC)

//todo

#### JAVA REMOTE METHOD INVOCATION (RMI)

//todo

#### SERVICIOS WEB

En la actualidad gran parte de la comunicación en internet ocurre vía los llamados servicios web

- Se usan protocolos HTTP o HTTPS en la capa de aplicación, típicamente sobre TCP/IP en la capa de transporte
- Se usa codificación de datos basada en XML o JSON
- Es posible utilizar protocolos complejos (SOAP) pero lo más frecuente es utilizar el método REST, caracterizado por
  - Los clientes solicitan un recurso especificando su URL
  - El servidor responde enviando el recurso en su versión actual o notificando un error
  - Cada petición es independiente a las demás: el servidor, una vez enviada la respuesta, no guarda información de estado de sesión o sobre el estado del cliente (REST es stateless)

#### Llamadas y sincronización

Las peticiones de recursos o documentos pueden hacerse desde una aplicación cualquiera ejecutándose en el cliente aunque en particular lo más frecuente es usar un programa JS ejecutándose en un navegador en el nodo cliente. Las peticiones se gestionan de forma

- Síncrona: el proceso cliente espera bloqueado la llegada de las respuestas. Se considera no aceptable en aplicaciones web interactivas (paraliza la interacción entre el user y la web)
- Asíncrona: el proceso cliente envía petición y continua. Cuando recibe respuesta, se ejecuta una función designada por el cliente al hacer la petición. La función recibe la respuesta como parámetro

## 

# SCD Tema 4: Introducción a los sistemas de tiempo real

> 1. Concepto de sistema de tiempo real. Medidas de tiempo y modelo de tareas. 
> 2. Esquemas de planificación

### 1. Concepto de sistema de tiempo real. Medidas de tiempo y modelo de tareas

Los sistemas de tiempo real (STR) son aquellos en los que la ejecución del sistema se debe producir dentro de unos plazos de tiempo predefinidos para que funcione con suficiente garantía. 

En un sistema que además sea concurrente será necesario que todos los procesos sobre un procesador o sobre varios se ejecuten en los plazos de tiempo predefinidos

Un sistema de tiempo real es aquel sistema cuyo funcionamiento correcto depende no sólo de resultados lógicos producidos por el mismo, sino también del instante de tiempo en el que se producen estos resultados.

**Corrección funcional + corrección temporal**: el no cumplimiento de esta última lleva a un fallo del sistema

#### TIPOS DE STR

- **Sistema en línea:** siempre está disponible, pero no se garantiza una respuesta en un intervalo de tiempo acotado
  - *Cajeros automáticos, sistema de reservas de vuelo*
- **Sistema interactivo:** suele ofrecer respuesta en un tiempo acotado, pero no importa si ocasionalmente tarda más
  - *Reproductor DVD, sistema de aire acondicionado, juegos*
- **Sistema de respuesta rápida:** el sistema ofrece una respuesta en un tiempo acotado y lo más corto posible
  - *Sistema antiincendios, alarma*

Los sistemas de tiempo real presentan todas estas caracterísiticas:

![image-20200122160554694](/home/clara/.config/Typora/typora-user-images/image-20200122160554694.png)

###### Ejemplos de STR

> - Automoción: sistema de ignición, transmisión, dirección asistida, frenos antibloqueo, control de la tracción...
> - Electrodomésticos: televisores, lavadoras, hornos microondas, reproductores de vídeo, sistemas de seguridad y vigilancia...
> - Aplicaciones aeroespaciales: control de vuelos, controladores de motores, pilotos automáticos...
> - Equipamiento médico: sistemas de monitorización de anestesia, monitores ECG...
> - Sistemas de defensa: sistemas radar de aviones, sistemas de radio, control de misiles...

#### PROPIEDADES DE LOS STR

Al depender la corrección del sistema de las restricciones temporales, los sistemas de tiempo real tienen que cumplir las siguientes propiedades

##### Reactividad

![image-20210110131303522](/home/clara/.config/Typora/typora-user-images/image-20210110131303522.png)

El sistema tiene que interaccionar con el entorno y responder de la manera esperada a los estímulos externos dentro del intervalo de tiempo previamente definido 

##### Predecibilidad

Tener un comportamiento predecible implica que la ejecución del sistema tiene que ser determinista

- Las respuestas han de producirse dentro de las restricciones temporales impuestas por el entorno
- Es necesario conocer el comportamiento temporal de los componentes software y hardware utilizados, así como el del lenguaje de programación
- Si no se puede tener un conocimiento temporal exacto, hay que definir marcos de tiempo acotados
  - P.ej. conocer el tiempo de peor ejecución de un algoritmo, el tiempo máximo de acceso a un dato E/S...

##### Confiabilidad

La confiabilidad (dependability) mide el grado de confianza que se tiene en el sistema. Depende de :

- **Disponibilidad (availability):** capacidad de proporcionar servicios siempre que se necesita
- **Robusted o Tolerancia a fallos (fault tolerant):** capacidad de operar en situaciones excepcionales sin poseer un comportamiento catastrófico
- **Fiabilidad (reliability):** capacidad de ofrecer siempre los mismos resultados bajo las mismas condiciones
- **Seguridad:**
  - **Safety:** capacidad de protegerse ante ataques o fallos accidentales o deliberados
  - **Security:** no vulnerabilidad de los datos

Cuando esta propiedad es crítica (su no cumplimiento lleva a pérdida humana o económica), el sistema se denomina **Sistema en tiempo real crítico (safety-critical system)**. Ej. sistemas de aviónica y automoción, centrales nucleares...

#### EL MODELO DE TAREAS

Un STR se estructura en tareas que acceden a los recursos del sistema

![image-20210110153915588](/home/clara/.config/Typora/typora-user-images/image-20210110153915588.png)

Una tarea es un conjunto de acciones que describen el comportamiento del sistema o parte de él en base a la ejecución secuencial de un trozo de código: equivale a un proceso o hebra.

##### Tareas

- Satisfacen una necesidad funcional concreta
- Tiene ciertas restricciones temporales definidas a partir de sus **Atributos temporales**
- Cuando se ejecutan o están pendientes de ello son una tarea activada
- Se activan cuando se hace necesario ejecutarlas una vez
- El **instante de activación** es el instante de tiempo a partir del cual debe ejecutarse

##### Recursos

Son elementos disponibles para la ejecución de tareas. Se distinguen:

- **Recursos activos:** procesador, red...
- **Recursos pasivos:** datos, memoria, E/S...

Asumimos que cada CPU disponible se dedica a ejecutar una o varias tareas, de acuerdo con el esquema de planificación de uso. **Si una CPU ejecuta más de una tarea, el tiempo de la CPU debe repartirse entre estas.**

Los requerimientos de un STR obligan a asociar un conjunto de atributos temporales a cada tarea del sistema: estos son restricciones acerca de cuándo se activa cada tarea y cuánto puede tardar en completarse desde que se activa.

##### Planificación de tareas

![image-20210110154734308](/home/clara/.config/Typora/typora-user-images/image-20210110154734308.png)

Es una labor de diseño que determina como se asignan a lo largo del tiempo a cada tarea los recursos activos del sistema (principalmente la CPU). Se debe garantizar el cumplimiento de las restricciones temporales de la tarea.

##### Atributos temporales de una tarea

<img src="/home/clara/.config/Typora/typora-user-images/image-20210110155023376.png" alt="image-20210110155023376" style="zoom: 150%;" />

- **Tiempo de cómputo/ejecución, C**: tiempo necesario para la ejecución de una tarea

- **Tiempo de respuesta, R:** tiempo que ha necesitado para completarse *a partir del instante de activación* (INCLUYE ESPERA)

- **Tiempo de respuesta máxima (*deadline*), R:** máximo tiempo de respuesta posible

- **Periodo, T:** intervalo de tiempo entre dos activaciones sucesivas en el caso de tareas periódicas.

  Tipos de tareas según recurrencia de activación:
  - *Aperiódica*: se activa en instantes arbitrarios (no tiene T)
  - *Periódica*: repetitiva, T es el tiempo exacto que transcurre entre activaciones
  - *Esporádica*: repetitiva, T es el intervalo mínimo entre activaciones

##### Diseño y esquema de planificación de tareas

El problema de la planificación supone:

- Determinar los procesadores disponibles a los que se puede asociar las tareas
- Determinar las relaciones de dependencia entre tareas
  - Relaciones de precedencia
  - Determinar recursos comunes entre tareas
- Determinar el orden de ejecución de tareas para garantizar las restricciones especificadas

Para determinar la planificabilidad de un conjunto de tareas se requiere un esquema de planificación que cubra estos aspectos:

- **Algoritmo de planificación:** define un criterio (*política de planificación*) que determina el orde de acceso de las tareas a los distintos procesadores

- **Método de análisis:** o *test de planificabilidad*, que permite predecir el comportamiento temporal del sistema y determina la viabilidad de la planificación sobre las restricciones especificadas

  - Se puede comprobar si los requisitos temporales están garantizados en **todos** los casos posibles

  - Y más particularmente, se estudia el **peor caso posible**, con el WCET (*Worst Case Execution Time*).

    El WCET es el máximo valor de C para cualquier ejecución posible de una tarea. Para obtenerlo:

    - **Medida directa** del tiempo de ejecución en el peor caso en la misma plataforma de ejecución.

      Medición de múltiples experimientos y análisis estadístico

    - **Análisis de código:** se calcula el peor tiempo...

      - Descomponiendo el código en un grafo de bloques secuenciales
      - Se calcula el tiempo de ejecución de cada bloque
      - Se busca el camino más largo

    En los ejemplos y ejercicios, y mientras no se indique lo contrario, asumimos que siempre se tarda lo mismo (C) en ejecutar una tarea determinada: es decir, `C = C en el peor caso`.

##### Restricciones temporales de una tarea

Para determinar la planificación de un sistema necesitamos, pues, conocer las **restricciones temporales** de cada tarea. Algunos ejemplos:

> C = 2ms, T = D = 10ms
>
> Es una tarea periódica que se activa cada 10ms y se ejecuta en un tiempo de 2ms en el peor de los casos

> C = 10ms, T = 100ms, D = 90ms
>
> Es una tarea periódica que se activa cada 100ms, se ejecuta durante un peor caso de 10ms y desde que se inicia la activación hasta que concluye no pueden pasar más de 90ms

##### Factor de utilización

Las restricciones temporales de un conjunto de **n** tareas periódicas se especifican en tablas que contienen Ti, Ci, Di para cada una de ellas (con i desde 1 hasta n)

- Entonces, la i-ésima tarea ocupa una fracción `Ci / Ti` del tiempo total de CPU

- El **factor de utilización, U,** es la suma de esas fracciones para todas las tareas, es decir:

  ![image-20210110161553303](/home/clara/.config/Typora/typora-user-images/image-20210110161553303.png)

  En un hardware con **p** procesadores disponibles para ejecutar las tareas , si `U > p` entonces el sistema no es planificable: incluso dedicando 100% del tiempo de cada uno de los procesadores a la ejecución, alguna tarea no podrá acabar a tiempo

### 2. Esquemas de planificación

Los tipos de esquema de planificación en un sistema monoprocesador son los siguientes:

- **Planificación estática offline sin prioridades** (ejecutivo cíclico)
- **Planificación basada en prioridades de tareas**
  - **Estáticas**: prioridades preasignadas, no cambian:
    - **RMS, Rate Monotonic Scheduling:** prioridad a la tarea con menor periodo (T)
    - **DMS, Deadline Monotonic Scheduling: **prioridad a la tarea con menor deadline (D)
  - **Dinámicas**: prioridades cambiantes durante la ejecución:
    - **EDF, Earliest Deadline First:** prioridad a la tarea con el deadline más próximo
    - **LLF, Least Laxity First:** prioridad a la tarea de menor holgura (tiempo hasta deadline menos tiempo de ejecución restante)

#### PLANIFICACIÓN CÍCLICA

La planificación se basa en diseñar un programa (ejecutivo cíclico) que implementa un plan de ejecución (plan principal), que garantice los requerimientos temporales:

- El programa es un bucle infinito tal que cada iteración tiene una duración prefijada, siempre igual. El bucle es el ciclo principal
- En cada iteración del bucle principal se ejecuta otro bucle acotado con k iteraciones (k cte.). Cada iteración dura siempre lo mismo y en ella se ejecutan completamente una o varias tareas. A este bucle interno (acotado) se le denomina ciclo secundario
- El entrelazado de las tareas en cada iteración del ciclo principal es siempre el mismo. Una iteración número i del ciclo secundario puede tener un entrelazado distintos a otra iteración número j (i,j 1..k)

Las duraciones de ambos ciclos son valores enteros (se supone que el tiempo se mide en múltiplos enteros de alguna unidad de tiempo)

- La duración del ciclo principal se denomina **hiperperiodo (Tm): mínimo común múltiplo de los periodos de todas las tareas**, por tanto, los instantes de inicio de cada iteración del ciclo principal coinciden con los instantes en los cuales todas las tareas se activan de nuevo a la vez
- La duración del ciclo secundario se denomina Ts. Lógicamente, se debe cumplir **Tm = k \* Ts**  

###### Ejemplo e implementación

> | Tarea | T    | C    |
> | ----- | ---- | ---- |
> | A     | 25   | 10   |
> | B     | 25   | 8    |
> | C     | 50   | 5    |
> | D     | 50   | 4    |
> | E     | 100  | 2    |

- Tm = mcm(25,50,100) =100
  - *El ciclo principal dura 100ms*
- k= 4, 	Ts = 25ms
  - *4 ciclos secundarios de 15ms cada uno*

- Asumimos D = T

![image-20200122225819123](/home/clara/.config/Typora/typora-user-images/image-20200122225819123.png)

```
process EjecutivoCiclico;
	var inicio : time_point := now() 	//instante inicio
begin
	while true do begin					//ciclo principal
	//ejecutar las 4 iteraciones del ciclo secundario
		A(); B(); C();		sleep_until(inicio+25);
		A(); B(); D(); E();	sleep_until(inicio+50);
		A(); B(); C();		sleep_until(inicio+75);
		A(); B(); D();		sleep_until(inicio+100);
		inicio = inicio+100;
	end
end
    
```

##### Selección de duración del ciclo secundario

Para seleccionar un valor apropiado de Ts se deben tener en cuenta las siguientes restricciones y esta sugerencia:

- Restricciones:
  - **El valor de Ts tiene que ser un divisor de Tm**
  - El valor de Ts tiene que ser mayor o igual que el tiempo de cómputo C de cualquier tarea, es decir **max(C1, C2..Cn) <= Ts**
- Sugerencia: es aconsejable intentar en principio que el ciclo secundario sea **menor o igual que el mínimo deadline** (o periodo si no se proporciona deadline)

> El ejemplo anterior con un Ts=10ms y k=10:
>
> | 10   | 20   | 30   | 40   | 50   | 60   | 70   | 80   | 90   | 100  |
> | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
> | A    | BE   | CD   | A    | B    | A    | B    | CD   | A    | B    |

##### Propiedades de la planificación cíclica

- No hay concurrencia en la ejecución
  - Cada ciclo secundario es una secuencia de llamadas a procedimientos
  - No se necesita un núcleo de ejecución multitarea
- Los procedimientos pueden compartir datos
  - No necesitan mecanismos de EM como los semáforos o monitores
- No hace falta analizar el comportamiento temporal
  - El sistema es correcto por construcción

##### Problemas de la planificación cíclica

- Dificultad para incorporar tareas con periodos largos
- Las tareas esporádicas son difíciles de tratar
- El plan ciclo del proyecto es difícil de construir
  - Si los periodos son de diferentes órdenes de magnitud el número de ciclos puede ser muy grande
  - **Puede ser necesario partir una tarea en varios procedimientos**
  - Es el caso más general de sistemas en tiempo real críticos
- Es poco flexible y difícil de mantener
  - Cada vez que se cambia una tarea hay rehacer la planificación

#### PLANIFICACIÓN CON PRIORIDADES

La planificación con prioridades pretende solventar los problemas descritos anteriormente. Cada tarea tiene un valor asociado que describe su **prioridad**:

- Atributo entero no negativo, que depende de los atributos temporales y/o el entrelazamiento de tareas
- Por convención, a mayor el número, mayor urgencia (prioridad)
- Determina la selección de la tarea a ejecutarse en el procesador cuando esté libre
- Las prioridades pueden ser **estáticas** (fijadas previamente) o **dinámicas** (cambian con el tiempo), en este caso se deben calcular cada vez que se consultan

##### Planificación de tareas (*scheduling*)

En este tipo de planificaciones debe existir un componente software (planificador, *scheduler*) que:

- Asigne al procesador a una tarea activa o ejecutable (*dispatch*)
- Desasignar la CPU a la tareja en ejecución (desalojar la tarea)
- Una tarea puede estar en varios estados distintos
  - Suspendida, ejecutable, ejecutando
  - Las tareas ejecutables se despachan para su ejecución por orden de prioridad

![image-20210110165011782](/home/clara/.config/Typora/typora-user-images/image-20210110165011782.png)

El planificador actúa en cada instante en el que ocurren alguno de estos dos eventos:

- Una o más de una tarea se activan (pasan a ejecutable)
- La tarea en ejecución termina (pasa a suspendida)

A continuación, selecciona cualquier tarea **A** con una prioridad actual **Pa** máxima entre todas las tareas ejecutables, después...

- Si la CPU está libre, **A** pasa a ejecutando
- Si la CPU está ejecutando otra tarea **B** con una prioridad actual **Pb**...
  - Si **Pa>Pb**, la tarea **A** pasa a ejecutando y **B** va a ejecutable (**A** desplaza a **B**)
  - Si **Pa<=Pb** no hay cambios: **A** sigue en ejecutable, y **B** en ejecutando

Al inicio todas las tareas se activan a la vez.

##### Planificación RMS: *Rate Monotonic Scheduling*

Es un método de planificación **estático** online con asignación **mayor prioridad a las tareas más frecuentes** (con menor periodo). Es óptima en caso de todas las tareas periódicas y T=D

> | Tareas | C    | T    |
> | ------ | ---- | ---- |
> | A      | 10   | 30   |
> | B      | 5    | 40   |
> | C      | 9    | 50   |
>
> Las prioridades serían A>B>C

##### Test de planificabilidad

Los test de planificabilidad determinan si el conjunto de tareas es planificable según algoritmo de planificación antes de que lo ejecutemos. Existen diversos tipos de test aplicables según el tipo de algoritmo:

- **Test de planificabilidad suficientes:**
  - En caso de éxito el sistema es planificable
  - En caso contrario nos falta información: podría ser planificable o no
- **Test de planificabilidad exactos:** (suficiente y necesario)
  - En caso de éxito el sistema es planificable
  - En caso contrario no es planificable, dará error (no se cumplirá algún deadline)

##### Test de Liu & Layland

Es un test de planificabilidad **suficiente**, se aplica para un sistema de n tareas periódicas independientes con prioridad RMS

- Se usa el factor de utilización U y el factor de utilización máximo (U0n)

  - U = sumatorio\[i=1..n] (Ci/Ti)

    ![image-20210110170700329](/home/clara/.config/Typora/typora-user-images/image-20210110170700329.png)

  - U0n = n *  (raíz n-ésima(2) - 1)

    ![image-20210110170708065](/home/clara/.config/Typora/typora-user-images/image-20210110170708065.png)

- El valor de U0(1) es 1. A partir de ahí decrece según crece n, hasta el límite que es ln2

- Un sistema **pasa el test si U <= U0n**. En caso contrario, no podemos confirmar nada y recurrimos a mirar el cronograma

###### Ejemplo

> **Ejemplo 1: cumpliendo el test Liu&Layland**
>
> | Tareas | C    | D    | T    | C/T   |
> | ------ | ---- | ---- | ---- | ----- |
> | A      | 4    | 16   | 16   | 0.250 |
> | B      | 5    | 40   | 40   | 0.125 |
> | C      | 32   | 80   | 80   | 0.400 |
>
> U = sumatorio Ci/Ti = 0.250+0.125+0.400 = **0.775**
>
> U0(3) = 3 * (3√2  -1) = **0.77976**
>
> U <= U0? 0.775 < 0.779, SI ES PLANIFICABLE CON RMS

> **Ejemplo 2: no cumpliendo el test Liu&Layland**
>
> | Tareas | C    | D    | T    | C/T   |
> | ------ | ---- | ---- | ---- | ----- |
> | A      | 10   | 30   | 30   | 0.333 |
> | B      | 10   | 40   | 40   | 0.250 |
> | C      | 10   | 50   | 50   | 0.200 |
>
> U = sumatorio Ci/Ti = 0.333+0.250+0.200 = **0.783**
>
> U0(3) = 3 * (3√2  -1) = **0.77976**
>
> U <= U0? 0.783 > 0.779, NO SABEMOS SI ES PLANIFICABLE CON RMS
>
> Lo tenemos que comprobar en el cronograma

##### Cronograma

Para analizar la planificabilidad del sistema con dichas restricciones, hay que hacer el cronograma y verificar que:

- Para cada tarea i, se cumple el plazo de respuesta: `R < Di`. Esto se debe verificar para un hiperperiodo (después se repite)

###### Ejemplo

> Retomando el ejemplo anterior para comprobar su planificabilidad...
>
> Calculamos hiperperiodo:
>
> Tm = mcm(30,40,50) = 600
>
> ![image-20210110172807672](/home/clara/.config/Typora/typora-user-images/image-20210110172807672.png)

#### Planificación EDF (Earliest Deadline First)

Es un esquema de planificación con prioridades **dinámicas**. El primero el más urgente, **aquel cuya deadline está más cerca**

- En caso de igualdad se hace una elección no determinista de la siguiente tarea
- Es más óptimo porque no es necesario que las tareas sean periódicas
- Es menos pesimista que RMS

##### Test de planificabilidad

En este caso el test Liu&Layland se puede aplicar también, y se convierte en un test **exacto**: si pasa el test es planificable; si no, no.

- En este caso el sistema pasa el test si U <= 1
- **Esto implica que la planificación EDF puede aplicarse a más sistemas que la RMS** (ya que la U0n siempre es menor que 1 en RMS)  

###### Ejemplo 

> | Tareas | C    | D    | T    | C/T   |
> | ------ | ---- | ---- | ---- | ----- |
> | A      | 2    | 5    | 5    | 0.400 |
> | B      | 4    | 7    | 7    | 0.571 |
>
> U = sumatorio Ci/Ti = 0.400+0.571 = 0.97
>
> U < 1? 0.97 < 1, SI ES PLANIFICABLE CON EDF
>
> ![image-20210110173203067](/home/clara/.config/Typora/typora-user-images/image-20210110173203067.png)