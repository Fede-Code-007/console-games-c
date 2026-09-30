/*
 * TATETI (Tres en Raya) - juego de consola
 *
 * Modo 1: contra la PC | Modo 2: dos jugadores
 * Jugador 1 = 'X'      | Jugador 2 / PC = 'O'
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

#define LADO 3
#define CASILLAS (LADO * LADO)
#define CELDA(t, k) ((t)[(k) / LADO][(k) % LADO])

#define GANA_JUGADOR1 1
#define GANA_JUGADOR2 0   /* o la PC */
#define SIN_GANADOR   2   /* partida en curso o empate */

#ifdef _WIN32
#define COMANDO_LIMPIAR "cls"
#else
#define COMANDO_LIMPIAR "clear"
#endif

/* Las 8 lineas ganadoras (indices de casilla 0..8) */
static const int LINEAS[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8},   /* filas    */
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8},   /* columnas */
    {0, 4, 8}, {2, 4, 6}               /* diagonales */
};

/* ---------- Entrada segura ---------- */

static void limpiarPantalla(void) {
    system(COMANDO_LIMPIAR);
}

/* Lee una linea completa (sin el '\n'). Descarta lo que no entre en el buffer. */
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

static void inicializarTablero(char t[LADO][LADO]) {
    int k;
    for (k = 0; k < CASILLAS; k++) {
        CELDA(t, k) = (char)('A' + k);
    }
}

static void mostrarTablero(char t[LADO][LADO], int modoJuego) {
    int i, j;
    if (modoJuego == 1) {
        printf("TATETI VS PC!!!!\n---------------------\n\n");
    } else {
        printf("TATETI MULTIPLAYER!!!!\n------------------------\n\n");
    }
    for (i = 0; i < LADO; i++) {
        for (j = 0; j < LADO; j++) {
            printf(j < LADO - 1 ? " %c |" : " %c ", t[i][j]);
        }
        if (i < LADO - 1) {
            printf("\n-----------\n");
        }
    }
    printf("\n\n");
}

static bool casillaOcupada(char t[LADO][LADO], int k) {
    return CELDA(t, k) == 'X' || CELDA(t, k) == 'O';
}

/* 1 = gana X (jugador 1), 0 = gana O (jugador 2 / PC), 2 = nadie todavia */
static int ganador(char t[LADO][LADO]) {
    int l;
    for (l = 0; l < 8; l++) {
        char a = CELDA(t, LINEAS[l][0]);
        char b = CELDA(t, LINEAS[l][1]);
        char c = CELDA(t, LINEAS[l][2]);
        if ((a == 'X' || a == 'O') && a == b && b == c) {
            return (a == 'X') ? GANA_JUGADOR1 : GANA_JUGADOR2;
        }
    }
    return SIN_GANADOR;
}

/* ---------- Turnos ---------- */

static void introducirFichaJugador(char t[LADO][LADO], int numJugador, char ficha) {
    char buf[8];
    int k;

    printf("Turno del jugador %d (%c):\n", numJugador, ficha);
    for (;;) {
        printf("En que posicion quieres colocar la ficha? (A-I)\n");
        if (!leerLinea(buf, sizeof buf)) {
            exit(EXIT_SUCCESS);
        }
        if (strlen(buf) != 1) {
            printf("Entrada no valida. Escribe una sola letra de la A a la I.\n\n");
            continue;
        }
        k = toupper((unsigned char)buf[0]) - 'A';
        if (k < 0 || k >= CASILLAS) {
            printf("Posicion no valida. Escribe una letra de la A a la I.\n\n");
            continue;
        }
        if (casillaOcupada(t, k)) {
            printf("La casilla esta ocupada! Intentalo con otra letra!!\n\n");
            continue;
        }
        CELDA(t, k) = ficha;
        return;
    }
}

/* La PC elige al azar entre las casillas libres (siempre hay al menos una). */
static void introducirFichaPrograma(char t[LADO][LADO]) {
    int libres[CASILLAS];
    int n = 0, k;
    for (k = 0; k < CASILLAS; k++) {
        if (!casillaOcupada(t, k)) {
            libres[n++] = k;
        }
    }
    CELDA(t, libres[rand() % n]) = 'O';
}

/* ---------- Flujo del juego ---------- */

static int elegirModoJuego(void) {
    int modo;
    limpiarPantalla();
    printf("BIENVENIDO AL TATETI!!!!\n------------------------\n\n");
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

static int jugarPartida(char t[LADO][LADO], int modoJuego) {
    int turno = 0;
    int victoria = SIN_GANADOR;

    do {
        limpiarPantalla();
        mostrarTablero(t, modoJuego);
        if (turno % 2 == 0) {
            introducirFichaJugador(t, 1, 'X');
        } else if (modoJuego == 1) {
            introducirFichaPrograma(t);
        } else {
            introducirFichaJugador(t, 2, 'O');
        }
        victoria = ganador(t);
        turno++;
    } while (turno < CASILLAS && victoria == SIN_GANADOR);

    limpiarPantalla();
    mostrarTablero(t, modoJuego);
    return victoria;
}

static void mostrarResultado(int modoJuego, int victoria) {
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
    char tablero[LADO][LADO];
    int modoJuego, victoria;

    srand((unsigned)time(NULL));
    do {
        inicializarTablero(tablero);
        modoJuego = elegirModoJuego();
        victoria = jugarPartida(tablero, modoJuego);
        mostrarResultado(modoJuego, victoria);
    } while (jugarOtraVez());

#ifdef _WIN32
    system("pause");
#endif
    return 0;
}
