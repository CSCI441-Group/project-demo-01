#include "application.h"

Application::Application()
{
    userMenuStack.push(loginMenu);
    mainLoop();
}

void Application::displayMenu()
{
    system("CLS");

    for (size_t i{}; i < userMenuStack.top().size(); i++)
        std::cout << i + 1 << ") " << userMenuStack.top()[i].first << '\n';
}

void Application::getChoice()
{
    std::cout << "\nEnter your selection: ";

    // Get user's input
    int response;
    getInput(response);

    --response;

    // Call associated function
    if (response >= 0 && response < userMenuStack.top().size())
        (*this.*userMenuStack.top()[response].second)();
    else
        throw std::exception{};
}

template<typename T>
void Application::getInput(T& value)
{
    std::cin >> value;

    // Clears the buffer in case an invalid string was input
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Application::mainLoop()
{
    while (!userMenuStack.empty())
    {
        displayMenu();
        try
        {
            getChoice();
        }
        catch (const std::exception&)
        {
        }
    }
}

void Application::invalidInput()
{
    std::cout << "Invalid input!\n";

    pressToContinue();
}

void Application::pressToContinue()
{
    std::cout << "Press any button to continue... ";
    std::cin.get();
}

std::string Application::getEmployeeTypeString(Employee::Type type)
{
    switch (type)
    {
    case Employee::Type::Manager:
        return "Manager";
    case Employee::Type::Waiter:
        return "Waiter";
    case Employee::Type::Cook:
        return "Cook";
    case Employee::Type::Busser:
        return "Busser";
    default:
        return "Error";
    }
}

std::string Application::getTableStatusString(Table::Status status)
{
    switch (status)
    {
    case Table::Status::OutOfUse:
        return "Out of use";
    case Table::Status::Ready:
        return "Ready";
    case Table::Status::Seated:
        return "Seated";
    case Table::Status::Dirty:
        return "Dirty";
    default:
        return "Error";
    }
}

void Application::printEmployeeList()
{
    system("CLS");

    std::vector<Employee> employees;
    databaseInterface.getEmployees(employees);

    std::cout << "Employees:\n";
    for (const auto& employee : employees)
    {
        std::cout << "\nType:\t\t" << getEmployeeTypeString(employee.type) << "\nID:\t\t" << employee.certification.id << "\nPassword:\t" << employee.certification.password << "\nFirst name:\t" << employee.firstName << "\nLast name:\t" << employee.lastName << '\n';
    }
}

void Application::printTables()
{
    system("CLS");

    std::vector<Table> tables{};
    databaseInterface.getTables(tables);

    if (tables.empty())
        std::cout << "There are no tables." << std::endl;
    else
    {
        std::cout << "Tables:\n" << std::endl;
        for (const auto& table : tables)
            std::cout << std::string{ "ID: " + std::to_string(table.id) + "\tStatus: " + getTableStatusString(table.status) + '\n' };
    }

    std::cout << '\n';
    pressToContinue();
}

void Application::updateTable()
{
    try
    {
        system("CLS");

        auto type{ databaseInterface.getEmployeeType(certification) };
        int id;
        std::vector<Table> tables{};
        databaseInterface.getTables(tables);
        Table::Status status;
        if (tables.empty())
        {
            std::cout << "There are no tables." << std::endl;
        }
        else
        {
            std::cout << "Tables:\n\n";

            switch (type)
            {
            case Employee::Type::Manager:
                for (const auto& table : tables)
                    std::cout << std::string{ "ID: " + std::to_string(table.id) + "\tStatus: " + getTableStatusString(table.status) + '\n' };

                std::cout << "\nEnter the table's data:\nID: ";
                getInput(id);
                std::cout << "New status (Out of use: 0 / Ready: 1 / Seated: 2 / Dirty: 3): ";
                int statusInt;
                getInput(statusInt);
                status = static_cast<Table::Status>(statusInt);

                if (databaseInterface.updateTableStatus(certification, id, status))
                {
                    std::cout << "Table updated successfully.\n";
                    break;
                }
                else
                {
                    invalidInput();
                    break;
                }

            case Employee::Type::Host:
                for (int i{}; i < tables.size(); i++)
                {
                    if (tables[i].status != Table::Status::Ready)
                    {
                        tables.erase(tables.begin() + i);
                        --i;
                    }
                }

                if (tables.empty())
                {
                    std::cout << "There are no tables you can update." << std::endl;
                    break;
                }

                for (const auto& table : tables)
                    std::cout << std::string{ "ID: " + std::to_string(table.id) + "\tStatus: " + getTableStatusString(table.status) + '\n' };

                std::cout << "\nEnter the ID of the table to mark as seated: ";
                getInput(id);

                if (databaseInterface.updateTableStatus(certification, id, Table::Status::Seated))
                {
                    std::cout << "\nTable updated successfully.\n";
                    break;
                }
                else
                {
                    invalidInput();
                    break;
                }

            case Employee::Type::Waiter:
                for (int i{}; i < tables.size(); i++)
                {
                    if (tables[i].status != Table::Status::Seated)
                    {
                        tables.erase(tables.begin() + i);
                        --i;
                    }
                }

                if (tables.empty())
                {
                    std::cout << "There are no tables you can update." << std::endl;
                    break;
                }

                for (const auto& table : tables)
                    std::cout << std::string{ "ID: " + std::to_string(table.id) + "\tStatus: " + getTableStatusString(table.status) + '\n' };

                std::cout << "\nEnter the ID of the table to mark as dirty: ";
                getInput(id);

                if (databaseInterface.updateTableStatus(certification, id, Table::Status::Dirty))
                {
                    std::cout << "\nTable updated successfully.\n";
                    break;
                }
                else
                {
                    invalidInput();
                    break;
                }
            case Employee::Type::Busser:
                for (int i{}; i < tables.size(); i++)
                {
                    if (tables[i].status != Table::Status::Dirty)
                    {
                        tables.erase(tables.begin() + i);
                        --i;
                    }
                }

                if (tables.empty())
                {
                    std::cout << "There are no tables you can update." << std::endl;
                    break;
                }

                for (const auto& table : tables)
                    std::cout << std::string{ "ID: " + std::to_string(table.id) + "\tStatus: " + getTableStatusString(table.status) + '\n' };

                std::cout << "\nEnter the ID of the table to mark as ready: ";
                getInput(id);

                if (databaseInterface.updateTableStatus(certification, id, Table::Status::Ready))
                {
                    std::cout << "\nTable updated successfully.\n";
                    break;
                }
                else
                {
                    invalidInput();
                    break;
                }
            }
        }

        std::cout << '\n';
        pressToContinue();
    }
    catch (...)
    {
        invalidInput();
    }
}

void Application::login()
{
    try
    {
        printEmployeeList();

        std::cout << "\nEnter your login information:\nID: ";
        getInput(certification.id);
        std::cout << "Password: ";
        getInput(certification.password);

        Employee::Type type{ databaseInterface.getEmployeeType(certification) };
        switch (type)
        {
        case Employee::Type::Manager:
            userMenuStack.push(managerMenu);
            break;
        case Employee::Type::Waiter:
            userMenuStack.push(waiterMenu);
            break;
        case Employee::Type::Error:
            std::cout << "\nIncorrect login information.  Press any key to continue: ";
            std::cin.get();
        default:
            break;
        }
    }
    catch (...)
    {
        invalidInput();
    }
}

void Application::exit()
{
    // Clear the menu stack
    while (!userMenuStack.empty())
        userMenuStack.pop();
}

void Application::logout()
{
    // Clear stored certification
    certification = Certification{};

    // Return to the login menu
    while (userMenuStack.top() != loginMenu)
        userMenuStack.pop();
}

void Application::addEmployee()
{
    try
    {
        system("CLS");

        std::string firstName, lastName, password;
        int type;
        std::cout << "Enter the employee's first name: ";
        std::cin >> firstName;
        std::cout << "Enter the employee's last name: ";
        std::cin >> lastName;
        std::cout << "Enter the employee's password: ";
        std::cin >> password;
        std::cout << "Enter the employee's type: ";
        std::cin >> type;

        Employee employee{ Certification{ password }, static_cast<Employee::Type>(type), firstName, lastName };

        if (!databaseInterface.addEmployee(certification, employee))
            throw std::exception{};
    }
    catch (std::exception e)
    {
        invalidInput();
    }
}

void Application::addExampleMenu()
{
    std::vector<std::string> menus{
        "Beverages",
        "Appetizers",
        "Entrees",
        "Desserts"
    };
    for (const auto& menu : menus)
        databaseInterface.addMenu(certification, BASE_MENU_ID, menu);

    std::vector<std::tuple<int, std::string, double>> items{
        std::make_tuple(1, "Beverage A", 0.00),
        std::make_tuple(1, "Beverage B", 0.99),
        std::make_tuple(2, "Appetizer A", 5.99),
        std::make_tuple(3, "Entree A", 8.99),
        std::make_tuple(3, "Entree B", 8.99),
        std::make_tuple(3, "Entree C", 11.99),
        std::make_tuple(4, "Dessert A", 3.99)
    };
    for (const auto& item : items)
        databaseInterface.addItem(certification, std::get<0>(item), std::get<1>(item), std::get<2>(item));

    std::cout << "Example menu added successfully.\n";
    pressToContinue();
}

void Application::addTable()
{
    if (databaseInterface.addTable(certification))
    {
        std::cout << "Table added successfully.\n";
        pressToContinue();
    }
    else
        invalidInput();
}

void Application::printMenu()
{
    system("CLS");

    Menu menu;
    // CRASH 123529
    databaseInterface.getMenu(menu);

    std::cout << '\n';
    menu.print();
    std::cout << '\n';

    pressToContinue();
}