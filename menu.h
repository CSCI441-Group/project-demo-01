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

struct Party
{
    enum class Status
    {
        InWaitQueue,
        Seated,
        Finished
    };

    Party(int id, int tableId, int size, Status status) :
        id{ id }, tableId{ tableId }, size{ size }, status{ status }
    {}

    int id{};
    int tableId{};
    int size{};
    Status status{};
};

struct OrderItemAdjustment
{

};

struct OrderItem
{
    OrderItem(const int id, const int itemId, const std::string& name, const double price) :
        id{ id }, itemId{ itemId }, name{ name }, price{ price }
    {}

    int id{};
    int itemId{};
    std::string name{};
    double price{};
    std::vector<OrderItemAdjustment> adjustments{};
};

struct Order
{
    enum class Status
    {
        Placing,
        Placed,
        Made,
        Delivered,
        Paid,
        Cancelled
    };

    Order(int id, int partyId, Status status, double total) :
        id{ id }, partyId{ partyId }, status{ status }, total{ total }
    {}

    int id{};
    int partyId{};
    Status status{};
    double total{};
    std::vector<OrderItem> items{};
};