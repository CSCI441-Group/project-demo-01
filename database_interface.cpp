#include "database_interface.h"

DatabaseInterface::DatabaseInterface()
{
	rc = sqlite3_open("restaurant.db", &database);

	if (rc != SQLITE_OK)
		throw sqlite3_errmsg(database);

	if (!createTables())
		throw std::exception{};
}

DatabaseInterface::~DatabaseInterface()
{

}

bool DatabaseInterface::createTables()
{
	try
	{
		rc = sqlite3_exec(database, "CREATE TABLE IF NOT EXISTS menu( \
			  id INTEGER PRIMARY KEY, \
			  parent_id INTEGER, \
			  name TEXT, \
			  FOREIGN KEY(parent_id) REFERENCES menu(id) \
			); \
			 \
			CREATE TABLE IF NOT EXISTS item( \
			  id INTEGER PRIMARY KEY, \
			  menu_id INTEGER,  \
			  name TEXT, \
			  price REAL, \
			  FOREIGN KEY(menu_id) REFERENCES menu(id) \
			); \
			 \
			CREATE TABLE IF NOT EXISTS adjustment_group( \
			  id INTEGER PRIMARY KEY, \
			  item_id INTEGER, \
			  name TEXT, \
			  FOREIGN KEY(item_id) REFERENCES item(id) \
			); \
			 \
			CREATE TABLE IF NOT EXISTS adjustment( \
			  id INTEGER PRIMARY KEY, \
			  adjustment_group_id INTEGER, \
			  name TEXT, \
			  price REAL, \
			  FOREIGN KEY(adjustment_group_id) REFERENCES adjustment_group(id) \
			); \
			 \
			CREATE TABLE IF NOT EXISTS employee( \
			  id INTEGER PRIMARY KEY, \
			  first_name TEXT, \
			  last_name TEXT, \
			  password TEXT, \
			  type INTEGER, \
			  pay_rate REAL \
			); \
			 \
			CREATE TABLE IF NOT EXISTS shift( \
			  employee_id INTEGER, \
			  in_time TEXT, \
			  out_time TEXT, \
			  PRIMARY KEY(employee_id,in_time), \
			  FOREIGN KEY(employee_id) REFERENCES employee(id) \
			); \
			 \
			CREATE TABLE IF NOT EXISTS table_( \
			  id INTEGER PRIMARY KEY, \
			  waiter_id INTEGER, \
			  status INTEGER, \
			  FOREIGN KEY(waiter_id) REFERENCES employee(id) \
			); \
			 \
			CREATE TABLE IF NOT EXISTS party( \
			  id INTEGER PRIMARY KEY, \
			  table_id INTEGER, \
			  size INTEGER, \
			  status INTEGER, \
			  wait_queue_time TEXT, \
			  seated_time TEXT, \
			  finished_time TEXT, \
			  FOREIGN KEY(table_id) REFERENCES table_(id) \
			); \
			 \
			CREATE TABLE IF NOT EXISTS order_( \
			  id INTEGER PRIMARY KEY, \
			  party_id INTEGER, \
			  status INTEGER, \
			  total REAL, \
			  place_time TEXT, \
			  deliver_time TEXT, \
			  tip REAL, \
			  FOREIGN KEY(party_id) REFERENCES party(id) \
			); \
			 \
			CREATE TABLE IF NOT EXISTS order_item( \
			  id INTEGER PRIMARY KEY, \
			  order_id INTEGER, \
			  item_id INTEGER, \
			  FOREIGN KEY(order_id) REFERENCES order_(id), \
			  FOREIGN KEY(item_id) REFERENCES item(id) \
			); \
			 \
			CREATE TABLE IF NOT EXISTS order_item_adjustment( \
			  order_item_id INTEGER, \
			  adjustment_id INTEGER, \
			  PRIMARY KEY(order_item_id,adjustment_id), \
			  FOREIGN KEY(order_item_id) REFERENCES order_item(id), \
			  FOREIGN KEY(adjustment_id) REFERENCES adjustment(id) \
			); \
			 \
			CREATE TABLE IF NOT EXISTS payment( \
			  order_id INTEGER, \
			  amount REAL, \
			  type INTEGER, \
			  card_number TEXT \
			); \
			 \
			INSERT OR IGNORE INTO employee(id, first_name, last_name, password, type) \
			VALUES(0, 'Admin', 'User', 'password', 0); \
			 \
			INSERT OR IGNORE INTO menu(id, parent_id, name) \
			VALUES(0, 0, 'Menu');", nullptr, nullptr, &errorMessage);

		if (rc != SQLITE_OK)
			throw errorMessage;

		return true;
	}
	catch (...)
	{
		return false;
	}
}

