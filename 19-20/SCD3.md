# 3-Sistemas basados en paso de mensajes

>1.Mecanismos básicos en sistemas basados en paso de mensajes
>
>2.Paradigmas de interacción de procesos en programas distribuidos
>
>3.Mecanismos de alto nivel en sistemas distribuídos



## 1. Mecanismos básicos en sistemas basados en paso de mensajes

## 2. Paradimas de interacción de procesos en programas distribuidos

**Paradigma de interacción:** define un esquema de interacción entre procesos y una estructura de control que aparece en muchos programas. Unos pocos paradigmas se utilizan repetidamente para desarrollar muchos programas distribuídos.

- Maestro-esclavo
- Iteración síncrona
- Encauzamiento
- Cliente-servidor

### Maestro-esclavo

Dos entidades: un proceso maestro y múltiples procesos esclavos

- - proceso maestro descompone el problema en pequeñas subtareas, las distribuye entre los procesos esclavos y recibe los resultados parciales de estos
- Los procesos esclavos ejecutan un ciclo muy simple hasta que el maestro informa del final del cómputo
  - Reciben mensaje con la tarea > Procesan la tarea > Envían el resultado

### Iteración síncrona

En múltiples programas numéricos, un cálculo se repite y cada vez se obtiene un resultado que se emplea en el siguiente cálculo. Estos cálculos se pueden realizar concurrentemente

- En un bucle diversos procesos comienzan juntos en el inicio de cada iteración
- La siguiente iteración no puede comenzar hasta que todos los procesos hayan acabado la previa
- Los procesos suelen intercambiar información en cada iteración

## 3.Mecanismos de alto nivel en sistemas distribuídos

Veremos mecanismos de mayor nivel de abstracción

- Llamada a procedimiento remoto (RPC)
- Invocación remota de métodos