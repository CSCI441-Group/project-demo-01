#include "menu.h"

const bool Menu::hasMenu(int search) const
{
    if (id == search)
        return true;

    // Recursively determine if the result is a child
    for (const auto& submenu : submenus)
        if (submenu.hasMenu(search))
            return true;

    return false;
}

void Menu::print(int level) const
{
    // Print the current menu
    for (int i{}; i < level; i++)
        std::cout << '\t';
    std::cout << name << '\n';

    // Indent and print items in the menu
    ++level;
    for (const auto& item : items)
    {
        for (int i{}; i < level; i++)
        {
            std::cout << '\t';
        }
        std::cout << item.name << "  " << item.price << '\n';
    }

    // Recursively print out all submenus
    for (const auto& submenu : submenus)
        submenu.print(level);
}