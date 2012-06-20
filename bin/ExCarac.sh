#!/bin/bash

rm ../resultados/reconocer
rm ../resultados/entrenador
for directorio in arandela tornillo tuerca
do
	for imagen in $(ls ../$directorio/*.pgm)
	do		
		./umbralizar 205 $imagen patron_frontal.pgm | ./pgmor -e 3 -d 2 | caracteristicas $directorio >> ../resultados/reconocer
	done

	for imagen in $(ls ../../sdv/img/piezas/frontal/$directorio/*.pgm)
	do		
		./umbralizar 205 $imagen patron_frontal.pgm | ./pgmor -e 3 -d 2 | caracteristicas $directorio >> ../resultados/entrenador
	done
done