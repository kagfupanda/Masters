#include <iostream>
using namespace std;

void print_args(int argc, char **argv) // char *argv[] > Java String argv[]
{
	std::cout << "Argument count, arg " << argc << std::endl;
	for(int i =0; i < argc; ++i) {
		std::cout << argv[i] << std:: endl;
	}
	return; 
}
int main(int argc, char **argv)
{
	std::cout << "Hello World!" << std::endl;
	print_args(argc, argv);
	return 0;
}
