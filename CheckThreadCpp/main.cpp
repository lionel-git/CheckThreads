#include <iostream>
#include "process_checker.h"

int main(int argc, char** argv)
{
	try
	{
		process_checker checker;
		//checker.show_processes("stockfish");
		checker.show_processes("Example");
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