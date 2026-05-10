#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <iostream>
using namespace std;

class appointment
{
private:
    int   appointmentID;
    int   patientID;
    int   doctorID;
    char* date;      // DD-MM-YYYY
    char* timeSlot;  // 09:00 etc.
    char* status;    // pending / completed / cancelled / noshow

public:
    appointment();
    appointment(int aID, int pID, int dID,
        const char* d, const char* t, const char* s);

    // copy constructor and assignment for Storage<appointment>
    appointment(const appointment& other);
    appointment& operator=(const appointment& other);

    virtual ~appointment();

    // getters
    int         getAppointmentID() const;
    int         getPatientID()     const;
    int         getDoctorID()      const;
    const char* getDate()          const;
    const char* getTimeSlot()      const;
    const char* getStatus()        const;
    int getId() const;
    // setter
    void setStatus(const char* s);

    // == checks for scheduling CONFLICT (same doctor, date, slot, both not cancelled)
    bool operator==(const appointment& other) const;

    friend ostream& operator<<(ostream& out, const appointment& a);
};

#endif