#include "doctor.h"
#include "string.h"
#include <iostream>
using namespace std;

// default constructor
doctor::doctor() : person()
{
    specialization = new char[1];
    specialization[0] = '\0';
    fee = 0.0f;
    contact = new char[1];
    contact[0] = '\0';
}

// parameterized constructor
doctor::doctor(int ID, const char* n, const char* pass, const char* cont,
    const char* spec, float f)
    : person(ID, n, pass)
{
    int sLen = myStrLen(spec);
    specialization = new char[sLen + 1];
    myStrCopy(specialization, spec, sLen + 1);

    fee = f;

    int cLen = myStrLen(cont);
    contact = new char[cLen + 1];
    myStrCopy(contact, cont, cLen + 1);
}

// copy constructor for Storage<doctor>
doctor::doctor(const doctor& other) : person(other)
{
    int sLen = myStrLen(other.specialization);
    specialization = new char[sLen + 1];
    myStrCopy(specialization, other.specialization, sLen + 1);

    fee = other.fee;

    int cLen = myStrLen(other.contact);
    contact = new char[cLen + 1];
    myStrCopy(contact, other.contact, cLen + 1);
}

// assignment operator
doctor& doctor::operator=(const doctor& other)
{
    if (this == &other) return *this;

    person::operator=(other);

    delete[] specialization;
    delete[] contact;

    int sLen = myStrLen(other.specialization);
    specialization = new char[sLen + 1];
    myStrCopy(specialization, other.specialization, sLen + 1);

    fee = other.fee;

    int cLen = myStrLen(other.contact);
    contact = new char[cLen + 1];
    myStrCopy(contact, other.contact, cLen + 1);

    return *this;
}

// destructor
doctor::~doctor()
{
    delete[] specialization;
    delete[] contact;
}

// getters
const char* doctor::getSpecialization() const { return specialization; }
float       doctor::getFee()            const { return fee; }
const char* doctor::getContact()        const { return contact; }

void doctor::setFee(float f) { fee = f; }

// compare doctors by ID
bool doctor::operator==(const doctor& other) const
{
    return this->id == other.id;
}

// formatted output
ostream& operator<<(ostream& out, const doctor& d)
{
    out << "ID: " << d.id << "\n";
    out << "Name: " << d.name << "\n";
    out << "Specialization: " << d.specialization << "\n";
    out << "Fee: PKR " << d.fee << "\n";
    return out;
}

void doctor::displayMenu()
{
    cout << "Doctor Menu - managed by GUI\n";
}