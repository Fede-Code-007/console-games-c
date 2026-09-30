/*
 * 4 EN RAYA - juego de consola
 *
 * Modo 1: contra la PC | Modo 2: dos jugadores
 * Jugador 1 = 'O'      | Jugador 2 / PC = 'X'
 *
 * Cada columna es una pila: las fichas caen hasta el fondo.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define COLUMNAS 7
#define FILAS 6
#define CASILLAS_TOTALES (COLUMNAS * FILAS)
#define FICHA_J1 'O'
#define FICHA_J2 'X'

#define GANA_JUGADOR1 1
#define GANA_JUGADOR2 0   /* o la PC */
#define SIN_GANADOR   2   /* partida en curso o empate */

#ifdef _WIN32
#define COMANDO_LIMPIAR "cls"
#else
#define COMANDO_LIMPIAR "clear"
#endif

typedef struct {
    char casillaColumna[FILAS];   /* indice 0 = arriba, FILAS-1 = fondo */
    int topeColumna;              /* FILAS = vacia, 0 = llena */
} tPila;

/* ---------- Entrada segura ---------- */

static void limpiarPantalla(void) {
    system(COMANDO_LIMPIAR);
}

static bool leerLinea(char *buf, size_t tam) {
    size_t len;
    if (fgets(buf, (int)tam, stdin) == NULL) {
        return false;
    }
    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    return true;
}

/* Devuelve el entero leido, o -1 si la entrada no es un numero valido. */
static int leerEntero(void) {
    char buf[32];
    char *fin;
    long v;
    if (!leerLinea(buf, sizeof buf)) {
        exit(EXIT_SUCCESS);   /* fin de entrada (Ctrl+Z / Ctrl+D) */
    }
    v = strtol(buf, &fin, 10);
    if (fin == buf || *fin != '\0') {
        return -1;
    }
    return (int)v;
}

/* ---------- Tablero ---------- */

static void inicializarTablero(tPila tablero[COLUMNAS]) {
    int i, j;
    for (j = 0; j < COLUMNAS; j++) {
        for (i = 0; i < FILAS; i++) {
            tablero[j].casillaColumna[i] = ' ';
        }
        tablero[j].topeColumna = FILAS;
    }
}

static bool pilaLlena(tPila pila) {
    return pila.topeColumna == 0;
}

static void soltarFicha(tPila *pila, char ficha) {
    pila->topeColumna--;
    pila->casillaColumna[pila->topeColumna] = ficha;
}

static void mostrarTablero(tPila tablero[COLUMNAS], int modoJuego) {
    int i, j;

    if (modoJuego == 1) {
        printf("4 en RAYA VS PC!!!!\n-------------------\n\n");
    } else {
        printf("MODO MULTIPLAYER!!!!\n--------------------\n\n");
    }

    for (i = 0; i < FILAS; i++) {
        for (j = 0; j < COLUMNAS; j++) {
            printf(j < COLUMNAS - 1 ? " %c |" : " %c ", tablero[j].casillaColumna[i]);
        }
        printf("\n---------------------------\n");
    }
    for (j = 0; j < COLUMNAS; j++) {
        printf(j < COLUMNAS - 1 ? " %d |" : " %d ", j + 1);
    }
    printf("\n\n");
}

/*
 * 1 = gana el jugador 1, 0 = gana el jugador 2 / PC, 2 = nadie todavia.
 * Para cada ficha se buscan 4 iguales en 4 direcciones:
 * horizontal, vertical y las dos diagonales.
 */
static int ganador(tPila tablero[COLUMNAS]) {
    static const int DIR[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };  /* {dColumna, dFila} */
    int c, f, d, k;

    for (c = 0; c < COLUMNAS; c++) {
        for (f = 0; f < FILAS; f++) {
            char ficha = tablero[c].casillaColumna[f];
            if (ficha == ' ') {
                continue;
            }
            for (d = 0; d < 4; d++) {
                for (k = 1; k < 4; k++) {
                    int nc = c + DIR[d][0] * k;
                    int nf = f + DIR[d][1] * k;
                    if (nc < 0 || nc >= COLUMNAS || nf < 0 || nf >= FILAS ||
                        tablero[nc].casillaColumna[nf] != ficha) {
                        break;
                    }
                }
                if (k == 4) {
                    return (ficha == FICHA_J1) ? GANA_JUGADOR1 : GANA_JUGADOR2;
                }
            }
        }
    }
    return SIN_GANADOR;
}

