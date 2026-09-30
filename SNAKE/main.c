/*
 * SNAKE - juego de consola (solo Windows) con ranking de puntuaciones
 *
 * Controles: W A S D o flechas.
 * Archivos necesarios junto al ejecutable: ComerManzana.wav y PartidaPerdida.wav
 * (si faltan, el juego funciona igual pero sin sonido).
 *
 * Para Compilar:  añadir -lwinmm en el linker de los parametros del proyecto.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <conio.h>
#include <windows.h>
#include <mmsystem.h>

#define V 21                                  /* alto del campo  */
#define H 65                                  /* ancho del campo */
#define MAX_SERPIENTE ((V - 2) * (H - 2))     /* celdas interiores del mapa */
#define LARGO_NOMBRE 30
#define MAX_REGISTROS 20
#define PAUSA_FRAME_MS 100
#define ARCHIVO_PUNTUACIONES "ArchivoPuntuaciones.dat"

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

/* Caracteres del codigo OEM (CP437/CP850) de la consola de Windows */
#define ESQ_SUP_IZQ 201
#define ESQ_SUP_DER 187
#define ESQ_INF_IZQ 200
#define ESQ_INF_DER 188
#define LINEA_HORIZ 205
#define LINEA_VERT  186
#define FRUTA_CHAR  254

typedef char string[LARGO_NOMBRE];

typedef struct {
    int x, y;
    int modX, modY;      /* solo se usa en la cabeza (snake[0]) */
    char imagen;
} tSnake;

typedef struct {
    string nombre;
    unsigned int puntuacion;
} tRegistroPuntuaciones;

/* ---------- Consola y entrada segura ---------- */

static void habilitarColoresANSI(void) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo;
    if (GetConsoleMode(h, &modo)) {
        SetConsoleMode(h, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}

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

static char *cadenaMayusculas(char *cadena) {
    size_t i;
    for (i = 0; cadena[i] != '\0'; i++) {
        cadena[i] = (char)toupper((unsigned char)cadena[i]);
    }
    return cadena;
}

/* ---------- Campo de juego ---------- */

static void crearMapaVacio(unsigned char campo[V][H]) {
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
}

/* Escribe serpiente y fruta sobre el mapa. */
static void colocarObjetos(unsigned char campo[V][H], int tamSnake,
                           const tSnake snake[], const int fruta[2]) {
    int i;
    for (i = 0; i < tamSnake; i++) {
        if (snake[i].x >= 0 && snake[i].x < H && snake[i].y >= 0 && snake[i].y < V) {
            campo[snake[i].y][snake[i].x] = (unsigned char)snake[i].imagen;
        }
    }
    campo[fruta[1]][fruta[0]] = FRUTA_CHAR;
}

static void dibujarCampo(unsigned char campo[V][H]) {
    int i, j;
    irAlInicio();
    for (i = 0; i < V; i++) {
        for (j = 0; j < H; j++) {
            unsigned char c = campo[i][j];
            if (c == '0' || c == 'O') {
                printf("\033[1;32m%c\033[0m", c);
            } else if (c != ' ' && j != 0 && j != H - 1 && i != 0 && i != V - 1) {
                printf("\033[0;31m%c\033[0m", c);
            } else {
                putchar(c);
            }
        }
        putchar('\n');
    }
    fflush(stdout);
}

/* ---------- Fruta y serpiente ---------- */

/* Elige una casilla interior que no este ocupada por la serpiente. */
static void generarFruta(int fruta[2], int tamSnake, const tSnake snake[]) {
    bool libre;
    int i;
    do {
        fruta[0] = 1 + rand() % (H - 2);
        fruta[1] = 1 + rand() % (V - 2);
        libre = true;
        for (i = 0; i < tamSnake && libre; i++) {
            if (snake[i].x == fruta[0] && snake[i].y == fruta[1]) {
                libre = false;
            }
        }
    } while (!libre);
}

static bool frutaComida(const tSnake snake[], const int fruta[2]) {
    return snake[0].x == fruta[0] && snake[0].y == fruta[1];
}

static void incrementarTamanoSerpiente(int *tamSnake, tSnake snake[]) {
    if (*tamSnake < MAX_SERPIENTE) {
        snake[*tamSnake].x = snake[*tamSnake - 1].x;   /* nace sobre la cola */
        snake[*tamSnake].y = snake[*tamSnake - 1].y;
        snake[*tamSnake].imagen = '0';
        (*tamSnake)++;
    }
}

static bool muerte(int tamSnake, const tSnake snake[]) {
    int i;
    if (snake[0].x <= 0 || snake[0].x >= H - 1 || snake[0].y <= 0 || snake[0].y >= V - 1) {
        return true;
    }
    for (i = 1; i < tamSnake; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            return true;
        }
    }
    return false;
}

