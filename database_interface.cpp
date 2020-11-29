#include "database_interface.h"

using namespace std;

DatabaseInterface::DatabaseInterface()
{
	rc = sqlite3_open("restaurant.db", &database);

	if (rc != SQLITE_OK)
		throw sqlite3_errmsg(database);

	if (!createTables())
		throw exception{};
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
			INSERT OR IGNORE INTO employee(id, first_name, last_name, password, type, pay_rate) \
			VALUES(0, 'Admin', 'User', 'password', 0, 0.00); \
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
	vector<vector<string>> results = *reinterpret_cast<vector<vector<string>>*>(data);
	// Add a new row of values to the vector unless the vector provided has a row ready to be used
	results.push_back(vector<string>{});
	// Fill the row with values
	for (int i{}; i < argc; i++)
	{
		results.back().push_back(argv[i]);
	}

	return 0;
}

bool DatabaseInterface::querySql(const string& sql)
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

bool DatabaseInterface::querySql(const string& sql, vector<vector<string>>& results)
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
	string sql{ "SELECT type FROM employee WHERE id=" + to_string(certification.id) + " AND password='" + certification.password + "';" };
	vector<vector<string>> results{};
	
	if (!querySql(sql, results) || results.empty())
		return false;

	type = static_cast<Employee::Type>(stoi(results[0][0]));
	return true;
}


// Description:             Clocks in for a new shift
// Returns:                 True if the clock in occurred successfully, false otherwise
bool DatabaseInterface::clockIn(const Certification& certification)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	string sql{ "INSERT INTO shift(employee_id,in_time) \
		VALUES(" + to_string(certification.id) + ",datetime('now'));" };
	return querySql(sql);
}

// Description:             Clocks out the current shift
// Returns:                 True if the clock out occurred successfully, false otherwise
bool DatabaseInterface::clockOut(const Certification& certification)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	string sql{ "UPDATE shift \
		SET out_time = DATETIME('now') \
		WHERE employee_id =" + to_string(certification.id) + " AND out_time IS NULL;" };
	return querySql(sql);
}

// Description:             Updates the password of the user that matches the provided certification to newPassword
// Returns:                 True if the update occurred successfully, false otherwise
bool DatabaseInterface::updatePassword(const Certification& certification, const string& newPassword)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	string sql{ "UPDATE employee	\
		SET password='" + newPassword + "' " +
		"WHERE id=" + to_string(certification.id) + ";" };
	return querySql(sql);
}

