/*
 * HIMS - Hospital Information Management System
 * PART 3 : PAYMENT SYSTEM (simulated, no real bank)
 * Flow:  Payment -> Verification -> Confirm Appointment
 * Payment status: Pending / Verified / Failed
 * If the payment fails 3 times, the appointment is cancelled automatically.
 */

/* A dummy transaction number is "valid" if it has exactly 8 digits */
int isvalidtransaction(char t[])
{
    int i;
    if (strlen(t) != 8)
        return 0;
    for (i = 0; i < 8; i++) {
        if (t[i] < '0' || t[i] > '9')
            return 0;
    }
    return 1;
}

/* returns 1 if this transaction number was already used before */
int transactionused(char t[])
{
    int i;
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].transaction, t) == 0)
            return 1;
    }
    return 0;
}

/* shows all appointments of one patient, returns how many were found */
int showmyappointments(int patientid)
{
    int i, found = 0;

    printf("\n%-5s %-18s %-11s %-9s %-8s %-10s %s\n",
           "ID", "Doctor", "Date", "Time", "Fee", "Status", "Payment");
    printf("--------------------------------------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (appointments[i].patientid == patientid) {
            printf("%-5s %-18s %-11s %-9s %-8d %-10s %s\n",
                   appointments[i].id, appointments[i].doctorname, appointments[i].date,
                   appointments[i].time, appointments[i].fee, appointments[i].status,
                   appointments[i].payment);
            found++;
        }
    }
    if (found == 0)
        printf("You have no appointments.\n");
    return found;
}

/* Payment -> verification -> appointment Confirmed. The patient gets 3 tries. */
void makepayment(int patientid)
{
    char id[10], t[20];
    char methods[4][15] = {"JazzCash", "EasyPaisa", "Bank", "Card"};
    int i, attempt;

    if (showmyappointments(patientid) == 0)
        return;
    printf("\nEnter Appointment ID to pay for (e.g. A1): ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);

    i = findappointment(id, patientid);
    if (i == -1) {
        printf("Appointment not found.\n");
        return;
    }
    if (strcmp(appointments[i].status, "Pending") != 0) {
        printf("This appointment is %s, so it cannot be paid now.\n", appointments[i].status);
        return;
    }

    printf("\n========== PAYMENT ==========\n");
    printf("Doctor           : %s\n", appointments[i].doctorname);
    printf("Date / Time      : %s  %s\n", appointments[i].date, appointments[i].time);
    printf("Consultation Fee : Rs. %d\n", appointments[i].fee);
    printf("Payment Status   : %s\n", appointments[i].payment);
    printf("\nPayment method:\n1. JazzCash\n2. EasyPaisa\n3. Bank transfer\n4. Card\n");
    strcpy(appointments[i].method, methods[getrange("Choose: ", 1, 4) - 1]);

    for (attempt = 1; attempt <= 3; attempt++) {    /* 3 tries, after that the appointment is cancelled */
        printf("\nEnter transaction/reference number (8 digits): ");
        scanf("%19s", t);
        clearinput();
        printf("Verifying payment...\n");

        if (isvalidtransaction(t) && !transactionused(t)) {
            strcpy(appointments[i].transaction, t);
            strcpy(appointments[i].payment, "Verified");
            strcpy(appointments[i].status, "Confirmed");
            saveappointments();
            printf("\nPayment Status : VERIFIED\nAppointment %s is now CONFIRMED.\n", id);
            return;
        }
        if (isvalidtransaction(t))
            printf("Payment FAILED: this transaction number was already used.\n");
        else
            printf("Payment FAILED: invalid transaction number.\n");
        printf("Attempts left: %d\n", 3 - attempt);
        if (feof(stdin))
            break;
    }

    strcpy(appointments[i].payment, "Failed");
    cancelrecord(i);
    printf("\nPayment Status : FAILED\nAppointment %s has been CANCELLED automatically.\n", id);
}

void paymentstatus(int patientid)
{
    char id[10];
    int i;

    if (showmyappointments(patientid) == 0)
        return;
    printf("\nEnter Appointment ID to see payment details: ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);

    i = findappointment(id, patientid);
    if (i == -1) {
        printf("Appointment not found.\n");
        return;
    }
    printf("\n========== PAYMENT DETAILS ==========\n");
    printf("Appointment ID     : %s\n", appointments[i].id);
    printf("Doctor             : %s\n", appointments[i].doctorname);
    printf("Consultation Fee   : Rs. %d\n", appointments[i].fee);
    printf("Payment Method     : %s\n", appointments[i].method);
    printf("Transaction Number : %s\n", appointments[i].transaction);
    printf("Payment Status     : %s\n", appointments[i].payment);
    printf("Appointment Status : %s\n", appointments[i].status);
}
