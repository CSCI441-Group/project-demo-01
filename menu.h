#pragma once

#include <string>
#include <vector>

class Menu
{
public:

private:
    std::vector<Category> categories;
};

class Category
{
public:

private:
    int id{};
    std::string category{};
    std::vector<Item> items{};
};

class Item
{
public:

private:
    int id;
    std::string name;
    double price;
    std::vector<AdjustmentGroup> adjustmentGroups;
};

class AdjustmentGroup
{
public:

private:
    int id;
    std::string description;
    std::vector<Adjustment> adjustments;
};

class Adjustment
{
public:

private:
    int id;
    std::string description;
    double price;
};