// FileHandler.cpp
// The ONLY class allowed to perform file I/O in the system.
// All data files are CSV (comma-separated), one record per line.
#define _CRT_SECURE_NO_WARNINGS
#include "filehandler.h"
#include "string.h"
#include "exception.h"
#include <fstream>
#include <ctime>
#include <cstdio>
using namespace std;

// ============================================================
//  Constructor / Destructor
// ============================================================
FileHandler::FileHandler() {}
FileHandler::~FileHandler() {}

// ============================================================
//  PRIVATE: extractField
//  Pulls field number fieldIndex from a CSV line.
//  Comma is the delimiter.
// ============================================================
void FileHandler::extractField(const char* line, int fieldIndex,
    char* output, int maxLen)
{
    int currentField = 0;
    int outPos = 0;

    for (int i = 0; line[i] != '\0' && outPos < maxLen - 1; i++)
    {
        if (line[i] == ',')
        {
            currentField++;
        }
        else if (currentField == fieldIndex)
        {
            output[outPos++] = line[i];
        }
    }
    output[outPos] = '\0';
}

// ============================================================
//  PRIVATE: int/float parsers
// ============================================================
int FileHandler::parseIntField(const char* str)
{
    int val = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] >= '0' && str[i] <= '9')
            val = val * 10 + (str[i] - '0');
    }
    return val;
}

float FileHandler::parseFloatField(const char* str)
{
    float val = 0.0f;
    float fraction = 0.0f;
    float divisor = 1.0f;
    bool  inFrac = false;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '.')
        {
            inFrac = true;
        }
        else if (str[i] >= '0' && str[i] <= '9')
        {
            if (!inFrac)
                val = val * 10.0f + (str[i] - '0');
            else
            {
                fraction += (str[i] - '0') / (divisor *= 10.0f);
            }
        }
    }
    return val + fraction;
}

// ============================================================
//  PRIVATE: getLineCount
//  Counts non-empty lines in a file (including header).
// ============================================================
int FileHandler::getLineCount(const char* filename)
{
    ifstream file(filename);
    if (!file.is_open()) return 0;

    int   count = 0;
    char  buf[4096];
    while (file.getline(buf, 4096))
    {
        if (myStrLen(buf) > 0)
            count++;
    }
    file.close();
    return count;
}

// ============================================================
//  PRIVATE: rewriteFile
//  Writes all lines back to a file (used for update/delete).
// ============================================================
bool FileHandler::rewriteFile(const char* filename, char** lines, int lineCount)
{
    ofstream file(filename);
    if (!file.is_open()) return false;

    for (int i = 0; i < lineCount; i++)
        file << lines[i] << "\n";

    file.close();
    return true;
}

// ============================================================
//  PRIVATE: appendLine
//  Appends one CSV line to the end of a file.
// ============================================================
bool FileHandler::appendLine(const char* filename, const char* line)
{
    ofstream file(filename, ios::app);
    if (!file.is_open()) return false;
    file << line << "\n";
    file.close();
    return true;
}

// ============================================================
//  PRIVATE: getMaxID
//  Scans a file and returns the maximum value in column idColumn.
// ============================================================
int FileHandler::getMaxID(const char* filename, int idColumn)
{
    ifstream file(filename);
    if (!file.is_open()) return 0;

    char  buf[4096];
    char  field[50];
    int   maxId = 0;

    while (file.getline(buf, 4096))
    {
        if (myStrLen(buf) == 0) continue;
        extractField(buf, idColumn, field, 50);
        // skip header (non-numeric first char)
        if (field[0] < '0' || field[0] > '9') continue;
        int id = parseIntField(field);
        if (id > maxId) maxId = id;
    }
    file.close();
    return maxId;
}

// ============================================================
//  PUBLIC: getNextID
// ============================================================
int FileHandler::getNextID(const char* filename, int idColumn)
{
    return getMaxID(filename, idColumn) + 1;
}

bool FileHandler::fileExists(const char* filename)
{
    ifstream f(filename);
    return f.good();
}

