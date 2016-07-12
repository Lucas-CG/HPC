gnu:
	g++ -fopenmp -std=c++11 -g -O3 serialQuickSort.cpp -o serialQuickSort
	g++ -fopenmp -std=c++11 -g -O3 parallelQuickSort.cpp -o parallelQuickSort

intel:
	icpc -qopenmp -std=c++11 -g -O3 serialQuickSort.cpp -o serialQuickSort
	icpc -qopenmp -std=c++11 -g -O3 parallelQuickSort.cpp -o parallelQuickSort

clean:
	rm -f serialQuickSort parallelQuickSort *.o
