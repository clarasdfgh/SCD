# 2-Sincronización en memoria compartida

> 1.Introducción a la sincronización en memoria compartida
>
> 2.Semáforos para sincronización
>
> 3.Monitores como mecanismo de alto nivel
>
> 4.Soluciones software con espera ocupada para E.M.
>
> 5.Soluciones hardware con espera ocupada (cerrojos) para E.M.

En este tema estudiaremos soluciones de exclusión mutua y sincronización basadas en el uso de memoria compartida entre los procesos involucrados. Estas soluciones se dividen dos categorías:

-  Soluciones de bajo nivel con espera ocupada con bucles para realizar las esperas
-  Soluciones de alto nivel que parten de las anteriores. La sincronización se consigue bloqueando el proceso que tenga que esperar
   -  Semáforos
   -  Regiones críticas
   -  Monitores

Cuanto un proceso tiene que esperar, entra en un bucle indefinido del que no sale hasta que se cumplan las condiciones necesarias

### Semáforos para sincronización

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

### Estructura de un semáforo

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

> procedure sem_wait( s :semaphore);
>
> begin
>
> ​	if s.valor == 0 then	
>
> ​		bloquearme( s.procesos ) ;
>
> ​		s.valor := s.valor - 1 ;
>
> end 

>procedure sem_signal( s :semaphore);
>
>begin
>
>​	s.valor := s.valor + 1;	
>
>​	if not vacio( s.procesos ) then
>
>​		desbloquear_uno( s.procesos ) ;
>
>end 

Estas operaciones se ejecutan en EM, no puede haber dos procesos distintos ejecutando instr en un mismo semáforo

##### Limitaciones de los semáforos

Son muy útiles para problemas de exclusión mutua y sincronización sencilla, pero los problemas más complejos de sincronización son más difíciles de verificar; y un programa erróneo o malintencionado puede provocar estados de bloqueo indefinido o incorrectos.

### Monitores como mecanismo de alto nivel

Inconvenientes de usar mecanismos como los semáforos:

- Se basan en variables globales, lo cual impide el diseño modular y reduce la escalabilidad (incorporar procesos al programa)
- El uso y función de las variables no se hace explícito en el programa, lo cual dificulta razonar la correción del código
- Las operaciones están dispersas y sin proteger

Es necesario un mecanismo que permita encapsulación y acceso estructurado, que proporcione herramientas para manejar la EM y la sincronización

##### Estructura y funcionalidad de un monitor

Monitor: mecanismo de alto nivel que permite definir objetos abstractos compartidos, que incluyen:

	- Una colección de variables encapsuladas (datos) que representan un recurso compartido por varios procesos
	- Un conjunto de procedimientos para manipular el recurso, que afectan a las variables encapsuladas

Estos conjuntos de elementos permiten invocar los procedimientos de forma que se garantiza la EM y se implementa la sincronización requerida mediante esperas bloqueadas

Un monitor es un recurso compartido que se usa como un objeto al que se accede concurrentemente. Sus propiedades son:

- Acceso estructurado y encapsulación

  - El proceso solo puede acceder a un recurso mediante un conjunto de operaciones
  - El proceso ignora las variables que representan al recurso y la implementación de las operaciones asociadas

- Exclusión mutua en el acceso a procedimientos

  - La EM está **garantizada por definición**
  - La implementación del monitor garantiza que dos procesos nunca estarán ejecutando simultaneamente algún procedimiento del monitor (el mismo o distintos)

  ##### Ventajas sobre los semáforos

  El uso de monitores facilita el diseño e implementación:

  - Las variables están protegidas, solo se pueden modificar desde el monitor
  - La EM está garantizada, no hay que usar mecanismos explícitos de EM en el acceso a variables compartidas
  - Las operaciones de espera bloqueada y señalización se programan exclusivamente desde dentro del monitor

##### Sintaxis

El uso del monitor se hace exclusivamente con los procedimientos exportados.  Las variables permanentes y procedimientos no exportados no se pueden acceder desde fuera, por lo que la implementación de las operaciones se puede cambiar sin modificar la semántica.

```
monitor nombre_monitor		//Identificador
	var						//decl. vari. permanentes privadas
	.......;	
	export 					//nombres procedimientos
		nom_exp_1,
		nom_exp_2,	
		.......;

procedure nom_exp_1 ( ... )	//decl. e impl. procedimientos
	var .......;			//var. locales

	begin					//decl. resto procedimientos
		.......;
	end
	
begin						//init. var. permanentes
	.......;
end
```

