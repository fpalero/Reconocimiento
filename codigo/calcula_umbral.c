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

  int saca_pgm();
  //char filename[]="../img/piezas/frontal/";
  char filename[]="../fotos_entrenamiento/";
  char filepatron[]="../img/piezas/frontal/patron_frontal.pgm";
  char aux[255];
  char ruta[255];
  //char linea[255];
  char cadena[20];
  FILE *fd;
  FILE *patron;
  FILE *entrenador;

  int cols, rows, i, j, bucle, carpeta, nresultados;
  analisis a;
  float carac[11];
  //int umbral[3];
  //int acumulados[3][256];
  //float acum[3];
  //float total[3];
  gray   maxval;
  gray **imagen;
  gray **imgpatron;

  struct dirent **resultados=NULL;

  // inicializamos todas las variables
  pgm_init(&argc, argv);
  nresultados=0;

  printf("Haciendo magia...\n");

  patron = fopen(filepatron, "r");

  if (patron == NULL ) {
    pm_error("error: no puedo abrir los ficheros");
    exit(-1);
  }
 imgpatron = pgm_readpgm(patron, &cols, &rows, &maxval);

  if (imgpatron == NULL ) {
    pm_error("error: no puedo leer las imagenes");
    exit(-1);
  }

entrenador = fopen("entrenador","w");
if (entrenador == NULL ) {
    pm_error("error: no puedo crear entrenador");
    exit(-1);
}

//Bucle principal
for(carpeta=0; carpeta<3; carpeta++){
  aux[0]='\0';
  cadena[0]='\0';
  strcat(aux,filename);
  //escaneamos la carpeta para obtener todos los ficheros
  switch(carpeta){
    case 0:
      strcat(aux,"arandela");
      strcat(cadena,"arandela");
      break;
    case 1:
      strcat(aux,"tornillo");
      strcat(cadena,"tornillo");
      break;
    case 2:
      strcat(aux,"tuerca");
      strcat(cadena,"tuerca");
      break;
  }
  nresultados = scandir(aux, &resultados, saca_pgm, NULL);

  for(bucle=2; bucle< nresultados; bucle++){
    //Creamos la cadena con la direccion del fichero
    ruta[0]='\0';
    strcat(ruta,aux);
    strcat(ruta,"/");
    strcat(ruta,resultados[bucle]->d_name);
    //printf("%s\n",ruta);

    // abrimos el fichero de imagen en modo lectura
    fd = fopen(ruta, "r");

    if (fd == NULL ) {
      pm_error("error: no puedo abrir los ficheros");
      exit(-1);
    }

    // leemos la imagen
    imagen = pgm_readpgm(fd, &cols, &rows, &maxval);

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
	/* erode */
	pgm_erode(imagen, cols, rows, 2);
	/* dilate */
	pgm_dilate(imagen, cols, rows, 2);
	
	

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
    for(i=0; i<1; i++){
        if(a.area[i] <=0.0f){
		printf("error garrafal ¿?\n");
		exit(-1);
	}
    	carac[0] = ((a.diametro_mayor[i]*a.diametro_mayor[i]) / a.area[i]);
	carac[1] = ((a.diametro_menor[i]*a.diametro_menor[i]) / a.area[i]);
	carac[2] = (a.diametro_mayor[i]/a.diametro_menor[i]);
	carac[3] = ((a.perimetro[i]*a.perimetro[i])/a.area[i]);
	
	//escribimos en fichero las caracteristicas
	fprintf(entrenador,"%f %f %f %f %f %f %f %s\n",a.area[i], a.diametro_mayor[i], a.diametro_menor[i],a.perimetro[i], carac[0], carac[1], carac[3], cadena);
	//fprintf(entrenador,"%f %f %f %f %f %f %f %f %f %f %f %f %f %s\n", a.perimetro[i], a.diametro_mayor[i], a.diametro_menor[i], a.area[i], carac[0], carac[1], carac[2], carac[3],a.diametro_mayor[i]*a.diametro_mayor[i],a.diametro_menor[i]*a.diametro_menor[i],carac[6],carac[7],carac[9], cadena);
   }

    vaciar_analisis(&a);
    fclose(fd);
    //Liberamos la memoria de la imagen, para cargar otra
    pgm_freearray(imagen, rows);

  }//fin bucle

}//fin carpeta
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

