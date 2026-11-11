# 🏥 Hospital & Appointment Management System (C CLI)

A CLI-based Hospital Information System written in C. Features multi-role access, real-time slot lockouts, payment verification, automatic waitlists, dynamic doctor ratings, and local text file storage.

---

## 📌 Features

### 👤 Patient Portal
* Register, log in, update info, and change passwords
* Search & filter doctors by name, specialty, fee, rating, or availability
* Dynamic time-slot reservation (09:00 AM, 09:30 AM, etc.) with double-booking locks
* Reschedule, cancel, or track visits using Appointment IDs (AXXXXX)
* Rate doctors (1-5 stars) and write reviews post-consultation

### 💳 Payment Verification Module
* Simulated status flow: Pending -> Enter Ref No. -> Verification -> Confirmed
* Auto-cancels appointment and releases slot if verification fails
* Generates clear printable ASCII receipts upon confirmation

### ⏳ Queue & Auto Waitlist
* Prompt to join a priority FIFO waitlist when slots are full
* Automatically assigns freed slot to top waitlisted patient upon cancellation

### 👨‍⚕️ Doctor Portal
* Set working days, time slots, and consultation fees
* Review incoming requests, accept/reject visits, and view patient history

### 🏥 Admin Dashboard & Statistics
* CRUD management for doctors and clinical categories
* Real-time metrics: total revenue, completed/pending/cancelled visits
* System insights: top-rated doctors, high-cancellation slots, popular categories
* Export offline reports for monthly metrics and revenue audits

---

## 📁 File Architecture

All data persists in local flat text files via standard C File I/O (stdio.h):

* `base.c` - Core menus, business logic, and analytics
* `login.c` - Role authentication and session handling
* `patients.txt` - Patient accounts and personal records
* `doctors.txt` - Doctor profiles, rates, and schedule slots
* `appointments.txt` - Central booking records
* `payments.txt` - Payment transaction references
* `reviews.txt` - Doctor ratings and feedback logs
* `waitlist.txt` - Sequential queue data for filled slots

---

## 🔒 Safety & Validation

* Strict role separation for Patient, Doctor, and Admin views
* Stdin buffer clearing to eliminate terminal loops and menu crashes
* Uniqueness checks on account IDs, doctor profiles, and payment refs

---

## ⚡ Compilation & Run

Compile using GCC:

```bash
gcc base.c login.c -o hospital_sys
./hospital_sys