- Variables permanentes: son el estado interno del monitor y sólo se pueden acceder desde este mismo (en métodos e inicialización)
- Procedimientos: modifican el estado interno en EM, pueden tener variables y parámetros locales. Algunos constituyen la interfaz externa del monitor pueden ser llamados por otros procesos que compartan recurso
- Código de inicialización: fija el estado interno inicial, es opcional y sólo se ejecuta una vez, al principio antes de cualquier llamada a métodos

> ```
> monitor variableCompartida;
> var x : int;					//var. permanente
> export incremento, valor;		
> 
> procedure incremento();
> begin
> 	x := x+1;
> end
> 
> function valor() : int;
> begin
> 	result := x
> end
> 	
> begin 							//inicialización
> 	x := 0 						
> end
> ```

>>```
>>//EJEMPLO DE USO
>>monitor : VarCompartida;
>>.......;
>>
>>monitor.incremento();
>>k := monitor.valor();
>>```

#### Funcionamiento 

**Comunicación monitor-mundo exterior**: cuando un proceso necesita operar sobre un recurso compartido del monitor, deberá realizar una llamada a un procedimento exportado por el monitor.

**Exclusión mutua**: si un proceso está dentro de un monitor, cualquier otro proceso que llame a un procedimiento de ese monitor deberá esperar a que el primero salga del mismo. Esto asegura que las variables permanentes nunca serán accedidas concurrentemente.

**Objetos pasivos**: tras ejecutarse la inicialización, el monitor es un obj pasivo y sus procedimientos solo se ejecutan cuando son llamados

**Instanciación**: podemos crear múltiples instancias independientes en un monitor. Cada instancia tiene variables permanentes propias y EM por separado.

>```
>class monitor VarComp(pini, pinc : int)
>var x, inc : int
>export incremento, valor;
>
>procedure incremento();
>begin
>	x := x+inc;
>end
>
>function valor() : int;
>begin
>	result := x
>end
>	
>begin
>	x := pini;
>    inc := pinc
>end
>```

##### Cola del monitor para exclusión mutua

El control de la EM se basa en la existencia de la cola del monitor:

- Si un proceso está dentro del monitor y otro proceso intenta ejecutar un método del monitor, este último proceso queda bloqueado y se inserta a la cola
- Cuando un proceso abandona el monitor, se desbloquea un proceso de la cola
- Si la cola está vacía el monitor está libre, y el siguiente proceso que llame a un método entrará al monitor
- Para garantizar la vivacidad, la cola sigue política FIFO

##### Diagrama de estados

![image-20191120004634251](/home/clara/.config/Typora/typora-user-images/image-20191120004634251.png)

El estado del monitor incluye la cola de procesos esperando a ejecutar el código del mismo

#### Sincronización en monitores

Para implementar la sincronización se requieren herramientas para la espera bloqueada. En semáforos usábamos sem_wait y sem_signal y el valor del semáforo. 

En monitores sólo se dispone de sentencias de bloqueo y activación, y los valores de las variables permanentes para determinar las condiciones

##### Variables condition

Podemos declarar variables permanentes tipo condition. Cada var condition tiene asociada una lista o cola de procesos en espera hasta que la condición sea cierta, y pueden invocar las operaciones wait y signal (esperar a que ocurra la condición, señalar que la condición ocurre)

Dada una variable cond, se definen las siguientes operaciones como mínimo:

- cond.wait(): bloqueo incondicional al proceso que la llama y lo lleva a la cola de cond
- cond.signal(): si hay procesos bloqueados libera uno. Si no, no se hace nada.
- cond.queue(): función lógica que devuelve true si hay procesos en la cola

##### Espera bloqueada y EM

Ya que los procesos pueden estar dentro del monitor pero bloqueados...

- Cuando se llama wait y queda bloqueado, se debe liberar la EM del monitor, si no se produce interbloqueo con seguridad

- Cuando un proceso es reactivado tras una espera, adquiere de nuevo EM antes de ejecutar la sentencia siguiente a wait

  ![image-20191120010912431](/home/clara/.config/Typora/typora-user-images/image-20191120010912431.png)

#### Verificación de monitores

Verificar la corrección de un programa concurrente con monitores requiere probar la corrección de cada monitor, cada proceso y la ejecución concurrente de los procesos. 

No conocemos a priori la interfoliación concreta de las llamadas de los métodos del monitor, así que utilizaremos un invariante del monitor: es una propiedad que el monitor cumple siempre

