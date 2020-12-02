#pragma once

#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>
#include <limits>
#include <stack>
#include <string>
#include <vector>

#include "database_interface.h"

class Application
{
public:
    Application();

private:
    void mainLoop();

    void displayMenu();
    void getChoice();
    template <typename T>
    ReturnCode getInput(T&);
    void printEmployeeList();

    std::string getEmployeeTypeString(Employee::Type type);
    std::string getTableStatusString(Table::Status status);

    void invalidInput();
    void error();
    void uncertified();
    void pressToContinue();

    // Base menu functions
    void login();
    void exit();
    void logout();
    void addEmployee();
    void addTable();
    void printMenu();
    void printTables();
    void updateTable();
    void addExampleMenu();
    void addExampleEmployees();
    void addParty();
    void seatParty();
    void addOrder();
    void addOrderItems();
    // DEPENDING ON CERTIFICATIONS PRINT POSSIBLE ORDER IDS AND STATUSES TO UPDATE, GET INPUT ORDER ID, USE DATABASE FUNCTION updateOrderStatus
    void updateOrderAsPlaced();
    void updateOrderAsMade();
    void updateOrderAsDelivered();
    void updateOrderAsPaid();
    void updateOrderAsCanceled();
    void updateOrderStatus();
    void finishParty();
    

    DatabaseInterface databaseInterface;
    std::stack<std::vector<std::pair<std::string, void(Application::*)()>>> userMenuStack;
    Certification certification;

    std::vector<std::pair<std::string, void(Application::*)()>> loginMenu{
        std::make_pair<std::string, void(Application::*)()>("Login", &Application::login),
        std::make_pair<std::string, void(Application::*)()>("Exit application", &Application::exit)
    };

    std::vector<std::pair<std::string, void(Application::*)()>> managerMenu{
        std::make_pair<std::string, void(Application::*)()>("Add employee", &Application::addEmployee),
        std::make_pair<std::string, void(Application::*)()>("Add table", &Application::addTable),
        std::make_pair<std::string, void(Application::*)()>("Add example employees", &Application::addExampleEmployees),
        std::make_pair<std::string, void(Application::*)()>("Add example menu", &Application::addExampleMenu),
        std::make_pair<std::string, void(Application::*)()>("Print menu", &Application::printMenu),
        std::make_pair<std::string, void(Application::*)()>("Print tables", &Application::printTables),
        std::make_pair<std::string, void(Application::*)()>("Update table", &Application::updateTable),
        std::make_pair<std::string, void(Application::*)()>("Add party", &Application::addParty),
        std::make_pair<std::string, void(Application::*)()>("Finish party", &Application::finishParty),
        std::make_pair<std::string, void(Application::*)()>("Seat party", &Application::seatParty),
        std::make_pair<std::string, void(Application::*)()>("Add order", &Application::addOrder),
        std::make_pair<std::string, void(Application::*)()>("Add items to order", &Application::addOrderItems),
        std::make_pair<std::string, void(Application::*)()>("Place order", &Application::updateOrderAsPlaced),
        std::make_pair<std::string, void(Application::*)()>("Update order as made", &Application::updateOrderAsMade),
        std::make_pair<std::string, void(Application::*)()>("Update order as delivered", &Application::updateOrderAsDelivered),
        std::make_pair<std::string, void(Application::*)()>("Update order as paid", &Application::updateOrderAsPaid),
        std::make_pair<std::string, void(Application::*)()>("Logout", &Application::logout)
    };

    std::vector<std::pair<std::string, void(Application::*)()>> hostMenu{
        std::make_pair<std::string, void(Application::*)()>("Add party", &Application::addParty),
        std::make_pair<std::string, void(Application::*)()>("Seat party", &Application::seatParty),
        std::make_pair<std::string, void(Application::*)()>("Finish party", &Application::finishParty),
        std::make_pair<std::string, void(Application::*)()>("Logout", &Application::logout)
    };

    std::vector<std::pair<std::string, void(Application::*)()>> waiterMenu{
        std::make_pair<std::string, void(Application::*)()>("Update table as dirty", &Application::updateTable),
        std::make_pair<std::string, void(Application::*)()>("Add order", &Application::addOrder),
        std::make_pair<std::string, void(Application::*)()>("Add items to order", &Application::addOrderItems),
        std::make_pair<std::string, void(Application::*)()>("Place order", &Application::updateOrderAsPlaced),
        std::make_pair<std::string, void(Application::*)()>("Update order as delivered", &Application::updateOrderAsDelivered),
        std::make_pair<std::string, void(Application::*)()>("Update order as paid", &Application::updateOrderAsPaid),
        std::make_pair<std::string, void(Application::*)()>("Finish party", &Application::finishParty),
        std::make_pair<std::string, void(Application::*)()>("Logout", &Application::logout)
    };

    std::vector<std::pair<std::string, void(Application::*)()>> cookMenu{
        std::make_pair<std::string, void(Application::*)()>("Update order as made", &Application::updateOrderAsMade),
        std::make_pair<std::string, void(Application::*)()>("Logout", &Application::logout)
    };

    std::vector<std::pair<std::string, void(Application::*)()>> busserMenu{
        std::make_pair<std::string, void(Application::*)()>("Update table as ready", &Application::updateTable),
        std::make_pair<std::string, void(Application::*)()>("Logout", &Application::logout)
    };
};