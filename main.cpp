
#include <iostream>

#include "application.h"

int main()
{
	try
	{
		Application application{};
	}
	catch (...)
	{
		std::cout << "An error occurred!" << std::endl;
	}

	return 0;
}