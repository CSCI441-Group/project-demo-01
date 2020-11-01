#include "database_interface.h"

DatabaseInterface::DatabaseInterface()
{
	rc = sqlite3_open("restaurant.db", &database);

	if (rc != SQLITE_OK)
		throw sqlite3_errmsg(database);

	if (createTables() != ReturnCode::Success)
		throw std::exception{};
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

ReturnCode DatabaseInterface::createTables()
{
	try
	{
		rc = sqlite3_exec(database, "CREATE TABLE IF NOT EXISTS menu(  id INTEGER PRIMARY KEY,  parent_id INTEGER,  name TEXT,  FOREIGN KEY(parent_id) REFERENCES menu(id));CREATE TABLE IF NOT EXISTS item(  id INTEGER PRIMARY KEY,  menu_id INTEGER,   name TEXT,  price REAL,  FOREIGN KEY(menu_id) REFERENCES menu(id));CREATE TABLE IF NOT EXISTS adjustment_group(  id INTEGER PRIMARY KEY,  item_id INTEGER,  name TEXT,  FOREIGN KEY(item_id) REFERENCES item(id));CREATE TABLE IF NOT EXISTS adjustment(  id INTEGER PRIMARY KEY,  adjustment_group_id INTEGER,  name TEXT,  price REAL,  FOREIGN KEY(adjustment_group_id) REFERENCES adjustment_group(id));CREATE TABLE IF NOT EXISTS employee(  id INTEGER PRIMARY KEY,  first_name TEXT,  last_name TEXT,  password TEXT,  type INTEGER);CREATE TABLE IF NOT EXISTS table_(  id INTEGER PRIMARY KEY,  waiter_id INTEGER,  status INTEGER,  FOREIGN KEY(waiter_id) REFERENCES employee(id));CREATE TABLE IF NOT EXISTS party(  id INTEGER PRIMARY KEY,  table_id INTEGER,  size INTEGER,  status INTEGER,  FOREIGN KEY(table_id) REFERENCES table_(id));CREATE TABLE IF NOT EXISTS order_(  id INTEGER PRIMARY KEY,  party_id INTEGER,  status INTEGER,  total REAL,  FOREIGN KEY(party_id) REFERENCES party(id));CREATE TABLE IF NOT EXISTS order_item(  id INTEGER PRIMARY KEY,  order_id INTEGER,  item_id INTEGER,  FOREIGN KEY(order_id) REFERENCES order_(id),  FOREIGN KEY(item_id) REFERENCES item(id));CREATE TABLE IF NOT EXISTS order_item_adjustment(  order_item_id INTEGER,  adjustment_id INTEGER,  PRIMARY KEY(order_item_id,adjustment_id),  FOREIGN KEY(order_item_id) REFERENCES order_item(id),  FOREIGN KEY(adjustment_id) REFERENCES adjustment(id));INSERT OR IGNORE INTO employee(id, first_name, last_name, password, type)VALUES(0, 'Admin', 'User', 'password', 0);INSERT OR IGNORE INTO menu(id, parent_id, name)VALUES(0, 0, 'Menu');", nullptr, nullptr, &errorMessage);

		if (rc != SQLITE_OK)
			throw errorMessage;

		return ReturnCode::Success;
	}
	catch (...)
	{
		return ReturnCode::Error;
	}
}

ReturnCode DatabaseInterface::insertSql()
{
	try
	{
		rc = sqlite3_exec(database, sql.c_str(), nullptr, nullptr, &errorMessage);

		if (rc != SQLITE_OK)
			throw errorMessage;

		return ReturnCode::Success;
	}
	catch (...)
	{
		return ReturnCode::Error;
	}
}

ReturnCode DatabaseInterface::querySql(std::vector<std::vector<std::string>>& results)
{
	try
	{
		rc = sqlite3_exec(database, sql.c_str(), callback, &results, &errorMessage);

		if (rc != SQLITE_OK)
			throw errorMessage;

		return ReturnCode::Success;
	}
	catch (...)
	{
		return ReturnCode::Error;
	}
}

ReturnCode DatabaseInterface::getEmployeeType(const Certification& certification, Employee::Type& type)
{
	sql = std::string{ "SELECT type FROM employee WHERE id=" + std::to_string(certification.id) + " AND password='" + certification.password + "';" };
	std::vector<std::vector<std::string>> results{};
	querySql(results);

	if (!results.empty())
	{
		type = static_cast<Employee::Type>(std::stoi(results[0][0]));
		return ReturnCode::Success;
	}
	else
		return ReturnCode::Error;
}

ReturnCode DatabaseInterface::addEmployee(const Certification& certification, const Employee& employee)
{
	Employee::Type type;
	if (getEmployeeType(certification, type) != ReturnCode::Success)
		return ReturnCode::Uncertified;

	// Only add a new employee if the user attempting to add a new employee is a manager
	if (type == Employee::Type::Manager)
	{
		sql = std::string{ "INSERT INTO employee(first_name, last_name, password, type) \
			VALUES('" + employee.firstName + "','" + employee.lastName + "','" + employee.certification.password + "'," + std::to_string(static_cast<int>(employee.type)) + ");" };
		
		return insertSql();
	}
	else
		return ReturnCode::Uncertified;
}

ReturnCode DatabaseInterface::addOrder(const Certification& certification, const int partyId)
{
	Employee::Type type;
	if (getEmployeeType(certification, type) != ReturnCode::Success)
		return ReturnCode::Uncertified;

	if (type == Employee::Type::Waiter || type == Employee::Type::Manager)
	{
		sql = std::string{ "INSERT INTO order_(party_id,status,total) \
			VALUES(" + std::to_string(partyId) + "," + std::to_string(static_cast<int>(Order::Status::Placing)) + "," + std::to_string(0) + ");" };
		
		return insertSql();
	}
	else
		return ReturnCode::Uncertified;
}

ReturnCode DatabaseInterface::addParty(const Certification& certification, const int size)
{
	Employee::Type type;
	if (getEmployeeType(certification, type) != ReturnCode::Success)
		return ReturnCode::Uncertified;

	if (type == Employee::Type::Host || type == Employee::Type::Manager)
	{
		sql = std::string{ "INSERT INTO party(table_id, size, status) \
			VALUES(" } + std::to_string(0) + "," + std::to_string(size) +"," + std::to_string(static_cast<int>(Party::Status::InWaitQueue)) + ");";
		
		return insertSql();
	}
	else
		return ReturnCode::Uncertified;
}

ReturnCode DatabaseInterface::addMenu(const Certification& certification, const int parentId, const std::string& name)
{
	Employee::Type type;
	if(getEmployeeType(certification, type) != ReturnCode::Success)
		return ReturnCode::Uncertified;

	// Only add a new menu if the user is a manager
	if (type == Employee::Type::Manager)
	{
		sql = std::string{ "SELECT * FROM menu WHERE parent_id=" + std::to_string(parentId) + " AND name='" + name + "';" };
		std::vector<std::vector<std::string>> results;
		if (querySql(results) != ReturnCode::Success)
			return ReturnCode::Error;

		if (results.empty())
		{
			sql = std::string{ "INSERT INTO menu(parent_id, name) \
			VALUES(" + std::to_string(parentId) + ",'" + name + "');" };

			return insertSql();
		}
		else
			return ReturnCode::Duplicate;
	}
	else
		return ReturnCode::Uncertified;
}

ReturnCode DatabaseInterface::addOrderItem(const Certification& certification, const int orderId, const int itemId)
{
	Employee::Type type;
	if (getEmployeeType(certification, type) != ReturnCode::Success)
		return ReturnCode::Uncertified;

	// Only add a new employee if the user is a waiter or manager
	if (type == Employee::Type::Waiter || type == Employee::Type::Manager)
	{
		sql = std::string{ "SELECT * FROM order_ WHERE id=" + std::to_string(orderId) + ";" };
		std::vector<std::vector<std::string>> results;
		if (querySql(results) != ReturnCode::Success)
			return ReturnCode::Error;
		if (results.empty() || static_cast<Order::Status>(std::stoi(results[0][INDEX_ORDER_STATUS])) != Order::Status::Placing)
			return ReturnCode::NonexistentId;
		
		sql = std::string{ "SELECT * FROM item WHERE id=" + std::to_string(itemId) + ";" };
		results.clear();
		if (querySql(results) != ReturnCode::Success)
			return ReturnCode::Error;
		if (results.empty())
			return ReturnCode::NonexistentId;

		sql = std::string{ "INSERT INTO order_item(order_id,item_id) \
			VALUES(" + std::to_string(orderId) + "," + std::to_string(itemId) + ");" };
		return insertSql();
	}
	else
		return ReturnCode::Uncertified;
}

ReturnCode DatabaseInterface::addItem(const Certification& certification, const int menuId, const std::string& name, const double price)
{
	Employee::Type type;
	if (getEmployeeType(certification, type) != ReturnCode::Success)
		return ReturnCode::Uncertified;

	// Only add a new employee if the user is a manager
	if (type == Employee::Type::Manager)
	{
		sql = std::string{ "SELECT * FROM item WHERE menu_id=" + std::to_string(menuId) + " AND name='" + name + "';" };
		std::vector<std::vector<std::string>> results;
		if (querySql(results) != ReturnCode::Success)
			return ReturnCode::Error;

		if (results.empty())
		{
			sql = std::string{ "INSERT INTO item(menu_id, name, price) \
			VALUES(" + std::to_string(menuId) + ",'" + name + "'," + std::to_string(price) + ");" };

			return insertSql();
		}
		else
			return ReturnCode::Duplicate;
	}
	else
		return ReturnCode::Uncertified;
}

ReturnCode DatabaseInterface::addTable(const Certification& certification)
{
	Employee::Type type;
	if (getEmployeeType(certification, type) != ReturnCode::Success)
		return ReturnCode::Uncertified;

	// Only add a new table if the user is a manager
	if (type == Employee::Type::Manager)
	{
		sql = std::string{ "INSERT INTO table_(status,waiter_id) \
			VALUES(" + std::to_string(static_cast<int>(Table::Status::OutOfUse)) + "," + std::to_string(0) + ");" };
		return insertSql();
	}
	return
		ReturnCode::Uncertified;
}

ReturnCode DatabaseInterface::finishParty(const Certification& certification, const int partyId)
{
	Employee::Type type;
	if (getEmployeeType(certification, type) != ReturnCode::Success)
		return ReturnCode::Uncertified;

	sql = std::string{ "SELECT * FROM party WHERE id=" + std::to_string(partyId) + ";" };
	std::vector<std::vector<std::string>> results{};
	if (querySql(results) != ReturnCode::Success)
		return ReturnCode::Error;
	else if (results.empty())
		return ReturnCode::NonexistentId;

	if (type == Employee::Type::Manager ||
		type == Employee::Type::Host ||
		type == Employee::Type::Waiter)
	{
		sql = std::string{ "UPDATE party SET status=" + std::to_string(static_cast<int>(Party::Status::Finished)) + " WHERE id=" + std::to_string(partyId) + ";" +
			"UPDATE table_ SET status=" + std::to_string(static_cast<int>(Table::Status::Dirty)) + " WHERE id=(SELECT table_id FROM party WHERE party.id=" + std::to_string(partyId) + ");"
			"UPDATE order_ SET status=" + std::to_string(static_cast<int>(Order::Status::Cancelled)) + ",total=" + std::to_string(0.0) + " WHERE party_id=" + std::to_string(partyId) + " AND status!=" + std::to_string(static_cast<int>(Order::Status::Paid)) + ";" };
		
		return insertSql();
	}
	else
		return ReturnCode::Uncertified;
}

ReturnCode DatabaseInterface::seatParty(const Certification& certification, const int partyId, const int tableId)
{
	Employee::Type type;
	if (getEmployeeType(certification, type) != ReturnCode::Success)
		return ReturnCode::Uncertified;

	sql = std::string{ "SELECT * FROM party WHERE id=" + std::to_string(partyId) + ";" };
	std::vector<std::vector<std::string>> results{};
	if (querySql(results) != ReturnCode::Success)
		return ReturnCode::Error;
	if (results.empty())
		return ReturnCode::NonexistentId;
	auto currentTable{ std::stoi(results[0][INDEX_PARTY_TABLE_ID]) };
	auto currentStatus{ static_cast<Party::Status>(std::stoi(results[0][INDEX_PARTY_STATUS])) };

	sql = std::string{ "SELECT * FROM table_ WHERE id=" + std::to_string(tableId) + ";" };
	results.clear();
	if (querySql(results) != ReturnCode::Success)
		return ReturnCode::Error;
	if (results.empty())
		return ReturnCode::NonexistentId;
	else if (static_cast<Table::Status>(std::stoi(results[0][INDEX_TABLE_STATUS])) != Table::Status::Ready)
		return ReturnCode::TableInUse;
	
	if (type == Employee::Type::Manager ||
		type == Employee::Type::Host && currentStatus == Party::Status::InWaitQueue)
	{
		sql = "UPDATE party SET status=" + std::to_string(static_cast<int>(Party::Status::Seated)) + " WHERE id=" + std::to_string(partyId) + ";" +
			"UPDATE table_ SET status=" + std::to_string(static_cast<int>(Table::Status::Seated)) + " WHERE id=" + std::to_string(tableId) + ";";
		if (currentStatus != Party::Status::InWaitQueue)
			sql.append(" UPDATE table_ SET status=" + std::to_string(static_cast<int>(Table::Status::Dirty)) + " WHERE id=" + std::to_string(currentTable));

		return insertSql();
	}
	else
		return ReturnCode::Uncertified;
}

ReturnCode DatabaseInterface::updateOrderStatus(const Certification& certification, const int orderId, const Order::Status newStatus)
{
	Employee::Type type;
	if (getEmployeeType(certification, type) != ReturnCode::Success)
		return ReturnCode::Uncertified;

	sql = std::string{ "SELECT * FROM order_ WHERE id=" + std::to_string(orderId) + ";" };
	std::vector<std::vector<std::string>> results{};
	if (querySql(results) != ReturnCode::Success)
		return ReturnCode::Error;

	// Do nothing if the table does not exist
	if (results.empty())
		return ReturnCode::NonexistentId;

	auto currentStatus{ static_cast<Order::Status>(std::stoi(results[0][INDEX_ORDER_STATUS])) };

	// Only allow the update to occur if the attempt matches with employee type permissions
	if (type == Employee::Type::Manager ||
		type == Employee::Type::Waiter && currentStatus == Order::Status::Placing && newStatus == Order::Status::Placed ||
		type == Employee::Type::Waiter && newStatus == Order::Status::Cancelled ||
		type == Employee::Type::Cook && currentStatus == Order::Status::Placed && newStatus == Order::Status::Made ||
		type == Employee::Type::Waiter && currentStatus == Order::Status::Made && newStatus == Order::Status::Delivered ||
		type == Employee::Type::Waiter && currentStatus == Order::Status::Delivered && newStatus == Order::Status::Paid)
	{
		sql = std::string{ "UPDATE order_ SET status=" + std::to_string(static_cast<int>(newStatus)) + " WHERE id=" + std::to_string(orderId) + ";" };
		return insertSql();
	}
	else
		return ReturnCode::Uncertified;
}

ReturnCode DatabaseInterface::updatePartyStatus(const Certification& certification, const int partyId, const Party::Status newStatus)
{
	Employee::Type type;
	if (getEmployeeType(certification, type) != ReturnCode::Success)
		return ReturnCode::Uncertified;

	sql = std::string{ "SELECT * FROM party WHERE id=" + std::to_string(partyId) + ";" };
	std::vector<std::vector<std::string>> results{};
	if (querySql(results) != ReturnCode::Success)
		return ReturnCode::Error;

	// Do nothing if the table does not exist
	if (results.empty())
		return ReturnCode::NonexistentId;

	auto currentStatus{ static_cast<Party::Status>(std::stoi(results[0][INDEX_PARTY_STATUS])) };

	// Only allow the update to occur if the attempt matches with employee type permissions
	if (type == Employee::Type::Manager ||
		type == Employee::Type::Host && currentStatus == Party::Status::InWaitQueue && newStatus == Party::Status::Seated)
	{
		if (newStatus == Party::Status::Finished)
			return finishParty(certification, partyId);

		sql = std::string{ "UPDATE party SET status=" + std::to_string(static_cast<int>(newStatus)) + " WHERE id=" + std::to_string(partyId) + ";" };
		return insertSql();
	}
	else if (type == Employee::Type::Waiter && currentStatus == Party::Status::Seated && newStatus == Party::Status::Finished)
		return finishParty(certification, partyId);
	else
		return ReturnCode::Uncertified;
}

ReturnCode DatabaseInterface::updateTableStatus(const Certification& certification, const int tableId, const Table::Status newStatus)
{
	Employee::Type type;
	if (getEmployeeType(certification, type) != ReturnCode::Success)
		return ReturnCode::Uncertified;
	
	sql = std::string{ "SELECT * FROM table_ WHERE id=" + std::to_string(tableId) + ";" };
	std::vector<std::vector<std::string>> results{};
	if (querySql(results) != ReturnCode::Success)
		return ReturnCode::Error;

	// Do nothing if the table does not exist
	if (results.empty())
		return ReturnCode::NonexistentId;

	auto currentStatus{ static_cast<Table::Status>(std::stoi(results[0][INDEX_TABLE_STATUS])) };

	// Only allow the update to occur if the attempt matches with employee type permissions
	if (type == Employee::Type::Manager ||
		type == Employee::Type::Host && currentStatus == Table::Status::Ready && newStatus == Table::Status::Seated ||
		type == Employee::Type::Waiter && currentStatus == Table::Status::Seated && newStatus == Table::Status::Dirty ||
		type == Employee::Type::Busser && currentStatus == Table::Status::Dirty && newStatus == Table::Status::Ready)
	{
		sql = std::string{ "UPDATE table_ SET status=" + std::to_string(static_cast<int>(newStatus)) + " WHERE id=" + std::to_string(tableId) + ";" };
		return insertSql();
	}
	else
		return ReturnCode::Uncertified;
}

ReturnCode DatabaseInterface::getTables(std::vector<Table>& tables)
{
	sql = std::string{ "SELECT * FROM table_;" };
	std::vector<std::vector<std::string>> results{};
	if (querySql(results) != ReturnCode::Success)
		return ReturnCode::Error;

	for (const auto& table : results)
		tables.push_back(Table{ std::stoi(table[INDEX_TABLE_ID]), static_cast<Table::Status>(std::stoi(table[INDEX_TABLE_STATUS])) });

	return ReturnCode::Success;
}

ReturnCode DatabaseInterface::getEmployees(std::vector<Employee>& employees)
{
	sql = std::string{ "SELECT * FROM employee;" };
	std::vector<std::vector<std::string>> results{};
	if (querySql(results) != ReturnCode::Success)
		return ReturnCode::Error;

	for (const auto& employee : results)
		employees.push_back(Employee{ Certification{ std::stoi(employee[INDEX_EMPLOYEE_ID]), employee[INDEX_EMPLOYEE_PASSWORD] }, static_cast<Employee::Type>(std::stoi(employee[INDEX_EMPLOYEE_TYPE])), employee[INDEX_EMPLOYEE_FIRST_NAME], employee[INDEX_EMPLOYEE_LAST_NAME] });
	
	return ReturnCode::Success;
}

ReturnCode DatabaseInterface::getParties(std::vector<Party>& parties)
{
	sql = std::string{ "SELECT * FROM party;" };
	std::vector<std::vector<std::string>> results{};
	if (querySql(results) != ReturnCode::Success)
		return ReturnCode::Error;

	for (const auto& party : results)
		parties.push_back(Party{ std::stoi(party[INDEX_PARTY_ID]), std::stoi(party[INDEX_PARTY_TABLE_ID]), std::stoi(party[INDEX_PARTY_SIZE]), static_cast<Party::Status>(std::stoi(party[INDEX_PARTY_STATUS])) });
	
	return ReturnCode::Success;
}

ReturnCode DatabaseInterface::getOrders(std::vector<Order>& orders)
{
	sql = std::string{ "SELECT * FROM order_;" };
	std::vector<std::vector<std::string>> results{};
	if (querySql(results) != ReturnCode::Success)
		return ReturnCode::Error;

	for (const auto& order : results)
	{
		orders.push_back(Order{ std::stoi(order[INDEX_ORDER_ID]), std::stoi(order[INDEX_ORDER_PARTY_ID]), static_cast<Order::Status>(std::stoi(order[INDEX_ORDER_STATUS])), std::stod(order[INDEX_ORDER_TOTAL]) });

		getOrderItems(orders[orders.size() - 1].id, orders[orders.size() - 1].items);
	}

	return ReturnCode::Success;
}

ReturnCode DatabaseInterface::getOrderItems(const int orderId, std::vector<OrderItem>& orderItems)
{
	sql = std::string{ "SELECT * FROM order_item WHERE order_id=" + std::to_string(orderId) + ";" };
	std::vector<std::vector<std::string>> orderItemResults{};
	if (querySql(orderItemResults) != ReturnCode::Success)
		return ReturnCode::Error;

	for (const auto& orderItem : orderItemResults)
	{
		sql = std::string{ "SELECT * FROM item WHERE id=" + orderItem[INDEX_ORDER_ITEM_ITEM_ID] + ";" };
		std::vector<std::vector<std::string>> itemResults{};
		if (querySql(itemResults) != ReturnCode::Success || itemResults.empty())
			return ReturnCode::Error;

		orderItems.push_back(OrderItem{ std::stoi(orderItem[INDEX_ORDER_ITEM_ID]), std::stoi(orderItem[INDEX_ORDER_ITEM_ITEM_ID]), itemResults[0][INDEX_ITEM_NAME], std::stod(itemResults[0][INDEX_ITEM_PRICE]) });
	}

	return ReturnCode::Success;
}

ReturnCode DatabaseInterface::getMenu(Menu& menu)
{
	sql = std::string{ "SELECT * FROM menu;" };
	std::vector<std::vector<std::string>> results{};
	if (querySql(results) != ReturnCode::Success)
		return ReturnCode::Error;

	// Insert the base menu menu and remove it from the results
	menu.id = std::stoi(results[0][INDEX_MENU_ID]);
	menu.name = results[0][INDEX_MENU_NAME];
	results.erase(results.begin());

	// Fill in the menu's submenus
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
	if (querySql(results) != ReturnCode::Success)
		return ReturnCode::Error;
	
	// Fill items into their correct menus
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

	return ReturnCode::Success;
}