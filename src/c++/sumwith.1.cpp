#include <iostream>
#include <thread>
#include <random>
#include <string>
#include <chrono>

/*
* Function to initialize array with random float values
*/
void initDataArray(float *array_p, size_t arr_size) {
	if(array_p == nullptr) // ceck for null pionter
		return;
	std::random_device rnd_dev;
	std::mt19937 rnd_generator(rnd_dev()); // engine generates random integer
	std::uniform_real_distribution<> uniform_dis(1.0, 2.0); // double between 1.0 and 2.0

	for (int i = 0; i < arr_size; ++i) {
		array_p[i] = uniform_dis(rnd_generator); //uniform distribution using generator
	}
	return;
}

/*
* Function to print array of float
*/

void printDataArray(float *array_p, size_t arr_size) {
	if(array_p == nullptr) // check null pointer
		return;
	for (int i = 0; i < arr_size; ++i) {
		std::cout << array_p[i] << " "; //print space after each number
	}
	std::cout << std::endl; //print newline in the end
	return;
}

int main(int argc, char **argv) {
	std::cout << "Program to allocate array, initialize, and print its values" << std::endl;
	if(argc < 2) {
		std::cout << "Usage: " <<argv[0] << " <array_size> " << std::endl;
		return 1;
	}
	int n_items = atoi(argv[1]); //convert string to int
	if (n_items == 0) { // 0 size not allowed
		std::cout << "Usage: " << argv[0] << " <array_size> " << std::endl;
		return 1;
	}

	//allocate heap memory for array of n_items and store pointer in f__array_p
	float *f_array_p = nullptr; //initialze with null
	std::cout << "Requesting " << n_items * sizeof(float) << "bytes of memory." << std::endl;
	f_array_p = new float[n_items] (); // n_items * sizeof(float) bytes of memory
	if (f_array_p == nullptr) {
		std::cout << "Memory allocation failed: " 
			<< n_items * sizeof(float)
			<< " bytes of memory."
		    << std::endl;
		return 1;
	}

	std::cout << "Initialize array.. ";
	initDataArray(f_array_p, n_items); //call initDataArray function pass ptr and size
	std::cout << "done." << std::endl;

	std::cout << "Printing array: ";
	printDataArray(f_array_p, n_items); // print data array


	//Release heap memory back to system
	delete[] f_array_p; // use delete[] for array

	std::cout << "End of program" << std::endl;
}

