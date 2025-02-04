#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "sqlite3.h"
#include "local_database.h"

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
#define INDEX_PARTY_WAIT_QUEUE_TIME 4
#define INDEX_PARTY_SEATED_TIME 5
#define INDEX_PARTY_FINISHED_TIME 6

#define INDEX_EMPLOYEE_ID 0
#define INDEX_EMPLOYEE_FIRST_NAME 1
#define INDEX_EMPLOYEE_LAST_NAME 2
#define INDEX_EMPLOYEE_PASSWORD 3
#define INDEX_EMPLOYEE_TYPE 4
#define INDEX_EMPLOYEE_PAY_RATE 5

#define INDEX_SHIFT_EMPLOYEE_ID 0
#define INDEX_SHIFT_IN_TIME 1
#define INDEX_SHIFT_OUT_TIME 2

#define INDEX_ORDER_ID 0
#define INDEX_ORDER_PARTY_ID 1
#define INDEX_ORDER_STATUS 2
#define INDEX_ORDER_TOTAL 3
#define INDEX_ORDER_PLACE_TIME 4
#define INDEX_ORDER_DELIVER_TIME 5
#define INDEX_ORDER_TIP 6
#define INDEX_ORDER_PAID 7

#define INDEX_ORDER_ITEM_ID 0
#define INDEX_ORDER_ITEM_ORDER_ID 1
#define INDEX_ORDER_ITEM_ITEM_ID 2

#define INDEX_ORDER_ITEM_ADJUSTMENT_ORDER_ITEM_ID 0
#define INDEX_ORDER_ITEM_ADJUSTMENT_ADJUSTMENT_ID 1

#define INDEX_PAYMENT_ORDER_ID 0
#define INDEX_PAYMENT_AMOUNT 1
#define INDEX_PAYMENT_TYPE 2
#define INDEX_PAYMENT_CARD_NUMBER 3

#define BASE_MENU_ID 0

using namespace std;

class DatabaseInterface
{
public:

	DatabaseInterface();
	~DatabaseInterface();


	bool getEmployeeType(const Certification& certification, Employee::Type& type);

	// Universal functionalities
	bool clockIn(const Certification& certification);
	bool clockOut(const Certification& certification);
	bool updatePassword(const Certification& certification, const string& newPassword);
	
	// Host functionalities
	bool addParty(const Certification& certification, const int size);
	bool updatePartyAsSeated(const Certification& certification, const int partyId, const int tableId);

	// Waiter functionalities
	bool addOrder(const Certification& certification, const int partyId);
	bool addOrderItem(const Certification& certification, const int orderId, const int itemId);
	bool addOrderAdjustment(const Certification& certification, const int orderItemId, const int adjustmentId);
	bool addOrderPayment(const Certification& certification, const int orderId, double amount, const Payment::Type type, const string& cardNumber);
	bool cancelOrder(const Certification& certification, const int orderId);
	bool removeOrderItem(const Certification& certification, const int orderItemId);
	bool removeOrderItemAdjustment(const Certification& certification, const int orderAdjustmentId);
	bool updateOrderAsPlaced(const Certification& certification, const int orderId);
	bool updateOrderAsDelivered(const Certification& certification, const int orderId);
	bool updateOrderAsFinished(const Certification& certification, const int orderId);
	bool updatePartyAsFinished(const Certification& certification, const int partyId);
	bool updateTableAsDirty(const Certification& certification, const int tableId);

	// Cook functionalities
	bool updateOrderAsMade(const Certification& certification, const int orderId);

	// Busser functionalities
	bool updateTableAsReady(const Certification& certification, const int tableId);

	// Managerial employee functionalities
	bool addEmployee(const Certification& certification, const Employee::Type newType, const string& firstName, const string& lastName, const string& password, const double payRate);
	bool updateEmployeeFirstName(const Certification& certification, const string& newFirstName);
	bool updateEmployeeLastName(const Certification& certification, const string& newLastName);
	bool updateEmployeeType(const Certification& certification, const Employee::Type newType);
	bool updateEmployeePayRate(const Certification& certification, const double newPayRate);

	// Managerial menu functionalities
	bool addMenu(const Certification& certification, const string& name, const int parentId = BASE_MENU_ID);
	bool addItem(const Certification& certification, const int menuId, const string& name, const double price);
	bool addAdjustmentGroup(const Certification& certification, const int itemId, const string& name);
	bool addAdjustment(const Certification& certification, const int adjustmentGroupId, const string& name, const double price);
	bool updateMenuParent(const Certification& certification, const int menuId, const int newParentId);
	bool updateMenuName(const Certification& certification, const int menuId, const string& newName);
	bool updateItemMenu(const Certification& certification, const int itemId, const int newMenuId);
	bool updateItemName(const Certification& certification, const int itemId, const string& newName);
	bool updateItemPrice(const Certification& certification, const int itemId, double newPrice);
	bool updateAdjustmentGroupName(const Certification& certification, const int adjustmentGroupId, const string& newName);
	bool updateAdjustmentName(const Certification& certification, const int adjustmentId, const string& newName);
	bool updateAdjustmentPrice(const Certification& certification, const int adjustmentId, const double newPrice);
	bool removeMenu(const Certification& certification, const int menuId);
	bool removeItem(const Certification& certification, const int itemId);
	bool removeAdjustmentGroup(const Certification& certification, const int adjustmentGroupId);
	bool removeAdjustment(const Certification& certification, const int adjustmentId);

	// Managerial table functionalities
	bool addTable(const Certification& certification);
	bool removeTable(const Certification& certification, const int tableId);
	bool updateTableAsOutOfUse(const Certification& certification, const int tableId);

	


	bool isClockedIn(const Certification& certification);

	bool getEmployees(const Certification& certification, vector<Employee>& employees);
	bool getMenu(Menu& menu);
	bool getTables(vector<Table>& tables);
	bool getPartiesAtTable(vector<Party>& parties, const int tableId);
	bool getPartiesInWaitQueue(vector<Party>& parties);
	bool getOrdersWithParty(vector<Order>& orders, const int partyId);
	bool getOrderWithId(Order& order, const int orderId);
	bool getPlacedOrders(vector<Order>& orders);

private:
	bool createTables();
	bool querySql(const string& sql);
	bool querySql(const string& sql, vector<vector<string>>& results);

	vector<DatabaseSubscriber*> subscribers{};

	// SQLite-specific members
	static int callback(void* data, int argc, char** argv, char** azColName);
	sqlite3* database{};
	int rc{};
	char* errorMessage{};
};