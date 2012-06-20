#include "conjuntos.h"
#include "pgm.h"

typedef struct _tpunto_{
	int x, y;
}punto;

typedef struct _tanalisis_{
	int generada;			//variable de control
	bector *b;
	float *area;			//almacenamos areas
	float *perimetro;		//almacenamos perimetros
	punto *centro;			//almacenamos centros
	float * diametro_mayor;		//diametro mayor
	float * diametro_menor;		//diametro menor
	float ** momento;		//momentos
}analisis;

//constructores
void inicializar_analisis(analisis *a);
void generar_analisis(analisis *a, int net, int fil, int col);
void generar_componentes(analisis *a);

//destructor
void vaciar_analisis(analisis *a);

//funciones
void obtener_vecinos(gray **image, int i, int j, int max_col, int *vecinos);
int es_frontera(gray **image, int i, int j);

//propiedades
float distancia(punto *centro, int dx, int dy);
void obtener_propiedades(analisis *a);

//analizador, funcion principal
void analizar(analisis *a, gray **foto, int fil, int col);


