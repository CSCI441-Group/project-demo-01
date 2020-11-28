#pragma once

#include <string>
#include <vector>

using namespace std;

struct DateTime
{
    DateTime(string dateTime) {}

    int year{};
    int month{};
    int day{};
    int hour{};
    int minute{};
    int second{};
};

struct Adjustment
{
    Adjustment(const int id, const int adjustmentGroupId, const string& name, const double price) :
        id{ id }, adjustmentGroupId{ adjustmentGroupId }, name{ name }, price{ price }
    {}

    int id{};
    int adjustmentGroupId{};
    string name{};
    double price{};
};

struct AdjustmentGroup
{
    AdjustmentGroup(const int id, const int itemId, const string& name) :
        id{ id }, itemId{ itemId }, name{ name }
    {}

    int id{};
    int itemId{};
    string name{};
    vector<Adjustment> adjustments{};
};

struct Item
{
    Item(const int id, const int menuId, const string& name, const double price) :
        id{ id }, menuId{ menuId }, name{ name }, price{ price }
    {}

    int id{};
    int menuId{};
    string name{};
    double price{};
    vector<AdjustmentGroup> adjustmentGroups{};
};

struct Menu
{
    Menu(const int id, const int parentId, const string& name) :
        id{ id }, parentId{ parentId }, name{ name }
    {}

    int id{};
    int parentId{};
    string name{};
    vector<Menu> submenus{};
    vector<Item> items{};

    const bool hasMenu(int search) const
    {
        if (id == search)
            return true;

        // Recursively determine if the result is a child
        for (const auto& submenu : submenus)
            if (submenu.hasMenu(search))
                return true;

        return false;
    }
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

    Table(const int id, const Status status) :
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

    Party(const int id, const int tableId, const int size, const Status status, const DateTime& waitQueueTime, const DateTime& seatedTime, const DateTime& finishedTime) :
        id{ id }, tableId{ tableId }, size{ size }, status{ status }, waitQueueTime{ waitQueueTime }, seatedTime{ seatedTime }, finishedTime{ finishedTime }
    {}

    int id{};
    int tableId{};
    int size{};
    Status status{};
    DateTime waitQueueTime;
    DateTime seatedTime;
    DateTime finishedTime;
};

struct OrderItemAdjustment
{
    OrderItemAdjustment(const int orderItemId, const Adjustment& adjustment) :
        orderItemId{ orderItemId }, adjustment{ adjustment }
    {}

    int orderItemId{};
    Adjustment adjustment;
};

struct OrderItem
{
    OrderItem(const int id, const int orderId, const Item& item) :
        id{ id }, orderId{ orderId }, item{ item }
    {}

    int id{};
    int orderId{};
    Item item;
    vector<OrderItemAdjustment> adjustments{};
};

struct Order
{
    enum class Status
    {
        Placing,
        Placed,
        Made,
        Delivered,
        Finished
    };

    Order(const int id, const int partyId, const Status status, const double total, const DateTime placeTime, const DateTime deliverTime, const double tip, const bool paid) :
        id{ id }, partyId{ partyId }, status{ status }, total{ total }, placeTime{ placeTime }, deliverTime{ deliverTime }
    {}

    int id{};
    int partyId{};
    Status status{};
    double total{};
    DateTime placeTime;
    DateTime deliverTime;
    double tip{};
    bool paid{ false };
    vector<OrderItem> items{};
};

struct Payment
{
    enum class Type {
        Cash,
        GiftCard,
        CreditCard,
        DebitCard
    };

    Payment(const int orderId, const double amount, const Type type, const string cardNumber) :
        orderId{ orderId }, amount{ amount }, type{ type }, cardNumber{ cardNumber }
    {}

    int orderId{};
    double amount{};
    Type type{};
    string cardNumber{};
};

struct Certification
{
    Certification() {}
    Certification(const string& password) :
        password{ password }
    {}
    Certification(const int id, const string& password) :
        id{ id }, password{ password }
    {}

    int id{};
    string password{};
};

struct Employee
{
public:
    enum class Type
    {
        Manager = 0,
        Host = 1,
        Waiter = 2,
        Cook = 3,
        Busser = 4
    };

    Employee(const Certification& certification, const Type type, const string& firstName, const string lastName, const double payRate) :
        certification{ certification }, type{ type }, firstName{ firstName }, lastName{ lastName }, payRate{ payRate }
    {}

    Certification certification{};
    Type type;
    string firstName{}, lastName{};
    double payRate{};
};

class DatabaseSubscriber
{
public:
    virtual void sync() = 0;
};