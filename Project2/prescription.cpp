#include "prescription.h"
#include "string.h"

// default constructor
prescription::prescription()
{
    prescriptionId = 0;
    appointmentId = 0;
    patientId = 0;
    doctorId = 0;
    date = new char[1]; date[0] = '\0';
    medicine = new char[1]; medicine[0] = '\0';
    notes = new char[1]; notes[0] = '\0';
}

// parameterized constructor
prescription::prescription(int pId, int aId, int patId, int docId,
    const char* d, const char* med, const char* n)
{
    prescriptionId = pId;
    appointmentId = aId;
    patientId = patId;
    doctorId = docId;

    int dLen = myStrLen(d);
    date = new char[dLen + 1];
    myStrCopy(date, d, dLen + 1);

    int mLen = myStrLen(med);
    medicine = new char[mLen + 1];
    myStrCopy(medicine, med, mLen + 1);

    int nLen = myStrLen(n);
    notes = new char[nLen + 1];
    myStrCopy(notes, n, nLen + 1);
}

// copy constructor for Storage<prescription>
prescription::prescription(const prescription& other)
{
    prescriptionId = other.prescriptionId;
    appointmentId = other.appointmentId;
    patientId = other.patientId;
    doctorId = other.doctorId;

    int dLen = myStrLen(other.date);
    date = new char[dLen + 1];
    myStrCopy(date, other.date, dLen + 1);

    int mLen = myStrLen(other.medicine);
    medicine = new char[mLen + 1];
    myStrCopy(medicine, other.medicine, mLen + 1);

    int nLen = myStrLen(other.notes);
    notes = new char[nLen + 1];
    myStrCopy(notes, other.notes, nLen + 1);
}

// assignment operator
prescription& prescription::operator=(const prescription& other)
{
    if (this == &other) return *this;

    delete[] date;
    delete[] medicine;
    delete[] notes;

    prescriptionId = other.prescriptionId;
    appointmentId = other.appointmentId;
    patientId = other.patientId;
    doctorId = other.doctorId;

    int dLen = myStrLen(other.date);
    date = new char[dLen + 1];
    myStrCopy(date, other.date, dLen + 1);

    int mLen = myStrLen(other.medicine);
    medicine = new char[mLen + 1];
    myStrCopy(medicine, other.medicine, mLen + 1);

    int nLen = myStrLen(other.notes);
    notes = new char[nLen + 1];
    myStrCopy(notes, other.notes, nLen + 1);

    return *this;
}

// destructor
prescription::~prescription()
{
    delete[] date;
    delete[] medicine;
    delete[] notes;
}

// getters
int         prescription::getPrescriptionId() const { return prescriptionId; }
int         prescription::getAppointmentId()  const { return appointmentId; }
int         prescription::getPatientId()      const { return patientId; }
int         prescription::getDoctorId()       const { return doctorId; }
const char* prescription::getDate()           const { return date; }
const char* prescription::getMedicine()       const { return medicine; }
const char* prescription::getNotes()          const { return notes; }