/*
 * HIMS - Hospital Information Management System
 * PART 8 : PATIENT RECORDS
 */

/* patient ID, name, age, gender, contact and some totals */
void viewrecord(int p)
{
    char gender[3][8] = {"Male", "Female", "Other"};
    int i, total = 0, completed = 0, cancelled = 0, spent = 0;

    for (i = 0; i < appointmentcount; i++) {
        if (appointments[i].patientid == patients[p].id) {
            total++;
            if (strcmp(appointments[i].status, "Completed") == 0)
                completed++;
            if (strcmp(appointments[i].status, "Cancelled") == 0)
                cancelled++;
            if (isrevenue(i))
                spent = spent + appointments[i].fee;
        }
    }
    printf("\n========== PATIENT RECORD ==========\n");
    printf("Patient ID : %d\nName       : %s\n", patients[p].id, patients[p].name);
    printf("Age        : %d\nGender     : %s\n", patients[p].age, gender[patients[p].gender - 1]);
    printf("Contact    : %s\n", patients[p].phone);
    printf("\nTotal appointments : %d\nCompleted          : %d\n", total, completed);
    printf("Cancelled          : %d\nTotal paid         : Rs. %d\n", cancelled, spent);
}

/* date, doctor, specialization, status and fee of the appointments of one patient.
 * pastonly = 1 shows only the past ones (completed, cancelled or the date is over) */
void viewhistory(int patientid, int pastonly)
{
    int i, d, found = 0;

    printf("\n%-5s %-11s %-9s %-18s %-14s %-10s %s\n",
           "ID", "Date", "Time", "Doctor", "Specialization", "Status", "Fee");
    printf("---------------------------------------------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (appointments[i].patientid == patientid && !(pastonly && isupcoming(i))) {
            d = finddoctorindex(appointments[i].doctorid);
            printf("%-5s %-11s %-9s %-18s %-14s %-10s %d\n", appointments[i].id,
                   appointments[i].date, appointments[i].time, appointments[i].doctorname,
                   d == -1 ? "-" : doctors[d].specialization, appointments[i].status,
                   appointments[i].fee);
            found++;
        }
    }
    if (found == 0)
        printf("No appointments found.\n");
}
