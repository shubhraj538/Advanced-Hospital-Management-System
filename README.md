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

    main.c           main menu (patient / doctor / admin) + patient dashboard
    HIMS.h           structures, arrays, and the list of the other files
    common.c         input helpers, dates, time slots, reading/saving the text files
    waitlist.c       Part 19 - waiting list (+ cancelrecord, the one place that cancels)
    notifications.c  Part 10 - notifications
    patient.c        Part 1  - patient account + find a doctor
    appointments.c   Part 2  - book, cancel, reschedule, view
    payment.c        Part 3  - simulated payment
    reviews.c        Part 4  - ratings and reviews
    receipt.c        Part 9  - digital receipt
    records.c        Part 8  - patient records
    doctor.c         Part 5  - doctor side
    stats.c          Part 7  - statistics
    reports.c        Part 20 - reports
    admin.c          Part 6  - admin side

Appointment flow:
    Pending (booked) -> Confirmed (payment verified) -> Completed (doctor)
    Cancelled can happen from Pending or Confirmed (patient, doctor or admin).
    Payment status: Pending / Verified / Failed.

Text files (doctors.txt is included, the others are made automatically):
    doctors.txt  pendingdoctors.txt  patients.txt  appointments.txt
    reviews.txt  waitlist.txt  categories.txt


---

## 🔒 Safety & Validation

* Strict role separation for Patient, Doctor, and Admin views
* Stdin buffer clearing to eliminate terminal loops and menu crashes
* Uniqueness checks on account IDs, doctor profiles, and payment refs

---

## ⚡ Compilation & Run

Compile using GCC:

```bash
    gcc main.c -o hims
    ./hims            (on Windows: hims.exe)
