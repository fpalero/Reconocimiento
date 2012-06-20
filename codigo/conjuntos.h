
typedef struct _tnodo_{
	int x, y;
	struct _tnodo_ *sig;
}nodo;

typedef struct _tbase_{
	nodo *c;			//nodo a fronteras
	nodo *c_fin;
	nodo *h;			//nodo a huecos
	nodo *h_fin;
	int ins_c, ins_h;	//contadores para contorno y area
	int sum_x, sum_y;	//sumatorios para centros
	float mom[10];		//vector momentos
}base;

typedef struct _tbector_{
	int generada;		//variable de control para destructor
	int max_val;		//tamanyo del vector
	int n_conjuntos, control, fil, col, objetos;
	base *vec;
	int **dev;
}bector;

// constructores
void inicializar(bector *b);
void generar(bector *b, int tope, int filas, int columnas);

// destructores
void limpia_casilla(bector *b, int val);
void vaciar(bector *b);

// funciones momento de cada punto
float my_pow(int x, int p);
float funct_mom(int x, int y, int p, int q);

// inserci�n de puntos
void insertar(bector *b, int x_val, int y_val, int donde, int es_frontera);

//fusion de conjuntos
void unir(bector *b, int conj1, int conj2);

//apilar al comienzo del vector todos los conjuntos
void cambia_posicion(bector *b, int conj1, int conj2);
void agrupar(bector *b, int n_conjuntos);
