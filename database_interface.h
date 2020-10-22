#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "sqlite3.h"
#include "employee.h"

class DatabaseInterface
{
public:
	DatabaseInterface();
	~DatabaseInterface();
	
	void addCategory(Certification& certification, std::string& description);
	void addEmployee(Certification& certification, Employee& employee);

	void listEmployees();

private:
	void createTables();
	Employee::Type getEmployeeType(Certification& certification);
	
	static int callback(void* data, int argc, char** argv, char** azColName);
	
	sqlite3* database{};
	int rc{};
	char* errorMessage{};
	std::string sql{};
};

namespace sql
{
	const std::string CreateTables = "CREATE TABLE IF NOT EXISTS category(		\
		id INTEGER PRIMARY KEY,													\
		description TEXT														\
		);																		\
																				\
		CREATE TABLE IF NOT EXISTS item(										\
			id INTEGER PRIMARY KEY,												\
			category_id INTEGER,												\
			name TEXT,															\
			price REAL,															\
			FOREIGN KEY(category_id) REFERENCES category(id)					\
		);																		\
																				\
		CREATE TABLE IF NOT EXISTS adjustment_group(							\
			id INTEGER PRIMARY KEY,												\
			item_id INTEGER,													\
			description TEXT,													\
			FOREIGN KEY(item_id) REFERENCES item(id)							\
		);																		\
																				\
		CREATE TABLE IF NOT EXISTS adjustment(									\
			id INTEGER PRIMARY KEY,												\
			adjustment_group_id INTEGER,										\
			description TEXT,													\
			price REAL,															\
			FOREIGN KEY(adjustment_group_id) REFERENCES adjustment_group(id)	\
		);																		\
																				\
		CREATE TABLE IF NOT EXISTS table_(										\
			id INTEGER PRIMARY KEY,												\
			status INTEGER														\
		);																		\
																				\
		CREATE TABLE IF NOT EXISTS party(										\
			id INTEGER PRIMARY KEY,												\
			table_id INTEGER,													\
			size INTEGER,														\
			status INTEGER,														\
			FOREIGN KEY(table_id) REFERENCES table_(id)							\
		);																		\
																				\
		CREATE TABLE IF NOT EXISTS employee(									\
			id INTEGER PRIMARY KEY,												\
			first_name TEXT,													\
			last_name TEXT,														\
			password TEXT,														\
			type INTEGER														\
		);																		\
																				\
		CREATE TABLE IF NOT EXISTS order_(										\
			id INTEGER PRIMARY KEY,												\
			party_id INTEGER,													\
			waiter_id INTEGER,													\
			size INTEGER,														\
			status INTEGER,														\
			total REAL,															\
			FOREIGN KEY(party_id) REFERENCES party(id),							\
			FOREIGN KEY(waiter_id) REFERENCES employee(id)						\
		);																		\
																				\
		CREATE TABLE IF NOT EXISTS order_item(									\
			id INTEGER PRIMARY KEY,												\
			order_id INTEGER,													\
			item_id INTEGER,													\
			FOREIGN KEY(order_id) REFERENCES order_(id),						\
			FOREIGN KEY(item_id) REFERENCES item(id)							\
		);																		\
																				\
		CREATE TABLE IF NOT EXISTS order_item_adjustment(						\
			order_item_id INTEGER,												\
			adjustment_id INTEGER,												\
			PRIMARY KEY(order_item_id, adjustment_id),							\
			FOREIGN KEY(order_item_id) REFERENCES order_item(id),				\
			FOREIGN KEY(adjustment_id) REFERENCES adjustment(id)				\
		);																		\
																				\
		INSERT OR IGNORE INTO employee(id, first_name, last_name, password, type)			\
		VALUES(0, 'Admin', 'User', 'password', 0);";			
}