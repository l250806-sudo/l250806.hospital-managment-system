#include "patient.h"
#include "string.h"
#include <iostream>
using namespace std;

// default constructor
patient::patient() : person()
{
    age = 0;
    balance = 0.0f;
    gender = new char[1];
    gender[0] = '\0';
    contact = new char[1];
    contact[0] = '\0';
}

// parameterized constructor
patient::patient(int ID, const char* n, const char* pass, const char* cont,
    int a, const char* g, float b)
    : person(ID, n, pass)
{
    age = a;
    balance = b;

    int gLen = myStrLen(g);
    gender = new char[gLen + 1];
    myStrCopy(gender, g, gLen + 1);

    int cLen = myStrLen(cont);
    contact = new char[cLen + 1];
    myStrCopy(contact, cont, cLen + 1);
}

// copy constructor - deep copy required for Storage<patient>
patient::patient(const patient& other) : person(other)
{
    age = other.age;
    balance = other.balance;

    int gLen = myStrLen(other.gender);
    gender = new char[gLen + 1];
    myStrCopy(gender, other.gender, gLen + 1);

    int cLen = myStrLen(other.contact);
    contact = new char[cLen + 1];
    myStrCopy(contact, other.contact, cLen + 1);
}

// assignment operator - needed when Storage<> stores objects in array
patient& patient::operator=(const patient& other)
{
    if (this == &other) return *this;

    // call base class assignment to handle name/password/id
    person::operator=(other);

    delete[] gender;
    delete[] contact;

    age = other.age;
    balance = other.balance;

    int gLen = myStrLen(other.gender);
    gender = new char[gLen + 1];
    myStrCopy(gender, other.gender, gLen + 1);

    int cLen = myStrLen(other.contact);
    contact = new char[cLen + 1];
    myStrCopy(contact, other.contact, cLen + 1);

    return *this;
}

// destructor - free dynamic memory
patient::~patient()
{
    delete[] gender;
    delete[] contact;
}

// getters
int         patient::getAge()     const { return age; }
const char* patient::getGender()  const { return gender; }
float       patient::getBalance() const { return balance; }
const char* patient::getContact() const { return contact; }

void patient::setBalance(float b) { balance = b; }

// add amount to balance (top up)
void patient::operator+=(float amount)
{
    if (amount > 0)
        balance += amount;
}

// deduct amount from balance
void patient::operator-=(float amount)
{
    if (amount > 0 && balance >= amount)
        balance -= amount;
}

// compare two patients by ID
bool patient::operator==(const patient& other) const
{
    return this->id == other.id;
}

// formatted output for display
ostream& operator<<(ostream& out, const patient& p)
{
    out << "ID: " << p.id << "\n";
    out << "Name: " << p.name << "\n";
    out << "Age: " << p.age << "\n";
    out << "Gender: " << p.gender << "\n";
    out << "Contact: " << p.contact << "\n";
    out << "Balance: " << p.balance << "\n";
    return out;
}

// displayMenu is handled in the GUI, this stub satisfies the pure virtual
void patient::displayMenu()
{
    cout << "Patient Menu - managed by GUI\n";
}