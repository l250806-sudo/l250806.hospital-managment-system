#ifndef VALIDATOR_H
#define VALIDATOR_H

// Validator is the ONLY class allowed to do input validation
// All methods are static so you call them as Validator::validateXxx(...)
class Validator
{
public:
    static bool validateID(int id);
    static bool validateContact(const char* contact);   // must be exactly 11 digits
    static bool validatePassword(const char* pass);     // minimum 6 chars
    static bool validatePositiveFloat(float value);     // must be > 0
    static bool validateDate(const char* date);         // DD-MM-YYYY, current year or later
    static bool validateTimeSlot(const char* slot);     // one of the 8 fixed slots
    static bool validateMenuChoice(int choice, int max);// 1 to max
};

#endif