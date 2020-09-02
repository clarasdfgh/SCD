# SCD TEMA 3: Sistemas basados en paso de mensajes

> 1.  Mecanismos básicos en sistemas basados en paso de mensajes
> 2.  Paradigmas de interacción de procesos en programas distribuidos
> 3.  Mecanismos de alto nivel en sistemas distribuidos

## 1. Mecanismos básicos en sistemas basados en paso de mensajes

#### Memoria compartida vs. Sistema distribuido

- **Sistemas multiprocesador de memoria compartida:**
  - Más fácil programación (comparten variables) mediante cerrojos, semáforos y monitores
  - Implementación más costosa y escalabilidad hardware limitada, el acceso a memoria común es un cuello de botella

![image-20200121163212222](/home/clara/.config/Typora/typora-user-images/image-20200121163212222.png)

- **Sistemas distribuidos:**
  - Facilita la distribución de datos y recursos
  - Soluciona el problema de escalabilidad y coste
  - Mayor dificultad de programación: no hay direcciones de memoria comunes y mecanismos como los monitores son inviables

![image-20200123174042615](/home/clara/.config/Typora/typora-user-images/image-20200123174042615.png)

### Necesidad de una notación de programación distribuida

Se usan tres notaciones:

- Lenguajes tradicionales (de memoria compartida):
  - Operadores de asignación (cambio del estado interno de la máquina)
  - Estructuración (como secuencia, repetición, alternación...) 
- Envío y recepción (entorno externo):
  - Tan importantes como la asignación
  -  Permiten comunicar procesos que se ejecutan en paralelo
- Paso de mensajes: 
  - Abstracción (oculta hardware con una red de interconexión)
  - Portabilidad (se puede implementar eficientemente en cualquier arquitectura)
  - No requiere de mecanismos para asegurar la exclusión mutua

### 1.1. VISTA LÓGICA DE LA ARQUITECTURA Y MODELO DE EJECUCIÓN

#### Vista lógica de la arquitectura

Existen N procesos, cada uno con su propio espacio de direcciones (memoria). Los procesos se comunican mediante envío y recepción de mensajes.

![image-20200121173650550](/home/clara/.config/Typora/typora-user-images/image-20200121173650550.png)

- En un mismo procesador pueden residir físicamente varios procesos
- Por motivos de eficiencia, frecuentemente se adopta la política de alojar un único proceso en cada procesador disponible
- Cada interacción requiere cooperación entre 2 procesos: el emisor de los datos debe intervenir aunque no haya conexión lógica con el evento tratado en el receptor

#### Estructura de un programa de paso de mensajes

Diseñar un código diferente para cada proceso puede ser complejo. Una solución es el estilo **SPMD (Single Program Multiple Data)**: todos los procesos ejecutan un mismo código fuente, pero cada uno procesa datos distintos o ejecutar flujos de control distintos.

![image-20200121191628132](/home/clara/.config/Typora/typora-user-images/image-20200121191628132.png)

```
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

```
process ProcesoA ; 
	var var_orig : integer ; 
begin 
	var_orig := Produce(); 
	send( var_orig, ProcesoB ); 
end
```

```
process ProcesoB ; 
	var var_dest : integer ; 
begin 
	receive( var_dest, ProcesoA ); 
	Consume( var_dest ); 
end
```

### 1.2. PRIMITIVAS BÁSICAS DEL PASO DE MENSAJES

El paso de un mensaje entre dos procesos constituye una transferencia de una secuencia finita de bytes

![image-20200123094450841](/home/clara/.config/Typora/typora-user-images/image-20200123094450841.png)

- Se leen de una variable del proceso emisor (var_orig) y se escriben en una variable del proceso receptor (var_dest). Ambas variables son del mismo tipo
- Se transfieren a través de una red de interconexión
- Implica sincronización: los bytes acaban de recibirse después de iniciado el envío
- El efecto neto final es equivalente a una hipotética asignación var_dest = var_orig, si se pudiera hacer

#### Primitivas básicas del paso de mensajes

El proceso emisor realiza el envío invocando a **send**, y el receptor invocando a **receive**. Su sintaxis es la siguiente:

```
send(variable_origen, identificador_proceso_destino)
receive(variable_destino, identificador_proceso_origen)
```

Cada proceso nombra explícitamente al otro, indicando su nombre de proceso como id

```
process P1;
	var var_orig: integer;
