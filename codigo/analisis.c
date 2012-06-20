#include <stdlib.h>
#include <math.h>
#include "analisis.h"


const int n10 = 0;
const int n01 = 1;
const int n00 = 2;
const int n20 = 3;//vector de momentos
const int n02 = 4;
const int n11 = 5;
const int n30 = 6;
const int n12 = 7;
const int n21 = 8;
const int n03 = 9;

//constructores
void inicializar_analisis(analisis *a){
	a->area = 0;
	a->perimetro = 0;
	a->centro = 0;
	a->diametro_mayor = 0;
	a->diametro_menor = 0;
	a->momento = 0;
	a->b = 0;
	a->generada = 0;

}

void generar_analisis(analisis *a, int net, int fil, int col){
	if(a->generada > 0)
		vaciar_analisis(a);

	a->b = (bector *)malloc(sizeof(bector));

	generar(a->b, net, fil, col);
	a->generada = 1;
}

void generar_componentes(analisis *a){
	int i,j;
	a->area = (float *) malloc(sizeof(float) * a->b->n_conjuntos);
	a->centro = (punto *) malloc(sizeof(punto) * a->b->n_conjuntos);
	a->diametro_mayor = (float *) malloc(sizeof(float) * a->b->n_conjuntos);
	a->diametro_menor = (float *) malloc(sizeof(float) * a->b->n_conjuntos);
	a->perimetro = (float *) malloc(sizeof(float) * a->b->n_conjuntos);
	a->momento = (float **) malloc(sizeof(float *) * a->b->n_conjuntos);
	for(i=0; i<a->b->n_conjuntos;i++)
		a->momento[i] = (float *) malloc(sizeof(float) * 7);
		//Por que hay 7 momentos

	for(i=0; i<a->b->n_conjuntos ; i++){
		a->area[i] = 0.0f;
		a->centro[i].x = a->centro[i].y = 0;
		a->diametro_mayor[i] = 0.0f;
		a->diametro_menor[i] = 0.0f;
		a->perimetro[i] = 0.0f;
		for(j=0; j<7; j++)
			a->momento[i][j] = 0.0f;
	}

	a->generada = 2;
}

//destructor
void vaciar_analisis(analisis *a){
	int j;
	if(a->generada > 0){
		vaciar(a->b);
		if(a->generada > 1){
			free(a->area);
			free(a->centro);
			free(a->diametro_mayor);
			free(a->diametro_menor);
			free(a->perimetro);
			for(j=0; j<a->b->n_conjuntos; j++)
				free(a->momento[j]);
			free(a->momento);
		}
	}
	free(a->b);
	inicializar_analisis(a);

}


//funciones
void obtener_vecinos(gray **image, int i, int j, int max_col, int *vecinos){

	vecinos[0] = image[i-1][j];
	vecinos[1] = image[i][j-1];
	vecinos[2] = image[i-1][j-1];
	vecinos[3] = image[i-1][j+1];

}

int tiene_vecinos(int *vecinos, int tam){
	int ret=0;
	int i;
	for(i=0; i<tam;i++)
		ret+=vecinos[i];
	
	if(ret<=0)
		return 1;
	else
		return 0;
}

int es_frontera(gray **image, int i, int j){
	int suma = 0;
	
	suma = image[i+1][j] + image[i-1][j] + image[i][j+1] + 
		   image[i][j-1] + image[i+1][j+1] + image[i+1][j-1] +
		   image[i-1][j+1] + image[i-1][j-1];
	
	//Como el color del objeto es negro el valor del pixel es 0
	// si al sumar los pixeles vecinos el valor es mayor de cero
	//uno de los pixeles vecinos es fondo y por lo tanto es un pixel frontera
	if (suma > 0) {
		return 1;
	}else {
		return 0;
	}
}

//propiedades
float distancia(punto *centro, int dx, int dy){
	float x,y;
	x = centro->x - dx;
	y = centro->y - dy;
	return ((float)sqrt( (x*x)+(y*y) ));
}
/*
float my_pow(int x, int p){
	int i;
	int dev = 1;
	for(i=0; i<p; i++)
		dev*=x;

	return dev*1.0f;

}

float funct_mom(int x, int y, int p, int q){
	return (my_pow(x, p) * my_pow(y, q));
}
*/

void obtener_propiedades(analisis *a){
	int visitados = 0;
	int i = 0;
	float max, min, dist;
	
	nodo *aux;
	while(i < a->b->max_val && visitados < a->b->n_conjuntos){
		if( a->b->vec[i].ins_c > 0 ){
			//Hay objetos en ins_c, el conjunto existe
			
			max = 0.0f;
			min = 999.999f;
			//area
			a->area[visitados] = ((a->b->vec[i].ins_c + a->b->vec[i].ins_h)* (1.0f/a->b->fil) * (1.0f/a->b->col));
			//perimetro * (1.0f/a->b->fil) * (1.0f/a->b->col)
			a->perimetro[visitados] = ((a->b->vec[i].ins_c)* (1.0f/a->b->fil) * (1.0f/a->b->col));
			//centro
			a->centro[visitados].x = (a->b->vec[i].sum_x)/(a->b->vec[i].ins_c);
			a->centro[visitados].y = (a->b->vec[i].sum_y)/(a->b->vec[i].ins_c);
			//diametros
			aux = a->b->vec[i].c;
			while(aux){
				dist = distancia(&(a->centro[visitados]), aux->x, aux->y);
				if(dist>max)
					max = dist;
				if(dist<min)
					min = dist;
				aux=aux->sig;	
			}
	
			a->diametro_mayor[visitados] = max;
			a->diametro_menor[visitados] = min;

			visitados++;
			
		}
		i++;
	}
}


//analizador, funcion principal
void analizar(analisis *a, gray **foto, int fil, int col){
	int i, j, k;
	int vecinos[4];
	generar_analisis(a, 100, fil, col);
	for (i = 4; i < fil-4; i++){
		for (j = 4; j < col-4; j++){

			if(foto[i][j] == 0){ //Pixel de una figura detectado!
				//Obtenemos los vecinos
				//Arriba
				vecinos[0] = a->b->dev[i-1][j];
				//Izquierda
				vecinos[1] = a->b->dev[i][j-1];
				// Diagonal Arriba izquierda
				vecinos[2] = a->b->dev[i-1][j-1];
				// Diagonal arriba derecha
				vecinos[3] = a->b->dev[i-1][j+1];

				if (tiene_vecinos(vecinos, 4)) {
					//si no tiene vecinos, creamos un nuevo conjunto
					a->b->n_conjuntos++;
					a->b->control++;
					insertar(a->b, i, j, a->b->control, es_frontera(foto, i, j));
					
				}else{
					//necesario para gestionar correctamente n_conjuntos
					a->b->n_conjuntos++;
					insertar(a->b, i, j, 0, es_frontera(foto, i, j));
					for( k = 0; k < 4; k++)
						if(vecinos[k] > 0)
							unir(a->b, vecinos[k], a->b->dev[i][j]);
				}

			}
		}
	}
	// Foto analizada, pasamos al calculo de propiedades
	generar_componentes(a);
	obtener_propiedades(a);

	//a->b->dev			<---- MATRIZ INT RESULTANTE
	//a->b->n_conjuntos	<---- NUMERO DE FIGURAS DETECTADOS
	//a->area			<---- AREAS
	//a->perimetro		<---- PERIMETROS
	//a->centro			<---- CENTROS
	//a->diametro_mayor	<---- DIAMETRO MAYOR
	//a->diametro_menor <---- DIAMETRO MENOR

}

