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
	rc = sqlite3_exec(database, "CREATE TABLE IF NOT EXISTS menu(				\
			id INTEGER PRIMARY KEY,													\
			parent_id INTEGER,														\
			name TEXT,																\
			FOREIGN KEY(parent_id) REFERENCES menu(id)							\
		);																			\
																					\
		CREATE TABLE IF NOT EXISTS item(											\
			id INTEGER PRIMARY KEY,													\
			menu_id INTEGER,													\
			name TEXT,																\
			price REAL,																\
			FOREIGN KEY(menu_id) REFERENCES menu(id)						\
		);																			\
																					\
		CREATE TABLE IF NOT EXISTS adjustment_group(								\
			id INTEGER PRIMARY KEY,													\
			item_id INTEGER,														\
			name TEXT,																\
			FOREIGN KEY(item_id) REFERENCES item(id)								\
		);																			\
																					\
		CREATE TABLE IF NOT EXISTS adjustment(										\
			id INTEGER PRIMARY KEY,													\
			adjustment_group_id INTEGER,											\
			name TEXT,																\
			price REAL,																\
			FOREIGN KEY(adjustment_group_id) REFERENCES adjustment_group(id)		\
		);																			\
																					\
		CREATE TABLE IF NOT EXISTS table_(											\
			id INTEGER PRIMARY KEY,													\
			status INTEGER															\
		);																			\
																					\
		CREATE TABLE IF NOT EXISTS party(											\
			id INTEGER PRIMARY KEY,													\
			table_id INTEGER,														\
			size INTEGER,															\
			status INTEGER,															\
			FOREIGN KEY(table_id) REFERENCES table_(id)								\
		);																			\
																					\
		CREATE TABLE IF NOT EXISTS employee(										\
			id INTEGER PRIMARY KEY,													\
			first_name TEXT,														\
			last_name TEXT,															\
			password TEXT,															\
			type INTEGER															\
		);																			\
																					\
		CREATE TABLE IF NOT EXISTS order_(											\
			id INTEGER PRIMARY KEY,													\
			party_id INTEGER,														\
			waiter_id INTEGER,														\
			size INTEGER,															\
			status INTEGER,															\
			total REAL,																\
			FOREIGN KEY(party_id) REFERENCES party(id),								\
			FOREIGN KEY(waiter_id) REFERENCES employee(id)							\
		);																			\
																					\
		CREATE TABLE IF NOT EXISTS order_item(										\
			id INTEGER PRIMARY KEY,													\
			order_id INTEGER,														\
			item_id INTEGER,														\
			FOREIGN KEY(order_id) REFERENCES order_(id),							\
			FOREIGN KEY(item_id) REFERENCES item(id)								\
		);																			\
																					\
		CREATE TABLE IF NOT EXISTS order_item_adjustment(							\
			order_item_id INTEGER,													\
			adjustment_id INTEGER,													\
			PRIMARY KEY(order_item_id, adjustment_id),								\
			FOREIGN KEY(order_item_id) REFERENCES order_item(id),					\
			FOREIGN KEY(adjustment_id) REFERENCES adjustment(id)					\
		);																			\
																					\
		INSERT OR IGNORE INTO employee(id, first_name, last_name, password, type)	\
		VALUES(0, 'Admin', 'User', 'password', 0);									\
																					\
		INSERT OR IGNORE INTO menu(id, parent_id, name)							\
		VALUES(0, 0, 'Menu');", nullptr, nullptr, &errorMessage);

	if (rc != SQLITE_OK)
		throw errorMessage;

	std::cout << "Tables created successfully" << std::endl;
}

void DatabaseInterface::insertSql()
{
	rc = sqlite3_exec(database, sql.c_str(), nullptr, nullptr, &errorMessage);

	if (rc != SQLITE_OK)
		throw errorMessage;
}