##### Invariante del monitor (IM)

- Es una función lógica en cada estado del monitor a lo largo de la ejecución concurrente

- Su valor depende de la traza del monitor y de los valores de las var permanentes de este

  - La traza es la secuencia ordenada en el tiempo de las llamadas a procedimientos del monitor ya completadas, desde el inicio del programa hasta llegar al estado indicao

- El IM debe ser cierto en cualquier estado del programa concurrente, excepto cuando un proceso está ejecutando código del monitor en EM (porque se están actualizando las var permanentes)

  - Determina qué trazas y/o estados son posibles en un monitor
  - Puede incluir referencias a las variables y/o traza
    - Ej. num de veces que se ha ejecutado un procedimiento, último procedimiento ejecutado
  - Debe ser cierto...
    - En el estado inicial
    - Antes y después de cada llamada a procedimientos
    - Antes y después de cada operación wait y signal
      - Justo antes de signal sobre una var cond debe, además, ser cierta la cond lógica asociada a esta variable

  #### Patrones de solución con monitores

  ##### Espera única (EU)

  Un proceso antes de ejecutar una sentencia debe esperar a que otro proceso acabe otra sentencia (productor-consumidor)

  > ```
  > monitor EU;
  > var	terminado: 	bool;
  > 	cola:		condition;
  > export 	esperar,
  > 		notificar;
  > 		
  > procedure esperar();
  > begin
  > 	if not terminado then
  > 		cola.wait();
  > 	end
  > 
  > procedure notificar();
  > begin
  > 	terminado:= true;
  > 	cola.signal();
  > end
  > 
  > begin
  > 	terminado:= false;
  > end
  > ```

  > >```
  > >//Variable compartida
  > >var x: int;
  > >```
  > >
  > >```
  > >process Productor;
  > >var a: int;
  > >begin
  > >	a:= ProducirValor();
  > >	x:= a;
  > >	EU.notificar();
  > >end
  > >```
  > >
  > >```
  > >process Consumidor;
  > >var b: int;
  > >begin
  > >	EU.esperar();
  > >	b:= x;
  > >	UsarValor(b);
  > >end
  > >```

##### Exclusión mutua (EM)

Acceso en EM a una sección crítica por parte de un número arbitrario de procesos

> ```
> monitor EM;
> var	ocupada: 	bool;
> 	cola:		condition;
> export 	entrar,
> 		salir;
> 		
> procedure entrar();
> begin
> 	if ocupada then
> 		cola.wait();
> 		ocupada:= true
> 	end
> 
> procedure salir();
> begin
> 	ocupada:= false;
> 	cola.signal();
> end
> 
> begin
> 	ocupada:= false;
> end
> ```

> >```
> >process Usuario[i: 0..n];
> >begin
> >	while true do begin
> >		EM.entrar();
> >		...;
> >		EM.salir();
> >		...;
> >	end
> >end
> >```

##### Productor-Consumidor 

Similar a EU pero de forma cíclica

> ```
> process Productor;
> var a: int;
> begin
> 	while true do begin
> 		a:= ProducirValor();
> 		PC.escribir(a);
> 	end
> end
> ```
>
> ```
> process Consumidor;
> var b: int;
> begin
> 	while true do begin
> 		PC.leer(b);
> 		UsarValor(b);
> 
> 	end
> end
> ```
>
> ```
> monitor PC;
> var	valor_com: 	int;
> 	pendiente: 	bool;
> 	cola_prod:	condition;
> 	cola_cons:	condition;
> export 	esperar,
> 		notificar;
> 		
> procedure escribir(v: int);
> begin
> 	if pendiente then
> 		cola_prod.wait();
> 		valor_com:= v;
> 		pendiente:= true;
> 		
> 		cola_cons.signal();
> 	end
> end
> 
> function leer(): int;
> begin
> 	if not pendiente then
> 		cola_cons.wait();
> 		result:= valor_com;
> 		pendiente:= false;
> 		
> 		cola_prod.signal();
> 	end
> end
> 
> begin
> 	pendiente := false;
> end
> ```



#### Colas de condición con prioridad

Hemos visto que la cola de los monitores sigue política FIFO, pero en ciertas ocasiones puede convenir una cola organizada por prioridad. Se la daremos tal que ```cond.wait(p)```, siendo p un entero no negativo que refleja la prioridad (a más pequeño, más prioridad). 

Debemos tener particular cuidado con la inanición, pero la lógica del programa es la misma.

