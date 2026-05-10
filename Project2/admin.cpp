#include "Admin.h"
#include <iostream>
using namespace std;

Admin::Admin() : person() {}

Admin::Admin(int ID, const char* n, const char* pass)
    : person(ID, n, pass) {
}

Admin::Admin(const Admin& other) : person(other) {}

Admin& Admin::operator=(const Admin& other)
{
    if (this == &other) return *this;
    person::operator=(other);
    return *this;
}

Admin::~Admin() {}

// displayMenu is shown in the SFML GUI - this satisfies the pure virtual
void Admin::displayMenu()
{
    cout << "Admin Panel - managed by GUI\n";
}