begin
	var_orig := Produce();
	send(var_orig, P2);
end
```

```
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

  ```
  process P1;
  	var var_orig: integer;
  begin
  	var_orig := Produce();
  	send(var_orig, P2);
  end
  ```

  ```
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

  ```
  var buzon: channel of integer;
  ```

  ```
  process P1;
  	var var_orig: integer;
  begin
  	var_orig := Produce();
  	send(var_orig, buzon);
  end
  ```

  ```
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

```
process Emisor;
	var var_orig: integer = 100;
begin
	send(var_orig, Receptor);
	var_orig = 0;
end
```

```
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

```
i_send(var_orig, id_proceso_receptor, var_resguardo)
```

Indica el SPM que comience una operación de envía al receptor: se registra la solicitud de envío (SE) y acaba. No espera a FL ni a nada del receptor. Var_resguardo permite consultar después el estado del envío.

```
i_receive(var_dest, id_proceso_emisor, var_resguardo)
```

Indica al SPM que se inicie una recepción de un mensaje del emisor: se registra la SR, no espera FE ni ninguna acción del emisor. Var_resguardo permite consultar después el estado de la recepción

![image-20200123225243845](/home/clara/.config/Typora/typora-user-images/image-20200123225243845.png) 

En general, las operaciones asíncronas tardan mucho menos que las síncronas, ya que simplemente suponen el registro de la SE o SR

#### Esperar al final de operaciones asíncronas

Cuando el proceso hace i_send o i_receive puede continuar trabajando hasta que llega un momento ene l que debe esperar a que termine la operación. Se dispone de estos dos procedimientos:

```
wait_send(var_resguardo);
wait_recv(var_resguardo);
```

En ambos casos, se invoca a proceso emisor/receptor, y lo bloquea hasta que la operación de envío/recepción asociada a var_resguardo ha llegado al instante FL/FE (momento en el cual vuelve a ser seguro usar la variable)

![image-20200123230005032](/home/clara/.config/Typora/typora-user-images/image-20200123230005032.png)

> wait_send no espera emparejamiento: es seguro, pero no síncrono

Estas operaciones permiten a los procesos emisor y receptor hacer trabajo útil concurrentemente con la operación de envío o recepción. Mejora el tiempo de espera ociosa para poder aprovecharlo, pero hay que reestructurar el programa.

#### Consulta de estado de operaciones asíncronas

```
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

### 1.3. ESPERA SELECTIVA

//TODO

## 2. Paradigmas de interacción de procesos en programas distribuidos

**Paradigma de interacción:** define un esquema de interacción entre procesos y una estructura de control que aparece en muchos programas. Unos pocos paradigmas se utilizan repetidamente para desarrollar muchos programas distribuídos.

- Maestro-esclavo
- Iteración síncrona
- Encauzamiento (pipelining)
- Cliente-servidor

### 2.1. MAESTRO-ESCLAVO

Dos entidades: un proceso maestro y múltiples procesos esclavos

- El proceso maestro descompone el problema en pequeñas subtareas, las distribuye entre los procesos esclavos y recibe los resultados parciales de estos
- Los procesos esclavos ejecutan un ciclo muy simple hasta que el maestro informa del final del cómputo
  - Reciben mensaje con la tarea > Procesan la tarea > Envían el resultado

```
process Maestro; 
begin 
	for i=0 to num_esclavos-1 do 
		send(fila, Esclavo[i]);		//enviar trabajo a esclavo 
	while queden filas sin colorear do 
		select 
			for j=0 to ne−1 when receive(colores, Esclavo[j]) do 
				if quedan filas en la bolsa 
					then send(fila, Esclavo[j]);
					else send(fin, Esclavo[j]); 
				visualiza(colores); 
		end
end
```

```
process Esclavo[ i : 0..num_esclavos-1 ] ; 
begin 
	receive( mensaje, Maestro ); 
	while mensaje != fin do begin 
		colores := calcula_colores(mensaje.fila) ; 
		send (colores, Maestro ); 
		receive( mensaje, Maestro ); 
	end
end
```

### 2.2. ITERACIÓN SÍNCRONA

En múltiples programas numéricos, un cálculo se repite y cada vez se obtiene un resultado que se emplea en el siguiente cálculo. Estos cálculos se pueden realizar concurrentemente:

