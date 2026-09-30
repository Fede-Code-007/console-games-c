# 🎮 Juegos de Consola en C

Hola👋, hoy les traigo una colección de juegos de consola que desarrolle en **C durante 2022**, como parte de mis primeros proyectos de programación 😁.

> 📌 Estos proyectos corresponden a una etapa inicial de mi formación en programación y se conservan meramente como registro de aprendizaje y evolución técnica.

---

## 📚 Juegos incluidos

El repositorio contiene cuatro juegos clásicos implementados completamente en C:

| Juego            | Descripción                                                           | Modalidades      |
| ---------------- | --------------------------------------------------------------------- | ---------------- |
| 🟡 **4 en Raya** | Juego de tablero donde se deben conectar cuatro fichas consecutivas.  | PC / 2 jugadores |
| 🏓 **Pong**      | Versión de consola del clásico juego de tenis de mesa.                | PC / 2 jugadores |
| 🐍 **Snake**     | Juego de la serpiente con frutas, puntuaciones y ranking persistente. | 1 jugador        |
| ❌ **Tateti**     | Implementación del clásico tres en raya.                              | PC / 2 jugadores |

---

## 🛠️ Tecnologías utilizadas

* **Lenguaje:** C
* **IDE:** Dev-C++
* **Plataforma principal:** Windows
* **Entrada/salida:** Consola

### Librerías utilizadas

Dependiendo del juego se utilizaron diferentes librerías estándar y específicas de Windows:

* `stdio.h` — entrada y salida estándar.
* `stdlib.h` — utilidades generales, memoria y ejecución de comandos.
* `string.h` — manipulación de cadenas.
* `stdbool.h` — utilización del tipo booleano.
* `time.h` — generación de valores aleatorios.
* `ctype.h` — conversión y validación de caracteres.
* `conio.h` — lectura de teclado en tiempo real.
* `windows.h` — interacción con la consola de Windows.
* `mmsystem.h` — reproducción de sonidos en **Snake**.

---

## 🧠 Conceptos de programación aplicados

Estos proyectos fueron desarrollados utilizando principalmente conceptos fundamentales de la programación imperativa:

* Variables y constantes.
* Tipos de datos.
* Operadores y expresiones.
* Estructuras condicionales (`if`, `else`, `switch`).
* Bucles (`for`, `while`, `do while`).
* Funciones.
* Arrays unidimensionales y multidimensionales.
* Punteros.
* Estructuras (`struct`).
* Enumeración y manipulación de datos.
* Generación de números pseudoaleatorios.
* Manejo de cadenas.
* Entrada y salida por consola.
* Validación de entradas.
* Manejo de archivos.
* Persistencia de información mediante archivos binarios.
* Manejo de eventos de teclado.
* Manipulación de la consola de Windows.

---

## 🎮 Descripción de los juegos

### 🟡 4 en Raya

Implementación del clásico **Cuatro en Raya** utilizando un tablero de **7 columnas × 6 filas**.

El jugador puede elegir entre:

* **Singleplayer:** jugar contra la PC.
* **Multiplayer:** dos jugadores en la misma computadora.

Las fichas se introducen seleccionando una columna y caen hasta la posición disponible más baja.

El programa comprueba automáticamente las cuatro posibles direcciones de victoria:

* Horizontal.
* Vertical.
* Diagonal descendente.
* Diagonal ascendente.

La PC selecciona aleatoriamente una de las columnas disponibles.

---

### 🏓 Pong

Versión de consola del clásico **Pong**, desarrollada específicamente para Windows.

Incluye:

* Modo contra la PC.
* Modo para dos jugadores.
* Sistema de puntuación.
* Cantidad de goles configurable.
* Movimiento de la pelota.
* Rebotes contra las paredes.
* Colisiones con las raquetas.
* Control mediante teclado.
* Renderizado en tiempo real dentro de la consola.

#### Controles

**Jugador 1**

```text
W → Mover hacia arriba
S → Mover hacia abajo
```

**Jugador 2**

```text
↑ → Mover hacia arriba
↓ → Mover hacia abajo
```

En el modo singleplayer, la raqueta de la PC se mueve automáticamente mediante un comportamiento de patrullaje.

---

### 🐍 Snake

Implementación del clásico **Snake** para consola de Windows.

El jugador controla una serpiente que debe comer frutas para aumentar su tamaño y conseguir puntos.

