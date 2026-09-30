/*
 * PONG - juego de consola (solo Windows)
 *
 * Modo 1: contra la PC (la raqueta rival patrulla arriba y abajo, no sigue la bola)
 * Modo 2: dos jugadores
 *
 * Controles:  Jugador 1: W / S     Jugador 2: flechas arriba / abajo
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#define V 21                     /* alto del campo  */
#define H 75                     /* ancho del campo */
#define MAX_GOLES_PERMITIDOS 100
#define PAUSA_FRAME_MS 50

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#define COLOR_AZUL  "\033[0;34m"
#define COLOR_CIAN  "\033[0;36m"
#define COLOR_RESET "\033[0m"

/* Caracteres del codigo OEM (CP437/CP850) de la consola de Windows */
#define ESQ_SUP_IZQ 201
#define ESQ_SUP_DER 187
#define ESQ_INF_IZQ 200
#define ESQ_INF_DER 188
#define LINEA_HORIZ 205
#define LINEA_VERT  186
#define RAQUETA     178

/* Las raquetas ocupan las filas [inicio, fin) (fin excluido). */
typedef struct {
    int bolaX, bolaY;
    int modX, modY;                 /* direccion de la bola: -1 o 1 */
    int inicioJ1, finJ1;
    int inicioRival, finRival;
    int modIA;                      /* direccion de la raqueta de la PC */
    int maxGoles;
    int golesJ1, golesJ2;
    int modoJuego;
} tPong;

/* ---------- Consola ---------- */

static void habilitarColoresANSI(void) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo;
    if (GetConsoleMode(h, &modo)) {
        SetConsoleMode(h, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}

/* Vuelve al inicio de la pantalla sin borrarla (evita el parpadeo de cls). */
static void irAlInicio(void) {
    COORD origen = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), origen);
}

static void vaciarTeclado(void) {
    while (kbhit()) {
        getch();
    }
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
        exit(EXIT_SUCCESS);
    }
    v = strtol(buf, &fin, 10);
    if (fin == buf || *fin != '\0') {
        return -1;
    }
    return (int)v;
}

/* ---------- Configuracion ---------- */

static int elegirModoJuego(void) {
    int modo;
    for (;;) {
        printf("ELIGE MODO DE JUEGO:\n\n");
        printf("1) Singleplayer.\n");
        printf("2) Multiplayer.\n");
        modo = leerEntero();
        if (modo == 1 || modo == 2) {
            return modo;
        }
        system("cls");
        printf("ERROR. MODO DE JUEGO NO VALIDO. INTENTE NUEVAMENTE.\n\n");
    }
}

static int consultarMaxGoles(void) {
    int goles;
    for (;;) {
        printf("\nCuantos goles se debera embocar para finalizar el partido? (1-%d).\n",
               MAX_GOLES_PERMITIDOS);
        goles = leerEntero();
        if (goles >= 1 && goles <= MAX_GOLES_PERMITIDOS) {
            return goles;
        }
        printf("ERROR. Ingresa un numero entre 1 y %d.\n", MAX_GOLES_PERMITIDOS);
    }
}

static int direccionAleatoria(void) {
    return (rand() % 2) ? -1 : 1;
}

/* Deja bola y raquetas en el centro, con direcciones al azar. */
static void reiniciarPosiciones(tPong *p) {
    p->bolaX = 37;
    p->bolaY = 10;

    p->inicioJ1 = 8;
    p->finJ1 = 12;

    if (p->modoJuego == 1) {        /* la raqueta de la PC es mas grande */
        p->inicioRival = 6;
        p->finRival = 14;
    } else {
        p->inicioRival = 8;
        p->finRival = 12;
    }

    p->modX = direccionAleatoria();
    p->modY = direccionAleatoria();
    p->modIA = direccionAleatoria();
}

static void iniciarPartida(tPong *p) {
    system("cls");
    printf("BIENVENIDO AL PONG!!!!\n-------------------------------\n\n");
    p->modoJuego = elegirModoJuego();
    p->maxGoles = consultarMaxGoles();
    p->golesJ1 = 0;
    p->golesJ2 = 0;
    reiniciarPosiciones(p);
}

/* ---------- Dibujo ---------- */

static void construirCampo(unsigned char campo[V][H], const tPong *p) {
    int i, j;

    for (i = 0; i < V; i++) {
        for (j = 0; j < H; j++) {
            if (i == 0 && j == 0)               campo[i][j] = ESQ_SUP_IZQ;
            else if (i == 0 && j == H - 1)      campo[i][j] = ESQ_SUP_DER;
            else if (i == V - 1 && j == 0)      campo[i][j] = ESQ_INF_IZQ;
            else if (i == V - 1 && j == H - 1)  campo[i][j] = ESQ_INF_DER;
            else if (i == 0 || i == V - 1)      campo[i][j] = LINEA_HORIZ;
            else if (j == 0 || j == H - 1)      campo[i][j] = LINEA_VERT;
            else                                campo[i][j] = ' ';
        }
    }
    for (i = p->inicioJ1; i < p->finJ1; i++) {
        for (j = 2; j < 4; j++) {
            campo[i][j] = RAQUETA;
        }
    }
    for (i = p->inicioRival; i < p->finRival; i++) {
        for (j = H - 4; j < H - 2; j++) {
            campo[i][j] = RAQUETA;
        }
    }
    campo[p->bolaY][p->bolaX] = 'O';
}

