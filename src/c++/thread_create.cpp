#include <iostream>
#include <thread>
#include <chrono>

void example_thread() 
{
	std::cout << "I am example thread!" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	std::cout << "My thread Id: " << std::this_thread::get_id()
		<< std:: endl;
	return;
}

int main(int argc, char **argv) {
	std::thread thr_obj(example_thread); // threads starts rightaway

	//wait for thread to complete
	thr_obj.join();

	std::cout << "End of main" << std::endl;

	return 0;
}
