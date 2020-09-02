# SCD TEMA 4: introducción a los sistemas de tiempo real

> 1. Concepto de sistema de tiempo real. Medidas de tiempo y modelo de tareas
> 2. Esquemas de planificación

## 1. Concepto de sistema de tiempo real. Medidas de tiempo y modelo de tareas

### 1.1. DEFINICIÓN, TIPOS Y EJEMPLOS

Los sistemas de tiempo real son aquellos en los que la ejecución del sistema se debe producir dentro de unos plazos de tiempo predefinidos para que funcione con suficiente garantía. 

En un sistema que además sea concurrente será necesario que todos los procesos sobre un procesador o sobre varios se ejecuten en los plazos de tiempo predefinidos

Un sistema de tiempo real es aquel sistema cuyo funcionamiento correcto depende no sólo de resultados lógicos producidos por el mismo, sino también del instante de tiempo en el que se producen estos resultados

Corrección funcional + corrección temporal; el no cumplimiento de esta última lleva a un fallo del sistema

#### Tipos de sistemas de tiempo real

- **Sistema en línea:** siempre está disponible, pero no se garantiza una respuesta en un intervalo de tiempo acotado
  - *Cajeros automáticos, sistema de reservas de vuelo*
- **Sistema interactivo:** suele ofrecer respuesta en un tiempo acotado, pero no importa si ocasionalmente tarda más
  - *Reproductor DVD, sistema de aire acondicionado, juegos*
- **Sistema de respuesta rápida:** el sistema ofrece una respuesta en un tiempo acotado y lo más corto posible
  - *Sistema antiincendios, alarma*

Los sistemas de tiempo real presentan todas estas caracterísiticas:

![image-20200122160554694](/home/clara/.config/Typora/typora-user-images/image-20200122160554694.png)

#### Ejemplos de STR

- Automoción: sistema de ignición, transmisión, dirección asistida, frenos antibloqueo, control de la tracción...
- Electrodomésticos: televisores, lavadoras, hornos microondas, reproductores de vídeo, sistemas de seguridad y vigilancia...
- Aplicaciones aeroespaciales: control de vuelos, controladores de motores, pilotos automáticos...
- Equipamiento médico: sistemas de monitorización de anestesia, monitores ECG...
- Sistemas de defensa: sistemas radar de aviones, sistemas de radio, control de misiles...

### 1.2. PROPIEDADES DE LOS SISTEMAS DE TIEMPO REAL

Al depender la corrección del sistema de las restricciones temporales, los sistemas de tiempo real tienen que cumplir las siguientes propiedades

#### Reactividad

El sistema tiene que interaccionar con el entorno y responder de la manera esperada a los estímulos externos dentro del intervalo de tiempo previamente definido 

#### Predecibilidad

Tener un comportamiento predecible implica que la ejecución del sistema tiene que ser determinista

- Las respuestas han de producirse dentro de las restricciones temporales impuestas por el entorno
- Es necesario conocer el comportamiento temporal de los componentes software y hardware utilizados, así como el del lenguaje de programación
- Si no se puede tener un conocimiento temporal exacto, hay que definir marcos de tiempo acotados
  - P.ej. conocer el tiempo de peor ejecución de un algoritmo, el tiempo máximo de acceso a un dato E/S...

#### Confiabilidad

La confiabilidad (dependability) mide el grado de confianza que se tiene en el sistema. Depende de :

- **Disponibilidad (availability):** capacidad de proporcionar servicios siempre que se necesita
- **Robusted o Tolerancia a fallos (fault tolerant):** capacidad de operar en situaciones excepcionales sin poseer un comportamiento catastrófico
- **Fiabilidad (reliability):** capacidad de ofrecer siempre los mismos resultados bajo las mismas condiciones
- **Seguridad:**
  - **Safety:** capacidad de protegerse ante ataques o fallos accidentales o deliberados
  - **Security:** no vulnerabilidad de los datos

Cuando esta propiedad es crítica (su no cumplimiento lleva a pérdida humana o económica), el sistema se denomina **Sistema en tiempo real crítico (safety-critical system)**. Ej. sistemas de aviónica y automoción, centrales nucleares...

### 1.3. MODELO DE TAREAS

Los sistemas de tiempo real se estrucutran en tareas que van accediendo a los recursos del sistema