// Certification required:	Manager, Host
// Description:				Adds a new party to the database with status = InWaitQueue
// Returns:					True if the party was added successfully, false otherwise
bool DatabaseInterface::addParty(const Certification& certification, const int size)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Host || type == Employee::Type::Manager)
	{
		string sql{ "INSERT INTO party(size,status,wait_queue_time) \
			VALUES(" + to_string(size) + "," + to_string(static_cast<int>(Party::Status::InWaitQueue)) + ",DATETIME('now'));" };
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
		string sql{ "SELECT * FROM party WHERE id=" + to_string(partyId) + " AND status=" + to_string(static_cast<int>(Party::Status::InWaitQueue)) + ";" };
		vector<vector<string>> results{};
		if (!querySql(sql, results) || results.empty())
			return false;
	}

	if (type == Employee::Type::Manager || type == Employee::Type::Host)
	{
		string sql{ "UPDATE party \
			SET status=" + to_string(static_cast<int>(Party::Status::Seated)) + ",table_id=" + to_string(tableId) + " " +
			"WHERE id=" + to_string(partyId) + "; \
			UPDATE table_ \
			SET status=" + to_string(static_cast<int>(Table::Status::Seated)) + " " +
			"WHERE id=" + to_string(tableId) + ";" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager, Waiter
// Description:				Adds a new order and associates it with a party
// Returns:					True if the order was added successfully, false otherwise
bool DatabaseInterface::addOrder(const Certification& certification, const int partyId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager || type == Employee::Type::Waiter)
	{
		string sql{ "INSERT INTO order_(party_id,status,total,paid) \
			VALUES(" + to_string(partyId) + "," + to_string(static_cast<int>(Order::Status::Placing)) + ",0.00,0);" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager, Waiter
// Description:				Adds a new item to an order and updates the order's total
// Returns:					True if the order item was added successfully, false otherwise
bool DatabaseInterface::addOrderItem(const Certification& certification, const int orderId, const int itemId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager || type == Employee::Type::Waiter)
	{
		string sql{ "SELECT * FROM item WHERE id=" + to_string(itemId) + ";" };
		vector<vector<string>> results;
		if (!querySql(sql, results) || results.empty())
			return false;

		sql = string{ "INSERT INTO order_item(order_id,item_id) \
			VALUES(" + to_string(orderId) + "," + to_string(itemId) + "); \
			UPDATE order_ \
			SET total=total+" + results[0][INDEX_ITEM_PRICE] + " \
			WHERE id=" + to_string(orderId) + ";" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager, Waiter
// Description:				Adds a new adjustment to an order item to the database with orderItemId as the ID of the order item and adjustmentId as the ID of the adjustment to associate the order adjustment with
// Returns:					True if the order item was added successfully, false otherwise
bool DatabaseInterface::addOrderAdjustment(const Certification& certification, const int orderItemId, const int adjustmentId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager || type == Employee::Type::Waiter)
	{
		string sql{ "SELECT * FROM adjustment WHERE id=" + to_string(adjustmentId) + ";" };
		vector<vector<string>> results;
		if (!querySql(sql, results) || results.empty())
			return false;
		string adjustmentPrice{ results[0][INDEX_ADJUSTMENT_PRICE] };

		sql = string{ "SELECT * FROM order_item WHERE id=" + to_string(orderItemId) + ";" };
		results.clear();
		if (!querySql(sql, results) || results.empty())
			return false;
		string orderId{ results[0][INDEX_ORDER_ITEM_ORDER_ID] };

		sql = string{ "INSERT INTO order_item_adjustment(order_item_id,adjustment_id) \
			VALUES(" + to_string(orderItemId) + "," + to_string(adjustmentId) + "); \
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
// Returns:					True if the payment was added successfully, false otherwise
bool DatabaseInterface::addOrderPayment(const Certification& certification, const int orderId, double amount, const Payment::Type paymentType, const string& cardNumber)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager || type == Employee::Type::Waiter)
	{
		string sql{ "INSERT INTO payment(order_id,amount,type,card_number) \
			VALUES(" + to_string(orderId) + "," + to_string(amount) + "," + to_string(static_cast<int>(paymentType)) + ",'" + cardNumber + "');" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager, Waiter
// Description:				Removes an order and all of its associated items and adjustments
// Returns:					True if the order, items, and adjustments were removed successfully, false otherwise
bool DatabaseInterface::cancelOrder(const Certification& certification, const int orderId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager || type == Employee::Type::Waiter)
	{
		string sql{ "UPDATE order_ SET status=" + to_string(static_cast<int>(Order::Status::Finished)) + " WHERE id=" + to_string(orderId) + ";" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager, Waiter
// Description:				Removes an order item and all of its associated adjustments
// Returns:					True if the item and adjustments were removed successfully, false otherwise
bool DatabaseInterface::removeOrderItem(const Certification& certification, const int orderItemId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		string sql{ "SELECT * FROM order_item_adjustment WHERE order_item_id=" + to_string(orderItemId) + ";" };
		vector<vector<string>> results;
		if (!querySql(sql, results) || results.empty())
			return false;

		double price{};
		for (const auto& orderItemAdjustment : results)
		{
			sql = string{ "SELECT * FROM adjustment WHERE id=" + orderItemAdjustment[INDEX_ORDER_ITEM_ADJUSTMENT_ADJUSTMENT_ID] + ";" };
			results.clear();
			if (!querySql(sql, results) || results.empty())
				return false;
			price += stod(results[0][INDEX_ADJUSTMENT_PRICE]);
		}
		sql = string{ "SELECT * FROM order_item WHERE id=" + to_string(orderItemId) + ";" };
		results.clear();
		if (!querySql(sql, results) || results.empty())
			return false;
		string orderId{ results[0][INDEX_ORDER_ITEM_ORDER_ID] };
		
		sql = string{ "SELECT * FROM item WHERE id=" + results[0][INDEX_ORDER_ITEM_ITEM_ID] + ";" };
		results.clear();
		if (!querySql(sql, results) || results.empty())
			return false;
		price -= stod(results[0][INDEX_ITEM_PRICE]);

		sql = string{ "DELETE FROM order_item_adjustment WHERE order_item_id=" + to_string(orderItemId) + "; \
			DELETE FROM order_item WHERE id=" + to_string(orderItemId) + "; \
			UPDATE order_ \
			SET total=total-" + to_string(price) + " \
			WHERE id=" + orderId + ";" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager, Waiter
// Description:				Removes an order item adjustment
// Returns:					True if the adjustment was removed successfully, false otherwise
bool DatabaseInterface::removeOrderItemAdjustment(const Certification& certification, const int orderItemAdjustmentId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager || type == Employee::Type::Waiter)
	{
		string sql{ "SELECT * FROM order_item_adjustment WHERE id=" + to_string(orderItemAdjustmentId) + ";" };
		vector<vector<string>> results;
		if (!querySql(sql, results) || results.empty())
			return false;
		string adjustmentId{ results[0][INDEX_ORDER_ITEM_ADJUSTMENT_ADJUSTMENT_ID] };
		string orderItemId{ results[0][INDEX_ORDER_ITEM_ADJUSTMENT_ORDER_ITEM_ID] };

		sql = string{ "SELECT * FROM adjustment WHERE id=" + adjustmentId + ";" };
		results.clear();
		if (!querySql(sql, results) || results.empty())
			return false;
		string adjustmentPrice{ results[0][INDEX_ADJUSTMENT_PRICE] };

		sql = string{ "SELECT * FROM order_item WHERE id=" + orderItemId + ";" };
		results.clear();
		if (!querySql(sql, results) || results.empty())
			return false;
		string orderId{ results[0][INDEX_ORDER_ITEM_ITEM_ID] };

		sql = string{ "DELETE FROM order_item_adjustment WHERE id=" + to_string(orderItemAdjustmentId) + "; \
			UPDATE order_ \
			SET total=total-" + adjustmentPrice + " \
			WHERE id=" + orderId + ";" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager
// Description:				Adds an employee to the database
// Returns:					True if the employee was added successfully, false otherwise
bool DatabaseInterface::addEmployee(const Certification& certification, const Employee::Type newType, const string& firstName, const string& lastName, const string& password, const double payRate)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		string sql{ "INSERT INTO employee(first_name,last_name,password,type,pay_rate) \
			VALUES('" + firstName + "','" + lastName + "','" + password + "'," + to_string(static_cast<int>(newType)) + "," + to_string(payRate) + ");" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager
// Description:				Adds a new menu and associates it with a parent menu
//							If no parent is specified, it is automatically created as a child of the base menu
// Returns:					True if the menu was added successfully, false otherwise
bool DatabaseInterface::addMenu(const Certification& certification, const string& name, const int parentId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		string sql{ "INSERT INTO menu(parent_id,name) \
			VALUES(" + to_string(parentId) + ",'" + name + "');" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager
// Description:				Adds a new item to a menu
// Returns:					True if the item was added successfully, false otherwise
bool DatabaseInterface::addItem(const Certification& certification, const int menuId, const string& name, const double price)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		string sql{ "INSERT INTO item(menu_id,name,price) \
			VALUES(" + to_string(menuId) + ",'" + name + "'," + to_string(price) + ");" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager
// Description:				Adds a new adjustment group to an item
// Returns:					True if the adjustment group was added successfully, false otherwise
bool DatabaseInterface::addAdjustmentGroup(const Certification& certification, const int itemId, const string& name)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		string sql{ "INSERT INTO adjustment_group(item_id,name) \
			VALUES(" + to_string(itemId) + ",'" + name + "');" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager
// Description:				Adds a new adjustment to an adjustment group
// Returns:					True if the adjustment was added successfully, false otherwise
bool DatabaseInterface::addAdjustment(const Certification& certification, const int adjustmentGroupId, const string& name, const double price)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		string sql{ "INSERT INTO adjustment(adjustment_group_id,name,price) \
			VALUES(" + to_string(adjustmentGroupId) + ",'" + name + "'," + to_string(price) + ");" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager
// Description:				Removes a menu and all associated items, adjustment groups, and adjustments
//							The base menu cannot be removed
// Returns:					True if the menu was removed successfully, false otherwise
bool DatabaseInterface::removeMenu(const Certification& certification, const int menuId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		string finalSql{};

		string sql{ "SELECT * FROM item WHERE menu_id=" + to_string(menuId) + ";" };
		vector<vector<string>> items{};
		if (!querySql(sql, items))
			return false;

		for (const auto& item : items)
		{
			sql = string{ "SELECT * FROM adjustment_group WHERE item_id=" + item[INDEX_ITEM_ID] + ";" };
			vector<vector<string>> adjustmentGroups{};
			if (!querySql(sql, adjustmentGroups))
				return false;

			for (const auto& adjustmentGroup : adjustmentGroups)
				finalSql.append(string{ "DELETE FROM adjustment WHERE adjustment_group_id=" + adjustmentGroup[INDEX_ADJUSTMENT_GROUP_ID] + "; \
				DELETE FROM adjustment_group WHERE id=" + adjustmentGroup[INDEX_ADJUSTMENT_GROUP_ID] + ";" });
			finalSql.append(string{ "DELETE FROM item WHERE id=" + item[INDEX_ITEM_ID] + ";" });
		}
		finalSql.append(string{ "DELETE FROM menu WHERE id=" + to_string(menuId) + ";" });
		return querySql(finalSql);
	}
	else
		return false;
}

// Certification required:	Manager
// Description:				Removes an item and all associated adjustment groups and adjustments
// Returns:					True if the item was removed successfully, false otherwise
bool DatabaseInterface::removeItem(const Certification& certification, const int itemId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		string sql{ "SELECT * FROM adjustment_group WHERE item_id=" + to_string(itemId) + ";" };
		vector<vector<string>> results;
		if (!querySql(sql, results))
			return false;

		sql.clear();
		for (const auto& adjustmentGroup : results)
			sql.append(string{ "DELETE FROM adjustment WHERE adjustment_group_id=" + adjustmentGroup[INDEX_ADJUSTMENT_GROUP_ID] + "; \
				DELETE FROM adjustment_group WHERE id=" + adjustmentGroup[INDEX_ADJUSTMENT_GROUP_ID] + ";" });
		sql.append(string{ "DELETE FROM item WHERE id=" + to_string(itemId) + ";" });
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager
// Description:				Removes an adjustment group and all associated adjustments
// Returns:					True if the adjustment group was removed successfully, false otherwise
bool DatabaseInterface::removeAdjustmentGroup(const Certification& certification, const int adjustmentGroupId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		string sql{ "DELETE FROM adjustment WHERE adjustment_group_id=" + to_string(adjustmentGroupId) + "; \
			DELETE FROM adjustment_group WHERE id=" + to_string(adjustmentGroupId) + ";" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager
// Description:				Removes an adjustment
// Returns:					True if the adjustment was removed successfully, false otherwise
bool DatabaseInterface::removeAdjustment(const Certification& certification, const int adjustmentId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		string sql{ "DELETE FROM adjustment WHERE id=" + to_string(adjustmentId) + ";" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Any
// Description:				Gets whether an employee is clocked in or not
// Returns:					True if the employee is clocked in, false otherwise
bool DatabaseInterface::isClockedIn(const Certification& certification)
{
	string sql{ "SELECT * FROM shift WHERE employee_id=" + to_string(certification.id) + " AND out_time IS NULL;" };
	vector<vector<string>> shiftResults{};
	if (!querySql(sql, shiftResults) || shiftResults.empty())
		return false;
	else
		return true;
}

// Certification required:	Any
// Description:				Fills menu with the base menu with all submenus, items, adjustment groups, and adjustments filled
// Returns:					True if the menu was filled successfully, false otherwise
bool DatabaseInterface::getMenu(Menu& menu)
{
	string sql{ "SELECT * FROM menu;" };
	vector<vector<string>> menus{};
	if (!querySql(sql, menus))
		return false;

	// Insert the base menu menu and remove it from the results
	menu.id = stoi(menus[0][INDEX_MENU_ID]);
	menu.name = menus[0][INDEX_MENU_NAME];
	menus.erase(menus.begin());

	// Fill in the menu's submenus
	while (!menus.empty())
	{
		for (int i{}; i < menus.size(); i++)
		{
			Menu& parent{ menu };

			if (parent.hasMenu(stoi(menus[i][INDEX_MENU_PARENT_ID])))
			{
				bool found{ false };

				while (!found)
				{
					// Parent ID was matched
					if (stoi(menus[i][INDEX_MENU_PARENT_ID]) == parent.id)
					{
						found = true;
						parent.submenus.push_back(Menu{ stoi(menus[i][INDEX_MENU_ID]), stoi(menus[i][INDEX_MENU_PARENT_ID]), menus[i][INDEX_MENU_NAME] });
						menus.erase(menus.begin() + i);
						// The element was just erased, so decrement the iterator to correctly advance to placing the next menu
						--i;
					}
					// Otherwise, find the submenu which provides a path to the parent ID
					else
					{
						for (auto& submenu : parent.submenus)
						{
							if (submenu.hasMenu(stoi(menus[i][INDEX_MENU_PARENT_ID])))
							{
								parent = submenu;
							}
						}
					}
				}
			}
		}
	}

	sql = string{ "SELECT * FROM item;" };
	vector<vector<string>> items{};
	if (!querySql(sql, items))
		return false;

	// Fill items into their correct menus
	for (const auto& item : items)
	{
		Menu* search{ &menu };
		bool found{ false };
		while (!found)
		{
			// If the item belongs in the current menu, add it
			if (stoi(item[INDEX_ITEM_MENU_ID]) == search->id)
			{
				found = true;
				search->items.push_back(Item{ stoi(item[INDEX_ITEM_ID]), stoi(item[INDEX_ITEM_MENU_ID]), item[INDEX_ITEM_NAME], stod(item[INDEX_ITEM_PRICE]) });

				sql = string{ "SELECT * FROM adjustment_group WHERE item_id=" + to_string(search->items.back().id) + ";" };
				vector<vector<string>> adjustmentGroups{};
				if (!querySql(sql, adjustmentGroups))
					return false;

				// Add the item's associated adjustment groups
				for (const auto& adjustmentGroup : adjustmentGroups)
				{
					search->items.back().adjustmentGroups.push_back(AdjustmentGroup{ stoi(adjustmentGroup[INDEX_ADJUSTMENT_GROUP_ID]), stoi(adjustmentGroup[INDEX_ADJUSTMENT_GROUP_ITEM_ID]), adjustmentGroup[INDEX_ADJUSTMENT_GROUP_NAME] });
					
					sql = string{ "SELECT * FROM adjustment WHERE adjustment_group_id=" + to_string(search->items.back().adjustmentGroups.back().id) + ";" };
					vector<vector<string>> adjustments{};
					if (!querySql(sql, adjustments))
						return false;
					
					// Add the adjustment group's associated adjustments
					for (const auto& adjustment : adjustments)
						search->items.back().adjustmentGroups.back().adjustments.push_back(Adjustment{ stoi(adjustment[INDEX_ADJUSTMENT_ID]), stoi(adjustment[INDEX_ADJUSTMENT_ADJUSTMENT_GROUP_ID]), adjustment[INDEX_ADJUSTMENT_NAME], stod(adjustment[INDEX_ADJUSTMENT_PRICE]) });
				}
			}
			// Otherwise, find the submenu which provides a path to the parent ID
			else
			{
				for (auto& submenu : search->submenus)
				{
					if (submenu.hasMenu(stoi(item[INDEX_ITEM_MENU_ID])))
					{
						search = &submenu;
						break;
					}
				}
			}
		}
	}

	return true;
}

// Certification required:	Manager
// Description:				Fills employees with the details of all employees in the database
// Returns:					True if employees was filled successfully, false otherwise
bool DatabaseInterface::getEmployees(const Certification& certification, vector<Employee>& employees)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		string sql{ "SELECT * FROM employee;" };
		vector<vector<string>> results{};
		if (!querySql(sql, results))
			return false;

		for (const auto& employee : results)
			employees.push_back(Employee{ Certification{ stoi(employee[INDEX_EMPLOYEE_ID]), employee[INDEX_EMPLOYEE_PASSWORD] }, static_cast<Employee::Type>(stoi(employee[INDEX_EMPLOYEE_TYPE])), employee[INDEX_EMPLOYEE_FIRST_NAME], employee[INDEX_EMPLOYEE_LAST_NAME], stod(employee[INDEX_EMPLOYEE_PAY_RATE]) });
		
		return true;
	}
	else
		return false;
}

// Certification required:	Any
// Description:				Fills tables with the details of all tables in the database
// Returns:					True if tables was filled successfully, false otherwise
bool DatabaseInterface::getTables(vector<Table>& tables)
{
	string sql{ "SELECT * FROM tables;" };
	vector<vector<string>> results{};
	if (!querySql(sql, results))
		return false;

	for (const auto& table : results)
		tables.push_back(Table{ stoi(table[INDEX_TABLE_ID]), static_cast<Table::Status>(stoi(table[INDEX_TABLE_STATUS])) });

	return true;
}

// Certification required:	Any
// Description:				Fills parties with the details of all parties seated at a table
// Returns:					True if parties was filled successfully, false otherwise
bool DatabaseInterface::getPartiesAtTable(vector<Party>& parties, const int tableId)
{
	string sql{ "SELECT * FROM party WHERE table_id=" + to_string(tableId) + ";" };
	vector<vector<string>> results{};
	if (!querySql(sql, results))
		return false;

	for (const auto& party : results)
		parties.push_back(Party{ stoi(party[INDEX_PARTY_ID]), stoi(party[INDEX_PARTY_TABLE_ID]), stoi(party[INDEX_PARTY_SIZE]), static_cast<Party::Status>(stoi(party[INDEX_PARTY_STATUS])), DateTime{ party[INDEX_PARTY_WAIT_QUEUE_TIME] }, DateTime{ party[INDEX_PARTY_SEATED_TIME] }, DateTime{ party[INDEX_PARTY_FINISHED_TIME] } });

	return true;
}

// Certification required:	Any
// Description:				Fills parties with the details of all parties in the wait queue
//							The party that has been in the queue the longest will be at the front of the filled vector
// Returns:					True if parties was filled successfully, false otherwise
bool DatabaseInterface::getPartiesInWaitQueue(vector<Party>& parties)
{
	string sql{ "SELECT * FROM party WHERE status=" + to_string(static_cast<int>(Party::Status::InWaitQueue)) + " ORDER BY wait_queue_time ASC;" };
	vector<vector<string>> results{};
	if (!querySql(sql, results))
		return false;

	for (const auto& party : results)
		parties.push_back(Party{ stoi(party[INDEX_PARTY_ID]), stoi(party[INDEX_PARTY_TABLE_ID]), stoi(party[INDEX_PARTY_SIZE]), static_cast<Party::Status>(stoi(party[INDEX_PARTY_STATUS])), DateTime{ party[INDEX_PARTY_WAIT_QUEUE_TIME] }, DateTime{ party[INDEX_PARTY_SEATED_TIME] }, DateTime{ party[INDEX_PARTY_FINISHED_TIME] } });

	return true;
}

// Certification required:	Any
// Description:				Fills orders with the surface details of all associated orders
// Returns:					True if parties was filled successfully, false otherwise
bool DatabaseInterface::getOrdersWithParty(vector<Order>& orders, const int partyId)
{
	string sql{ "SELECT * FROM order_ WHERE party_id=" + to_string(partyId) + ";" };
	vector<vector<string>> results{};
	if (!querySql(sql, results))
		return false;

	for (const auto& order : results)
		orders.push_back(Order{ stoi(order[INDEX_ORDER_ID]), stoi(order[INDEX_ORDER_PARTY_ID]), static_cast<Order::Status>(stoi(order[INDEX_ORDER_STATUS])), stod(order[INDEX_ORDER_TOTAL]), DateTime{ order[INDEX_ORDER_PLACE_TIME] }, DateTime{ order[INDEX_ORDER_DELIVER_TIME] }, stod(order[INDEX_ORDER_TIP]), static_cast<bool>(stoi(order[INDEX_ORDER_PAID])) });

	return true;
}

// Certification required:	Any
// Description:				Fills order fully, including its surface details, order items, and order adjustments
// Returns:					True if parties was filled successfully, false otherwise
bool DatabaseInterface::getOrderWithId(Order& order, const int orderId)
{
	string sql{ "SELECT * FROM order_ WHERE id=" + to_string(orderId) + ";" };
	vector<vector<string>> orderResults{};
	if (!querySql(sql, orderResults) || orderResults.empty())
		return false;

	order = Order{ stoi(orderResults[0][INDEX_ORDER_ID]), stoi(orderResults[0][INDEX_ORDER_PARTY_ID]), static_cast<Order::Status>(stoi(orderResults[0][INDEX_ORDER_STATUS])), stod(orderResults[0][INDEX_ORDER_TOTAL]), DateTime{ orderResults[0][INDEX_ORDER_PLACE_TIME] }, DateTime{ orderResults[0][INDEX_ORDER_DELIVER_TIME] }, stod(orderResults[0][INDEX_ORDER_TIP]), static_cast<bool>(stoi(orderResults[0][INDEX_ORDER_PAID])) };
	
	sql = string{ "SELECT * FROM order_item WHERE order_id=" + to_string(orderId) + ";" };
	vector<vector<string>> orderItemResults{};
	if (!querySql(sql, orderItemResults))
		return false;

	for (const auto& orderItem : orderItemResults)
	{
		sql = string{ "SELECT * FROM item WHERE id=" + orderItem[INDEX_ORDER_ITEM_ITEM_ID] + ";" };
		vector<vector<string>> itemResults{};
		if (!querySql(sql, itemResults) || itemResults.empty())
			return false;

		order.items.push_back(OrderItem{ stoi(orderItem[INDEX_ORDER_ITEM_ID]), stoi(orderItem[INDEX_ORDER_ITEM_ORDER_ID]), Item{ stoi(itemResults[0][INDEX_ITEM_ID]), stoi(itemResults[0][INDEX_ITEM_MENU_ID]), itemResults[0][INDEX_ITEM_NAME], stod(itemResults[0][INDEX_ITEM_PRICE]) } });

		sql = string{ "SELECT * FROM order_item_adjustment WHERE order_item_id=" + to_string(order.items.back().id) + ";" };
		vector<vector<string>> orderAdjustmentResults{};
		if (!querySql(sql, orderAdjustmentResults))
			return false;

		for (const auto& orderAdjustment : orderAdjustmentResults)
		{
			sql = string{ "SELECT * FROM adjustment WHERE id=" + orderAdjustment[INDEX_ORDER_ITEM_ADJUSTMENT_ADJUSTMENT_ID] + ";" };
			vector<vector<string>> adjustmentResults{};
			if (!querySql(sql, adjustmentResults) || adjustmentResults.empty())
				return false;

			order.items.back().adjustments.push_back(OrderItemAdjustment{ order.items.back().id, Adjustment{ stoi(adjustmentResults[0][INDEX_ADJUSTMENT_ID]), stoi(adjustmentResults[0][INDEX_ADJUSTMENT_ADJUSTMENT_GROUP_ID]), adjustmentResults[0][INDEX_ADJUSTMENT_NAME], stod(adjustmentResults[0][INDEX_ADJUSTMENT_PRICE]) } });
		}
	}

	return true;
}

// Certification required:	Any
// Description:				Fully fills all orders being prepared by kitchen staff, including their surface details, order items, order adjustments
// Returns:					True if parties was filled successfully, false otherwise
bool DatabaseInterface::getPlacedOrders(vector<Order>& orders)
{
	string sql{ "SELECT * FROM order_ WHERE status=" + to_string(static_cast<int>(Order::Status::Placed)) + ";" };
	vector<vector<string>> orderResults{};
	if (!querySql(sql, orderResults))
		return false;

	for (const auto& order : orderResults)
	{
		orders.push_back(Order{ stoi(order[INDEX_ORDER_ID]), stoi(order[INDEX_ORDER_PARTY_ID]), static_cast<Order::Status>(stoi(order[INDEX_ORDER_STATUS])), stod(order[INDEX_ORDER_TOTAL]), DateTime{ order[INDEX_ORDER_PLACE_TIME] }, DateTime{ order[INDEX_ORDER_DELIVER_TIME] }, stod(order[INDEX_ORDER_TIP]), static_cast<bool>(stoi(order[INDEX_ORDER_PAID])) });

		sql = string{ "SELECT * FROM order_item WHERE order_id=" + to_string(orders.back().id) + ";" };
		vector<vector<string>> orderItemResults{};
		if (!querySql(sql, orderItemResults))
			return false;

		for (const auto& orderItem : orderItemResults)
		{
			sql = string{ "SELECT * FROM item WHERE id=" + orderItem[INDEX_ORDER_ITEM_ITEM_ID] + ";" };
			vector<vector<string>> itemResults{};
			if (!querySql(sql, itemResults) || itemResults.empty())
				return false;

			orders.back().items.push_back(OrderItem{ stoi(orderItem[INDEX_ORDER_ITEM_ID]), stoi(orderItem[INDEX_ORDER_ITEM_ORDER_ID]), Item{ stoi(itemResults[0][INDEX_ITEM_ID]), stoi(itemResults[0][INDEX_ITEM_MENU_ID]), itemResults[0][INDEX_ITEM_NAME], stod(itemResults[0][INDEX_ITEM_PRICE]) } });

			sql = string{ "SELECT * FROM order_item_adjustment WHERE order_item_id=" + to_string(orders.back().items.back().id) + ";" };
			vector<vector<string>> orderAdjustmentResults{};
			if (!querySql(sql, orderAdjustmentResults))
				return false;

			for (const auto& orderAdjustment : orderAdjustmentResults)
			{
				sql = string{ "SELECT * FROM adjustment WHERE id=" + orderAdjustment[INDEX_ORDER_ITEM_ADJUSTMENT_ADJUSTMENT_ID] + ";" };
				vector<vector<string>> adjustmentResults{};
				if (!querySql(sql, adjustmentResults) || adjustmentResults.empty())
					return false;

				orders.back().items.back().adjustments.push_back(OrderItemAdjustment{ orders.back().items.back().id, Adjustment{ stoi(adjustmentResults[0][INDEX_ADJUSTMENT_ID]), stoi(adjustmentResults[0][INDEX_ADJUSTMENT_ADJUSTMENT_GROUP_ID]), adjustmentResults[0][INDEX_ADJUSTMENT_NAME], stod(adjustmentResults[0][INDEX_ADJUSTMENT_PRICE]) } });
			}
		}
	}

	return true;
}

// Certification required:	Manager
// Description:				Adds a table to the database
// Returns:					True if the table was added successfully, false otherwise
bool DatabaseInterface::addTable(const Certification& certification)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		string sql{ "INSERT INTO table_(status) \
			VALUES(" + to_string(static_cast<int>(Table::Status::OutOfUse)) + ");" };
		return querySql(sql);
	}
	else
		return false;
}

// Certification required:	Manager
// Description:				Removes a table from the database
// Returns:					True if the table was removed successfully, false otherwise
bool DatabaseInterface::removeTable(const Certification& certification, const int tableId)
{
	Employee::Type type;
	if (!getEmployeeType(certification, type))
		return false;

	if (type == Employee::Type::Manager)
	{
		string sql{ "DELETE FROM table_ WHERE id=" + to_string(tableId) + ";" };
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
		string sql{  };
		return querySql(sql);
	}
	else
		return false;
}
*/