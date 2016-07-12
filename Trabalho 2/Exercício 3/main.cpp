//C++ Standard Library
#include <vector>	//std::vector
#include <iostream> //std::cout, std::endl
#include <cmath>	//std::pow
#include <chrono>	//std::chrono::system_clock, std::chrono::steady_clock, std::chrono::duration
#include <random>	//std::uniform_real_distribution, std::mt19937
#include <string>	//std::string
#include <cstdlib>	//std::stoi

//OpenMP
#include <omp.h>	//omp_get_thread_num, omp_set_num_threads, omp_set_dynamic, omp_get_num_threads

int threadID = 0;
int num_threads = 0;

int pointsInCircle = 0;


/* definitions:

-unit square, side 2R=1, R=0.5

-square and circle centered on (0, 0)

*/

typedef struct coordinates { double x = 0; double y = 0; } coordinates;


bool isInsideCircle(coordinates coords){

	return ( (std::pow(coords.x, 2) + std::pow(coords.y, 2)) <= 0.25 ) ? true : false;

}


std::vector<coordinates> getRandomPoints(int tid, int nth, int amount){

	std::vector<coordinates> points;
	points.reserve(amount);

	//Random seed: time
	unsigned seed = (std::chrono::system_clock::now().time_since_epoch().count() * tid) / nth;
	
	//Pseudo Random Number Generator Mersenne Twister 19937
	std::mt19937 generator(seed);

	//Mapping the integers to points in the unit square
	std::uniform_real_distribution<double> distribution(-0.5, 0.5);

	for (int i = 0; i < amount; i++){

		coordinates point;
		point.x = distribution(generator);
		point.y = distribution(generator);
		points.push_back(point);

	}

	return points;

}


int main(int argc, char const *argv[])
{
	
	if (argc != 3){

		std::cout << "Usage: ./picalc [number of threads] [number of points]" << std::endl;

	}

	auto startTime = std::chrono::steady_clock::now();
	auto endTime = std::chrono::steady_clock::now();	

	const std::string argument1 = argv[1];
	const std::string argument2 = argv[2];
	int pointsAmount = std::stoi(argv[2]);

	//Disabling dynamic thread groups (forces the requested amount of threads)
	omp_set_dynamic(0);

	num_threads = std::stoi(argv[1]);
	omp_set_num_threads(num_threads);

	std::cout << "Starting PI calculation through Monte Carlo Method, with " << num_threads << " threads..." << std::endl;
	

#pragma omp parallel reduction(+: pointsInCircle) default(none) shared(pointsAmount, std::cout) private(threadID, num_threads)
	{

	pointsInCircle = 0;

	threadID = omp_get_thread_num();
	num_threads = omp_get_num_threads();



	int threadPoints = (pointsAmount + num_threads - 1) / num_threads;

	threadPoints = (threadID * threadPoints < pointsAmount) ? threadPoints : (threadPoints - (threadID * threadPoints - pointsAmount));

	std::vector<coordinates> points = getRandomPoints(threadID, num_threads, threadPoints);

	for (auto & pt : points) {

		if(isInsideCircle(pt)) pointsInCircle++;

	}


	} //end of parallel region

	double pi = ((4.0 * pointsInCircle) / pointsAmount);

	std::cout << "Done!" << std::endl;

	std::cout << "pi = " << pi << std::endl;

	std::cout << "Execution time: " << std::chrono::duration<double, std::milli>(endTime - startTime).count() << " ms" << std::endl;


	return 0;
}

