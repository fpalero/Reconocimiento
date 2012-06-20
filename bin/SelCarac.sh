#!/bin/bash

rm ../resultados/validacion
rm ../resultados/entrenamiento
num=1
num2=`expr $# - $num` 
echo $num2 > ../resultados/validacion
echo $num2 > ../resultados/entrenamiento

cadena=$*

expresion=$(echo $cadena | tr " " ",")

cut -d " " -f $expresion ../resultados/reconocer >> ../resultados/validacion
cut -d " " -f $expresion ../resultados/entrenador >> ../resultados/entrenamiento

for vecinos in $(seq 1 2 23)
do
	error=$(./clasificar $vecinos)
	echo "vecinos: $vecinos $error" 
done

