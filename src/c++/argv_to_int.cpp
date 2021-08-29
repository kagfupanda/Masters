#include <iostream>
#include <string>

int main(int argc, char **argv)
{
	std::cout << "Program name: " << argv[0] << std::endl;

	if (argc < 2) { // no argument specified to convert
		std::cout << "Insufficient argument count: " << std::endl;
		std::cout << "Usage: " << argv[0] << " <integer_value> " << std::endl;
		return 1;
	}

	int num_value = std::atoi(argv[1]);
	std::cout << "Integer value read: " << num_value << std::endl;

	// convert string to float value
	float f_value = std::strtof(argv[2], NULL);
	std::cout << "Float value read: " << f_value << std::endl;

	//convert str to double
	double d_value = std::strtod(argv[3], NULL);
	std::cout << "Double value read: " << d_value << std::endl;

	
	return 0;
}
