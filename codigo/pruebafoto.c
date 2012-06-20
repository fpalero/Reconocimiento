#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pgm.h"
#include "analisis.h"


int main(int argc, char *argv[])
{
  analisis a;
  char  *filename;
  FILE  *fd;
  FILE  *tornar;
  int    cols, rows, i, j;
  gray **imagen;
  gray   maxval;
  // inicializamos las librerias netpbm
  pgm_init(&argc, argv);

  // leemos los parametros de entrada
  if (argc != 2)
    pm_error("Uso: programa imagen.pgm");
  else
    filename = argv[1];

  // abrimos el fichero de imagen en modo lectura
  fd = fopen(filename, "r");

  if (fd == NULL ) {
    pm_error("error: no se pudo abrir el fichero!");
    exit(-1);
  }

  // leemos la imagen
  imagen = pgm_readpgm(fd, &cols, &rows, &maxval);

  if (imagen == NULL) {
    pm_error("error: no puedo leer la imagen!");
    exit(-1);
  }

  inicializar_analisis(&a);
  analizar(&a,imagen,rows,cols);
	//a->b->dev

  //Etiquetado y procesado de la imagen
for(i = 0 ; i < rows ; i++){
	for(j = 0 ; j < cols ; j++){
		if( (a.b->dev[i][j]) <= 0){
			imagen[i][j]= 255;
			//printf(" %3d ", imagen[i][j]);
		}else{
			imagen[i][j] = 255-(255/(a.b->dev[i][j]+1));
			//printf(" %d ", resultado[i][j]);
		}
	}
}
imagen[a.centro[0].x][a.centro[0].y] = 0;

  tornar = fopen("devuelto.pgm", "w");
  if (tornar == NULL){
	pm_error("error: no puedo abrir el fichero devuelto");
	exit(-1);
	}
  pgm_writepgm(tornar, imagen, cols, rows, 255, 0);

  fclose(tornar);
  fclose(fd);

  // liberamos la memoria de la imagen
  pgm_freearray(imagen, rows);

  vaciar_analisis(&a);
  
  exit(0);

}




