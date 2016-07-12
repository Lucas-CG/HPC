#!/bin/bash

for compiler in gnu intel; do

	make $compiler

	#mkdir results.$compiler
	mkdir times.$compiler

	for times in 1; do

		for collapse in 1 2 3; do

			for scheduling in dynamic guided static; do

				#mkdir -p results.$compiler/$collapse.$scheduling/$times
				{ time ./parallel_wave_$collapse.$scheduling.exe; } 2>> times.$compiler/$collapse.$scheduling.txt

				#mv *.plot results.$compiler/$collapse.$scheduling/$times
				#mv *.dat results.$compiler/$collapse.$scheduling/$times
				#gnuplot results.$compiler/$collapse.$scheduling/$times/*.plot
				rm *.dat *.plot

			done

		done

	done

	make clean

done
