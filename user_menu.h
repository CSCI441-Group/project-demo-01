#pragma once

#include <cstdlib>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

#include "application.h"

struct UserMenuOption
{
    UserMenuOption(std::string name, void(Application::* function)()) :
        name{ name }, function{ function }
    {}

    std::string name;
    void(Application::* function)();
};

class UserMenu
{
public:
    UserMenu(std::string type, std::vector<UserMenuOption> options) :
        type{ type }, options{ options }
    {}

    void print();

    const std::vector<UserMenuOption>& getOptions() const { return options; };
private:
    std::string type;

    std::vector<UserMenuOption> options;
};

class UserInterface
{
public:
    void mainLoop(Application& application);

    void print();
    void getChoice(Application& application);
    void pushMenu(UserMenu& menu);
    void popMenu();
    void invalidInput();

    UserMenu& getMenu()  { return menus.top(); }

private:
    std::stack<UserMenu> menus;
};