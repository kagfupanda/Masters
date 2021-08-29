#include <iostream>
#include <random>

int main(int argc, char **argv) 
{
	std::random_device rnd_dev;
	std::mt19937 rnd_generator(rnd_dev());
	std::uniform_real_distribution<float> uniform_dis(1.0, 2.0); //between 1.0 and 2.0 real numbers
	std::uniform_int_distribution<int> uniform_int_dis(100, 200); 
	// generate 10 random float numbers
	for (int i = 0; i < 10; ++i) {
		double d = uniform_dis(rnd_generator);
		std::cout << "Random Value: " << d << std::endl;
		int i_num = uniform_int_dis(rnd_generator);
		std::cout << "Random int value: " << i_num << std::endl;
	}

	return 0;
}