// ============================================================
//  PRIVATE: CSV serializers (object -> line)
// ============================================================
void FileHandler::patientToCSV(const patient& p, char* out, int maxLen)
{
    char idBuf[12], ageBuf[12], balBuf[24];
    intToCharArr(p.getId(), idBuf);
    intToCharArr(p.getAge(), ageBuf);
    floatToCharArr(p.getBalance(), balBuf);

    out[0] = '\0';
    myStrCat(out, idBuf, maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, p.getName(), maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, ageBuf, maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, p.getGender(), maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, p.getContact(), maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, p.getPassword(), maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, balBuf, maxLen);
}

void FileHandler::doctorToCSV(const doctor& d, char* out, int maxLen)
{
    char idBuf[12], feeBuf[24];
    intToCharArr(d.getId(), idBuf);
    floatToCharArr(d.getFee(), feeBuf);

    out[0] = '\0';
    myStrCat(out, idBuf, maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, d.getName(), maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, d.getSpecialization(), maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, d.getContact(), maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, d.getPassword(), maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, feeBuf, maxLen);
}

void FileHandler::appointmentToCSV(const appointment& a, char* out, int maxLen)
{
    char aBuf[12], pBuf[12], dBuf[12];
    intToCharArr(a.getAppointmentID(), aBuf);
    intToCharArr(a.getPatientID(), pBuf);
    intToCharArr(a.getDoctorID(), dBuf);

    out[0] = '\0';
    myStrCat(out, aBuf, maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, pBuf, maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, dBuf, maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, a.getDate(), maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, a.getTimeSlot(), maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, a.getStatus(), maxLen);
}

void FileHandler::prescriptionToCSV(const prescription& p, char* out, int maxLen)
{
    char idBuf[12], aiBuf[12], piBuf[12], diBuf[12];
    intToCharArr(p.getPrescriptionId(), idBuf);
    intToCharArr(p.getAppointmentId(), aiBuf);
    intToCharArr(p.getPatientId(), piBuf);
    intToCharArr(p.getDoctorId(), diBuf);

    out[0] = '\0';
    myStrCat(out, idBuf, maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, aiBuf, maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, piBuf, maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, diBuf, maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, p.getDate(), maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, p.getMedicine(), maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, p.getNotes(), maxLen);
}

void FileHandler::billToCSV(const bill& b, char* out, int maxLen)
{
    char biBuf[12], piBuf[12], aiBuf[12], amtBuf[24];
    intToCharArr(b.getBillId(), biBuf);
    intToCharArr(b.getPatientId(), piBuf);
    intToCharArr(b.getAppointId(), aiBuf);
    floatToCharArr(b.getAmount(), amtBuf);

    out[0] = '\0';
    myStrCat(out, biBuf, maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, piBuf, maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, aiBuf, maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, amtBuf, maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, b.getStatus(), maxLen);
    myStrCat(out, ",", maxLen);
    myStrCat(out, b.getDate(), maxLen);
}

// ============================================================
//  PRIVATE: CSV parsers (line -> object)
// ============================================================
void FileHandler::parsePatientLine(const char* line, patient& out)
{
    char idStr[10], name[100], ageStr[10], gender[10];
    char contact[20], pass[50], balStr[20];

    extractField(line, 0, idStr, 10);
    extractField(line, 1, name, 100);
    extractField(line, 2, ageStr, 10);
    extractField(line, 3, gender, 10);
    extractField(line, 4, contact, 20);
    extractField(line, 5, pass, 50);
    extractField(line, 6, balStr, 20);

    int   id = parseIntField(idStr);
    int   age = parseIntField(ageStr);
    float balance = parseFloatField(balStr);

    out = patient(id, name, pass, contact, age, gender, balance);
}

void FileHandler::parseDoctorLine(const char* line, doctor& out)
{
    char idStr[10], name[100], spec[100], contact[20], pass[50], feeStr[20];

    extractField(line, 0, idStr, 10);
    extractField(line, 1, name, 100);
    extractField(line, 2, spec, 100);
    extractField(line, 3, contact, 20);
    extractField(line, 4, pass, 50);
    extractField(line, 5, feeStr, 20);

    int   id = parseIntField(idStr);
    float fee = parseFloatField(feeStr);

    out = doctor(id, name, pass, contact, spec, fee);
}