static void dibujarPantalla(const tPong *p) {
    unsigned char campo[V][H];
    int i, j;

    construirCampo(campo, p);
    irAlInicio();
    for (i = 0; i < V; i++) {
        if (i == 0 || i == V - 1) {
            printf(COLOR_AZUL);
            for (j = 0; j < H; j++) {
                putchar(campo[i][j]);
            }
            printf(COLOR_RESET);
        } else {
            printf(COLOR_CIAN);
            putchar(campo[i][0]);
            printf(COLOR_RESET);
            for (j = 1; j < H - 1; j++) {
                putchar(campo[i][j]);
            }
            printf(COLOR_CIAN);
            putchar(campo[i][H - 1]);
            printf(COLOR_RESET);
        }
        putchar('\n');
    }
    /* Los espacios finales tapan restos de numeros mas largos (10-9 -> 9-9) */
    printf("GOLES PARA GANAR: %-3d\n", p->maxGoles);
    printf("PUNTUACION: %d-%d      \n\n", p->golesJ1, p->golesJ2);
    fflush(stdout);
}

static void cuentaRegresiva(void) {
    int n;
    printf("\n");
    for (n = 3; n >= 1; n--) {
        printf("%d ", n);
        fflush(stdout);
        Beep(700, 400);
        Sleep(350);
    }
    printf("...\n");
    Beep(760, 400);
}

/* ---------- Logica ---------- */

static bool finPartida(const tPong *p) {
    return p->golesJ1 == p->maxGoles || p->golesJ2 == p->maxGoles;
}

static void moverRaqueta(int *inicio, int *fin, int delta) {
    if (*inicio + delta >= 1 && *fin + delta <= V - 1) {
        *inicio += delta;
        *fin += delta;
    }
}

static void procesarTeclas(tPong *p) {
    while (kbhit()) {
        int k = getch();
        if (k == 0 || k == 224) {              /* tecla especial: 2 codigos */
            int k2 = getch();
            if (p->modoJuego == 2) {           /* las flechas solo en multiplayer */
                if (k2 == 72) {
                    moverRaqueta(&p->inicioRival, &p->finRival, -1);
                } else if (k2 == 80) {
                    moverRaqueta(&p->inicioRival, &p->finRival, 1);
                }
            }
        } else {
            k = tolower(k);
            if (k == 'w') {
                moverRaqueta(&p->inicioJ1, &p->finJ1, -1);
            } else if (k == 's') {
                moverRaqueta(&p->inicioJ1, &p->finJ1, 1);
            }
        }
    }
}

static void moverIA(tPong *p) {
    if (p->inicioRival + p->modIA < 1 || p->finRival + p->modIA > V - 1) {
        p->modIA = -p->modIA;
    }
    p->inicioRival += p->modIA;
    p->finRival += p->modIA;
}

static void actualizar(tPong *p) {
    procesarTeclas(p);
    if (p->modoJuego == 1) {
        moverIA(p);
    }

    /* Rebote contra bordes superior e inferior (solo si se dirige hacia ellos) */
    if ((p->bolaY <= 1 && p->modY < 0) || (p->bolaY >= V - 2 && p->modY > 0)) {
        p->modY = -p->modY;
    }
    /* Rebote contra la raqueta del jugador 1 */
    if (p->bolaX == 4 && p->modX < 0 && p->bolaY >= p->inicioJ1 && p->bolaY < p->finJ1) {
        p->modX = 1;
    }
    /* Rebote contra la raqueta rival */
    if (p->bolaX == H - 5 && p->modX > 0 && p->bolaY >= p->inicioRival && p->bolaY < p->finRival) {
        p->modX = -1;
    }

    p->bolaX += p->modX;
    p->bolaY += p->modY;

    /* Goles */
    if (p->bolaX <= 1 || p->bolaX >= H - 2) {
        if (p->bolaX <= 1) {
            p->golesJ2++;
        } else {
            p->golesJ1++;
        }
        if (!finPartida(p)) {
            dibujarPantalla(p);
            cuentaRegresiva();
            reiniciarPosiciones(p);
            vaciarTeclado();
            system("cls");
        }
    }
}

static void bucleDelJuego(tPong *p) {
    system("cls");
    do {
        dibujarPantalla(p);
        actualizar(p);
        Sleep(PAUSA_FRAME_MS);
    } while (!finPartida(p));
    dibujarPantalla(p);
    vaciarTeclado();
}

/* ---------- Final de partida ---------- */

static void mostrarGanador(const tPong *p) {
    if (p->modoJuego == 1) {
        if (p->golesJ1 > p->golesJ2) {
            printf("FELICITACIONES HAS GANADO LA PARTIDA!!!\n\n");
        } else {
            printf("Vaya parece que has perdido...\n\n");
        }
    } else {
        printf("FELICITACIONES EL JUGADOR %d HA GANADO LA PARTIDA!!!\n\n",
               p->golesJ1 > p->golesJ2 ? 1 : 2);
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
    tPong partida;

    srand((unsigned)time(NULL));
    habilitarColoresANSI();

    do {
        iniciarPartida(&partida);
        bucleDelJuego(&partida);
        mostrarGanador(&partida);
    } while (jugarOtraVez());

    system("pause");
    return 0;
}
