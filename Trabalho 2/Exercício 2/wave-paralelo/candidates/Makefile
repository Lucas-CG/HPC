gnu:
	gcc -fopenmp -g -O3 parallel_wave_1_dynamic.cc -o parallel_wave_1.dynamic.exe
	gcc -fopenmp -g -O3 parallel_wave_2_dynamic.cc -o parallel_wave_2.dynamic.exe
	gcc -fopenmp -g -O3 parallel_wave_3_dynamic.cc -o parallel_wave_3.dynamic.exe
	gcc -fopenmp -g -O3 parallel_wave_1_static.cc -o parallel_wave_1.static.exe
	gcc -fopenmp -g -O3 parallel_wave_2_static.cc -o parallel_wave_2.static.exe
	gcc -fopenmp -g -O3 parallel_wave_3_static.cc -o parallel_wave_3.static.exe
	gcc -fopenmp -g -O3 parallel_wave_1_guided.cc -o parallel_wave_1.guided.exe
	gcc -fopenmp -g -O3 parallel_wave_2_guided.cc -o parallel_wave_2.guided.exe
	gcc -fopenmp -g -O3 parallel_wave_3_guided.cc -o parallel_wave_3.guided.exe

intel:
	icc -qopenmp -g -O3 parallel_wave_1_dynamic.cc -o parallel_wave_1.dynamic.exe
	icc -qopenmp -g -O3 parallel_wave_2_dynamic.cc -o parallel_wave_2.dynamic.exe
	icc -qopenmp -g -O3 parallel_wave_3_dynamic.cc -o parallel_wave_3.dynamic.exe
	icc -qopenmp -g -O3 parallel_wave_1_static.cc -o parallel_wave_1.static.exe
	icc -qopenmp -g -O3 parallel_wave_2_static.cc -o parallel_wave_2.static.exe
	icc -qopenmp -g -O3 parallel_wave_3_static.cc -o parallel_wave_3.static.exe
	icc -qopenmp -g -O3 parallel_wave_1_guided.cc -o parallel_wave_1.guided.exe
	icc -qopenmp -g -O3 parallel_wave_2_guided.cc -o parallel_wave_2.guided.exe
	icc -qopenmp -g -O3 parallel_wave_3_guided.cc -o parallel_wave_3.guided.exe	

clean:
	rm -f *.exe *.o