- En un bucle diversos procesos comienzan juntos en el inicio de cada iteración
- La siguiente iteración no puede comenzar hasta que todos los procesos hayan acabado la previa
- Los procesos suelen intercambiar información en cada iteración

### 2.3. ENCAUZAMIENTO

El problema se divide en una serie de tareas que se han de completar una detrás de otra. Cada tarea se ejecuta en un proceso separado.

Los procesos se organizan en un cauce (pipeline) donde cada proceso se corresponde con una etapa del cauce y es responsable de una tarea particular. Cada etapa del cauce contribuye al problema globar y devuelve información necesaria para las etapas posteriores del cauce.

Es un patrón de comunicación muy simple, ya que establece un flujo de datos entre tareas adyacentes en el cauce.

## 3. Mecanismos de alto nivel en sistemas distribuidos

Veremos mecanismos de mayor nivel de abstracción que los anteriores

- Llamada a procedimiento remoto (RPC)
- Invocación remota de métodos (RMI)

Ambos están basados en el método habitual por el cual un proceso llamador hace una llamada a procedimiento como sigue:

- El llamador indica el nombre del procedimiento y los valores de los parámetros
- El proceso llamador ejecuta el código del procedimiento
- Cuando el procedimiento termina, el llamador obtiene los resultados y continúa ejecutando el código tras la llamada

#### Llamada a procedimientos remotos

En el modelo de invocación remota o llamada a procedimiento remoto (RPC) se dan los mismos pasos, pero es otro proceso (el proceso llamado) el que ejecuta el código del procedimiento:

- El llamador indica el nombre del procedimiento y los valores de los parámetros
- El proceso llamador queda bloqueado. El proceso llamado ejecuta el código del procedimiento
- Cuando termina el procedimiento, el llamador obtiene los resultados y continua ejecutando el código tras las llamada

El RPC tiene un flujo de comunicación bidireccional (petición-respuesta) y permite que varios procesos invoquen un procedimiento gestionado por otro proceso (n-1)

### 3.1. EL PARADIGMA CLIENTE-SERVIDOR

Es el paradigma más frecuente en programación distribuida. Hay una relación asimétrica entre dos procesos: cliente y servidor

- **Proceso servidor:** gestiona un recurso (p.ej. una base de datos) y ofrece un servicio a otros procesos (clientes) para permitir que puedan acceder al recurso. Puede estar ejecutándose durante largos periodos de tiempo, pero no hace nada útil mientras espera peticiones de los clientes
- **Proceso cliente:** necesita el servicio y envía un mensaje de petición al servidor solicitando algo asociado al servicio proporcionado por este (p.ej. una consulta a la base de datos)

```
process Cliente[ i : 0..n − 1 ] ; 
begin 
	while true do begin 
		s_send( peticion, Servidor ); 
		receive( respuesta, Servidor ); 
	end 
end 
```

```
process Servidor ; 
begin 
	while true do 
		select 
			for i=0 to n-1 when condicion[i] receive(peticion, Cliente[i]) do 
				respuesta := servicio( peticion ) ; 
				s_send( respuesta, Cliente[i] ), 
		end 
end
```

No obstante, plantea problemas de seguridad:

- Si el servidor falla, el cliente queda esperando una respuesta que nunca llegará
- Si un cliente no invoca receive(respuesta, Servidor) y el servidor realiza s_send(respuesta, Cliente[j]) síncrono, el servidor queda bloqueado

Para resolver estos problemas:

- El par (recepción de petición, envío de respuesta) se debe considerar como una única operación de comunicación bidireccional en el servidor y no cómo dos órdenes separadas
- El mecanismo de llamada a procedimiento remoto (RPC) proporciona una solución en esta línea

### 3.2. LLAMADA A PROCEDIMIENTO (RPC)

**Llamada a procedimiento remoto (Remote Procedure Call):** mecanismo de comunicación entre procesos que sigue el esquema cliente-servidor y que permite realizar las comunicaciones como llamadas a procedimientos convencionales (locales). 

La principal diferencia respecto a una llamada a un procedmiento local es que el programa cliente que invoca el procedimiento y el invocado (que corre en el servidor) pueden pertenecer a distintas máquinas del sistema distribuído.

//TODO

### 3.3. JAVA REMOTE METHOD INVOCATION (RMI)

//TODO

### 3.4. SERVICIOS WEB

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