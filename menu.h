#pragma once

#include <iostream>
#include <string>
#include <vector>


struct Adjustment
{
    int id{};
    std::string name;
    double price{};
};

struct AdjustmentGroup
{
    int id{};
    std::string name;
    std::vector<Adjustment> adjustments;
};

struct Item
{
    Item(const int id, const std::string& name, const double price) :
        id{ id }, name{ name }, price{ price }
    {}

    int id{};
    std::string name;
    double price{};
    std::vector<AdjustmentGroup> adjustmentGroups;
};

struct Menu
{
    Menu()
    {}

    Menu(const int id, const std::string& name) :
        id{ id }, name{ name }
    {}

    void print(int level = 0) const;
    const bool hasMenu(int search) const;
    const bool hasItem(int search) const;
    const bool hasAdjustmentGroup(int search) const;

    int id{};
    std::string name;
    std::vector<Menu> submenus;
    std::vector<Item> items;
};

struct Table
{
public:
    enum class Status
    {
        Error = -1,
        OutOfUse = 0,
        Ready,
        Seated,
        Dirty
    };

    Table(int id, Status status) :
        id{ id }, status{ status }
    {}

    int id{};
    Status status{};
};