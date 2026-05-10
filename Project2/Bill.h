#ifndef BILL_H
#define BILL_H

class bill
{
private:
    int   billId;
    int   patientId;
    int   appointmentId;
    float amount;
    char* status;   // "paid", "unpaid", "cancelled"
    char* date;     // DD-MM-YYYY

public:
    bill();
    bill(int bId, int pId, int aId, float amt, const char* stat, const char* d);

    // copy constructor and assignment for Storage<bill>
    bill(const bill& other);
    bill& operator=(const bill& other);

    ~bill();

    // getters
    int         getBillId()     const;
    int         getPatientId()  const;
    int         getAppointId()  const;
    float       getAmount()     const;
    const char* getStatus()     const;
    const char* getDate()       const;
    int getId() const;
    // setters
    void setStatus(const char* stat);
    void setDate(const char* d);
};

#endif