int DatabaseInterface::callback(void* data, int argc, char** argv, char** azColName)
{
	// Reinterpret the user data as a vector that was passed in
	std::vector<std::vector<std::string>> results = *reinterpret_cast<std::vector<std::vector<std::string>>*>(data);
	// Add a new row of values to the vector unless the vector provided has a row ready to be used
	results.push_back(std::vector<std::string>{});
	// Fill the row with values
	for (int i{}; i < argc; i++)
	{
		results.back().push_back(argv[i]);
	}

	return 0;
}

bool DatabaseInterface::querySql(const std::string& sql)
{
	try
	{
		rc = sqlite3_exec(database, sql.c_str(), nullptr, nullptr, &errorMessage);

		if (rc != SQLITE_OK)
			throw errorMessage;

		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool DatabaseInterface::querySql(const std::string& sql, std::vector<std::vector<std::string>>& results)
{
	try
	{
		rc = sqlite3_exec(database, sql.c_str(), callback, &results, &errorMessage);

		if (rc != SQLITE_OK)
			throw errorMessage;

		return true;
	}
	catch (...)
	{
		return false;
	}
}

// Description:				Assigns type with the type of the employee whose certifications were passed in
// Returns:					True if the certification exists in the database, false otherwise
bool DatabaseInterface::getEmployeeType(const Certification& certification, Employee::Type& type)
{
	std::string sql{ "SELECT type FROM employee WHERE id=" + std::to_string(certification.id) + " AND password='" + certification.password + "';" };
	std::vector<std::vector<std::string>> results{};
	
	if (!querySql(sql, results) || results.empty())
		return false;

	type = static_cast<Employee::Type>(std::stoi(results[0][0]));
	return true;
}


// Description:             Clocks in for a new shift
// Returns:                 True if the clock in occurred successfully, false otherwise
bool DatabaseInterface::clockIn(const Certification& certification)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	std::string sql{ "INSERT INTO shift(employee_id,in_time) \
		VALUES(" + std::to_string(certification.id) + ",datetime('now'));" };
	return querySql(sql);
}

// Description:             Clocks out the current shift
// Returns:                 True if the clock out occurred successfully, false otherwise
bool DatabaseInterface::clockOut(const Certification& certification)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	std::string sql{ "UPDATE shift \
		SET out_time = DATETIME('now') \
		WHERE employee_id =" + std::to_string(certification.id) + " AND out_time IS NULL;" };
	return querySql(sql);
}

// Description:             Updates the password of the user that matches the provided certification to newPassword
// Returns:                 True if the update occurred successfully, false otherwise
bool DatabaseInterface::updatePassword(const Certification& certification, const std::string& newPassword)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	std::string sql{ "UPDATE employee	\
		SET password=" + newPassword + " " +
		"WHERE id=" + std::to_string(certification.id) + ";" };
	return querySql(sql);
}

