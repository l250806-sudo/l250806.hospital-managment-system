#ifndef PRESCRIPTION_H
#define PRESCRIPTION_H

class prescription
{
private:
    int   prescriptionId;
    int   appointmentId;
    int   patientId;
    int   doctorId;
    char* date;
    char* medicine;
    char* notes;

public:
    prescription();
    prescription(int pId, int aId, int patId, int docId,
        const char* d, const char* med, const char* n);

    // copy and assignment for Storage<prescription>
    prescription(const prescription& other);
    prescription& operator=(const prescription& other);

    ~prescription();

    // getters
    int         getPrescriptionId() const;
    int         getAppointmentId()  const;
    int         getPatientId()      const;
    int         getDoctorId()       const;
    const char* getDate()           const;
    const char* getMedicine()       const;
    const char* getNotes()          const;
};

#endif