void FileHandler::parseAppointmentLine(const char* line, appointment& out)
{
    char aIdStr[10], pIdStr[10], dIdStr[10], date[15], slot[10], status[20];

    extractField(line, 0, aIdStr, 10);
    extractField(line, 1, pIdStr, 10);
    extractField(line, 2, dIdStr, 10);
    extractField(line, 3, date, 15);
    extractField(line, 4, slot, 10);
    extractField(line, 5, status, 20);

    int aId = parseIntField(aIdStr);
    int pId = parseIntField(pIdStr);
    int dId = parseIntField(dIdStr);

    out = appointment(aId, pId, dId, date, slot, status);
}

void FileHandler::parsePrescriptionLine(const char* line, prescription& out)
{
    char pIdStr[10], aIdStr[10], patIdStr[10], docIdStr[10];
    char date[15], medicine[512], notes[312];

    extractField(line, 0, pIdStr, 10);
    extractField(line, 1, aIdStr, 10);
    extractField(line, 2, patIdStr, 10);
    extractField(line, 3, docIdStr, 10);
    extractField(line, 4, date, 15);
    extractField(line, 5, medicine, 512);
    extractField(line, 6, notes, 312);

    int pId = parseIntField(pIdStr);
    int aId = parseIntField(aIdStr);
    int patId = parseIntField(patIdStr);
    int docId = parseIntField(docIdStr);

    out = prescription(pId, aId, patId, docId, date, medicine, notes);
}

void FileHandler::parseBillLine(const char* line, bill& out)
{
    char bIdStr[10], pIdStr[10], aIdStr[10], amtStr[20], status[20], date[15];

    extractField(line, 0, bIdStr, 10);
    extractField(line, 1, pIdStr, 10);
    extractField(line, 2, aIdStr, 10);
    extractField(line, 3, amtStr, 20);
    extractField(line, 4, status, 20);
    extractField(line, 5, date, 15);

    int   bId = parseIntField(bIdStr);
    int   pId = parseIntField(pIdStr);
    int   aId = parseIntField(aIdStr);
    float amt = parseFloatField(amtStr);

    out = bill(bId, pId, aId, amt, status, date);
}

// ============================================================
//  PUBLIC: Load functions (called once at startup)
// ============================================================
Storage<patient> FileHandler::loadPatients()
{
    Storage<patient> store;
    ifstream file("patient.txt");
    if (!file.is_open()) return store; // empty storage if file missing

    char buf[4096];
    while (file.getline(buf, 4096))
    {
        if (myStrLen(buf) == 0) continue;
        // skip header line (starts with non-digit like 'p')
        if (buf[0] < '0' || buf[0] > '9') continue;

        patient p;
        parsePatientLine(buf, p);
        if (p.getId() > 0)
            store.add(p);
    }
    file.close();
    return store;
}

Storage<doctor> FileHandler::loadDoctors()
{
    Storage<doctor> store;
    ifstream file("doctor.txt");
    if (!file.is_open()) return store;

    char buf[4096];
    while (file.getline(buf, 4096))
    {
        if (myStrLen(buf) == 0) continue;
        if (buf[0] < '0' || buf[0] > '9') continue;

        doctor d;
        parseDoctorLine(buf, d);
        if (d.getId() > 0)
            store.add(d);
    }
    file.close();
    return store;
}

Storage<appointment> FileHandler::loadAppointments()
{
    Storage<appointment> store;
    ifstream file("appointments.txt");
    if (!file.is_open()) return store;

    char buf[4096];
    while (file.getline(buf, 4096))
    {
        if (myStrLen(buf) == 0) continue;
        if (buf[0] < '0' || buf[0] > '9') continue;

        appointment a;
        parseAppointmentLine(buf, a);
        if (a.getAppointmentID() > 0)
            store.add(a);
    }
    file.close();
    return store;
}

Storage<prescription> FileHandler::loadPrescriptions()
{
    Storage<prescription> store;
    ifstream file("prescriptions.txt");
    if (!file.is_open()) return store;

    char buf[4096];
    while (file.getline(buf, 4096))
    {
        if (myStrLen(buf) == 0) continue;
        if (buf[0] < '0' || buf[0] > '9') continue;

        prescription p;
        parsePrescriptionLine(buf, p);
        if (p.getPrescriptionId() > 0)
            store.add(p);
    }
    file.close();
    return store;
}