![image-20200122164928608](/home/clara/.config/Typora/typora-user-images/image-20200122164928608.png)

Una **tarea** es un conjunto de acciones que describen el comportamiento del sistema o parte de él en base a la ejecución secuencial de un trozo del código. Equivale a un proceso o hebra.

- La tarea satisface una necesidad funcional concreta
- La tarea tiene definidas ciertas restricciones temporales a partir de los Atributos temporales
- Una tarea activada es una tarea en ejecución o pendiente de ejecutar
- Una tarea se activa cuando se hace necesario ejecutarla una vez
- El instante de activación es el instante de tiempo a partir del cual debe ejecutarse (cuando pasa a estar activada)

Los **recursos** son los elementos disponibles para la ejecución de las tareas. Se distinguen dos tipos:

- Recursos activos: procesador, red...
- Recursos pasivos: datos, memoria, dispositivos E/S...

Asumimos que cada CPU disponible se dedica a ejecutar una o varias tareas, de acuerdo al esquema de planificación de uso. Si una CPU ejecuta más de una tarea, el tiempo de la CPU debe repartirse entre varias tareas.

Los requerimientos de un sistema de tiempo real obligan a asociar un conjunto de atributos temporales a cada tarea del sistema. Estos atributos son restricciones acerca de cuándo se ejecuta activa cada tarea y cuánto puede tardar en completarse desde que se activa.

#### Planificación de tareas

La planificación de tareas es la labor de diseño que determina cómo se le asignan a lo largo del tiempo a cada tarea los recursos activos del sistema (principalmente capacidad de procesamiento), de forma que se garantice el cumplimiento de las restricciones temporales.

![image-20200122171114718](/home/clara/.config/Typora/typora-user-images/image-20200122171114718.png)

#### Atributos temporales

- **Tiempo de cómputo o de ejecución (C):** tiempo necesario para la ejecución de la tarea
- **Tiempo de respuesta (R): **tiempo que ha necesitado el proceso para completarse totalmente a partir del instante de activación
- **Plazo de respuesta máxima o deadline (D):** máximo tiempo de respuesta posible
- **Periodo (T):** intervalo de tiempo entre dos activaciones sucesivas en el caso de una tarea periódica. Según la recurrencia de sus activaciones, una tarea puede ser:
  - Aperiódica, se repite en instantes arbitrarios (no hay T)
  - Periódica, se repite cada T tiempo
  - Esporádica, T es el intervalo mínimo entre activaciones

#### Diseño y esquema de planificación de tareas

El problema de la planificación supone:

- Determinar los procesadores disponibles a los que se puede asociar las tareas
- Determinar las relaciones de dependencias de las tareas
  - Relaciones de precedencia que hay entre tareas
  - Determinar los recursos comunes a los que acceden las tareas
- Determinar el orden de ejecución de las tareas para garantizar las restricciones especificadas

Para determinar la planificabilidad de un conjunto de tareas se requiere un esquema de planificación que cubra los dos siguientes aspectos:

- Un **algoritmo de planificación** que define el criterio (política de planificación) que determina el orden de acceso a las tareas a los distintos procesadores
- Un **método de análisis** (test de planificabilidad) que permite predecir el comportamiento temporal del sistema, y determina si la planificabilidad es factible bajo las condiciones o restricciones especificadas
  - Se puede comprobar si los requisitos temporales están garantizados en todos los casos posibles
  - En general se estudia el peor comportamiento posible, es decir el WCET (Worst Case Execution Time

#### Cálculo del WCET

Suponemos que siempre se conoce el valor WCET (Cw) , que es el máximo valor de C para cualquier ejecución posible de dicha tarea. Hay dos formas de obtener Cw:

- **Medida directa** del tiempo de ejecución (en el peor caso) en la plataforma de ejecución
  - Se realizan múltiples experimentos y se hace una estadística
- **Análisis de código:** calcular el peor tiempo:
  - Se descompone el código en un grafo de bloques secuenciales
  - Se calcula el tiempo de ejecución de cada bloque
  - Se busca el camino más largo

En los ejemplos y ejercicios, y mientras no se diga lo contrario, asumimos que siempre se tarda lo mismo (C) en ejecutar una tarea determinada: C=Cw

#### Restricciones temporales

Para determinar la planificación del sistema necesitamos conocer las restricciones temporales de cada tarea del sistema

> Ej1:
>
> C = 2ms, T = D = 10ms
>
> Es una tarea periódica que se activa cada 10ms y se ejecuta en un tiempo de 2ms en el peor de los casos

> Ej2:
>
> C = 10ms, T = 100ms, D = 90ms
>
> Es una tarea periódica que se activa cada 100ms, se ejecuta en cada activación un máximo de 10ms, y desde que se inicia la activación hasta que concluye no pueden pasar más de 90ms

Las restricciones temporales para un conjunto de n tareas periódicas se especifican dando una tabla con los valores de Ti, Ci, Di para cada una de ellas (i 1..n)

- Lógicamente, la i-ésima tarea ocupa una fracción Ci/Ti del tiempo total de la CPU
- El factor de utilización U es la suma de esas fracciones para todas las tareas, es decir: U = Sumatorio[i=1..n] (Ci/Ti)
- En un hardware con P procesadores disponibles para ejecutar las tareas, si U > P entonces el sistema no es planificable: incluso dedicando a ejecutar tareas el 100% del tiempo de cada uno de los procesadores, alguna tarea no podrá acabar en su periodo

## 2. Esquemas de planificación

Los tipos de esquema de planificación en un sistema monoprocesador son los siguientes:

- **Planificación estática offline sin prioridades** (ejecutivo cíclico)
- **Planificación basada en prioridades de tareas**
  - **Estáticas**: prioridades preasignadas, no cambian:
    - **RMS, Rate Monotonic Scheduling:** prioridad a la tarea con menor periodo (T)
    - **DMS, Deadline Monotonic Scheduling: **prioridad a la tarea con menor deadline (D)
  - **Dinámicas**: prioridades cambiantes durante la ejecución:
    - **EDF, Earliest Deadline First:** prioridad a la tarea con el deadline más próximo
    - **LLF, Least Laxity First:** prioridad a la tarea de menor holgura (tiempo hasta deadline menos tiempo de ejecución restante)

### 2.1. PLANIFICACIÓN CÍCLICA

La planificación se basa en diseñar un programa (ejecutivo cíclico) que implementa un plan de ejecución (plan principal), que garantice los requerimientos temporales:

- El programa es un bucle infinito tal que cada iteración tiene una duración prefijada, siempre igual. El bucle es el ciclo principal
- En cada iteración del bucle principal se ejecuta otro bucle acotado con k iteraciones (k cte.). Cada iteració dura siempre lo mismo y en ella se ejecutan completamente una o varias tareas. A este bucle interno (acotado) se le denomina ciclo secundario
- El entrelazado de las tareas en cada iteración del ciclo principal es siempre el mismo. Una iteración número i del ciclo secundario puede tener un entrelazado distintos a otra iteración número j (i,j 1..k)

Las duraciones de ambos ciclos son valores enteros (se supone que el tiempo se mide en múltiplos enteros de alguna unidad de tiempo)

- La duración del ciclo principal se denomina **hiperperiodo (Tm)**. Es el mínimo común múltiplo de los periodos de todas las tareas, por tanto, los instantes de inicio de cada iteración del ciclo principal coinciden con los instantes en los cuales todas las tareas se activan de nuevo a la vez
- La duración del ciclo secundario se denomina Ts. Lógicamente, se debe cumplir **Tm = k \* Ts**  

#### Ejemplo e implementación

> | Tarea | T    | C    |
> | ----- | ---- | ---- |
> | A     | 25   | 10   |
> | B     | 25   | 8    |
> | C     | 50   | 5    |
> | D     | 50   | 4    |
> | E     | 100  | 2    |

Tm = mcm(25,50,100) =100			*El ciclo principal dura 100ms*

k= 4, 	Ts = 25ms							*4 ciclos secundarios de 15ms cada uno*

Asumimos D = T

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

#### Selección de duración del ciclo secundario

Para seleccionar un valor apropiado de Ts se deben tener en cuenta las siguientes restricciones y esta sugerencia:

- Restricciones:
  - El valor de Ts tiene que ser un divisor de Tm
  - El valor de Ts tiene que ser mayor o igual que el tiempo de cómputo C de cualquier tarea, es decir max(C1, C2..Cn) <= Ts
- Sugerencia: es aconsejable intentar en principio que el ciclo secundario sea menor o igual que el mínimo deadline (o periodo si no se proporciona deadline)

> El ejemplo anterior con un Ts=10ms y k=10:
>
> | 10   | 20   | 30   | 40   | 50   | 60   | 70   | 80   | 90   | 100  |
> | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
> | A    | BE   | CD   | A    | B    | A    | B    | CD   | A    | B    |

#### Propiedades de la planificación cíclica

- No hay concurrencia en la ejecución
  - Cada ciclo secundario es una secuencia de llamadas a procedimientos
  - No se necesita un núcleo de ejecución multitarea
- Los procedimientos pueden compartir datos
  - No necesitan mecanismos de EM como los semáforos o monitores
- No hace falta analizar el comportamiento temporal
  - El sistema es correcto por construcción

#### Problemas de la planificación cíclica

- Dificultad para incorporar tareas con periodos largos
- Las tareas esporádicas son difíciles de tratar
- El plan ciclo del proyecto es difícil de construir
  - Si los periodos son de diferentes órdenes de magnitud el número de ciclos puede ser muy grande
  - Puede ser necesario partir una tarea en varios procedimientos
  - Es el caso más general de sistemas en tiempo real críticos
- Es poco flexible y difícil de mantener
  - Cada vez que se cambia una tarea hay rehacer la planificación

### 2.2. PLANIFICACIÓN CON PRIORIDADES

La planificación con prioridades permite solventar los problemas anteriores. Cada tarea tiene asociado un entero positivo, su prioridad:

- Depende de sus atributos temporales y/o entrelazamiento entre las tareas
- Por convención se asignan números mayores a más prioridad
- Las prioridades de cada una de las tareas determinan la selección de la tarea que puede ejecutarse en el (único) procesador cuando este queda libre
- Las prioridades pueden ser constantes fijadas de antemano (estáticas) o pueden cambiar con el tiempo (dinámicas)

#### Planificación de tareas

En este tipo de planificaciones debe existir un componente software, el scheduler o planificador, capaz de:

- Asignar el procesador a una tarea activa o ejecutable (a esto se le llama despachar o dispatch una tarea)
- Desasignar la CPU a la tarea en ejecución, o desalojar
- Una tarea puede estar en varios estados (suspendido, ejecutable, ejecutándose)
- Las tareas ejecutables se despachan para su ejecución en orden de prioridad

El planificador actúa en cada instante de tiempo en el que una o más tareas se activan (pasan a ejecutable); o cuando la tarea en ejecución termina (pasa a suspendida). A continuación, selecciona cualquier tarea A con la mayor prioridad Pa entre todas las tareas ejecutables. Si la CPU está libre, A pasa a ejecutándose. Si hay otra tarea B ejecutándose, y esta es de prioridad menor, A se ejecuta y B pasa a ejecutable. Si la prioridad de B fuera mayor no habría cambio alguno.

#### Planificación RMS (Rate Monotonic Scheduling)

Es un método de planificación estático online con asignación mayor prioridad a las tareas más frecuentes (con menor periodo). Es óptima en caso de todas las tareas periódicas y T=D

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

Es un test de planificabilidad suficiente, se aplica para un sistema de **n tareas** periódicas independientes con prioridad RMS

- Se usa el factor de utilización U y el factor de utilización máximo (U0n)
  - U = sumatorio\[i=1..n] (Ci/Ti)
  - U0n = n *  (raíz n-ésima(2) - 1)
- El valor de U0(1) es 1. A partir de ahí decrece según crece n, hasta el límite que es ln2
- Un sistema pasa el test si U <= U0n. En caso contrario, no podemos confirmar nada y recurrimos a mirar el cronograma

#### Planificación EDF (Earliest Deadline First)

Es un esquema de planificación con prioridades dinámicas. El primero el más urgente, aquel cuya deadline está más cerca

- En caso de igualdad se hace una elección no determinista de la siguiente tarea
- Es más óptimo porque no es necesario que las tareas sean periódicas
- Es menos pesimista que RMS

##### Test de planificabilidad

En este caso el test Liu&Layland se puede aplicar también, y se convierte en un test exacto: si pasa el test es planificable; si no, no.

- En este caso el sistema pasa el test si U <= 1
- Esto implica que la planificación EDF puede aplicarse a más sistemas que la RMS (ya que la U0n siempre es menor que 1 en RMS)  