/* Cada segmento ocupa el lugar del anterior; la cabeza avanza. */
static void moverSerpiente(int tamSnake, tSnake snake[]) {
    int i;
    for (i = tamSnake - 1; i > 0; i--) {
        snake[i].x = snake[i - 1].x;
        snake[i].y = snake[i - 1].y;
    }
    snake[0].x += snake[0].modX;
    snake[0].y += snake[0].modY;
}

static void cambiarDireccion(tSnake snake[]) {
    int k;
    int dx = snake[0].modX, dy = snake[0].modY;

    if (!kbhit()) {
        return;
    }
    k = getch();
    if (k == 0 || k == 224) {            /* flechas: llegan como 2 codigos */
        switch (getch()) {
            case 72: k = 'w'; break;
            case 80: k = 's'; break;
            case 75: k = 'a'; break;
            case 77: k = 'd'; break;
            default: return;
        }
    } else {
        k = tolower(k);
    }

    /* No se permite girar 180 grados */
    switch (k) {
        case 'w': if (dy != 1)  { snake[0].modX = 0;  snake[0].modY = -1; } break;
        case 's': if (dy != -1) { snake[0].modX = 0;  snake[0].modY = 1;  } break;
        case 'a': if (dx != 1)  { snake[0].modX = -1; snake[0].modY = 0;  } break;
        case 'd': if (dx != -1) { snake[0].modX = 1;  snake[0].modY = 0;  } break;
        default: break;
    }
}

static void inicializarVariables(int *tamSnake, unsigned char campo[V][H],
                                 unsigned int *cantFrutasComidas,
                                 tSnake snake[], int fruta[2]) {
    int i;

    *tamSnake = 4;
    snake[0].x = H / 2;
    snake[0].y = V / 2;
    snake[0].modX = 1;
    snake[0].modY = 0;
    snake[0].imagen = 'O';
    for (i = 1; i < *tamSnake; i++) {
        snake[i].x = snake[i - 1].x - 1;
        snake[i].y = snake[i - 1].y;
        snake[i].modX = 1;
        snake[i].modY = 0;
        snake[i].imagen = '0';
    }

    generarFruta(fruta, *tamSnake, snake);
    crearMapaVacio(campo);
    colocarObjetos(campo, *tamSnake, snake, fruta);
    *cantFrutasComidas = 0;
}

static void jugarPartida(unsigned char campo[V][H], int tamSnake,
                         unsigned int *cantFrutasComidas,
                         tSnake snake[], int fruta[2]) {
    system("cls");
    do {
        dibujarCampo(campo);
        if (frutaComida(snake, fruta)) {
            PlaySound("ComerManzana.wav", NULL, SND_ASYNC | SND_FILENAME);
            incrementarTamanoSerpiente(&tamSnake, snake);
            (*cantFrutasComidas)++;
            if (tamSnake < MAX_SERPIENTE) {
                generarFruta(fruta, tamSnake, snake);
            }
        }
        cambiarDireccion(snake);
        moverSerpiente(tamSnake, snake);
        crearMapaVacio(campo);
        colocarObjetos(campo, tamSnake, snake, fruta);
        Sleep(PAUSA_FRAME_MS);
    } while (!muerte(tamSnake, snake) && tamSnake < MAX_SERPIENTE);

    dibujarCampo(campo);
    PlaySound("PartidaPerdida.wav", NULL, SND_ASYNC | SND_FILENAME);
    vaciarTeclado();
}

/* ---------- Puntuaciones ---------- */

static void ingresarNombreJugador(string nombre) {
    int r;
    for (;;) {
        system("cls");
        printf("PARA INICIAR A JUGAR INGRESE EL NOMBRE DEL JUGADOR:\n");
        if (!leerLinea(nombre, LARGO_NOMBRE)) {
            exit(EXIT_SUCCESS);
        }
        if (nombre[0] == '\0') {
            continue;
        }
        printf("Seguro que te llamas %s?\n", nombre);
        printf("Si su respuesta es si presione 1. Si su respuesta es no presione 0.\n");
        for (;;) {
            r = leerEntero();
            if (r == 0 || r == 1) {
                break;
            }
            printf("ERROR. RESPUESTA NO VALIDA. INTENTE NUEVAMENTE.\n");
        }
        if (r == 1) {
            return;
        }
    }
}

/*
 * Agrega la puntuacion al archivo, deja los MAX_REGISTROS mejores
 * (de mayor a menor) y reescribe el archivo.
 */
