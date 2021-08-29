#include <iostream>
#include <thread>
#include <random>
#include <string>
#include <chrono>

/*
 * File: sumwith.3.cpp (implemented WorkerThread and total sum)
 * Changes: Fixes WorkerThread loops to launch enough WorkerThreads to sum
 *     and adjusts startindex for worker based on array size
 * Build: 
 *       g++ sumwith.3.cpp -lpthread -o sumwith.cpp
 * or add -DDEBUG to enable debug prints
 *       g++ -DDEBUG sumwith.3.cpp -lpthread -o sumwith.cpp
 * To run:
 *   ./sumwith 20 4 
 * where 20 is the size of array, 4 is number of workers 
 * or
 *   ./sumwith 50 10
 * where 50 is array size, 10 is number of workers
 */

#define USE_THREADS_DEFINE_THREADS_SHARED_DATA_GLOBAL_TO_ALL_THREADS
float *f_array_p; // This array is shared by threads so it MUST NOT be a local variable
float *worker_sum; // Array of worker_sum
const int NUM_ELEMENTS_PER_WORKER = 5; // Number of array elements to SUM by Worker

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
	if (arr_p == nullptr) return;

	float sum = 0.0; // additive identity, initialize
	// check i less than len AND less than start + 5 elements
	for (int i = startindex; i < len && i < startindex+NUM_ELEMENTS_PER_WORKER; ++i) {
		sum = sum + arr_p[i];
	}

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

	float serial_sum = 0.0;
	auto starttime = std::chrono::steady_clock::now();
	for (int i = 0; i < n_items; ++i) serial_sum += f_array_p[i];
	auto endtime = std::chrono::steady_clock::now();
	std::chrono::duration<double> time_taken = endtime - starttime;
	std::cout << "Serial sum: " << serial_sum
		 << " took " << time_taken.count() << " seconds"
	     << std::endl;

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

	// VERSION - INVOKES ONLY NEEDED THREADS AND HANDLES ANY ARRAY SIZE, THREADS
	// Suppose array size is N, number of threads is P, and NELEMS is number of
	//  of elements per thread.
	// Number of parallel sums possible: N/NELEMS (CAUTION: Integer division!!)
	// N/NELEMS will work ONLY IF array size is EXACT multiple of NELEMS
	// CORRECT FORMULA: (N+NELEMS-1)/NELEMS is # of parallel sums possible
	// Example: Array size N=10, NELEMS=5. Then (10+5-1)/5 = 14/5 = 2 parallel sums
	//     Worker 0: Sum of Index 0,1,2,3,4 (Parallel Sum#1)
	//     Worker 1: Sum of Index 5,6,7,8,9 (Parallel Sum#2)
	// Example: Array size N=10, NELEMS=3. # of Parallel Sums: (10+3-1)/3 = 12/3 = 4
	//     Worker 0: Sum of Index 0,1,2 (Parallel Sum#1)
	//     Worker 1: Sum of Index 3,4,5 (Parallel Sum#2)
	//     Worker 2: Sum of Index 6,7,8 (Parallel Sum#3)
	//     Worker 3: Sum of Index 9     (Parallel Sum#4) [Rest index out-of-range!]
	//
	// If NPS is number of Parallel Sums (based on above calculations), and P is the
	// number of parallel threads allowed.
	//   CASE 0: P == NPS:
	//        Launch P worker threads ONCE. Note: (NPS+P-1)/P = 1 (one)
	//   CASE 1: P < NPS: (number of threads less than number of parallel sums)
	//        Loop (NPS+P-1)/P times: Launch P threads at a time
	//   CASE 2: P > NPS: more threads than needed
	//        Launch NPS threads (avoid waste launching P-NPS workers with nothing to do)
	// In this code, P -> max_worker_threads_count
	//               N -> n_items
	// NPS: Number of parallel sums possible is
	int NPS = (n_items+NUM_ELEMENTS_PER_WORKER-1)/NUM_ELEMENTS_PER_WORKER;
	
	float total_sum = 0.0; // initialize total sum
	auto pstarttime = std::chrono::steady_clock::now();
	for (int step = 0;
	     step < (NPS+max_worker_threads_count-1)/max_worker_threads_count; // (NPS+P-1)/P
		 ++step) {

		// Start operating at index step*max_worker_threads
		int base_index = step*max_worker_threads_count*NUM_ELEMENTS_PER_WORKER;

	    for (int i = 0; i < max_worker_threads_count; ++i) { // Launch P threads
			// XXX: Always launching P threads. What about CASE 2?
		    workers[i] = std::thread(WorkerThread, f_array_p, n_items,
		                             base_index+i*NUM_ELEMENTS_PER_WORKER,
			                          &worker_sum[i]);
	    }

	    // wait for workers to complete
	    for (int i = 0; i < max_worker_threads_count; ++i) {
		    workers[i].join();
	    }

	    // Accumulate the sum of worker_sum to get total
	    for (int i = 0; i < max_worker_threads_count; ++i) {
		    total_sum = total_sum + worker_sum[i];
	    }

#ifdef DEBUG
	    // print worker_sum for DEBUG purposes
	    for (int i = 0; i < max_worker_threads_count; ++i) {
	       std::cout << "Worker[" << i << "] sum = " << worker_sum[i] << std::endl;
	    }
#endif
    }
	auto pendtime = std::chrono::steady_clock::now();
	std::chrono::duration<double> ptime_taken = pendtime - pstarttime;


	std::cout << "Parallel sum: " << total_sum
	     << " took " << ptime_taken.count() << " seconds."
	     << std::endl;

	// Release heap memory back to system
	delete[] f_array_p; // use delete[] for array
	delete[] worker_sum; // use delete[] for array

	std::cout << "End of program!" << std::endl;
}
