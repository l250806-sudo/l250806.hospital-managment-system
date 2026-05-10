#ifndef PATIENT_H
#define PATIENT_H

#include "person.h"
#include <iostream>
using namespace std;

class patient : public person
{
private:
    int   age;
    char* gender;
    float balance;
    char* contact;

public:
    patient();
    patient(int ID, const char* n, const char* pass, const char* cont,
        int a, const char* g, float b);

    // copy constructor and assignment needed for Storage<patient>
    patient(const patient& other);
    patient& operator=(const patient& other);

    virtual ~patient();

    // getters
    int         getAge()     const;
    const char* getGender()  const;
    float       getBalance() const;
    const char* getContact() const;

    // setter
    void setBalance(float b);

    // required operator overloads
    void operator+=(float amount);          // top up balance
    void operator-=(float amount);          // deduct from balance
    bool operator==(const patient& other) const; // compare by ID

    friend ostream& operator<<(ostream& out, const patient& p);

    // pure virtual implementations
    virtual void displayMenu();
    virtual const char* getRole() { return "Patient"; }
};

#endif