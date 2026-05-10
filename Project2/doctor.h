#ifndef DOCTOR_H
#define DOCTOR_H

#include "person.h"
#include <iostream>
using namespace std;

class doctor : public person
{
private:
    char* specialization;
    float fee;
    char* contact;

public:
    doctor();
    doctor(int ID, const char* n, const char* pass, const char* cont,
        const char* spec, float f);

    // copy constructor and assignment for Storage<doctor>
    doctor(const doctor& other);
    doctor& operator=(const doctor& other);

    virtual ~doctor();

    // getters
    const char* getSpecialization() const;
    float       getFee()            const;
    const char* getContact()        const;

    // setter
    void setFee(float f);

    // required operators
    bool operator==(const doctor& other) const; // compare by ID
    friend ostream& operator<<(ostream& out, const doctor& d);

    // pure virtual implementations
    virtual void displayMenu();
    virtual const char* getRole() { return "Doctor"; }
};

#endif