Storage<bill> FileHandler::loadBills()
{
    Storage<bill> store;
    ifstream file("bills.txt");
    if (!file.is_open()) return store;

    char buf[4096];
    while (file.getline(buf, 4096))
    {
        if (myStrLen(buf) == 0) continue;
        if (buf[0] < '0' || buf[0] > '9') continue;

        bill b;
        parseBillLine(buf, b);
        if (b.getBillId() > 0)
            store.add(b);
    }
    file.close();
    return store;
}

// Returns a heap-allocated Admin pointer; caller must delete it
Admin* FileHandler::loadAdmin()
{
    ifstream file("admin.txt");
    if (!file.is_open()) return nullptr;

    char buf[256];
    while (file.getline(buf, 256))
    {
        if (myStrLen(buf) == 0) continue;
        if (buf[0] < '0' || buf[0] > '9') continue;

        char idStr[10], name[100], pass[50];
        extractField(buf, 0, idStr, 10);
        extractField(buf, 1, name, 100);
        extractField(buf, 2, pass, 50);

        int id = parseIntField(idStr);
        file.close();
        return new Admin(id, name, pass);
    }
    file.close();
    return nullptr;
}

// ============================================================
//  PUBLIC: Add functions (append new records)
// ============================================================
bool FileHandler::addPatient(const patient& p)
{
    char line[512];
    patientToCSV(p, line, 512);
    return appendLine("patient.txt", line);
}

bool FileHandler::addDoctor(const doctor& d)
{
    char line[512];
    doctorToCSV(d, line, 512);
    return appendLine("doctor.txt", line);
}

bool FileHandler::addAppointment(const appointment& a)
{
    char line[256];
    appointmentToCSV(a, line, 256);
    return appendLine("appointments.txt", line);
}

bool FileHandler::addPrescription(const prescription& p)
{
    char line[1024];
    prescriptionToCSV(p, line, 1024);
    return appendLine("prescriptions.txt", line);
}

bool FileHandler::addBill(const bill& b)
{
    char line[256];
    billToCSV(b, line, 256);
    return appendLine("bills.txt", line);
}

// ============================================================
//  PUBLIC: Update functions
//  Rewrites the whole file, replacing the matching record.
// ============================================================
// Generic helper: loads all lines, replaces the one matching id in column idCol
static bool genericUpdate(const char* filename, int id, int idCol,
    const char* newLine)
{
    ifstream file(filename);
    if (!file.is_open()) return false;

    // count lines
    char buf[4096];
    int  total = 0;
    while (file.getline(buf, 4096))
        if (myStrLen(buf) > 0) total++;
    file.close();

    if (total == 0) return false;

    // read all lines into dynamic array
    char** lines = new char* [total];
    int lineCount = 0;

    ifstream f2(filename);
    while (f2.getline(buf, 4096))
    {
        if (myStrLen(buf) == 0) continue;
        int len = myStrLen(buf);
        lines[lineCount] = new char[len + 1];
        myStrCopy(lines[lineCount], buf, len + 1);
        lineCount++;
    }
    f2.close();

    // find and replace the matching line
    char fieldBuf[20];
    bool found = false;
    for (int i = 0; i < lineCount; i++)
    {
        // skip header
        if (lines[i][0] < '0' || lines[i][0] > '9') continue;

        // extract the ID field
        int cur = 0;
        int field = 0;
        int pos = 0;
        char tmp[20];
        tmp[0] = '\0';
        for (int c = 0; lines[i][c] != '\0'; c++)
        {
            if (lines[i][c] == ',') { field++; }
            else if (field == idCol && pos < 19)
                tmp[pos++] = lines[i][c];
        }
        tmp[pos] = '\0';

        int lineId = 0;
        for (int c = 0; tmp[c] != '\0'; c++)
            if (tmp[c] >= '0' && tmp[c] <= '9')
                lineId = lineId * 10 + (tmp[c] - '0');

        if (lineId == id)
        {
            delete[] lines[i];
            int nLen = myStrLen(newLine);
            lines[i] = new char[nLen + 1];
            myStrCopy(lines[i], newLine, nLen + 1);
            found = true;
            break;
        }
    }

    bool ok = false;
    if (found)
    {
        ofstream out(filename);
        if (out.is_open())
        {
            for (int i = 0; i < lineCount; i++)
                out << lines[i] << "\n";
            out.close();
            ok = true;
        }
    }

    for (int i = 0; i < lineCount; i++) delete[] lines[i];
    delete[] lines;
    return ok;
}

