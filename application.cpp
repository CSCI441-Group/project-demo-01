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
ReturnCode Application::getInput(T& value)
{
    try
    {
        std::cin >> value;

        // Clears the buffer in case an invalid string was input
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        return ReturnCode::Success;
    }
    catch (...)
    {
        invalidInput();
        return ReturnCode::InvalidInput;
    }
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
    std::cout << "\nInvalid input!\n";
    pressToContinue();
}

void Application::pressToContinue()
{
    std::cout << "\nPress any button to continue... ";
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

void Application::error()
{
    std::cout << "An error occurred!" << std::endl;
    pressToContinue();
}

void Application::uncertified()
{
    std::cout << "You lack the certification to perform this action!" << std::endl;
    pressToContinue();
}

void Application::printEmployeeList()
{
    system("CLS");

    std::vector<Employee> employees;
    if (databaseInterface.getEmployees(employees) != ReturnCode::Success)
    {
        error();
        return;
    }

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
    if (databaseInterface.getTables(tables) != ReturnCode::Success)
    {
        error();
        return;
    }

    if (tables.empty())
        std::cout << "There are no tables." << std::endl;
    else
    {
        std::cout << "Tables:\n" << std::endl;
        for (const auto& table : tables)
            std::cout << std::string{ "ID: " + std::to_string(table.id) + "\tStatus: " + getTableStatusString(table.status) + '\n' };
    }

    pressToContinue();
}

void Application::updateTable()
{
    system("CLS");

    Employee::Type type;
    if (databaseInterface.getEmployeeType(certification, type) != ReturnCode::Success)
    {
        error();
        return;
    }

    int id;
    std::vector<Table> tables{};
    ReturnCode rc;
    if (databaseInterface.getTables(tables) != ReturnCode::Success)
    {
        error();
        return;
    }
    Table::Status status;
    if (tables.empty())
    {
        std::cout << "There are no tables." << std::endl;
        pressToContinue();
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

            rc = databaseInterface.updateTableStatus(certification, id, status);
            if (rc == ReturnCode::Success)
            {
                std::cout << "\nTable updated successfully.\n";
                pressToContinue();
            }
            else if (rc == ReturnCode::NonexistentId)
                std::cout << "\nA table with ID " << id << " does not exist!\n";
            else if (rc == ReturnCode::Error)
                error();

            break;

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

            rc = databaseInterface.updateTableStatus(certification, id, Table::Status::Seated);
            if (rc == ReturnCode::Success)
            {
                std::cout << "\nTable updated successfully.\n";
                pressToContinue();
            }
            else if (rc == ReturnCode::NonexistentId)
                std::cout << "\nA table with ID " << id << " does not exist!\n";
            else if (rc == ReturnCode::Error)
                error();

            break;

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

            rc = databaseInterface.updateTableStatus(certification, id, Table::Status::Dirty);
            if (rc == ReturnCode::Success)
            {
                std::cout << "\nTable updated successfully.\n";
                pressToContinue();
            }
            else if (rc == ReturnCode::NonexistentId)
                std::cout << "\nA table with ID " << id << " does not exist!\n";
            else if (rc == ReturnCode::Error)
                error();

            break;

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

            rc = databaseInterface.updateTableStatus(certification, id, Table::Status::Ready);
            if (rc == ReturnCode::Success)
            {
                std::cout << "\nTable updated successfully.\n";
                pressToContinue();
            }
            else if (rc == ReturnCode::NonexistentId)
                std::cout << "\nA table with ID " << id << " does not exist!\n";
            else if (rc == ReturnCode::Error)
                error();

            break;
        }
    }
}

void Application::login()
{
    printEmployeeList();

    std::cout << "\nEnter your login information.\nID: ";
    getInput(certification.id);
    std::cout << "Password: ";
    getInput(certification.password);

    Employee::Type type;
    if (databaseInterface.getEmployeeType(certification, type) != ReturnCode::Success)
    {
        std::cout << "\nIncorrect login information.";
        pressToContinue();
        return;
    }

    switch (type)
    {
    case Employee::Type::Manager:
        userMenuStack.push(managerMenu);
        break;
    
    case Employee::Type::Host:
        userMenuStack.push(hostMenu);
        break;
    
    case Employee::Type::Waiter:
        userMenuStack.push(waiterMenu);
        break;

    case Employee::Type::Cook:
        userMenuStack.push(cookMenu);
        break;

    case Employee::Type::Busser:
        userMenuStack.push(busserMenu);
        break;
    
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

void Application::addExampleEmployees()
{
    system("CLS");

    Certification password{ "password" };
    std::vector<Employee> employees{
        Employee{ password, Employee::Type::Manager, "John", "Smith" },
        Employee{ password, Employee::Type::Host, "Jane", "Williams" },
        Employee{ password, Employee::Type::Waiter, "Ron", "Brown" },
        Employee{ password, Employee::Type::Cook, "Stephanie", "Davis" },
        Employee{ password, Employee::Type::Busser, "David", "Miller" },
    };

    for (const auto& employee : employees)
        databaseInterface.addEmployee(certification, employee);

    std::cout << "Employees added successfully.\n";
    pressToContinue();
}

void Application::addParty()
{
    system("CLS");

    std::vector<Table> tables{};
    if (databaseInterface.getTables(tables) != ReturnCode::Success)
    {
        error();
        return;
    }
    if (tables.empty())
    {
        std::cout << "There are no tables available.\n";
        pressToContinue();
    }

    //std::cout << "\nEnter the "

    std::cout << "Enter the party's size: ";
    int size;
    if (getInput(size) != ReturnCode::Success)
    {
        invalidInput();
        return;
    }
    
    auto rc{ databaseInterface.addParty(certification, size) };
    if (rc == ReturnCode::Success)
    {
        std::cout << "Party added successfully.\n";
        pressToContinue();
    }
    else if (rc == ReturnCode::Uncertified)
        uncertified();
    else if (rc == ReturnCode::Error)
        error();
}

void Application::addEmployee()
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
    auto rc{ databaseInterface.addEmployee(certification, employee) };
    if (rc == ReturnCode::Success)
    {
        std::cout << "\nEmployee added successfully.\n";
        pressToContinue();
    }
    else if (rc == ReturnCode::Uncertified)
        uncertified();
    else if (rc == ReturnCode::Error)
        error();
}

void Application::addExampleMenu()
{
    system("CLS");

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
    system("CLS");

    auto rc{ databaseInterface.addTable(certification) };
    if (rc == ReturnCode::Success)
    {
        std::cout << "Table added successfully.\n";
        pressToContinue();
    }
    else if (rc == ReturnCode::Uncertified)
        uncertified();
    else if (rc == ReturnCode::Error)
        error();
}

void Application::printMenu()
{
    system("CLS");

    Menu menu;
    if (databaseInterface.getMenu(menu) == ReturnCode::Success)
    {
        std::cout << '\n';
        menu.print();
        std::cout << '\n';

        pressToContinue();
    }
    else
        error();
}