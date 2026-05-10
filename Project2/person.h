#ifndef PERSON_H
#define PERSON_H

// Abstract base class shared by Patient, Doctor and Admin
// holds the common identity data every user type has
class person
{
protected:
    int   id;
    char* name;
    char* password;

public:
    person();
    person(int ID, const char* n, const char* pass);

    // copy constructor - needed so Storage<T> works without crashes
    person(const person& other);

    // assignment operator - needed for Storage<T> array assignment
    person& operator=(const person& other);

    virtual ~person();

    // getters
    int         getId()       const;
    const char* getName()     const;
    const char* getPassword() const;

    // pure virtual methods - every subclass MUST implement these
    virtual void displayMenu() = 0;
    virtual const char* getRole() = 0;
};

#endif