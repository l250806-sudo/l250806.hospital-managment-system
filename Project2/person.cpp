#include "person.h"
#include "string.h"

// default constructor - empty person
person::person()
{
    id = 0;
    name = new char[1];
    name[0] = '\0';
    password = new char[1];
    password[0] = '\0';
}

// parameterized constructor
person::person(int ID, const char* n, const char* pass)
{
    id = ID;
    int nameLen = myStrLen(n);
    name = new char[nameLen + 1];
    myStrCopy(name, n, nameLen + 1);

    int passLen = myStrLen(pass);
    password = new char[passLen + 1];
    myStrCopy(password, pass, passLen + 1);
}

// copy constructor - deep copy so Storage<T> works safely
person::person(const person& other)
{
    id = other.id;

    int nameLen = myStrLen(other.name);
    name = new char[nameLen + 1];
    myStrCopy(name, other.name, nameLen + 1);

    int passLen = myStrLen(other.password);
    password = new char[passLen + 1];
    myStrCopy(password, other.password, passLen + 1);
}

// assignment operator - needed when we do data[i] = newObject in Storage
person& person::operator=(const person& other)
{
    if (this == &other) return *this; // guard against self-assignment

    delete[] name;
    delete[] password;

    id = other.id;

    int nameLen = myStrLen(other.name);
    name = new char[nameLen + 1];
    myStrCopy(name, other.name, nameLen + 1);

    int passLen = myStrLen(other.password);
    password = new char[passLen + 1];
    myStrCopy(password, other.password, passLen + 1);

    return *this;
}

// destructor
person::~person()
{
    delete[] name;
    delete[] password;
}

int         person::getId()       const { return id; }
const char* person::getName()     const { return name; }
const char* person::getPassword() const { return password; }