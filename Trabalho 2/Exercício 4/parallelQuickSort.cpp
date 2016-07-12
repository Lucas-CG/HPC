//C++ Standard Library
#include <vector>	//std::vector
#include <iostream> //std::cout, std::endl
#include <random>	//std::uniform_int_distribution, std::mt19937
#include <cstdlib>	//std::stoi
#include <string>	//std::string
#include <chrono>	//std::chrono::steady_clock, std::chrono::duration
#include <climits>	//INT_MAX
#include <cmath>	//std::pow

//OpenMP - for parallel vector generation
#include <omp.h>	//omp_get_thread_num, omp_set_dynamic, omp_get_num_threads

//Parallel vector generation
std::vector<int> generateRandomSequence(int size){

	std::vector<int> result(size, 0);
	result.reserve(size);


#pragma omp parallel shared(size, result) num_threads(4)
{

	int num_threads = omp_get_num_threads();

	int threadID = omp_get_thread_num();

	unsigned seed = ( (std::chrono::system_clock::now().time_since_epoch().count() * (threadID+1) ) / (num_threads+1) );

	//Mersenne Twister -> 1 por thread
	std::mt19937 generator(seed);

	std::uniform_int_distribution<int> distribution(0, INT_MAX);


	for (int i = threadID; i < result.size(); i += num_threads){
		
		result.at(i) = distribution(generator);

	}



} //end of parallel region


	return result;

}


void swap(int & a, int & b){

	int aux = a;

	a = b;

	b = aux;

}

int partition(std::vector<int> & sequence, int first, int last){

	const int pivotPos = first + (last - first) / 2;
	const int pivot = sequence.at(pivotPos);

	//Moving pivot to front
	swap(sequence.at(pivotPos), sequence.at(first));

	int i = first + 1;
	int j = last;

	while (i <= j){

		while (i <= j && sequence.at(i) <= pivot) ++i;

		while (i <= j && sequence.at(j) > pivot) --j;

		if (i < j) swap(sequence.at(i), sequence.at(j));

	}

	swap(sequence.at(i - 1), sequence.at(first));

	return i - 1;


}
//passing vector by reference to avoid copying
void quicksort(std::vector<int> & sequence, int first, int last){

	if(first >= last) return;

	int part = partition(sequence, first, last);

	#pragma omp parallel default(none) shared(sequence, first, last, part)
	{
		

		#pragma omp sections
		{

			#pragma omp section
			{

				quicksort(sequence, first, part - 1);

			}

			#pragma omp section
			{

				quicksort(sequence, part + 1, last);

			}

		}

	}

}



int main(int argc, char const *argv[]) {
	
	if(argc != 2) {
	
		std::cout << "Usage: ./parallelQuickSort [power of 2 to be used]" << std::endl;
		std::cout << "Don't forget to export the OMP_NUM_THREADS environment variable!" << std::endl;

	}

	std::string argument = argv[1];

	int exponent = stoi(argument);

	int size = (int)(std::pow(2.0, exponent));

	//Disabling dynamic thread groups (forces the requested amount of threads)
	omp_set_dynamic(0);

	auto vStartTime = std::chrono::steady_clock::now();

	std::vector<int> seq = generateRandomSequence(size);

	auto vEndTime = std::chrono::steady_clock::now();


	auto qStartTime = std::chrono::steady_clock::now();

	quicksort(seq, 0, (seq.size() - 1));

	auto qEndTime = std::chrono::steady_clock::now();

	//for (auto & it : seq) std::cout << it << std::endl; //debug

	std::cout << "Vector generation time: " << std::chrono::duration<double, std::milli>(vEndTime - vStartTime).count() << " ms" << std::endl;
	std::cout << "QuickSort execution time: " << std::chrono::duration<double, std::milli>(qEndTime - qStartTime).count() << " ms" << std::endl;
	std::cout << "Total time: " << std::chrono::duration<double, std::milli>(qEndTime - vStartTime).count() << " ms" << std::endl;

	return 0;

}