bool FileHandler::updatePatient(const patient& p)
{
    char line[512];
    patientToCSV(p, line, 512);
    return genericUpdate("patient.txt", p.getId(), 0, line);
}

bool FileHandler::updateAppointment(const appointment& a)
{
    char line[256];
    appointmentToCSV(a, line, 256);
    return genericUpdate("appointments.txt", a.getAppointmentID(), 0, line);
}

bool FileHandler::updateBill(const bill& b)
{
    char line[256];
    billToCSV(b, line, 256);
    return genericUpdate("bills.txt", b.getBillId(), 0, line);
}

bool FileHandler::updateDoctor(const doctor& d)
{
    char line[512];
    doctorToCSV(d, line, 512);
    return genericUpdate("doctor.txt", d.getId(), 0, line);
}

// ============================================================
//  Generic delete helper
//  Removes the line whose field idCol equals id.
// ============================================================
static bool genericDelete(const char* filename, int id, int idCol)
{
    ifstream file(filename);
    if (!file.is_open()) return false;

    char buf[4096];
    int  total = 0;
    while (file.getline(buf, 4096))
        if (myStrLen(buf) > 0) total++;
    file.close();

    if (total == 0) return false;

    char** lines = new char* [total];
    int    lineCount = 0;

    ifstream f2(filename);
    while (f2.getline(buf, 4096))
    {
        if (myStrLen(buf) == 0) continue;
        int len = myStrLen(buf);
        lines[lineCount] = new char[len + 1];
        myStrCopy(lines[lineCount], buf, len + 1);
        lineCount++;
    }
    f2.close();

    // build output without the matching line
    ofstream out(filename);
    bool found = false;
    for (int i = 0; i < lineCount; i++)
    {
        // keep header or non-matching lines
        if (lines[i][0] < '0' || lines[i][0] > '9')
        {
            out << lines[i] << "\n";
            delete[] lines[i];
            continue;
        }

        // extract id field
        int field = 0, pos = 0;
        char tmp[20]; tmp[0] = '\0';
        for (int c = 0; lines[i][c] != '\0'; c++)
        {
            if (lines[i][c] == ',') field++;
            else if (field == idCol && pos < 19)
                tmp[pos++] = lines[i][c];
        }
        tmp[pos] = '\0';

        int lineId = 0;
        for (int c = 0; tmp[c] != '\0'; c++)
            if (tmp[c] >= '0' && tmp[c] <= '9')
                lineId = lineId * 10 + (tmp[c] - '0');

        if (lineId == id && !found)
        {
            found = true; // skip this line (don't write it)
        }
        else
        {
            out << lines[i] << "\n";
        }
        delete[] lines[i];
    }
    out.close();
    delete[] lines;
    return found;
}

bool FileHandler::deleteDoctor(int doctorId)
{
    return genericDelete("doctor.txt", doctorId, 0);
}

bool FileHandler::deletePatientRecord(int patientId)
{
    return genericDelete("patient.txt", patientId, 0);
}

bool FileHandler::deleteAppointmentsByPatient(int patientId)
{
    // must remove ALL appointments for this patient
    // read all, write back only those that don't match patientId in col 1
    ifstream file("appointments.txt");
    if (!file.is_open()) return false;

    char buf[4096];
    int  total = 0;
    while (file.getline(buf, 4096))
        if (myStrLen(buf) > 0) total++;
    file.close();

    char** lines = new char* [total];
    int lineCount = 0;

    ifstream f2("appointments.txt");
    while (f2.getline(buf, 4096))
    {
        if (myStrLen(buf) == 0) continue;
        int len = myStrLen(buf);
        lines[lineCount] = new char[len + 1];
        myStrCopy(lines[lineCount], buf, len + 1);
        lineCount++;
    }
    f2.close();

    ofstream out("appointments.txt");
    for (int i = 0; i < lineCount; i++)
    {
        if (lines[i][0] < '0' || lines[i][0] > '9')
        {
            out << lines[i] << "\n";
            delete[] lines[i];
            continue;
        }

        // patient id is field 1
        int field = 0, pos = 0;
        char tmp[20]; tmp[0] = '\0';
        for (int c = 0; lines[i][c] != '\0'; c++)
        {
            if (lines[i][c] == ',') field++;
            else if (field == 1 && pos < 19)
                tmp[pos++] = lines[i][c];
        }
        tmp[pos] = '\0';

        int pid = 0;
        for (int c = 0; tmp[c] != '\0'; c++)
            if (tmp[c] >= '0' && tmp[c] <= '9')
                pid = pid * 10 + (tmp[c] - '0');

        if (pid != patientId)
            out << lines[i] << "\n";
        delete[] lines[i];
    }
    out.close();
    delete[] lines;
    return true;
}

