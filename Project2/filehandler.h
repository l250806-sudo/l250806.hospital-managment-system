#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#define _CRT_SECURE_NO_WARNINGS
#include "storage.h"
#include "patient.h"
#include "doctor.h"
#include "appointment.h"
#include "prescription.h"
#include "Bill.h"
#include "Admin.h"

// FileHandler is the ONLY class allowed to do file I/O
// All data is stored as comma-separated values, one record per line
class FileHandler
{
private:
    // extracts field fieldIndex from a CSV line into output
    static void extractField(const char* line, int fieldIndex,
        char* output, int maxLen);

    // parse a single CSV line into an object
    static void parsePatientLine(const char* line, patient& out);
    static void parseDoctorLine(const char* line, doctor& out);
    static void parseAppointmentLine(const char* line, appointment& out);
    static void parsePrescriptionLine(const char* line, prescription& out);
    static void parseBillLine(const char* line, bill& out);

    // convert object back to CSV line (written into output buffer)
    static void patientToCSV(const patient& p, char* out, int maxLen);
    static void doctorToCSV(const doctor& d, char* out, int maxLen);
    static void appointmentToCSV(const appointment& a, char* out, int maxLen);
    static void prescriptionToCSV(const prescription& p, char* out, int maxLen);
    static void billToCSV(const bill& b, char* out, int maxLen);

    // low-level file helpers
    static int  getLineCount(const char* filename);
    static bool rewriteFile(const char* filename, char** lines, int lineCount);
    static bool appendLine(const char* filename, const char* line);
    static int  getMaxID(const char* filename, int idColumn);

    // helper: parse CSV int field
    static int parseIntField(const char* str);
    // helper: parse CSV float field
    static float parseFloatField(const char* str);

public:
    FileHandler();
    ~FileHandler();

    // startup: load all data from files into Storage
    Storage<patient>      loadPatients();
    Storage<doctor>       loadDoctors();
    Storage<appointment>  loadAppointments();
    Storage<prescription> loadPrescriptions();
    Storage<bill>         loadBills();
    Admin* loadAdmin();  // caller must delete

    // writing new records
    bool addPatient(const patient& p);
    bool addDoctor(const doctor& d);
    bool addAppointment(const appointment& a);
    bool addPrescription(const prescription& p);
    bool addBill(const bill& b);

    // updating existing records (rewrites the whole file)
    bool updatePatient(const patient& p);
    bool updateAppointment(const appointment& a);
    bool updateBill(const bill& b);
    bool updateDoctor(const doctor& d);

    // deleting records (for discharge)
    bool deleteDoctor(int doctorId);
    bool deletePatientRecord(int patientId);
    bool deleteAppointmentsByPatient(int patientId);
    bool deleteBillsByPatient(int patientId);
    bool deletePrescriptionsByPatient(int patientId);

    // archive patient to discharged.txt then remove from active files
    bool archiveAndDischargePatient(int patientId,
        Storage<patient>& patients,
        Storage<appointment>& appts,
        Storage<bill>& bills,
        Storage<prescription>& prescriptions);

    // security log
    void logSecurityEvent(const char* role, int enteredId, const char* result);

    // utility: returns max ID + 1 for the given file and ID column
    int getNextID(const char* filename, int idColumn);

    // check if file exists
    bool fileExists(const char* filename);
};

#endif