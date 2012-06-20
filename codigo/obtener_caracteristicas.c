#include <stddef.h>
//#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "pgm.h"
#include "analisis.h"

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

  int cols, rows, i, nresultados;
  analisis a;
  float carac[11];
  gray   maxval;
  gray **imagen;

  // inicializamos todas las variables
  pgm_init(&argc, argv);
  nresultados=0;


    // leemos la imagen
    imagen = pgm_readpgm(stdin, &cols, &rows, &maxval);

    if (imagen == NULL ) {
      pm_error("error: no puedo leer las imagenes argv[1]");
      exit(-1);
    }

    //Obtenemos caracteristicas de la imagen umbralizada
    inicializar_analisis(&a);
    analizar(&a,imagen,rows,cols);
    //a->b->dev			<---- MATRIZ INT RESULTANTE
	//a->b->n_conjuntos	<---- NUMERO DE FIGURAS DETECTADOS
	//a->area			<---- AREAS
	//a->perimetro		<---- PERIMETROS
	//a->centro			<---- CENTROS
	//a->diametro_mayor	<---- DIAMETRO MAYOR
	//a->diametro_menor <---- DIAMETRO MENOR
    for(i=0; i<a.b->objetos; i++){
        if(a.area[i] <=0.0f){
		printf("error garrafal ¿?\n");
		exit(-1);
	}
    	carac[0] = ((a.diametro_mayor[i]*a.diametro_mayor[i]) / a.area[i]);
	carac[1] = ((a.diametro_menor[i]*a.diametro_menor[i]) / a.area[i]);
	
	
	//escribimos en fichero las caracteristicas
	printf("%f %f %s\n",carac[0], carac[1], argv[1]);
	
   }

    vaciar_analisis(&a);
   
    //Liberamos la memoria de la imagen, para cargar otra
    pgm_freearray(imagen, rows);

  exit(0);

}
