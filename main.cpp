
#include <iostream>

#include "application.h"

int main()
{
	try
	{
		Application application{};
	}
	catch (const char* errorMsg)
	{
		std::cout << errorMsg << std::endl;
	}

	return 0;
}