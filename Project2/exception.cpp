#include "exception.h"
#include "string.h"

// ---- HospitalException ----
HospitalException::HospitalException(const char* msg)
{
    int len = myStrLen(msg);
    message = new char[len + 1];
    myStrCopy(message, msg, len + 1);
}

HospitalException::~HospitalException()
{
    delete[] message;
}

const char* HospitalException::what() const
{
    return message;
}

// ---- FileNotFoundException ----
FileNotFoundException::FileNotFoundException()
    : HospitalException("Required file could not be opened.")
{
}

FileNotFoundException::FileNotFoundException(const char* filename)
    : HospitalException(filename) // pass the filename as part of message
{
}

// ---- InsufficientFundsException ----
InsufficientFundsException::InsufficientFundsException()
    : HospitalException("Insufficient balance. Please top up your account.")
{
}

// ---- InvalidInputException ----
InvalidInputException::InvalidInputException()
    : HospitalException("Invalid input. Please check and try again.")
{
}

InvalidInputException::InvalidInputException(const char* detail)
    : HospitalException(detail)
{
}

// ---- SlotUnavailableException ----
SlotUnavailableException::SlotUnavailableException()
    : HospitalException("This time slot is already booked. Please choose another.")
{
}