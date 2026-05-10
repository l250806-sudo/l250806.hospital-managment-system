#ifndef ADMIN_H
#define ADMIN_H

#include "person.h"

// Admin inherits from person just like Patient and Doctor
class Admin : public person
{
public:
    Admin();
    Admin(int ID, const char* n, const char* pass);

    // copy and assignment for completeness (Storage<Admin> not used but good habit)
    Admin(const Admin& other);
    Admin& operator=(const Admin& other);

    virtual ~Admin();

    // pure virtual implementations from person
    virtual void displayMenu();
    virtual const char* getRole() { return "Admin"; }
};

#endif