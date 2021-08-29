#include <iostream>
#include <thread>
#include <random>
#include <string>
#include <chrono>

#define USE_THREADS_DEFINE_THREADS_SHARED_DATA_GLOBAL_TO_ALL_THREADS
float *f_array_p; // This array is shared by threads so it MUST NOT be a local variable
float *worker_sum; // Array of worker_sum

/*
 * Function to initialize array with random float values
 */
void initDataArray(float *array_p, size_t arr_size)
{
	if (array_p == nullptr) // check for null pointer
		return;

	std::random_device rnd_dev;
	std::mt19937 rnd_generator(rnd_dev()); // engine generates random integer
	std::uniform_real_distribution<> uniform_dis(1.0, 2.0); // double between 1.0 and 2.0

	for (int i = 0; i < arr_size; ++i) {
		array_p[i] = uniform_dis(rnd_generator); // uniform distribution using generator
	}
	return;
}

/*
 * Function to print array of float
 */
void printDataArray(float *array_p, size_t arr_size)
{
	if (array_p == nullptr) // check null pointer
		return;

	for (int i = 0; i < arr_size; ++i) {
		std::cout << array_p[i] << " "; // print space after each number
	}
	std::cout << std::endl; // print newline in the end
	return;
}

/*
 * Define Worker Thread to sum array elements
 * Each worker thread is given a startindex of the first element in array
 * THe worker thread will total up 5 (FIVE) elements from startindex
 * Check index against array length 'len' to not go out-of-bounds
 * The WorkerThread will return its sum by storing in ret_sum location
 */
void WorkerThread(float *arr_p, size_t len, int startindex, float *ret_sum)
{
	const int NUM_ELEMENTS_PER_WORKER = 5;
	if (arr_p == nullptr) return;

	float sum = 0.0; // additive identity, initialize
	// check i less than len AND less than start + 5 elements
	for (int i = startindex; i < len && i < startindex+NUM_ELEMENTS_PER_WORKER; ++i) {
		sum = sum + arr_p[i];
	}
	std::cout << "worker sum: " << sum << std::endl;

	if (ret_sum != nullptr) // check for nullptr
	    *ret_sum = sum; // return the sum in the location provided
    return;
}

int main(int argc, char **argv)
{
	std::cout << "Program to allocate array, initialize, and print its values"
	    << std::endl;

    if (argc < 3) {
		std::cout << "Usage: " << argv[0] << " <array_size> <n_parallel_threads> " << std::endl;
		return 1;
	}

	int n_items = atoi(argv[1]); // convert string to int
	if (n_items == 0) { // 0 size not allowed
		std::cout << "Usage: " << argv[0] << " <array_size> " << std::endl;
		return 1;
	}

	/*
	 * allocate heap memory for array of n_items and store pointer in f_array_p
	 */
#ifdef USE_THREADS_DEFINE_THREADS_SHARED_DATA_GLOBAL_TO_ALL_THREADS
	// float *f_array_p = nullptr; // initialize with null (ORIGINAL code with no threads)
	f_array_p = nullptr; // initialize global f_array_p with null
#endif
	std::cout << "Requesting " << n_items * sizeof(float) << " bytes of memory."
	    << std::endl;
	f_array_p = new float[n_items](); // n_items * sizeof(float) bytes of memory
	if (f_array_p == nullptr) {
		std::cout << "Memory allocation failed: "
			<< n_items * sizeof(float)
			<< " bytes of memory."
		    << std::endl;
		return 1;
	}

	std::cout << "Initialize array..";
	initDataArray(f_array_p, n_items); // call initDataArray function, pass ptr and size
	std::cout << "done." << std::endl;

	std::cout << "Printing array..";
	printDataArray(f_array_p, n_items); // printDataArray function, pass ptr and size
	std::cout << "done." << std::endl;

	// Start WorkerThread to compute sum of array elements
	int max_worker_threads_count = atoi(argv[2]); // Number of parallel threads argv[2]
	if (max_worker_threads_count == 0) { // invalid value
		// Release the array memory
		delete[] f_array_p;
		std::cout << "Usage: " << argv[0] << " <array_size> <n_parallel_threads> " << std::endl;
		return 1;
	}

	// PARALLEL WORKER THREAD SECTION
	std::thread  workers[max_worker_threads_count]; // declare worker threads
	worker_sum =  new float[max_worker_threads_count](); // array with one element for each worker
	for (int i = 0; i < max_worker_threads_count; ++i) worker_sum[i] = 0.0; // initialize

	// Each Worker counts 5 elements XXX:
	// worker 0 starts at 0*5, worker 1: 1*5, worker 2: 2*5 etc..
	// FIX: To loop multiple steps if NUMBER of WORKERS * NUM_ELEMENTS_PER_WORKER < TOTAL_ARRAY SIZE
	for (int i = 0; i < max_worker_threads_count; ++i) {
		workers[i] = std::thread(WorkerThread, f_array_p, n_items,
		    i*5, // XXX: 5 hardcoded, refers to 5 elems counted by each worker
			&worker_sum[i]);
	}
	// wait for workers to complete
	for (int i = 0; i < max_worker_threads_count; ++i) {
		workers[i].join();
	}

	// Accumulate the sum of worker_sum to get total
	float total_sum = 0.0;
	for (int i = 0; i < max_worker_threads_count; ++i) {
		total_sum = total_sum + worker_sum[i];
	}

	// print worker_sum for DEBUG purposes
	for (int i = 0; i < max_worker_threads_count; ++i) {
	   std::cout << "Worker[" << i << "] sum = " << worker_sum[i] << std::endl;
	}

	std::cout << "Total sum: " << total_sum << std::endl;

	// Release heap memory back to system
	delete[] f_array_p; // use delete[] for array
	delete[] worker_sum; // use delete[] for array

	std::cout << "End of program!" << std::endl;
}

