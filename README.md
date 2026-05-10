# Project2
# MediCore Hospital Management System

**Course:** Object Oriented Programming (OOP)  
**Semester:** Spring 2026  
**Section:** BCS-2B  

---

## What Is This Project?

This is a Hospital Management System built in C++ with a graphical window (using SFML).  
It lets three types of users log in — **Patient**, **Doctor**, and **Admin** — and do things like book appointments, write prescriptions, and manage hospital records.

All data is saved in `.txt` files so nothing is lost when you close and reopen the program.

---

## Files in This Project

| File | What it does |
|------|--------------|
| `Main.cpp` | Starts the program and runs the GUI window |
| `person.h / .cpp` | Base class — shared data for Patient, Doctor, Admin |
| `patient.h / .cpp` | Patient class with balance operators |
| `doctor.h / .cpp` | Doctor class |
| `Admin.h / admin.cpp` | Admin class |
| `appointment.h / .cpp` | Appointment class with conflict check operator |
| `Bill.h / bill.cpp` | Bill class |
| `prescription.h / .cpp` | Prescription class |
| `storage.h` | Template storage class (holds up to 100 objects) |
| `filehandler.h / .cpp` | Reads and writes all `.txt` files |
| `Validator.h / .cpp` | Validates all user input |
| `exception.h / .cpp` | Custom exception classes |
| `string.h / .cpp` | Custom string functions (no std::string used) |
| `Astro.ttf` | Font file used in the window |
| `patient.txt` | Stores patient data |
| `doctor.txt` | Stores doctor data |
| `admin.txt` | Stores admin login |
| `appointments.txt` | Stores all appointments |
| `bills.txt` | Stores all bills |
| `prescriptions.txt` | Stores all prescriptions |
| `security_log.txt` | Stores failed login attempts |
| `discharged.txt` | Stores records of discharged patients |

---

## What You Need to Compile

You need:
- A C++ compiler (g++ recommended)
- SFML 2.6 installed on your computer

### How to Install SFML on Windows (MinGW)

1. Go to https://www.sfml-dev.org/download.php
2. Download **SFML 2.6** for your compiler (MinGW 64-bit)
3. Extract it somewhere, for example `C:\SFML`

---

## How to Compile

### On Windows (using g++ in terminal / command prompt)

Open terminal in the project folder and run this command:

## Run

Place all `.txt` data files and `Astro.ttf` in the same folder, then run `MediCore.exe`.

## Login Credentials

- Patient: ID 1, password pass123  
- Doctor: ID 1, password doc456  
- Admin: ID 1, password admin123  

## GitHub

https://github.com/l250806-sudo/l250806.hospital-managment-system.git
