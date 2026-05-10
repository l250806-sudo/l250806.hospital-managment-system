#include "appointment.h"
#include "string.h"
#include <iostream>
using namespace std;

// default constructor - empty appointment
appointment::appointment()
{
    appointmentID = 0;
    patientID = 0;
    doctorID = 0;
    date = new char[1]; date[0] = '\0';
    timeSlot = new char[1]; timeSlot[0] = '\0';
    status = new char[1]; status[0] = '\0';
}

// parameterized constructor
appointment::appointment(int aID, int pID, int dID,
    const char* d, const char* t, const char* s)
{
    appointmentID = aID;
    patientID = pID;
    doctorID = dID;

    int l1 = myStrLen(d);
    date = new char[l1 + 1];
    myStrCopy(date, d, l1 + 1);

    int l2 = myStrLen(t);
    timeSlot = new char[l2 + 1];
    myStrCopy(timeSlot, t, l2 + 1);

    int l3 = myStrLen(s);
    status = new char[l3 + 1];
    myStrCopy(status, s, l3 + 1);
}

// copy constructor - needed for Storage<appointment>
appointment::appointment(const appointment& other)
{
    appointmentID = other.appointmentID;
    patientID = other.patientID;
    doctorID = other.doctorID;

    int l1 = myStrLen(other.date);
    date = new char[l1 + 1];
    myStrCopy(date, other.date, l1 + 1);

    int l2 = myStrLen(other.timeSlot);
    timeSlot = new char[l2 + 1];
    myStrCopy(timeSlot, other.timeSlot, l2 + 1);

    int l3 = myStrLen(other.status);
    status = new char[l3 + 1];
    myStrCopy(status, other.status, l3 + 1);
}

// assignment operator
appointment& appointment::operator=(const appointment& other)
{
    if (this == &other) return *this;

    delete[] date;
    delete[] timeSlot;
    delete[] status;

    appointmentID = other.appointmentID;
    patientID = other.patientID;
    doctorID = other.doctorID;

    int l1 = myStrLen(other.date);
    date = new char[l1 + 1];
    myStrCopy(date, other.date, l1 + 1);

    int l2 = myStrLen(other.timeSlot);
    timeSlot = new char[l2 + 1];
    myStrCopy(timeSlot, other.timeSlot, l2 + 1);

    int l3 = myStrLen(other.status);
    status = new char[l3 + 1];
    myStrCopy(status, other.status, l3 + 1);

    return *this;
}

// destructor - must free all three dynamic strings
appointment::~appointment()
{
    delete[] date;
    delete[] timeSlot;
    delete[] status;
}

// getters
int         appointment::getAppointmentID() const { return appointmentID; }
int         appointment::getPatientID()     const { return patientID; }
int         appointment::getDoctorID()      const { return doctorID; }
const char* appointment::getDate()          const { return date; }
const char* appointment::getTimeSlot()      const { return timeSlot; }
const char* appointment::getStatus()        const { return status; }

// update status in memory (caller must also update the file)
void appointment::setStatus(const char* s)
{
    delete[] status;
    int len = myStrLen(s);
    status = new char[len + 1];
    myStrCopy(status, s, len + 1);
}

// CONFLICT CHECK: same doctor + same date + same slot + neither is cancelled
bool appointment::operator==(const appointment& other) const
{
    // different doctor means no conflict
    if (doctorID != other.doctorID)
        return false;

    // cancelled appointments don't block slots
    if (status[0] == 'c') return false;       // "cancelled"
    if (other.status[0] == 'c') return false;

    // compare dates
    if (!myStrEq(date, other.date))
        return false;

    // compare time slots
    if (!myStrEq(timeSlot, other.timeSlot))
        return false;

    return true; // same doctor, date, slot, both not cancelled = CONFLICT
}

// formatted output
ostream& operator<<(ostream& out, const appointment& a)
{
    out << "Appt ID: " << a.appointmentID
        << " | PatID: " << a.patientID
        << " | DocID: " << a.doctorID
        << " | Date: " << a.date
        << " | Slot: " << a.timeSlot
        << " | Status: " << a.status;
    return out;
}
int appointment::getId() const
{
    return appointmentID;
}