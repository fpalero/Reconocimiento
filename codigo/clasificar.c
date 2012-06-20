#include <stdio.h>
#include "pgm.h"
#include "clasif.h"

int main(int argc, char **argv ){
 
  FILE *validacion;
  FILE *entrenamiento;
  clasif_t clas;
  double error = 0.0;	
  int knn = 1;	
	validacion = fopen("../resultados/validacion","r");
	if (validacion == NULL ) {
		printf("error: no puedo abrir validacion\n");
		exit(-1);
	}
	
	entrenamiento = fopen("../resultados/entrenamiento","r");
	if (entrenamiento == NULL ) {
		printf("error: no puedo abrir entrenamiento\n");
		exit(-1);
	}
	
	if(argc >= 2){
	   knn = atoi(argv[1]);
	}
	//printf("Paso0: Comenzar clasificación.\n");
	sdv_clasif_crear(CLASIF_KNN, knn, validacion, &clas);
	//printf("Paso1: Crear clasificador.\n");
	sdv_clasif_eval(clas, entrenamiento, &error);
	//printf("Paso2: Comparar.\n");
	sdv_clasif_eliminar(clas);
	//printf("Paso3: Borrar clasificador.\n");
	printf("Error: %f\n", error);
	

return 0;	
}
