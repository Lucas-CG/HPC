#!/bin/bash

mkdir results-varyingthreads
mkdir results-varyingsize

make clean
make intel

i=0

while [ $i -lt 1000 ]; do

	#set size = 10000000 -> 10^7
	
	for threads in 1 4 8 12 16 20 24 28 32 36 40 44 48 52; do

		./picalc $threads 10000000 | grep pi | awk '{print $3}' >> results-varyingthreads/$threads.txt

	done

	
	#set threads = 4

	for size in 100 1000 10000 100000 1000000 10000000; do

		./picalc 4 $size | grep pi | awk '{print $3}' >> results-varyingsize/$size.txt

	done

	let i=i+1

done


make clean