###### Reloj despertador

>```
>Monitor Despertador
>	var	ahora:		int;
>		despertar:	condition;
>	export  despiertame,
>			tick;
>			
>procedure despiertame(n: int);
>	var alarma: int;
>begin
>	alarma:= ahora+n;
>	while ahora < alarma do
>		despertar.wait();
>	end
>	despertar.signal();
>end
>
>procedure tick();
>begin
>	ahora:= ahora+1;
>	despertar.signal();
>
>begin
>	ahora:= 0;
>end
>```



#### Semántica de las señales de los monitores

Cuando un proceso hace signal en una cola no vacía, se denomina proceso *señalador*. El proceso que esperaba en cola que se activa se denomina *señalado*. 

Suponemos que hay código restante tras el wait y el signal. Inmediatamente después de señalar **no es posible** que señalador y señalado continuen la ejecución de su código. La semántica de señales es la política que establece la forma concreta en la que se resuelve un conflicto tras hacer signal en una cola no vacía.

##### Señalar y continuar (SC)

> Señalador continúa la ejecución tras el signal, señalado espera bloqueado

![image-20191120032028640](/home/clara/.config/Typora/typora-user-images/image-20191120032028640.png)

- Tanto el señalador como otros procesos pueden hacer falsa la condición después de que el señalado abandone la cola condición
- Por tanto, en el proceso señalado no se puede garantiar que la condición asociada a cond es cierta al terminar cond.wait(), y es necesario comprobarla antes
- Esta semántica obliga a programar la operación wait en bucle
  - ```while not cond_logica_desbloqueo do```
  - ```cond.wait```

##### Señalar y salir (SS)

> Señalado se reactiva inmediatamente, señalador abandona el monitor tras hacer signal sin ejecutar el código siguiente

![image-20191120032048827](/home/clara/.config/Typora/typora-user-images/image-20191120032048827.png)

- En este paso la operación signal conlleva liberar al proceso señalado, y terminar el procedimiento del monitor que estaba ejecutando el proceso señalador
- Está asegurado el estado que permite al proceso señalado continuar la ejecución del procedimiento del monitor en el que se bloqueó (se cumple la condición de desbloqueo)
- Esta semántica condiciona el estilo de programación ya que obliga a colocar siempre la operación signal como última instrucción de los procedimientos de monitor que la usen

##### Señalar y esperar (SE)

> Señalado se reactiva inmediatamente, señalador queda bloqueado en la cola del monitor

![image-20191120032102695](/home/clara/.config/Typora/typora-user-images/image-20191120032102695.png)

- Está asegurado el estado que permite al proceso señalado continuar la ejecución del procedimiento del monitor en el que se bloqueó
- El proceso señalador entra en la cole de procesos del monitor, por lo que está al mismo nivel que el resto de procesos que compiten por la EM del monitor
- Se puede considerar una semántica injusta para el proceso señalador, ya que este ya había obtenido el acceso al monitor y debería tener prioridad antes que otros procesos que compiten por el monitor

##### Señalar y espera urgente (SU)

> Señalado se reactiva inmediatamente, señalador queda bloqueado en una cola específica con una prioridad mayor que otros procesos

![image-20191120032118335](/home/clara/.config/Typora/typora-user-images/image-20191120032118335.png)

- Similar a SE, pero intenta corregir el problema de inequitatividad
- Está asegurado el estado que permite al proceso señalado continuar al ejecución del procedimiento del monitor en el que se bloqueó
- El proceso señalador entra en una nueva cola de procesos que esperan para acceder al monitor, la cola de urgentes
  - Los procesos de esta cola tienen prioridad frente a los procesos que esperan en la cola del monitor
  - ![image-20191120124609764](/home/clara/.config/Typora/typora-user-images/image-20191120124609764.png)

### Soluciones software con espera ocupada para EM

Para simplificar análisis, se hacen estas suposiciones

- Cada proceso tiene una única SC, formada por un único bloque contiguo de instrucciones
- Proceso es un bucle infinito que ejecuta dos pasos repetidamente
  - Sección crítica (con PE antes y el PS después)
  - Resto de sentencias: se emplea un tiempo arbitrario no acotado, eincluso el procesopodría finalizar en esta sección.
- No suposición sobre cuántas veces un proceso intenta acceder SC.En general, es deseable que el tiempo empleado en SC sea el menor posible.

##### Refinamiento sucesivo de Dijkstra

