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

    // Universal functions
    void logout();

    // Manager menu functions
    void addEmployee();
    void addTable();
    void printMenu();
    void printTables();
    void updateTable();

    void addExampleMenu();
    void addExampleEmployees();

    // Host menu functions
    void addParty();

    ///////////////////////////////////// TO-DO /////////////////////////////////////
    // PRINT PARTY IDS NOT YET SEATED, GET INPUT PARTY ID, USE DATABASE FUNCTION seatParty
    void seatParty();
    // PRINT PARTY IDS, GET INPUT PARTY ID, USE DATABASE FUNCTION addOrder
    void addOrder();
    // PRINT ORDER IDS, GET INPUT ORDER ID, PRINT ITEM IDS NAMES AND PRICES, GET INPUT ITEM ID, USE DATABASEINTERFACE FUNCTION addOrderItem
    void addOrderItem();
    // DEPENDING ON CERTIFICATIONS PRINT POSSIBLE PARTY IDS AND STATUSES TO UPDATE, GET INPUT PARTY ID, USE DATABASE FUNCTION updatePartyStatus
    void updatePartyStatus();
    // DEPENDING ON CERTIFICATIONS PRINT POSSIBLE ORDER IDS AND STATUSES TO UPDATE, GET INPUT ORDER ID, USE DATABASE FUNCTION updateOrderStatus
    void updateOrderStatus();
    // PRINT PARTY IDS, GET INPUT PARTY ID, USE DATABASE FUNCTION finishParty
    // This database function cancels all active orders for parties, accounting for the possibility of unpaid orders and additionally allows parties to leave before being seated, etc
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
        std::make_pair<std::string, void(Application::*)()>("Logout", &Application::logout)
    };

    std::vector<std::pair<std::string, void(Application::*)()>> hostMenu{
        std::make_pair<std::string, void(Application::*)()>("Update table as seated", &Application::updateTable),
        std::make_pair<std::string, void(Application::*)()>("Logout", &Application::logout)
    };

    std::vector<std::pair<std::string, void(Application::*)()>> waiterMenu{
        std::make_pair<std::string, void(Application::*)()>("Update table as dirty", &Application::updateTable),
        std::make_pair<std::string, void(Application::*)()>("Logout", &Application::logout)
    };

    std::vector<std::pair<std::string, void(Application::*)()>> cookMenu{
        std::make_pair<std::string, void(Application::*)()>("Logout", &Application::logout)
    };

    std::vector<std::pair<std::string, void(Application::*)()>> busserMenu{
        std::make_pair<std::string, void(Application::*)()>("Update table as ready", &Application::updateTable),
        std::make_pair<std::string, void(Application::*)()>("Logout", &Application::logout)
    };
};