void DatabaseInterface::querySql(std::vector<std::vector<std::string>>& results)
{
	rc = sqlite3_exec(database, sql.c_str(), callback, &results, &errorMessage);

	if (rc != SQLITE_OK)
		throw errorMessage;
}

const Employee::Type DatabaseInterface::getEmployeeType(const Certification& certification)
{
	sql = std::string{ "SELECT type FROM employee WHERE id=" + std::to_string(certification.id) + " AND password='" + certification.password + "';" };
	std::vector<std::vector<std::string>> results{};

	querySql(results);

	if (!results.empty())
		return static_cast<Employee::Type>(std::stoi(results[0][0]));
	else
		return Employee::Type::Error;
}

bool DatabaseInterface::addEmployee(const Certification& certification, const Employee& employee)
{
	auto type{ getEmployeeType(certification) };
	// Only add a new employee if the user attempting to add a new employee is a manager
	if (type == Employee::Type::Manager)
	{
		sql = std::string{ "INSERT OR IGNORE INTO employee(first_name, last_name, password, type) \
			VALUES('" + employee.firstName + "','" + employee.lastName + "','" + employee.certification.password + "'," + std::to_string(static_cast<int>(employee.type)) + ");" };
		
		insertSql();

		return true;
	}

	return false;
}

bool DatabaseInterface::addMenu(const Certification& certification, const int parentId, const std::string& name)
{
	auto type{ getEmployeeType(certification) };
	// Only add a new menu if the user is a manager
	if (type == Employee::Type::Manager)
	{
		sql = std::string{ "SELECT * FROM menu WHERE parent_id=" + std::to_string(parentId) + " AND name='" + name + "';" };
		std::vector<std::vector<std::string>> results;
		querySql(results);

		if (results.empty())
		{
			sql = std::string{ "INSERT INTO menu(parent_id, name) \
			VALUES(" + std::to_string(parentId) + ",'" + name + "');" };

			insertSql();

			return true;
		}
	}

	return false;
}

bool DatabaseInterface::addItem(const Certification& certification, const int menuId, const std::string& name, const double price)
{
	auto type{ getEmployeeType(certification) };
	// Only add a new employee if the user is a manager
	if (type == Employee::Type::Manager)
	{
		sql = std::string{ "SELECT * FROM item WHERE menu_id=" + std::to_string(menuId) + " AND name='" + name + "';" };
		std::vector<std::vector<std::string>> results;
		querySql(results);

		if (results.empty())
		{
			sql = std::string{ "INSERT INTO item(menu_id, name, price) \
			VALUES(" + std::to_string(menuId) + ",'" + name + "'," + std::to_string(price) + ");" };

			insertSql();

			return true;
		}
	}

	return false;
}

bool DatabaseInterface::addTable(const Certification& certification)
{
	auto type{ getEmployeeType(certification) };
	// Only add a new table if the user is a manager
	if (type == Employee::Type::Manager)
	{
		sql = std::string{ "INSERT INTO table_(status) \
			VALUES(" + std::to_string(static_cast<int>(Table::Status::OutOfUse)) + ");" };
		insertSql();

		return true;
	}

	return false;
}

bool DatabaseInterface::updateTableStatus(const Certification& certification, const int tableId, const Table::Status newStatus)
{
	auto type{ getEmployeeType(certification) };
	
	sql = std::string{ "SELECT * FROM table_ WHERE id=" + std::to_string(tableId) + ";" };
	std::vector<std::vector<std::string>> results{};
	querySql(results);

	// Do nothing if the table does not exist
	if (results.empty())
		return false;

	auto currentStatus{ static_cast<Table::Status>(std::stoi(results[0][INDEX_TABLE_STATUS])) };

	// Only allow the update to occur if the attempt matches with employee type permissions
	if (type == Employee::Type::Manager ||
		type == Employee::Type::Host && currentStatus == Table::Status::Ready && newStatus == Table::Status::Seated ||
		type == Employee::Type::Waiter && currentStatus == Table::Status::Seated && newStatus == Table::Status::Dirty ||
		type == Employee::Type::Busser && currentStatus == Table::Status::Dirty && newStatus == Table::Status::Ready)
	{
		sql = std::string{ "UPDATE table_ SET status=" + std::to_string(static_cast<int>(newStatus)) + " WHERE id=" + std::to_string(tableId) + ";" };
		insertSql();

		return true;
	}

	return false;
}

