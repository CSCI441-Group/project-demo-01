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
    void getInput(T&);
    void printEmployeeList();

    std::string getEmployeeTypeString(Employee::Type type);
    std::string getTableStatusString(Table::Status status);

    void invalidInput();
    void pressToContinue();

    // Base menu functions
    void login();
    void exit();

    // Universal functions
    void logout();

    // Manager menu functions
    void addEmployee();
    void addExampleMenu();
    void addTable();
    void printMenu();
    void printTables();
    void updateTable();

    DatabaseInterface databaseInterface;
    std::stack<std::vector<std::pair<std::string, void(Application::*)()>>> userMenuStack;
    Certification certification;

    std::vector<std::pair<std::string, void(Application::*)()>> loginMenu{
        std::make_pair<std::string, void(Application::*)()>("Login", &Application::login),
        std::make_pair<std::string, void(Application::*)()>("Exit application", &Application::exit)
    };

    std::vector<std::pair<std::string, void(Application::*)()>> managerMenu{
        std::make_pair<std::string, void(Application::*)()>("Add employee", &Application::addEmployee),
        std::make_pair<std::string, void(Application::*)()>("Add example menu", &Application::addExampleMenu),
        std::make_pair<std::string, void(Application::*)()>("Add table", &Application::addTable),
        std::make_pair<std::string, void(Application::*)()>("Print menu", &Application::printMenu),
        std::make_pair<std::string, void(Application::*)()>("Print tables", &Application::printTables),
        std::make_pair<std::string, void(Application::*)()>("Update table", &Application::updateTable),
        std::make_pair<std::string, void(Application::*)()>("Logout", &Application::logout)
    };

    std::vector<std::pair<std::string, void(Application::*)()>> waiterMenu{
        std::make_pair<std::string, void(Application::*)()>("Update table as dirty", &Application::updateTable),
        std::make_pair<std::string, void(Application::*)()>("Logout", &Application::logout)
    };
};