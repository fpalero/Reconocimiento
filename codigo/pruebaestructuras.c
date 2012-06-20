#include <stdlib.h>
#include <stdio.h>
#include "analisis.h"


int main(){
	analisis a;
	int    cols = 7, rows = 7, i, j;
	int dibujo[7][7]={	{255, 255, 255, 255, 255, 255, 255},
				{255, 0  , 255, 0  , 255, 0  , 255},
				{255, 0  , 255, 0  , 255, 0  , 255},
				{255, 0  , 0  , 0  , 255, 255, 255},
				{255, 0  , 0  , 0  , 255, 0  , 255},
				{255, 0  , 0  , 0  , 255, 0  , 255},
				{255, 255, 255, 255, 255, 255 ,255},
	};
	int dibujo2[7][7]={	{255, 255, 255, 255, 255, 255, 255},
				{255, 0  , 0  , 0  , 255, 0  , 255},
				{255, 0  , 255, 0  , 255, 0  , 255},
				{255, 0  , 0  , 0  , 255, 0  , 255},
				{255, 0  , 0  , 0  , 255, 0  , 255},
				{255, 0  , 0  , 0  , 255, 0  , 255},
				{255, 255, 255, 255, 255, 255 ,255},
	};
	unsigned char **m;
	unsigned char **m2;
	
	m = (unsigned char **) malloc(sizeof(unsigned char *)*7);
	m2 = (unsigned char **) malloc(sizeof(unsigned char *)*7);
	for (i = 0; i < 7; i++) {
		m[i] = (unsigned char *) malloc(sizeof(unsigned char)*7);
		m2[i] = (unsigned char *) malloc(sizeof(unsigned char)*7);
		for (j = 0; j < 7; j++) {
			m[i][j] = (unsigned char)dibujo[i][j];
			m2[i][j]= (unsigned char)dibujo2[i][j];
		}
	}
	inicializar_analisis(&a);

	analizar(&a,m,rows,cols);

	vaciar_analisis(&a);
	inicializar_analisis(&a);

	analizar(&a,m2,rows,cols);
	vaciar_analisis(&a);
	return 1;
}
