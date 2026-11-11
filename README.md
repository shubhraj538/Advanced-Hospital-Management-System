🏥 Hospital & Appointment Management System (C CLI)
A CLI-based Hospital Informlti-role authentication, slot lockouts, simulated billing verification, and dynamic FIFO waitlists.

📌 Key Modules & Features

👤 1. Patient Portal
Account Control: Register, authenticate, update profiles (age, gender, contact), and change passwords.
Doctor Discovery: Filter doctors by name, specialty, fee, ratings, or real-time schedule availability.
Smart Booking Engine: Real-time time-slot allocation (09:00 AM, 09:30 AM). Immediate slot locking prevents double booking.
Appointment Management: Track bookings via unique AXXXXX IDs, reschedule, or cancel anytime.
Feedback System: Submit 1–5 star reviews post-appointment to dynamically recalculate doctor ratings.

💳 2. Payment & Verification Engine
Simulated Billing Flow: Pending ➔ Transaction Reference Entry ➔ Verification ➔ Confirmed.
Auto-Cancellation: Invalidates reserved slots if payment verification fails.
Digital Receipt Generator: Outputs formatted ASCII receipts:
================================================
              APPOINTMENT RECEIPT               
================================================
 Appointment ID : A10234                        
 Patient        : Ali                           
 Doctor         : Dr. Ahmed (Cardiologist)      
 Date & Time    : 18 Sept 2026 @ 10:30 AM       
 Consultation   : Rs. 1,500                     
 Payment Status : VERIFIED                      
 Booking Status : CONFIRMED                     
================================================


⏳ 3. Dynamic Queue & Automatic Waitlist
Slot Interception: Prompts patients to join a FIFO priority queue when target slots are full.
Auto-Reassignment: When a booking is cancelled, the system automatically pulls the top patient from waitlist.txt and assigns the freed slot.

👨‍⚕️ 4. Doctor Dashboard
Schedule Controls: Configure active workdays, dynamic time slots, and consultation rates.
Queue Management: Accept/reject incoming booking requests and view patient medical visit histories.

🏥 5. Admin Panel & System Analytics
Hospital CRUD: Add, update, remove, or verify doctors and clinical categories.
System Diagnostics:
===== SYSTEM STATISTICS =====
Total Patients        : 250
Total Doctors         : 42
Appointments Today    : 37
Completed             : 25
Cancelled             : 5
Pending               : 7
Total Revenue         : Rs. 56,500
==================================
Performance Insights: Identifies top-rated doctors, high-cancellation slots, popular specialties, and revenue metrics.
Report Exporting: Generates offline .txt reports for monthly appointments, revenue audits, and registration trends.

📁 File Structure & Data Storage
State management is handled via native C File I/O (stdio.h) using flat file persistence:
├── base.c              # Core UI menu, data models, and analytics routines
├── login.c             # Multi-role authentication & session handling
├── patients.txt        # Encoded patient profiles and credentials
├── doctors.txt         # Doctor details, schedules, rates, and stats
├── appointments.txt    # Central booking ledger
├── payments.txt        # Transaction reference logs
├── reviews.txt         # Review logs and doctor rating metrics
└── waitlist.txt        # Sequential queue buffers for filled slots

🔒 System Guardrails & Security
Role-Based Access Control (RBAC): Strict menu boundaries for Patient, Doctor, and Admin sessions.
Input Stream Sanitization: Clears stdin buffers to prevent terminal loops, menu crashes, or string overflow exploits.
Primary Key Enforcement: Validates uniqueness across Patient IDs, Doctor IDs, and Payment Reference keys.

⚡ Compilation & Execution
# Compile
gcc base.c login.c -o hospital_sys

# Run
./hospital_sys
