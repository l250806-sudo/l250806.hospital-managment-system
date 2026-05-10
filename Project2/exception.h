#ifndef EXCEPTION_H
#define EXCEPTION_H

// Base class for all hospital exceptions
// stores message in a dynamic char array, has virtual what()
class HospitalException
{
private:
    char* message; // dynamically allocated error message

public:
    HospitalException(const char* msg);
    virtual ~HospitalException();
    virtual const char* what() const;
};

// thrown when a required file cannot be opened at startup
class FileNotFoundException : public HospitalException
{
public:
    FileNotFoundException();
    FileNotFoundException(const char* filename);
};

// thrown when patient balance is less than needed amount
class InsufficientFundsException : public HospitalException
{
public:
    InsufficientFundsException();
};

// thrown when user types bad input (wrong format, out of range, etc.)
class InvalidInputException : public HospitalException
{
public:
    InvalidInputException();
    InvalidInputException(const char* detail);
};

// thrown when patient tries to book a slot that is already taken
class SlotUnavailableException : public HospitalException
{
public:
    SlotUnavailableException();
};

#endif