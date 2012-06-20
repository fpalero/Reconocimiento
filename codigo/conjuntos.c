#include <stdlib.h>
#include "conjuntos.h"



// constructores
void inicializar(bector *b){
	b->vec = 0;
	b->generada = 0;
	b->max_val = 0;
	b->n_conjuntos = 0;
	b->control = 0;
	b->fil = 0;
	b->col = 0;
	b->dev = 0;
	b->objetos = 0;
}

void generar(bector *b, int tope, int filas, int columnas){
	int i, j;
	if(b->generada > 0)
		vaciar(b);

	inicializar(b);
	b->max_val = tope;
	b->fil = filas;
	b->col = columnas;
	b->vec = (base *) malloc(sizeof(base) * tope);
	
	for(i=0; i< b->max_val; i++){
		b->vec[i].c = 0;
		b->vec[i].h = 0;
		b->vec[i].h_fin = 0;
		b->vec[i].c_fin = 0;
		b->vec[i].ins_c = 0;
		b->vec[i].ins_h = 0;
		b->vec[i].sum_x = 0;
		b->vec[i].sum_y = 0;
		for(j=0;j<10;j++)
			b->vec[i].mom[j] = 0.0f;
	}
	
	b->dev = (int **) malloc(sizeof(int *) * filas);
	for(i=0; i < filas; i++)
		b->dev[i] = (int *) malloc(sizeof(int) * columnas);

	for(i = 0; i< filas; i++)
		for(j=0; j < columnas; j++)
			b->dev[i][j] = 0;

	b->generada = 1;

}



// destructores
void limpia_casilla(bector *b, int val){
	nodo *aux, *aborrar;
	if( (val < b->max_val) && (b->vec[val].c != 0) ){
		//Hay informacion insertada en frontera
		//borrar la lista
		aux = b->vec[val].c;
		aborrar = aux;
		while( aux != 0 ){
			
			aux = aux->sig;
			//free(aborrar);
			aborrar = aux;
		}
	}
	if( (val < b->max_val) && (b->vec[val].h != 0 ) ){
		//Hay informacion insertada en huecos
		//borrar la lista
		aux = b->vec[val].h;
		while(aux != 0 ){
			aborrar = aux;
			aux = aux->sig;
			free(aborrar);
		}
	}
	//La lista ha sido borrada

	b->vec[val].c = 0;
	b->vec[val].c_fin = 0;
	b->vec[val].h = 0;
	b->vec[val].h_fin = 0;
	b->vec[val].ins_c = 0;
	b->vec[val].ins_c = 0;
	b->vec[val].ins_h = 0;
	b->vec[val].sum_x = 0;
	b->vec[val].sum_y = 0;

}

void vaciar(bector *b){
	int i=0;
	if(b->generada > 0){
		for(i=0; i < b->max_val; i++)
			limpia_casilla(b,i);

		free(b->vec);

		for(i=0; i < b->fil; i++)
			free(b->dev[i]);
		free(b->dev);

		inicializar(b);
	}

}

float my_pow(int x, int p){
	int i;
	int dev = 1;
	
	if(p == 0)
	   return 1.0;

	for(i=0; i<p; i++)
		dev*=x;

	return dev*1.0f;

}
float funct_mom(int x, int y, int p, int q){
	return (my_pow(x, p) * my_pow(y, q));
}

// inserci�n de puntos
void insertar(bector *b, int x_val, int y_val, int donde, int es_frontera){
	nodo *nuevo;
	if(b == 0)
		return;
	
	nuevo = (nodo *) malloc(sizeof(nodo));
	nuevo->x = x_val;
	nuevo->y = y_val;
	nuevo->sig = 0;
	if(es_frontera >= 1){
		//el punto a insertar es una frontera
		if(b->vec[donde].c == 0){
			//lista vacia, insertamos primer elemento	
			b->vec[donde].c = nuevo;
			b->vec[donde].c_fin = nuevo;
			//b->vec[donde].c_fin->sig = nuevo->sig;
			b->objetos++;
		}
		else{
			//lista con algun elemento ya, insertamos al inicio
			nuevo->sig = b->vec[donde].c;
			b->vec[donde].c = nuevo;
		}
		//punto insertado
		b->dev[nuevo->x][nuevo->y] = donde;
		//incrementamos ins_c para el contorno y el area
		b->vec[donde].ins_c++;
		//acumulamos x e y para obtener los centros m�s adelante
		b->vec[donde].sum_x += nuevo->x;
		b->vec[donde].sum_y += nuevo->y;
	}
	else{
		//el punto a insertar es un hueco
		if(b->vec[donde].h == 0){
			//lista vacia, insertamos primer elemento	
			b->vec[donde].h = nuevo;
			b->vec[donde].h_fin = nuevo;
			//b->vec[donde].h_fin->sig = nuevo->sig;
		}
		else{
			//lista con algun elemento ya, insertamos al inicio
			nuevo->sig = b->vec[donde].h;
			b->vec[donde].h = nuevo;
		}
		//punto insertado
		b->dev[nuevo->x][nuevo->y] = donde;

		b->vec[donde].ins_h++;
	}
	//momentos
/*
const int n20 = 0;//vector de momentos
const int n02 = 1;
const int n11 = 2;
const int n30 = 3;
const int n12 = 4;
const int n21 = 5;
const int n03 = 6;
*/
	/*b->vec[donde].mom[0] += funct_mom(x_val, y_val, 1, 0);
	b->vec[donde].mom[1] += funct_mom(x_val, y_val, 0, 1);
	b->vec[donde].mom[2] += funct_mom(x_val, y_val, 0, 0);*/
	//b->vec[donde].mom[3] += funct_mom(x_val, y_val, 3, 0);
	//b->vec[donde].mom[4] += funct_mom(x_val, y_val, 1, 2);
	//b->vec[donde].mom[5] += funct_mom(x_val, y_val, 2, 1);
	//b->vec[donde].mom[6] += funct_mom(x_val, y_val, 0, 3);

}

