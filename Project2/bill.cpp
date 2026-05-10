#include "Bill.h"
#include "string.h"

// default constructor
bill::bill()
{
    billId = 0;
    patientId = 0;
    appointmentId = 0;
    amount = 0.0f;
    status = new char[1]; status[0] = '\0';
    date = new char[1]; date[0] = '\0';
}

// parameterized constructor
bill::bill(int bId, int pId, int aId, float amt, const char* stat, const char* d)
{
    billId = bId;
    patientId = pId;
    appointmentId = aId;
    amount = amt;

    int sLen = myStrLen(stat);
    status = new char[sLen + 1];
    myStrCopy(status, stat, sLen + 1);

    int dLen = myStrLen(d);
    date = new char[dLen + 1];
    myStrCopy(date, d, dLen + 1);
}

// copy constructor for Storage<bill>
bill::bill(const bill& other)
{
    billId = other.billId;
    patientId = other.patientId;
    appointmentId = other.appointmentId;
    amount = other.amount;

    int sLen = myStrLen(other.status);
    status = new char[sLen + 1];
    myStrCopy(status, other.status, sLen + 1);

    int dLen = myStrLen(other.date);
    date = new char[dLen + 1];
    myStrCopy(date, other.date, dLen + 1);
}

// assignment operator
bill& bill::operator=(const bill& other)
{
    if (this == &other) return *this;

    delete[] status;
    delete[] date;

    billId = other.billId;
    patientId = other.patientId;
    appointmentId = other.appointmentId;
    amount = other.amount;

    int sLen = myStrLen(other.status);
    status = new char[sLen + 1];
    myStrCopy(status, other.status, sLen + 1);

    int dLen = myStrLen(other.date);
    date = new char[dLen + 1];
    myStrCopy(date, other.date, dLen + 1);

    return *this;
}

// destructor
bill::~bill()
{
    delete[] status;
    delete[] date;
}

// getters
int         bill::getBillId()    const { return billId; }
int         bill::getPatientId() const { return patientId; }
int         bill::getAppointId() const { return appointmentId; }
float       bill::getAmount()    const { return amount; }
const char* bill::getStatus()    const { return status; }
const char* bill::getDate()      const { return date; }

// update status
void bill::setStatus(const char* stat)
{
    delete[] status;
    int len = myStrLen(stat);
    status = new char[len + 1];
    myStrCopy(status, stat, len + 1);
}

void bill::setDate(const char* d)
{
    delete[] date;
    int len = myStrLen(d);
    date = new char[len + 1];
    myStrCopy(date, d, len + 1);
}
int bill::getId() const
{
    return billId;

}