bool FileHandler::deleteBillsByPatient(int patientId)
{
    ifstream file("bills.txt");
    if (!file.is_open()) return false;

    char buf[4096];
    int total = 0;
    while (file.getline(buf, 4096))
        if (myStrLen(buf) > 0) total++;
    file.close();

    char** lines = new char* [total];
    int lineCount = 0;

    ifstream f2("bills.txt");
    while (f2.getline(buf, 4096))
    {
        if (myStrLen(buf) == 0) continue;
        int len = myStrLen(buf);
        lines[lineCount] = new char[len + 1];
        myStrCopy(lines[lineCount], buf, len + 1);
        lineCount++;
    }
    f2.close();

    ofstream out("bills.txt");
    for (int i = 0; i < lineCount; i++)
    {
        if (lines[i][0] < '0' || lines[i][0] > '9')
        {
            out << lines[i] << "\n";
            delete[] lines[i];
            continue;
        }

        // patient id is field 1
        int field = 0, pos = 0;
        char tmp[20]; tmp[0] = '\0';
        for (int c = 0; lines[i][c] != '\0'; c++)
        {
            if (lines[i][c] == ',') field++;
            else if (field == 1 && pos < 19)
                tmp[pos++] = lines[i][c];
        }
        tmp[pos] = '\0';

        int pid = 0;
        for (int c = 0; tmp[c] != '\0'; c++)
            if (tmp[c] >= '0' && tmp[c] <= '9')
                pid = pid * 10 + (tmp[c] - '0');

        if (pid != patientId)
            out << lines[i] << "\n";
        delete[] lines[i];
    }
    out.close();
    delete[] lines;
    return true;
}

bool FileHandler::deletePrescriptionsByPatient(int patientId)
{
    ifstream file("prescriptions.txt");
    if (!file.is_open()) return false;

    char buf[4096];
    int total = 0;
    while (file.getline(buf, 4096))
        if (myStrLen(buf) > 0) total++;
    file.close();

    char** lines = new char* [total];
    int lineCount = 0;

    ifstream f2("prescriptions.txt");
    while (f2.getline(buf, 4096))
    {
        if (myStrLen(buf) == 0) continue;
        int len = myStrLen(buf);
        lines[lineCount] = new char[len + 1];
        myStrCopy(lines[lineCount], buf, len + 1);
        lineCount++;
    }
    f2.close();

    ofstream out("prescriptions.txt");
    for (int i = 0; i < lineCount; i++)
    {
        if (lines[i][0] < '0' || lines[i][0] > '9')
        {
            out << lines[i] << "\n";
            delete[] lines[i];
            continue;
        }

        // patient id is field 2
        int field = 0, pos = 0;
        char tmp[20]; tmp[0] = '\0';
        for (int c = 0; lines[i][c] != '\0'; c++)
        {
            if (lines[i][c] == ',') field++;
            else if (field == 2 && pos < 19)
                tmp[pos++] = lines[i][c];
        }
        tmp[pos] = '\0';

        int pid = 0;
        for (int c = 0; tmp[c] != '\0'; c++)
            if (tmp[c] >= '0' && tmp[c] <= '9')
                pid = pid * 10 + (tmp[c] - '0');

        if (pid != patientId)
            out << lines[i] << "\n";
        delete[] lines[i];
    }
    out.close();
    delete[] lines;
    return true;
}

