#include <stddef.h>
//#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "pgm.h"


/* constants */
#define TRUE   1
#define FALSE  0

#define OBJECT        0
#define BACKGROUND  255
#define MIN(a, b) ( ((a) < (b)) ? (a) : (b) )

void pgm_dilate(
		gray  **pgm, 
		int     cols, 
		int     rows, 
		int     num
		)
{
  int     n, i, j;
  gray  **tmp_pgm;

  /* allocate space */
  tmp_pgm = pgm_allocarray(cols, rows);
  if (tmp_pgm == NULL)
    pm_error("no more memory.");

  for (n=0; n < num; n++) {

    /* copy pgm -> tmp_pgm */
    for (i=0; i < rows; i++)
      for (j=0; j < cols; j++)
	tmp_pgm[i][j] = pgm[i][j];
    
    /* dilate 1 pixel */
    for (i=1; i < rows-1; i++)
      for (j=1; j < cols-1; j++)
	if (tmp_pgm[i][j] == BACKGROUND &&
	    (tmp_pgm[i-1][j-1] == OBJECT ||
	     tmp_pgm[i-1][j  ] == OBJECT ||
	     tmp_pgm[i-1][j+1] == OBJECT ||
	     tmp_pgm[i  ][j-1] == OBJECT ||
	     tmp_pgm[i  ][j+1] == OBJECT ||
	     tmp_pgm[i+1][j-1] == OBJECT ||
	     tmp_pgm[i+1][j  ] == OBJECT ||
	     tmp_pgm[i+1][j+1] == OBJECT))
	  pgm[i][j] = OBJECT;

  } /* end for */

  pgm_freearray(tmp_pgm, rows);
  
} /* end pgm_dilate() */

void pgm_erode(
		gray  **pgm, 
		int     cols, 
		int     rows, 
		int     num
)		
{
  int     n, i, j;
  gray  **tmp_pgm;

  /* allocate space */
  tmp_pgm = pgm_allocarray(cols, rows);
  if (tmp_pgm == NULL)
    pm_error("no more memory.");

  for (n=0; n < num; n++) {

    /* copy pgm -> tmp_pgm */
    for (i=0; i < rows; i++)
      for (j=0; j < cols; j++)
	tmp_pgm[i][j] = pgm[i][j];
    
    /* dilate 1 pixel */
    for (i=1; i < rows-1; i++)
      for (j=1; j < cols-1; j++)
	if (tmp_pgm[i][j] == OBJECT &&
	    (tmp_pgm[i-1][j-1] == BACKGROUND ||
	     tmp_pgm[i-1][j  ] == BACKGROUND ||
	     tmp_pgm[i-1][j+1] == BACKGROUND ||
	     tmp_pgm[i  ][j-1] == BACKGROUND ||
	     tmp_pgm[i  ][j+1] == BACKGROUND ||
	     tmp_pgm[i+1][j-1] == BACKGROUND ||
	     tmp_pgm[i+1][j  ] == BACKGROUND ||
	     tmp_pgm[i+1][j+1] == BACKGROUND))
	  pgm[i][j] = BACKGROUND;

  } /* end for */

  pgm_freearray(tmp_pgm, rows);
  
} /* end pgm_erode() */


int main(int argc, char *argv[])
{

  int saca_pgm();
  FILE *fd;
  FILE *fd1;
 

  int cols, rows, i, j, bucle, carpeta, nresultados;
  
  float carac[11];
  gray   maxval;
  gray **imagen;
  gray **imagen1;

  struct dirent **resultados=NULL;

  // inicializamos todas las variables
  pgm_init(&argc, argv);
  nresultados=0;

  printf("Haciendo magia...\n");

  fd = fopen(filepatron, "r");

  if (fd == NULL ) {
    pm_error("error: no puedo abrir los ficheros");
    exit(-1);
  }
  imagen = pgm_readpgm(fd, &cols, &rows, &maxval);

  if (imgpatron == NULL ) {
    pm_error("error: no puedo leer las imagenes");
    exit(-1);
  }

 // abrimos el fichero de imagen en modo lectura
    fd1 = fopen(argv[1], "r");

    if (fd1 == NULL ) {
      pm_error("error: no puedo abrir los ficheros");
      exit(-1);
    }

    // leemos la imagen
    imagen1 = pgm_readpgm(fd1, &cols, &rows, &maxval);

    if (imagen == NULL ) {
      pm_error("error: no puedo leer las imagenes");
      exit(-1);
    }

    //*******************************************************************//
    //en 'imagen' tenemos la foto a analizar para el fichero clasificador//
    //*******************************************************************//

    for(i=0; i<rows;i++){
      for(j=0; j<cols; j++){
        //Retiramos fondo por division
        imagen[i][j] = MIN(255, 255*((float)imagen[i][j]/(float)imgpatron[i][j]) );
	//Aplicamos umbral
	if(imagen[i][j] <= 210)
		imagen[i][j] = 0;
	else
		imagen[i][j] = 255;
       }
	
    }
    // <----- falta erosionar y dilatar!!!
	
	/* dilate */
	pgm_dilate(imagen, cols, rows, 2);
	/* erode */
	pgm_erode(imagen, cols, rows, 2);
 
	pgm_writepgm(stdout, imagen, cols, rows, 255, 0);
 	
fclose(fd);
    //Liberamos la memoria de la imagen, para cargar otra
    pgm_freearray(imagen, rows);
	
 printf("Fin de la magia...\n");
  fclose(patron);
  exit(0);

}

int saca_pgm(struct dirent *entry){
  char *ptr;

  if( (strcmp(entry->d_name, ".")==0) || (strcmp(entry->d_name, "..")==0) )
    return 0;

  //Revisar extensiones de los ficheros
  ptr = rindex(entry->d_name, '.');


  if( (ptr!=NULL) && (strcmp(ptr, ".pgm")==0))
    return 1;
  else
    return 0;

}

