#include "Validator.h"
#include "string.h"

bool Validator::validateID(int id)
{
    return id > 0;
}

// contact must be exactly 11 characters, all digits
bool Validator::validateContact(const char* contact)
{
    int len = myStrLen(contact);
    if (len != 11)
        return false;
    for (int i = 0; i < len; i++)
    {
        if (contact[i] < '0' || contact[i] > '9')
            return false;
    }
    return true;
}

// password must be at least 6 characters long
bool Validator::validatePassword(const char* pass)
{
    return myStrLen(pass) >= 6;
}

// amount or fee must be greater than zero
bool Validator::validatePositiveFloat(float value)
{
    return value > 0.0f;
}

// date format: DD-MM-YYYY, must be current year (2026) or later
bool Validator::validateDate(const char* date)
{
    // must be exactly 10 characters
    if (myStrLen(date) != 10)
        return false;

    // hyphens at positions 2 and 5
    if (date[2] != '-' || date[5] != '-')
        return false;

    // all other positions must be digits
    for (int i = 0; i < 10; i++)
    {
        if (i == 2 || i == 5) continue;
        if (date[i] < '0' || date[i] > '9')
            return false;
    }

    // extract day, month, year
    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100
        + (date[8] - '0') * 10 + (date[9] - '0');

    if (month < 1 || month > 12) return false;
    if (day < 1 || day   > 31) return false;

    // months with only 30 days
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
        return false;

    // February - simplified (treats every year as non-leap for safety)
    if (month == 2 && day > 28)
        return false;

    // must be current year or later (assignment says 2026)
    if (year < 2026)
        return false;

    return true;
}

// must be one of the 8 fixed daily slots
bool Validator::validateTimeSlot(const char* slot)
{
    const char* validSlots[8] = {
        "09:00", "10:00", "11:00", "12:00",
        "13:00", "14:00", "15:00", "16:00"
    };
    for (int i = 0; i < 8; i++)
    {
        if (myStrEq(slot, validSlots[i]))
            return true;
    }
    return false;
}

// choice must be between 1 and max inclusive
bool Validator::validateMenuChoice(int choice, int max)
{
    return choice >= 1 && choice <= max;
}