void DatabaseInterface::getTables(std::vector<Table>& tables)
{
	sql = std::string{ "SELECT * FROM table_;" };
	std::vector<std::vector<std::string>> results{};
	querySql(results);

	for (const auto& table : results)
		tables.push_back(Table{ std::stoi(table[INDEX_TABLE_ID]), static_cast<Table::Status>(std::stoi(table[INDEX_TABLE_STATUS])) });
}

void DatabaseInterface::getEmployees(std::vector<Employee>& employees)
{
	sql = std::string{ "SELECT * FROM employee;" };
	std::vector<std::vector<std::string>> results{};
	querySql(results);

	for (const auto& employee : results)
	{
		employees.push_back(Employee{ Certification{ std::stoi(employee[INDEX_EMPLOYEE_ID]), employee[INDEX_EMPLOYEE_PASSWORD] }, static_cast<Employee::Type>(std::stoi(employee[INDEX_EMPLOYEE_TYPE])), employee[INDEX_EMPLOYEE_FIRST_NAME], employee[INDEX_EMPLOYEE_LAST_NAME] });
	}
}

void DatabaseInterface::getMenu(Menu& menu)
{
	sql = std::string{ "SELECT * FROM menu;" };
	std::vector<std::vector<std::string>> results{};
	querySql(results);

	// Insert the base menu menu and remove it from the results
	menu.id = std::stoi(results[0][INDEX_MENU_ID]);
	menu.name = results[0][INDEX_MENU_NAME];
	results.erase(results.begin());

	// Fill in the menu's child categories
	while (!results.empty())
	{
		for (int i{}; i < results.size(); i++)
		{
			Menu& parent{ menu };

			if (parent.hasMenu(std::stoi(results[i][INDEX_MENU_PARENT_ID])))
			{
				bool found{ false };

				while (!found)
				{
					// Parent ID was matched
					if (std::stoi(results[i][INDEX_MENU_PARENT_ID]) == parent.id)
					{
						found = true;
						parent.submenus.push_back(Menu{ std::stoi(results[i][INDEX_MENU_ID]), results[i][INDEX_MENU_NAME] });
						results.erase(results.begin() + i);
						// The element was just erased, so decrement the iterator to correctly advance to placing the next menu
						--i;
					}
					// Otherwise, find the submenu which provides a path to the parent ID
					else
					{
						for (auto& submenu : parent.submenus)
						{
							if (submenu.hasMenu(std::stoi(results[i][INDEX_MENU_PARENT_ID])))
							{
								parent = submenu;
							}
						}
					}
				}
			}
		}
	}

	sql = std::string{ "SELECT * FROM item;" };
	results.clear();
	querySql(results);
	for (const auto& item : results)
	{
		Menu* search{ &menu };
		bool found{ false };
		while (!found)
		{
			// If the item belongs in the current menu, add it
			if (std::stoi(item[INDEX_ITEM_MENU_ID]) == search->id)
			{
				found = true;
				search->items.push_back(Item{ std::stoi(item[INDEX_ITEM_ID]), item[INDEX_ITEM_NAME], std::stod(item[INDEX_ITEM_PRICE]) });
			}
			// Otherwise, find the submenu which provides a path to the parent ID
			else
			{
				for (auto& submenu : search->submenus)
				{
					if (submenu.hasMenu(std::stoi(item[INDEX_ITEM_MENU_ID])))
					{
						search = &submenu;
						break;
					}
				}
			}
		}
	}
}