// ============================================================
//  PUBLIC: archiveAndDischargePatient
//  Copies patient + all related records to discharged.txt,
//  then removes them from active files.
// ============================================================
bool FileHandler::archiveAndDischargePatient(int patientId,
    Storage<patient>& patients,
    Storage<appointment>& appts,
    Storage<bill>& bills,
    Storage<prescription>& prescriptions)
{
    // find the patient
    patient* p = patients.findByID(patientId);
    if (!p) return false;

    // write patient row to discharged.txt
    char pLine[512];
    patientToCSV(*p, pLine, 512);
    appendLine("discharged.txt", pLine);

    // write all their appointments to discharged.txt
    for (int i = 0; i < appts.size(); i++)
    {
        if (appts.getAll()[i].getPatientID() == patientId)
        {
            char aLine[256];
            appointmentToCSV(appts.getAll()[i], aLine, 256);
            appendLine("discharged.txt", aLine);
        }
    }

    // write all their bills
    for (int i = 0; i < bills.size(); i++)
    {
        if (bills.getAll()[i].getPatientId() == patientId)
        {
            char bLine[256];
            billToCSV(bills.getAll()[i], bLine, 256);
            appendLine("discharged.txt", bLine);
        }
    }

    // write all their prescriptions
    for (int i = 0; i < prescriptions.size(); i++)
    {
        if (prescriptions.getAll()[i].getPatientId() == patientId)
        {
            char rxLine[1024];
            prescriptionToCSV(prescriptions.getAll()[i], rxLine, 1024);
            appendLine("discharged.txt", rxLine);
        }
    }

    // now delete from active files
    deletePatientRecord(patientId);
    deleteAppointmentsByPatient(patientId);
    deleteBillsByPatient(patientId);
    deletePrescriptionsByPatient(patientId);

    return true;
}

// ============================================================
//  PUBLIC: logSecurityEvent
//  Appends a failed/locked login attempt to security_log.txt
// ============================================================
void FileHandler::logSecurityEvent(const char* role, int enteredId,
    const char* result)
{
    // get current timestamp using ctime
    time_t now = time(nullptr);
    char   timeBuf[32];
    struct tm* t = localtime(&now);
    // format: DD-MM-YYYY HH:MM:SS
    // manually build it
    char day[3], mon[3], yr[5], hr[3], min[3], sec[3];
    intToCharArr(t->tm_mday, day);
    intToCharArr(t->tm_mon + 1, mon);
    intToCharArr(t->tm_year + 1900, yr);
    intToCharArr(t->tm_hour, hr);
    intToCharArr(t->tm_min, min);
    intToCharArr(t->tm_sec, sec);

    // pad single digits with 0
    char dayP[3], monP[3], hrP[3], minP[3], secP[3];
    if (myStrLen(day) == 1) { dayP[0] = '0'; dayP[1] = day[0]; dayP[2] = '\0'; }
    else myStrCopy(dayP, day, 3);
    if (myStrLen(mon) == 1) { monP[0] = '0'; monP[1] = mon[0]; monP[2] = '\0'; }
    else myStrCopy(monP, mon, 3);
    if (myStrLen(hr) == 1) { hrP[0] = '0';  hrP[1] = hr[0];  hrP[2] = '\0'; }
    else myStrCopy(hrP, hr, 3);
    if (myStrLen(min) == 1) { minP[0] = '0'; minP[1] = min[0]; minP[2] = '\0'; }
    else myStrCopy(minP, min, 3);
    if (myStrLen(sec) == 1) { secP[0] = '0'; secP[1] = sec[0]; secP[2] = '\0'; }
    else myStrCopy(secP, sec, 3);

    timeBuf[0] = '\0';
    myStrCat(timeBuf, dayP, 32); myStrCat(timeBuf, "-", 32);
    myStrCat(timeBuf, monP, 32); myStrCat(timeBuf, "-", 32);
    myStrCat(timeBuf, yr, 32); myStrCat(timeBuf, " ", 32);
    myStrCat(timeBuf, hrP, 32); myStrCat(timeBuf, ":", 32);
    myStrCat(timeBuf, minP, 32); myStrCat(timeBuf, ":", 32);
    myStrCat(timeBuf, secP, 32);

    char idBuf[12];
    intToCharArr(enteredId, idBuf);

    // build the CSV line: timestamp,role,enteredId,result
    char line[256];
    line[0] = '\0';
    myStrCat(line, timeBuf, 256);
    myStrCat(line, ",", 256);
    myStrCat(line, role, 256);
    myStrCat(line, ",", 256);
    myStrCat(line, idBuf, 256);
    myStrCat(line, ",", 256);
    myStrCat(line, result, 256);

    appendLine("security_log.txt", line);
}