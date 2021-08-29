#include <iostream>
#include <thread>
#include <chrono>

void worker_thread(int i)
{
	std::cout << "Worker thread " << i << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	std::cout << i << " woke up " << std::endl;

	return;
}

void manager_thread(int i)
{
	std::cout << "Manager thread " << i << std::endl;
	std::cout << "My thread id " << std::this_thread::get_id() << std::endl;
	return;
}

int main(int argc, char **argv) {
	std::thread threads[10]; // array of thread objects

	for(int i = 0; i < 10; ++i) {
		threads[i] = std::thread(worker_thread, i);
	}

	// create manager thread
	std::thread mgr_thread(manager_thread, 10); // thread created and started
	
	//wait for all the worker threads
	for(int i = 0; i < 10; ++i) {
		threads[i].join();
	}

	//wait for manager thread
	mgr_thread.join();

	std::cout << "end of main" << std::endl;

	return 0;
}