// Certification required:	Manager, Host
// Description:				Adds a new party to the database with status = InWaitQueue
// Returns:					True if the party was added succefully, false otherwise
bool DatabaseInterface::addParty(const Certification& certification, const int size)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Host || type == Employee::Type::Manager)
	{
		std::string sql{ "INSERT INTO party(size,status,wait_queue_time) \
			VALUES(" + std::to_string(size) + "," + std::to_string(static_cast<int>(Party::Status::InWaitQueue)) + ",DATETIME('now'));" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager, Host (If party is in wait queue)
// Description:				Updates the party and the table as seated
// Returns:					True if the party and table were updated successfully, false otherwise
bool DatabaseInterface::updatePartyAsSeated(const Certification& certification, const int partyId, const int tableId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Host)
	{
		std::string sql{ "SELECT * FROM party WHERE id=" + std::to_string(partyId) + " AND status=" + std::to_string(static_cast<int>(Party::Status::InWaitQueue)) + ";" };
		std::vector<std::vector<std::string>> results{};
		if (!querySql(sql, results) || results.empty())
			return false;
	}

	if (type == Employee::Type::Manager || type == Employee::Type::Host)
	{
		std::string sql{ "UPDATE party \
			SET status=" + std::to_string(static_cast<int>(Party::Status::Seated)) + ",table_id=" + std::to_string(tableId) + " " +
			"WHERE id=" + std::to_string(partyId) + "; \
			UPDATE table_ \
			SET status=" + std::to_string(static_cast<int>(Table::Status::Seated)) + " " +
			"WHERE id=" + std::to_string(tableId) + ";" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager, Waiter
// Description:				Adds a new order and associates it with a party
// Returns:					True if the order was added succefully, false otherwise
bool DatabaseInterface::addOrder(const Certification& certification, const int partyId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager || type == Employee::Type::Waiter)
	{
		std::string sql{ "INSERT INTO order_(party_id,status,total) \
			VALUES(" + std::to_string(partyId) + "," + std::to_string(static_cast<int>(Order::Status::Placing)) + ",0.00);" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager, Waiter
// Description:				Adds a new item to an order and updates the order's total
// Returns:					True if the order item was added succefully, false otherwise
bool DatabaseInterface::addOrderItem(const Certification& certification, const int orderId, const int itemId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager || type == Employee::Type::Waiter)
	{
		std::string sql{ "SELECT * FROM item WHERE id=" + std::to_string(itemId) + ";" };
		std::vector<std::vector<std::string>> results;
		if (!querySql(sql, results) || results.empty())
			return false;

		sql = std::string{ "INSERT INTO order_item(order_id,item_id) \
			VALUES(" + std::to_string(orderId) + "," + std::to_string(itemId) + "); \
			UPDATE order_ \
			SET total=total+" + results[0][INDEX_ITEM_PRICE] + " \
			WHERE id=" + std::to_string(orderId) + ";" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager, Waiter
// Description:				Adds a new adjustment to an order item to the database with orderItemId as the ID of the order item and adjustmentId as the ID of the adjustment to associate the order adjustment with
// Returns:					True if the order item was added succefully, false otherwise
bool DatabaseInterface::addOrderAdjustment(const Certification& certification, const int orderItemId, const int adjustmentId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager || type == Employee::Type::Waiter)
	{
		std::string sql{ "SELECT * FROM adjustment WHERE id=" + std::to_string(adjustmentId) + ";" };
		std::vector<std::vector<std::string>> results;
		if (!querySql(sql, results) || results.empty())
			return false;
		std::string adjustmentPrice{ results[0][INDEX_ADJUSTMENT_PRICE] };

		sql = std::string{ "SELECT * FROM order_item WHERE id=" + std::to_string(orderItemId) + ";" };
		results.clear();
		if (!querySql(sql, results) || results.empty())
			return false;
		std::string orderId{ results[0][INDEX_ORDER_ITEM_ORDER_ID] };

		sql = std::string{ "INSERT INTO order_item_adjustment(order_item_id,adjustment_id) \
			VALUES(" + std::to_string(orderItemId) + "," + std::to_string(adjustmentId) + "); \
			UPDATE order_ \
			SET total=total+" + adjustmentPrice + " \
			WHERE id=" + orderId + ";" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager, Waiter
// Description:				Adds a new payment and associates it with an order
// Returns:					True if the payment was added succefully, false otherwise
bool DatabaseInterface::addOrderPayment(const Certification& certification, const int orderId, double amount, const Payment::Type paymentType, const std::string& cardNumber)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager || type == Employee::Type::Waiter)
	{
		std::string sql{ "INSERT INTO payment(order_id,amount,type,card_number) \
			VALUES(" + std::to_string(orderId) + "," + std::to_string(amount) + "," + std::to_string(static_cast<int>(paymentType)) + cardNumber + ");" };
		return querySql(sql);
	}
	else
		return false;
}

bool DatabaseInterface::removeOrder(const Certification& certification, const int orderId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		std::string sql{  };
		return querySql(sql);
	}
	else
		return false;
}

bool DatabaseInterface::removeOrderItem(const Certification& certification, const int orderItemId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		std::string sql{  };
		return querySql(sql);
	}
	else
		return false;
}

bool DatabaseInterface::removeOrderItemAdjustment(const Certification& certification, const int orderItemAdjustmentId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager || type == Employee::Type::Waiter)
	{
		std::string sql{ "SELECT * FROM order_item_adjustment WHERE id=" + std::to_string(orderItemAdjustmentId) + ";" };
		std::vector<std::vector<std::string>> results;
		if (!querySql(sql, results) || results.empty())
			return false;
		std::string adjustmentId{ results[0][INDEX_ORDER_ITEM_ADJUSTMENT_ADJUSTMENT_ID] };
		std::string orderItemId{ results[0][INDEX_ORDER_ITEM_ADJUSTMENT_ORDER_ITEM_ID] };

		sql = std::string{ "SELECT * FROM order_item WHERE id=" + orderItemId + ";" };
		results.clear();
		if (!querySql(sql, results) || results.empty())
			return false;
		std::string orderId{ results[0][INDEX_ORDER_ITEM_ITEM_ID] };

		//sql{  };
		return querySql(sql);
	}
	else
		return false;
}





// IGNORE PAST THIS LINE // INCOMPLETE //
/*
bool DatabaseInterface::example(const Certification& certification)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		std::string sql{  };
		return querySql(sql);
	}
	else
		return false;
}







// Certification required:	Manager
// Description:				Updates the table as seated
// Returns:					True if the table was updated succefully, false otherwise
bool DatabaseInterface::updateTableAsSeated(const Certification& certification, const int tableId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		std::string sql{ "UPDATE table_ \
			SET status=" + std::to_string(static_cast<int>(Table::Status::Seated)) + " " +
			"WHERE id=" + std::to_string(tableId) + ";" };
		return querySql(sql);
	}
	else
		return false;
}




























// Certification required:	Manager
// Description:				Adds a new employee to the database with information provided
// Returns:					True if the employee was added succefully, false otherwise
bool DatabaseInterface::addEmployee(const Certification& certification, const Employee::Type type, const std::string& firstName, const std::string& lastName, const std::string& password)
{

}

// Certification required:	Manager
// Description:				Adds a new menu to the database with parentId as the ID of the parent menu
// Returns:					True if the menu was added succefully, false otherwise
bool DatabaseInterface::addMenu(const Certification& certification, const std::string& name, const int parentId = BASE_MENU_ID)
{

}

// Certification required:	Manager
// Description:				Adds a new item to the database with menuId as the ID of the menu to associate the item with
// Returns:					True if the item was added succefully, false otherwise
bool DatabaseInterface::addItem(const Certification& certification, const int menuId, const std::string& name, const double price)
{

}

// Certification required:	Manager
// Description:				Adds a new adjustment group to the database with itemId as the ID of the item to associate the adjustment group with
// Returns:					True if the adjustment group was added succefully, false otherwise
bool DatabaseInterface::addAdjustmentGroup(const Certification& certification, const int itemId, const std::string& name)
{

}

// Certification required:	Manager
// Description:				Adds a new adjustment to the database with adjustmentGroupId as the ID of the adjustmentGroup to associate the adjustment with
// Returns:					True if the adjustment was added succefully, false otherwise
bool DatabaseInterface::addAdjustment(const Certification& certification, const int adjustmentGroupId, const std::string& name, const double price)
{

}

// Certification required:	Manager
// Description:				Adds a new table to the database
// Returns:					True if the table was added succefully, false otherwise
bool DatabaseInterface::addTable(const Certification& certification)
{

}

// TO-DO: AUTOCALL THIS FUNCTION WHEN ALL PARTIES AT A TABLE ARE FINISHED
// Certification required:	Manager, Waiter (If table is seated)
// Description:				Updates the the table with tableId as the ID of the table as dirty
// Returns:					True if the table was updated succefully, false otherwise
bool DatabaseInterface::updateTableAsDirty(const Certification& certification, const int tableId)
{

}

// Certification required:	Manager, Busser (If table is dirty)
// Description:				Updates the the table with tableId as the ID of the table as ready
// Returns:					True if the table was updated succefully, false otherwise
bool DatabaseInterface::updateTableAsReady(const Certification& certification, const int tableId)
{

}

// Certification required:	Manager
// Description:				Updates the the table with tableId as the ID of the table as out of use
// Returns:					True if the table was updated succefully, false otherwise
bool DatabaseInterface::updateTableAsOutOfUse(const Certification& certification, const int tableId)
{

}

// Certification required:	Manager, Host (If party is in wait queue), Waiter (If party is seated)
// Description:				Updates the party with partyId as the ID of the party to finished, and cancels all outstanding orders associated with the party
//							If the table the party was seated at is now empty, the table is updated as dirty
// Returns:					True if the party and table were updated successfully, false otherwise
bool DatabaseInterface::updatePartyAsFinished(const Certification& certification, const int partyId)
{

}

// Certification required:	Manager, Waiter
// Description:				Updates the the order with orderId as the ID of the order as placed
// Returns:					True if the order was updated succefully, false otherwise
bool DatabaseInterface::updateOrderAsPlaced(const Certification& certification, const int orderId)
{

}

// Certification required:	Manager, Cook
// Description:				Updates the the order with orderId as the ID of the order as made
// Returns:					True if the order was updated succefully, false otherwise
bool DatabaseInterface::updateOrderAsMade(const Certification& certification, const int orderId)
{

}

// Certification required:	Manager, Waiter
// Description:				Updates the the order with orderId as the ID of the order as delivered
// Returns:					True if the order was updated succefully, false otherwise
bool DatabaseInterface::updateOrderAsDelivered(const Certification& certification, const int orderId)
{

}

// Certification required:	Manager, Waiter
// Description:				Updates the the order with orderId as the ID of the order as finished and finalizes the amount received for it
// Returns:					True if the order was updated succefully, false otherwise
bool DatabaseInterface::updateOrderAsFinished(const Certification& certification, const int orderId)
{

}*/