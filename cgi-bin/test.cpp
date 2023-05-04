#include <iostream>
#include <cstdlib>
#include <string>

int main()
{
	int a = 5;
	int b = 10;

	std::string command = "python multiple.py " + std::to_string(a) + " " + std::to_string(b);
	// Construct the command to run the Python script with a and b as arguments

	std::system(command.c_str()); // Run the command as a subprocess

	return 0;
}
