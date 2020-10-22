
#include <iostream>

#include "database_interface.h"
#include "employee.h"

void displayMenu()
{
	std::cout << "\n1) List employees\n2) Add employee\nEnter your choice: ";
}

void addEmployee(DatabaseInterface& di)
{
	Certification cert{};

	std::cout << "Enter your ID: ";
	std::cin >> cert.id;
	std::cout << "Enter your password: ";
	std::cin >> cert.password;

	std::string firstName, lastName, password;
	int type;
	std::cout << "Enter the new employee's first name: ";
	std::cin >> firstName;
	std::cout << "Enter the new employee's last name: ";
	std::cin >> lastName;
	std::cout << "Enter the new employee's password: ";
	std::cin >> password;
	std::cout << "Enter the new employee's type: ";
	std::cin >> type;

	Employee employee{ Certification{ password }, static_cast<Employee::Type>(type), firstName, lastName };

	di.addEmployee(cert, employee);
}

int main()
{
	try
	{
		DatabaseInterface di{};

		char cont;
		int choice;
		do
		{
			displayMenu();
			
			std::cin >> choice;
			std::cout << '\n';

			switch (choice)
			{
			case 1:
				di.listEmployees();
				break;
			case 2:
				addEmployee(di);
				break;
			default:
				std::cout << "Invalid input." << std::endl;
			}

			std::cout << "\nContinue? (y/n): ";
			std::cin >> cont;
		} while (cont == 'y' || cont == 'Y');

		//Certification certification{ 0, "password" };
		//Employee employee{ Certification{ "managerpass"}, Employee::Type::Manager, "John", "Smith" };
		//di.addEmployee(certification, employee);

		di.listEmployees();
	}
	catch (const char* errorMsg)
	{
		std::cout << errorMsg << std::endl;
	}

	return 0;
}