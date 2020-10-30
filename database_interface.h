#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "sqlite3.h"
#include "employee.h"
#include "menu.h"

#define INDEX_MENU_ID 0
#define INDEX_MENU_PARENT_ID 1
#define INDEX_MENU_NAME 2

#define INDEX_ITEM_ID 0
#define INDEX_ITEM_MENU_ID 1
#define INDEX_ITEM_NAME 2
#define INDEX_ITEM_PRICE 3

#define INDEX_ADJUSTMENT_GROUP_ID 0
#define INDEX_ADJUSTMENT_GROUP_ITEM_ID 1
#define INDEX_ADJUSTMENT_GROUP_NAME 2

#define INDEX_ADJUSTMENT_ID 0
#define INDEX_ADJUSTMENT_ADJUSTMENT_GROUP_ID 1
#define INDEX_ADJUSTMENT_NAME 2
#define INDEX_ADJUSTMENT_PRICE 3

#define INDEX_TABLE_ID 0
#define INDEX_TABLE_STATUS 1

#define INDEX_PARTY_ID 0
#define INDEX_PARTY_TABLE_ID 1
#define INDEX_PARTY_SIZE 2
#define INDEX_PARTY_STATUS 3

#define INDEX_EMPLOYEE_ID 0
#define INDEX_EMPLOYEE_FIRST_NAME 1
#define INDEX_EMPLOYEE_LAST_NAME 2
#define INDEX_EMPLOYEE_PASSWORD 3
#define INDEX_EMPLOYEE_TYPE 4

#define INDEX_ORDER_ID 0
#define INDEX_ORDER_PARTY_ID 1
#define INDEX_ORDER_WAITER_ID 2
#define INDEX_ORDER_SIZE 3
#define INDEX_ORDER_STATUS 4
#define INDEX_ORDER_TOTAL 5

#define INDEX_ORDER_ITEM_ID 0
#define INDEX_ORDER_ITEM_ORDER_ID 1
#define INDEX_ORDER_ITEM_ITEM_ID 2

#define INDEX_ORDER_ITEM_ADJUSTMENT_ORDER_ITEM_ID 0
#define INDEX_ORDER_ITEM_ADJUSTMENT_ADJUSTMENT_ID 1

#define BASE_MENU_ID 0

class DatabaseInterface
{
public:
	DatabaseInterface();
	~DatabaseInterface();

	const Employee::Type getEmployeeType(const Certification& certification);

	bool addEmployee(const Certification& certification, const Employee& employee);
	bool addMenu(const Certification& certification, const int parentId, const std::string& name);
	bool addItem(const Certification& certification, const int menuId, const std::string& name, const double price);
	bool addAdjustmentGroup(const Certification& certification, const int itemId, const std::string& name);
	bool addAdjustment(const Certification& certification, const int adjustmentGroupId, const std::string& name, const double price);
	bool addTable(const Certification& certification);

	bool updateTableStatus(const Certification& certification, const int tableId, const Table::Status newStatus);

	// Fills employees with all employees in the database
	void getEmployees(std::vector<Employee>& employees);
	void getMenu(Menu& menu);
	void getTables(std::vector<Table>& tables);

private:
	void createTables();
	void insertSql();
	void querySql(std::vector<std::vector<std::string>>& results);
	
	static int callback(void* data, int argc, char** argv, char** azColName);
	
	sqlite3* database{};
	int rc{};
	char* errorMessage{};
	std::string sql{};
};