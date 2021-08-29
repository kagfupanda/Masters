#include <iostream>
#include <chrono>

int fib(int n)
{
	if (n < 2) return 1;
	return fib(n-1) + fib(n-2);
}

int main(void) {
	auto start = std::chrono::steady_clock::now();
	int fib10 = fib(10);
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> time_taken = end - start;

	std::cout << "fib 10 took " << time_taken.count() << " seconds to compelte. " << std::endl;

	return 0;
}
