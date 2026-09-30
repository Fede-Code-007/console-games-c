#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char nombre [30];
	unsigned int puntuacion;
}tRegistro;

tRegistro registroJugador;
FILE* archivoPuntuaciones;

void crearArchivoBinario ();
void cerrarArchivoBinario ();

int main (){
	crearArchivoBinario ();
	cerrarArchivoBinario();
	return 0;
}

void crearArchivoBinario (){
	unsigned short i;
	archivoPuntuaciones = fopen ("ArchivoPuntuaciones.dat", "wb");
	printf ("ARCHIVO CREADO CON EXITO");
	strcpy(registroJugador.nombre, " ");
	registroJugador.puntuacion=0;
	for (i=0; i<20; i++){
		fwrite (&registroJugador, sizeof (tRegistro), 1, archivoPuntuaciones);
	}
}

void cerrarArchivoBinario (){
	fclose (archivoPuntuaciones);
}