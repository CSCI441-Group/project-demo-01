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
#define INDEX_TABLE_WAITER_ID 1
#define INDEX_TABLE_STATUS 2

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
#define INDEX_ORDER_STATUS 2
#define INDEX_ORDER_TOTAL 3

#define INDEX_ORDER_ITEM_ID 0
#define INDEX_ORDER_ITEM_ORDER_ID 1
#define INDEX_ORDER_ITEM_ITEM_ID 2

#define INDEX_ORDER_ITEM_ADJUSTMENT_ORDER_ITEM_ID 0
#define INDEX_ORDER_ITEM_ADJUSTMENT_ADJUSTMENT_ID 1

#define BASE_MENU_ID 0

enum class ReturnCode
{
	Success,
	Duplicate,
	Uncertified,
	NonexistentId,
	InvalidInput,
	TableInUse,
	Error
};

class DatabaseInterface
{
public:

	DatabaseInterface();
	~DatabaseInterface();

	ReturnCode getEmployeeType(const Certification& certification, Employee::Type& type);

	ReturnCode addEmployee(const Certification& certification, const Employee& employee);
	ReturnCode addMenu(const Certification& certification, const int parentId, const std::string& name);
	ReturnCode addItem(const Certification& certification, const int menuId, const std::string& name, const double price);
	ReturnCode addAdjustmentGroup(const Certification& certification, const int itemId, const std::string& name);
	ReturnCode addAdjustment(const Certification& certification, const int adjustmentGroupId, const std::string& name, const double price);
	ReturnCode addTable(const Certification& certification);
	ReturnCode addParty(const Certification& certification, const int size);
	ReturnCode addOrder(const Certification& certification, const int partyId);
	ReturnCode addOrderItem(const Certification& certification, const int orderId, const int itemId);

	ReturnCode updateTableStatus(const Certification& certification, const int tableId, const Table::Status newStatus);
	ReturnCode updatePartyStatus(const Certification& certification, const int partyId, const Party::Status newStatus);
	ReturnCode updateOrderStatus(const Certification& certification, const int orderId, const Order::Status newStatus);

	ReturnCode seatParty(const Certification& certification, const int partyId, const int tableId);
	ReturnCode finishParty(const Certification& certification, const int partyId);
	

	// Fills employees with all employees in the database
	ReturnCode getEmployees(std::vector<Employee>& employees);
	ReturnCode getMenu(Menu& menu);
	ReturnCode getTables(std::vector<Table>& tables);
	ReturnCode getParties(std::vector<Party>& parties);
	ReturnCode getOrders(std::vector<Order>& orders);
	ReturnCode getOrderItems(const int orderId, std::vector<OrderItem>& orderItems);

private:
	ReturnCode createTables();
	ReturnCode insertSql();
	ReturnCode querySql(std::vector<std::vector<std::string>>& results);
	
	static int callback(void* data, int argc, char** argv, char** azColName);
	
	sqlite3* database{};
	int rc{};
	char* errorMessage{};
	std::string sql{};
};