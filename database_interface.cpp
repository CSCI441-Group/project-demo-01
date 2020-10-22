#include "database_interface.h"

DatabaseInterface::DatabaseInterface()
{
	rc = sqlite3_open("test.db", &database);

	if (rc != SQLITE_OK)
		throw sqlite3_errmsg(database);

	createTables();
}

DatabaseInterface::~DatabaseInterface()
{
	sqlite3_close(database);
}

int DatabaseInterface::callback(void* data, int argc, char** argv, char** azColName)
{
	// Reinterpret the user data as a vector that was passed in
	std::vector<std::vector<std::string>>* results = reinterpret_cast<std::vector<std::vector<std::string>>*>(data);
	// Add a new row of values to the vector unless the vector provided has a row ready to be used
	results->push_back(std::vector<std::string>{});
	// Fill the row with values
	for (int i{}; i < argc; i++)
	{
		results->back().push_back(argv[i]);
	}

	return 0;
}

void DatabaseInterface::createTables()
{
	rc = sqlite3_exec(database, sql::CreateTables.c_str(), nullptr, nullptr, &errorMessage);

	if (rc != SQLITE_OK)
		throw errorMessage;

	std::cout << "Tables created successfully" << std::endl;
}

Employee::Type DatabaseInterface::getEmployeeType(Certification& certification)
{
	sql = std::string{ "SELECT type FROM employee WHERE id=" + std::to_string(certification.id) + " AND password='" + certification.password + "';" };
	std::vector<std::vector<std::string>> results{};
	rc = sqlite3_exec(database, sql.c_str(), callback, &results, &errorMessage);

	if (rc != SQLITE_OK)
		throw errorMessage;

	return static_cast<Employee::Type>(std::stoi(results[0][0]));
}

void DatabaseInterface::addEmployee(Certification& certification, Employee& employee)
{
	// Get the type of the employee attempting to add a new user
	Employee::Type type{ getEmployeeType(certification) };

	// Only add a new employee if the user attempting to add a new employee is an admin or manager
	if (type == Employee::Type::Admin || type == Employee::Type::Manager)
	{
		sql = std::string{ "INSERT OR IGNORE INTO employee(first_name, last_name, password, type)			\
		VALUES('" + employee.getFirstName() + "','" + employee.getLastName() + "','" + employee.getCertification().password + "'," + std::to_string(static_cast<int>(employee.getType())) + ");" };

		rc = sqlite3_exec(database, sql.c_str(), nullptr, nullptr, &errorMessage);

		if (rc != SQLITE_OK)
			throw errorMessage;

		std::cout << "Employee added successfully!" << std::endl;
	}
}

void DatabaseInterface::addCategory(Certification& certification, std::string& description)
{

}

void DatabaseInterface::listEmployees()
{
	sql = std::string{ "SELECT * FROM employee;" };
	std::vector<std::vector<std::string>> results{};
	rc = sqlite3_exec(database, sql.c_str(), callback, &results, &errorMessage);

	if (rc != SQLITE_OK)
		throw errorMessage;

	std::cout << "Employees:\n";
	for (int i{}; i < results.size(); i++)
	{
		for (int j{}; j < results[i].size(); j++)
			std::cout << results[i][j] << '\t';
		std::cout << '\n';
	}
}