/* ---------- Turnos ---------- */

static void introducirFichaJugador(tPila tablero[COLUMNAS], int numJugador, char ficha) {
    int col;

    printf("Turno del jugador %d (%c):\n", numJugador, ficha);
    for (;;) {
        printf("En que columna quieres colocar la ficha? (1-%d)\n", COLUMNAS);
        col = leerEntero();
        if (col < 1 || col > COLUMNAS) {
            printf("COLUMNA INEXISTENTE. ESCOJA OTRA!!!\n\n");
            continue;
        }
        if (pilaLlena(tablero[col - 1])) {
            printf("COLUMNA LLENA. ESCOJA OTRA!!!\n\n");
            continue;
        }
        soltarFicha(&tablero[col - 1], ficha);
        return;
    }
}

/* La PC elige al azar entre las columnas que no estan llenas. */
static void introducirFichaPrograma(tPila tablero[COLUMNAS]) {
    int libres[COLUMNAS];
    int n = 0, j;
    for (j = 0; j < COLUMNAS; j++) {
        if (!pilaLlena(tablero[j])) {
            libres[n++] = j;
        }
    }
    soltarFicha(&tablero[libres[rand() % n]], FICHA_J2);
}

/* ---------- Flujo del juego ---------- */

static int elegirModoJuego(void) {
    int modo;
    limpiarPantalla();
    printf("BIENVENIDO A CUATRO EN RAYA!!!!\n-------------------------------\n\n");
    for (;;) {
        printf("ELIGE MODO DE JUEGO:\n\n");
        printf("1) Singleplayer.\n");
        printf("2) Multiplayer.\n");
        modo = leerEntero();
        if (modo == 1 || modo == 2) {
            return modo;
        }
        limpiarPantalla();
        printf("ERROR. MODO DE JUEGO NO VALIDO. INTENTE NUEVAMENTE.\n\n");
    }
}

static int jugarPartida(tPila tablero[COLUMNAS], int modoJuego) {
    int turno = 0;
    int victoria = SIN_GANADOR;

    do {
        limpiarPantalla();
        mostrarTablero(tablero, modoJuego);
        if (turno % 2 == 0) {
            introducirFichaJugador(tablero, 1, FICHA_J1);
        } else if (modoJuego == 1) {
            introducirFichaPrograma(tablero);
        } else {
            introducirFichaJugador(tablero, 2, FICHA_J2);
        }
        victoria = ganador(tablero);
        turno++;
    } while (turno < CASILLAS_TOTALES && victoria == SIN_GANADOR);

    limpiarPantalla();
    mostrarTablero(tablero, modoJuego);
    return victoria;
}

static void mostrarGanador(int modoJuego, int victoria) {
    if (victoria == SIN_GANADOR) {
        printf("Ha sido un empate.\n\n");
    } else if (modoJuego == 1) {
        if (victoria == GANA_JUGADOR1) {
            printf("FELICITACIONES. HAS GANADO LA PARTIDA.\n\n");
        } else {
            printf("Que mala suerte, parece que has perdido... :(\n\n");
        }
    } else {
        printf("FELICITACIONES. El jugador %d ha GANADO LA PARTIDA.\n\n",
               victoria == GANA_JUGADOR1 ? 1 : 2);
    }
}

static bool jugarOtraVez(void) {
    int r;
    printf("\nSi desea jugar otra partida presione 1, si quiere cerrar el programa presione 0.\n");
    for (;;) {
        r = leerEntero();
        if (r == 0 || r == 1) {
            return r == 1;
        }
        printf("ERROR. RESPUESTA NO VALIDA. INTENTE NUEVAMENTE:\n");
    }
}

int main(void) {
    tPila tablero[COLUMNAS];
    int modoJuego, victoria;

    srand((unsigned)time(NULL));   /* una sola vez, no en cada turno */
    do {
        inicializarTablero(tablero);
        modoJuego = elegirModoJuego();
        victoria = jugarPartida(tablero, modoJuego);
        mostrarGanador(modoJuego, victoria);
    } while (jugarOtraVez());

#ifdef _WIN32
    system("pause");
#endif
    return 0;
}
