#include <iostream>
#include "process_checker.h"

int main(int argc, char** argv)
{
	std::cout << "Hello world" << std::endl;

	process_checker checker;
	//checker.show_processes("svc");
	checker.show_processes("stockfish");
}