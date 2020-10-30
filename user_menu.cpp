#include "user_menu.h"

void UserMenu::print()
{
    system("CLS");

    std::cout << type << " Menu\n\n";

    for (int i{}; i < options.size(); i++)
        std::cout << i + 1 << ") " << options[i].name << '\n';
}


void UserInterface::mainLoop(Application& application)
{
    while (!menus.empty())
    {
        print();
        getChoice(application);
    }
}

void UserInterface::print()
{
    menus.top().print();
}

void UserInterface::pushMenu(UserMenu& menu)
{
    menus.push(menu);
}

void UserInterface::popMenu()
{
    menus.pop();
}

void UserInterface::getChoice(Application& application)
{
    try
    {
        std::cout << "\nEnter your selection: ";

        // Get user's input
        int response;
        std::cin >> response;

        --response;

        // Call associated function
        if (response >= 0 && response < menus.top().getOptions().size())
            (application.*menus.top().getOptions()[response].function)();
        else
            throw std::exception{};
    }
    catch (const std::exception& e)
    {
        invalidInput();
    }
}

void UserInterface::invalidInput()
{
    std::cout << "Invalid input. Press any button to continue: ";
    getchar();
}