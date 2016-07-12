#!/bin/bash


make clean
make intel

#versão sequencial

mkdir serialTimes
mkdir parallelTimes

for k in 20 21; do

	i=0

	while [ $i -lt 200 ]; do

		var=$(./serialQuickSort $k | grep QuickSort | awk '{print $4}')

		echo $k,$var >> serialTimes/results.txt

		let i=i+1

	done

	for threads in 2 4 8; do

		i=0

		export OMP_NUM_THREADS=$threads

		while [ $i -lt 200 ]; do

			var=$(./parallelQuickSort $k | grep QuickSort | awk '{print $4}')

			echo $k,$var >> parallelTimes/results.$threads.txt

			let i=i+1

		done

	done

done

make clean