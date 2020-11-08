#include <iostream>
#include "process_checker.h"

int main(int argc, char** argv)
{
	try
	{
		std::cout << "Hello world" << std::endl;

		process_checker checker;
		//checker.show_processes("svc");
		checker.show_processes("stockfish");
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Error" << std::endl;
	}
}