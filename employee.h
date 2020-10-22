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

class Employee
{
public:
    enum class Type
    {
        Error = -1,
        Admin = 0,
        Manager = 1,
        Waiter = 2,
        Cook = 3,
        Busser = 4
    };

    Employee(Certification certification, Type type, std::string firstName, std::string lastName) :
        certification{ certification }, type{ type }, firstName{ firstName }, lastName{ lastName }
    {}

    void setCertification(Certification& c) { certification = c; }
    void setType(Type t) { type = t; }
    void setFirstName(std::string& f) { firstName = f; }
    void setLastName(std::string& l) { lastName = l; }

    const Certification& getCertification() const { return certification; }
    const Type& getType() const { return type; }
    const std::string& getFirstName() const { return firstName; }
    const std::string& getLastName() const { return lastName; }
    
private:
    Certification certification{};
    Type type;
    std::string firstName{}, lastName{};
};