Incluye:

* Movimiento en tiempo real.
* Control mediante `W`, `A`, `S`, `D` o las flechas.
* Crecimiento de la serpiente.
* Detección de colisiones.
* Generación aleatoria de frutas.
* Sistema de puntuación.
* Ranking persistente de jugadores.
* Almacenamiento mediante archivo binario.
* Reproducción de efectos de sonido.
* Interfaz de consola con colores.

#### Controles

```text
W / ↑ → Arriba
S / ↓ → Abajo
A / ← → Izquierda
D / → → Derecha
```

Las puntuaciones se almacenan en:

```text
ArchivoPuntuaciones.dat
```

El ranking conserva hasta **20 registros**, ordenados de mayor a menor puntuación.

---

### ❌ Tateti

Implementación del clásico **Tres en Raya** utilizando un tablero de **3 × 3**.

El programa permite elegir entre:

* **Singleplayer:** jugar contra la PC.
* **Multiplayer:** dos jugadores.

Las posiciones del tablero se identifican mediante las letras:

```text
A | B | C
---------
D | E | F
---------
G | H | I
```

El programa comprueba las **8 combinaciones posibles de victoria**:

* 3 filas.
* 3 columnas.
* 2 diagonales.

En el modo singleplayer, la PC selecciona aleatoriamente una de las casillas disponibles.

---

## 📁 Estructura del proyecto

```text
.
├── 4 en RAYA/
│   ├── 4enRaya.c
│   └── 4enRaya.exe
│
├── PONG/
│   ├── pong.c
│   └── pong.exe
│
├── SNAKE/
│   ├── ArchivoPuntuaciones.dat
│   ├── ArchivoPuntuacionesCreacion.c
│   ├── ArchivoPuntuaciones.exe
│   ├── ComerManzana.wav
│   ├── main.c
│   ├── main.o
│   ├── Makefile.win
│   ├── PartidaPerdida.wav
│   ├── SNAKE.dev
│   ├── SNAKE.exe
│   └── SNAKE.layout
│
└── TATETI/
    ├── tateti.c
    └── tateti.exe
```

---

## ▶️ Ejecución

### Opción 1 — Ejecutables incluidos

En Windows, algunos juegos cuentan con su ejecutable precompilado.

Simplemente ingresar a la carpeta correspondiente y ejecutar:

```text
4enRaya.exe
pong.exe
SNAKE.exe
tateti.exe
```

En **Snake**, deben mantenerse junto al ejecutable los archivos de sonido:

```text
ComerManzana.wav
PartidaPerdida.wav
```

y el archivo:

```text
ArchivoPuntuaciones.dat
```

para conservar el ranking existente.

---

### Opción 2 — Compilar desde el código fuente

Se recomienda utilizar un compilador compatible con C, como **GCC/MinGW**.

Por ejemplo:

```bash
gcc 4enRaya.c -o 4enRaya.exe
```

Para Tateti:

```bash
gcc tateti.c -o tateti.exe
```

Para Pong:

```bash
gcc pong.c -o pong.exe
```

Snake utiliza funcionalidades específicas de Windows y reproducción de sonido, por lo que requiere enlazar la librería `winmm`:

```bash
gcc main.c -o SNAKE.exe -lwinmm
```

---

## 💾 Persistencia en Snake

A diferencia de los demás juegos, **Snake** incorpora persistencia de información mediante archivos.

La estructura utilizada para almacenar cada registro es:

```c
typedef struct {
    string nombre;
    unsigned int puntuacion;
} tRegistroPuntuaciones;
```

Los registros se almacenan en:

```text
ArchivoPuntuaciones.dat
```

Cada vez que termina una partida, la puntuación se incorpora al ranking y se conservan los **20 mejores resultados**.

---

## 🎯 Objetivo del proyecto

Estos juegos fueron desarrollados durante **2022**, en una etapa temprana de mi aprendizaje de programación.

El principal objetivo fue aprender y practicar los fundamentos del lenguaje **C** mediante proyectos interactivos, aplicando conceptos de programación imperativa a problemas concretos.

Actualmente, el repositorio funciona también como un registro de mi **evolución como programador**, desde mis primeros programas hasta proyectos posteriores con tecnologías y arquitecturas más avanzadas.


Este proyecto fue desarrollado con fines educativos y forma parte de mi historial personal de aprendizaje en programación.
