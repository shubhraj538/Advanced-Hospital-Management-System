/*
 * HIMS - Hospital Information Management System
 * PART 9 : DIGITAL RECEIPT
 */

/* A receipt is only for appointments with a verified payment (Confirmed or Completed). */
int ispaid(int i)
{
    return strcmp(appointments[i].payment, "Verified") == 0 &&
           (strcmp(appointments[i].status, "Confirmed") == 0 ||
            strcmp(appointments[i].status, "Completed") == 0);
}

/* lists the paid appointments of one patient, returns how many */
int showpaid(int patientid)
{
    int i, found = 0;

    printf("\n%-5s %-18s %-11s %-9s %s\n", "ID", "Doctor", "Date", "Time", "Fee");
    printf("-------------------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (appointments[i].patientid == patientid && ispaid(i)) {
            printf("%-5s %-18s %-11s %-9s %d\n", appointments[i].id,
                   appointments[i].doctorname, appointments[i].date,
                   appointments[i].time, appointments[i].fee);
            found++;
        }
    }
    if (found == 0)
        printf("You have no paid appointments yet.\n");
    return found;
}

void printreceipt(int a, int p)
{
    char months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char spec[20] = "-", status[15], payment[15];
    int d, m, y, i = finddoctorindex(appointments[a].doctorid);

    if (i != -1)
        strcpy(spec, doctors[i].specialization);
    strcpy(status, appointments[a].status);
    strcpy(payment, appointments[a].payment);
    touppercase(status);
    touppercase(payment);
    sscanf(appointments[a].date, "%d-%d-%d", &d, &m, &y);     /* 25-11-2026 becomes 25 Nov 2026 */

    printf("\n================================\n");
    printf("       APPOINTMENT RECEIPT\n");
    printf("================================\n");
    printf("Appointment ID : %s\n", appointments[a].id);
    printf("Patient        : %s\n", patients[p].name);
    printf("Doctor         : %s\n", appointments[a].doctorname);
    printf("Specialization : %s\n", spec);
    printf("Date           : %d %s %d\n", d, months[m - 1], y);
    printf("Time           : %s\n", appointments[a].time);
    printf("Consultation   : Rs. %d\n", appointments[a].fee);
    printf("Payment        : %s\n", payment);
    printf("Status         : %s\n", status);
    printf("================================\n");
}

void getreceipt(int p)
{
    char id[10];
    int a;

    if (showpaid(patients[p].id) == 0)
        return;
    printf("\nEnter Appointment ID: ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);

    a = findappointment(id, patients[p].id);
    if (a == -1 || !ispaid(a))
        printf("No paid appointment with this ID.\n");
    else
        printreceipt(a, p);
}