//fusion de conjuntos
void unir(bector *b, int conj1, int conj2){
	int min, max;
	nodo *aux;
	
	if( b==0 || b->vec[conj1].ins_c == 0 || b->vec[conj2].ins_c == 0)
		//alguno de los conjuntos o la base valen 0
		return;
	
	if(conj1 == conj2)
		//los dos conjuntos son el mismo
		return;
	
	//vamos a unir conjuntos sobre el de menor indice de los dos
	if(conj1 < conj2)
		min = conj1;
	else
		min = conj2;
	
	if(conj1 > conj2)
		max = conj1;
	else
		max = conj2;

	if (min == 0) {
		min = max;
		max = 0;
	}

	//Actualizamos matriz dev
	aux = b->vec[max].c;
	while(aux != 0){
		b->dev[aux->x][aux->y] = min;
		aux = aux->sig;
	}
	aux = b->vec[max].h;
	while(aux != 0){
		b->dev[aux->x][aux->y] = min;
		aux = aux->sig;
	}

	//Union de conjuntos sobre min
	//b->vec[min].c_fin->sig	= b->vec[max].c;
	//b->vec[min].c_fin	= b->vec[max].c_fin;
	
	b->vec[max].c_fin->sig	= b->vec[min].c;
	b->vec[min].c	= b->vec[max].c;

	if(b->vec[min].ins_h > 0 && b->vec[max].ins_h > 0){
		//caso min y max tienen huecos
		b->vec[max].h_fin->sig = b->vec[min].h;
		//b->vec[max].h_fin->sig = b->vec[min].h_fin->sig;
		b->vec[min].h = b->vec[max].h;
	}else if(b->vec[min].ins_h == 0 && b->vec[max].ins_h > 0){
		//caso min NO tiene huecos pero max si
		b->vec[min].h = b->vec[max].h;
		b->vec[min].h_fin = b->vec[max].h_fin;
		
	}	//caso min tiene pero MAX no -> no se hace nada

	b->vec[min].ins_c += b->vec[max].ins_c;
	b->vec[min].ins_h += b->vec[max].ins_h;
	b->vec[min].sum_x += b->vec[max].sum_x;
	b->vec[min].sum_y += b->vec[max].sum_y;
	b->vec[min].mom[0] += b->vec[max].mom[0];
	b->vec[min].mom[1] += b->vec[max].mom[1];
	b->vec[min].mom[2] += b->vec[max].mom[2];
	
	//Limpiamos indice max
	b->vec[max].c = 0;
	b->vec[max].c_fin = 0;
	b->vec[max].h = 0;
	b->vec[max].h_fin = 0;
	b->vec[max].ins_c = 0;
	b->vec[max].ins_h = 0;
	b->vec[max].sum_x = 0;
	b->vec[max].sum_y = 0;
		
	b->vec[min].mom[0] = 0;
	b->vec[min].mom[1] = 0;
	b->vec[min].mom[2] = 0;
	

	//Un conjunto menos
	b->n_conjuntos--;
	b->objetos--;
}

//apilar al comienzo del vector todos los conjuntos
void cambia_posicion(bector *b, int conj1, int conj2){
	b->vec[conj1].c = b->vec[conj2].c;
	b->vec[conj1].c_fin = b->vec[conj2].c_fin;
	b->vec[conj1].ins_c = b->vec[conj2].ins_c;
	b->vec[conj1].ins_h = b->vec[conj2].ins_h;
	b->vec[conj2].c = 0;
	b->vec[conj2].c_fin = 0;
	b->vec[conj2].ins_c = 0;
	b->vec[conj2].ins_h = 0;

}

void agrupar(bector *b, int n_conjuntos){
	int visitados = 0;
	int i=1;
	int j;
	if(b == 0)
		return;


	while(visitados <n_conjuntos){
		//magia...
		if(b->vec[i].ins_c ==0){
			j=i+1;
			while(j < b->max_val && b->vec[j].ins_c == 0)
				j++;

			cambia_posicion(b,i,j);
		}
		i++;
		visitados++;
	}
}