static void registrarPuntuacion(const string nombre, unsigned int puntuacion) {
    tRegistroPuntuaciones reg[MAX_REGISTROS + 1];
    tRegistroPuntuaciones nuevo;
    int n = 0, i, j, aGuardar;
    FILE *f;

    /* 1. Leer lo que ya hay (solo los registros que existen de verdad) */
    f = fopen(ARCHIVO_PUNTUACIONES, "rb");
    if (f != NULL) {
        while (n < MAX_REGISTROS && fread(&reg[n], sizeof(tRegistroPuntuaciones), 1, f) == 1) {
            reg[n].nombre[LARGO_NOMBRE - 1] = '\0';
            n++;
        }
        fclose(f);
    }

    /* 2. Agregar el nuevo */
    memset(&nuevo, 0, sizeof nuevo);
    strncpy(nuevo.nombre, nombre, LARGO_NOMBRE - 1);
    nuevo.puntuacion = puntuacion;
    reg[n++] = nuevo;

    /* 3. Ordenar de mayor a menor (insercion: estable, empates conservan el orden) */
    for (i = 1; i < n; i++) {
        tRegistroPuntuaciones clave = reg[i];
        j = i - 1;
        while (j >= 0 && reg[j].puntuacion < clave.puntuacion) {
            reg[j + 1] = reg[j];
            j--;
        }
        reg[j + 1] = clave;
    }

    /* 4. Guardar los mejores */
    aGuardar = (n < MAX_REGISTROS) ? n : MAX_REGISTROS;
    f = fopen(ARCHIVO_PUNTUACIONES, "wb");
    if (f == NULL) {
        printf("\nNo se pudo guardar la puntuacion.\n");
        return;
    }
    fwrite(reg, sizeof(tRegistroPuntuaciones), (size_t)aGuardar, f);
    fclose(f);
}

static void mostrarArchivoPuntuaciones(void) {
    tRegistroPuntuaciones reg;
    int i = 0;
    FILE *f;

    system("cls");
    printf("RANKING DE JUGADORES\n");
    printf("---------------------\n\n");

    f = fopen(ARCHIVO_PUNTUACIONES, "rb");
    if (f == NULL) {
        printf("Todavia no hay puntuaciones registradas.\n");
        return;
    }
    while (fread(&reg, sizeof reg, 1, f) == 1) {
        reg.nombre[LARGO_NOMBRE - 1] = '\0';
        i++;
        printf("%d. NOMBRE: %s. PUNTUACION: %u.\n", i, reg.nombre, reg.puntuacion);
    }
    fclose(f);
}

static void mostrarPuntuacion(unsigned int cantFrutasComidas) {
    char respuesta[8];

    printf("\n\nParece que has perdido... \nFRUTAS COMIDAS: %u.\n\n", cantFrutasComidas);
    for (;;) {
        printf("Le gustaria ver el ranking de puntuaciones? (1-Si/ 2-No)\n");
        if (!leerLinea(respuesta, sizeof respuesta)) {
            return;
        }
        cadenaMayusculas(respuesta);
        if (strcmp(respuesta, "1") == 0 || strcmp(respuesta, "SI") == 0) {
            mostrarArchivoPuntuaciones();
            return;
        }
        if (strcmp(respuesta, "2") == 0 || strcmp(respuesta, "NO") == 0) {
            return;
        }
        printf("\nERROR. RESPUESTA NO VALIDA INTENTE NUEVAMENTE...\n");
    }
}

static bool consultarJugarDeNuevo(void) {
    int r;
    printf("\nSi desea jugar otra partida presione 1. Si desea salir del programa presione 0.\n");
    for (;;) {
        r = leerEntero();
        if (r == 0 || r == 1) {
            return r == 1;
        }
        printf("ERROR. RESPUESTA NO VALIDA. INTENTE NUEVAMENTE:\n");
    }
}

int main(void) {
    string nombreJugador;
    tSnake snake[MAX_SERPIENTE];
    int fruta[2];
    int tamSnake;
    unsigned int cantFrutasComidas;
    unsigned char campo[V][H];

    srand((unsigned)time(NULL));   /* una sola vez */
    habilitarColoresANSI();

    do {
        ingresarNombreJugador(nombreJugador);
        inicializarVariables(&tamSnake, campo, &cantFrutasComidas, snake, fruta);
        jugarPartida(campo, tamSnake, &cantFrutasComidas, snake, fruta);
        registrarPuntuacion(nombreJugador, cantFrutasComidas);
        mostrarPuntuacion(cantFrutasComidas);
    } while (consultarJugarDeNuevo());

    system("pause");
    return 0;
}