El Refinamiento sucesivo de Dijskstra hace referencia a una serie de algoritmos que intentan resolver el problema de EM.-Comienza con una versión muy simple,incorrecta (no cumple alguna propiedad), y se hacen sucesivas mejoras para intentar cumplir las 3 propiedades mínimas.(EM, PROGRESO Y ESPERA LIMITADA)

- La versión final correcta se denomina Algoritmo de Dekker.
- Por simplicidad, únicamente algoritmos para 2 procesos.
- Suponemos 2 procesos, P0 y P1, cada uno ejecuta bucle infinito conteniendo: PE, SC, PS y RS.

###### Versión 1:

Usa variable lógica compartida p01 sc que valdrá true solo si algún proceso está en SC

###### Versión 2: 

Se usará una única variable lógica (turno0),cuyo valor indicará cuál debe entrar a SC. Valdrá true si es P0, y false si es P1.

###### Versión 3:

Para impedir la alternancia, se usan dos variables lógicas (p0sc, p1sc) en lugar de solo una. Cada variable vale true si el correspondiente procesoestá en SC.

###### Versión 4:

Para solucionar el problema anterior se puede cambiar el orden de las dos sentencias del PE. Variables lógicas p0sc y p1sc están a true cuandoel correspondiente proceso está en SC o intenta entrar a SC desde PE.

###### Versión 5:

Para solucionarlo, si un proceso ve que elotro quiere entrar, el primero pone su variable temporalmente a false.

##### Algoritmo de Dekker

El algoritmo de Dekker, es un algoritmo correcto (cumple propiedades mínimas). Se puede considerar que el resultado final del refinamiento de Dijkstra:

- Incorpora espera de cortesía en cada proceso (como versión 5), durante la cual cede al otro la posibilidad de acceder SC, cuando ambos coinciden en PE.

- Evita interbloqueos mediante variable turno: la espera de cortesía solo la realiza uno de los dos procesos de forma alterna.–Variable turno permite también detectar final de espera decortesía, implementada mediante espera ocupada.

##### Algoritmo de Peterson

El algoritmo de Peterson, es otro algoritmocorrectopero más simple y más eficiente que el algoritmo de Dekker.

- También usa dos variables lógicas que expresan la presencia de cada proceso en PE o SC, más una variable de turno que permite romper el interbloqueo en caso de acceso simultáneo al PE.
- Asignación de turno se hace al inicio del PE en lugar de en PS, con lo cual, en caso de acceso simultáneo al PE, el último proceso en ejecutar la asignación (atómica) al turno da preferencia al anterior.
- A diferencia de Dekker, el PE no usa dos bucles anidados, sino que unifica ambos en uno solo.

### Soluciones hardware con espera ocupada para EM -Cerrojos

Los cerrojos son una solución hardware basada en espera ocupada que puede usarse en procesos concurrentes de memoria compartida para solucionar el problema de EM. 

El cerrojo es un  **valor lógico en posición de memoria compartida** que  indica si algún proceso está en SC. En el resto de sentencias se actualiza el cerrojo para reflejar SC libre.

```
var sc_ocupada: bool:= false;

process P[i: 1..n]
begin
	while true do begin
		while TestAndSet(sc_ocupada) do begin end
		sc_ocupada:= false;
	end
end
```

##### TestAndSet

Es una instrucción máquina disponible en algunos procesadores. Recibe como argumento la dirección de memoria de la variable lógica que actua como cerrojo, se invoca desde un lenguaje de alto nivel y ejecuta las siguientes acciones: leer el valor anterior del cerrojo, ponerlo a true, devolver el valor anterior. Durante su ejecución, ningún otro método o instrucción puede tocar el cerrojo, es atómica.

##### Uso y desventajas de los cerrojos

Son una solución válida para EM: consumen poca memoria, son eficientes y sirven para cualquier número de procesos. Sin embargo, presenta desventajas:

- La espera ocupada consume tiempo de CPU que se podría dedicar a otros procesos
- Se puede acceder directamente a los cerrojos y por tanto un programa erróneo o malintencionado puede poner un cerrojo en un estado incorrecto, dejando a otros procesos en espera ocupada de forma indefinida
- No cumple ciertas condiciones de equidad

Estas desventajas hacen que su uso se haya restringido por seguridad y eficiencia

- **Seguridad:** solo se usan desde componentes software que forman parte del SO, librerías de hebras, tiempo real... ya que suelen estar bien comprobados y libres de errores
- **Eficiencia:** solo en casos en los que la ejecución de la SC es muy corta