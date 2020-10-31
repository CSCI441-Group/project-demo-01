#pragma once

#include <string>

struct Certification
{
    Certification() {}
    Certification(std::string password) :
        password{ password }
    {}
    Certification(int id, std::string password) :
        id{ id }, password{ password }
    {}

    int id{};
    std::string password{};
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

    Employee(Certification certification, Type type, std::string firstName, std::string lastName) :
        certification{ certification }, type{ type }, firstName{ firstName }, lastName{ lastName }
    {}
    
    Certification certification{};
    Type type;
    std::string firstName{}, lastName{};
};