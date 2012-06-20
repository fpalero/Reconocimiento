#!/bin/bash
num2=$(head -n +1 prova) 

for tipo in arandela tuerca tornillo
do
  rm ../resultados/$tipo
done

for tipo in arandela tuerca tornillo
do
  grep $tipo ../resultados/validacion | cut -d " " -f -$num2 >> ../resultados/$tipo
  grep $tipo ../resultados/entrenamiento | cut -d " " -f -$num2 >> ../resultados/$tipo
done


