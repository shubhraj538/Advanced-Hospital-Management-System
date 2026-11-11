/*
 * HIMS - Hospital Information Management System
 * PART 3 : PAYMENT SYSTEM (simulated, no real bank)
 *
 * Flow:  Payment -> Verification -> Confirm Appointment
 * Payment status    : Pending / Verified / Failed
 * If payment fails 3 times, the appointment is cancelled automatically.
 *
 * appointments.txt -> appointment records (already written)
 * Each line: appointmentID patientID doctorID fee date time
 *            appointmentStatus paymentStatus method transactionNo doctorName
 */

#include <stdio.h>
#include <string.h>

/* ===================== STRUCTURE ===================== */

struct appointment {
    char id[10];
    int  patientid;
    char doctorid[10];
    int  fee;
    char date[15];
    char time[10];
    char status[15];         /* Pending / Confirmed / Cancelled */
    char payment[15];        /* Pending / Verified / Failed */
    char method[15];         /* JazzCash / EasyPaisa / Bank / Card */
    char transaction[20];    /* the reference number the patient typed */
    char doctorname[50];
};

struct appointment appointments[100];
int appointmentcount = 0;

/* ===================== INPUT HELPERS ===================== */

/* Throws away everything left on the line (up to Enter) */
void clearinput()
{
    int c = getchar();
    while (c != '\n' && c != EOF)
        c = getchar();
}

/* Reads a number. If the user types letters, it asks again. */
int getnumber()
{
    int n;
    while (scanf("%d", &n) != 1) {
        if (feof(stdin))
            return 0;
        printf("Please enter a number: ");
        clearinput();
    }
    clearinput();
    return n;
}

/* ===================== FILES ===================== */

void loadappointments()
{
    FILE *fp = fopen("appointments.txt", "r");
    struct appointment a;
    int len;

    if (fp == NULL) {
        printf("Error: appointments.txt not found!\n");
        return;
    }

    appointmentcount = 0;
    while (fscanf(fp, "%s %d %s %d %s %s %s %s %s %s ",
                  a.id, &a.patientid, a.doctorid, &a.fee, a.date, a.time,
                  a.status, a.payment, a.method, a.transaction) == 10) {
        fgets(a.doctorname, 50, fp);        /* rest of the line is the name */
        len = strlen(a.doctorname);
        if (len > 0 && a.doctorname[len - 1] == '\n')
            a.doctorname[len - 1] = '\0';
        appointments[appointmentcount] = a;
        appointmentcount++;
    }
    fclose(fp);
}

void saveappointments()
{
    FILE *fp = fopen("appointments.txt", "w");
    int i;

    for (i = 0; i < appointmentcount; i++) {
        fprintf(fp, "%s %d %s %d %s %s %s %s %s %s %s\n",
                appointments[i].id, appointments[i].patientid,
                appointments[i].doctorid, appointments[i].fee,
                appointments[i].date, appointments[i].time,
                appointments[i].status, appointments[i].payment,
                appointments[i].method, appointments[i].transaction,
                appointments[i].doctorname);
    }
    fclose(fp);
}

/* ===================== CHECKS ===================== */

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

/* ===================== SHOW ===================== */

/* shows all appointments of one patient, returns how many were found */
int showmyappointments(int patientid)
{
    int i, found = 0;

    printf("\n%-5s %-18s %-11s %-8s %-8s %-10s %s\n",
           "ID", "Doctor", "Date", "Time", "Fee", "Status", "Payment");
    printf("---------------------------------------------------------------------\n");

    for (i = 0; i < appointmentcount; i++) {
        if (appointments[i].patientid == patientid) {
            printf("%-5s %-18s %-11s %-8s %-8d %-10s %s\n",
                   appointments[i].id, appointments[i].doctorname,
                   appointments[i].date, appointments[i].time,
                   appointments[i].fee, appointments[i].status,
                   appointments[i].payment);
            found++;
        }
    }
    if (found == 0)
        printf("You have no appointments.\n");
    return found;
}

/* ===================== PAYMENT ===================== */

void makepayment(int patientid)
{
    char id[10], t[50];
    char methods[4][15] = {"JazzCash", "EasyPaisa", "Bank", "Card"};
    int i, choice, attempt;

    if (showmyappointments(patientid) == 0)
        return;

    printf("\nEnter Appointment ID to pay for (e.g. A1): ");
    scanf("%s", id);
    clearinput();

    /* find the appointment, it must belong to this patient */
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].id, id) == 0 &&
            appointments[i].patientid == patientid)
            break;
    }
    if (i == appointmentcount) {
        printf("Appointment not found.\n");
        return;
    }

    if (strcmp(appointments[i].payment, "Verified") == 0) {
        printf("This appointment is already paid.\n");
        return;
    }
    if (strcmp(appointments[i].status, "Cancelled") == 0) {
        printf("This appointment was cancelled. You cannot pay for it.\n");
        return;
    }

    printf("\n========== PAYMENT ==========\n");
    printf("Doctor           : %s\n", appointments[i].doctorname);
    printf("Date / Time      : %s  %s\n", appointments[i].date, appointments[i].time);
    printf("Consultation Fee : Rs. %d\n", appointments[i].fee);
    printf("Payment Status   : %s\n", appointments[i].payment);

    /* choose payment method */
    printf("\nPayment method:\n");
    printf("1. JazzCash\n2. EasyPaisa\n3. Bank transfer\n4. Card\n");
    printf("Choose: ");
    choice = getnumber();
    while (choice < 1 || choice > 4) {
        printf("Invalid choice. Enter 1 to 4: ");
        choice = getnumber();
    }
    strcpy(appointments[i].method, methods[choice - 1]);

    /* the patient gets 3 tries to enter a correct transaction number */
    for (attempt = 1; attempt <= 3; attempt++) {
        printf("\nEnter transaction/reference number (8 digits): ");
        scanf("%s", t);
        clearinput();

        printf("Verifying payment...\n");

        if (isvalidtransaction(t) && !transactionused(t)) {
            /* payment verified -> appointment confirmed */
            strcpy(appointments[i].transaction, t);
            strcpy(appointments[i].payment, "Verified");
            strcpy(appointments[i].status, "Confirmed");
            saveappointments();

            printf("\nPayment Status : VERIFIED\n");
            printf("Appointment %s is now CONFIRMED.\n", appointments[i].id);
            return;
        }

        if (transactionused(t))
            printf("Payment FAILED: this transaction number was already used.\n");
        else
            printf("Payment FAILED: invalid transaction number.\n");
        printf("Attempts left: %d\n", 3 - attempt);
    }

    /* 3 failed tries -> cancel the appointment automatically */
    strcpy(appointments[i].payment, "Failed");
    strcpy(appointments[i].status, "Cancelled");
    saveappointments();

    printf("\nPayment Status : FAILED\n");
    printf("Payment could not be verified.\n");
    printf("Appointment %s has been CANCELLED automatically.\n", appointments[i].id);
}

void paymentstatus(int patientid)
{
    char id[10];
    int i;

    if (showmyappointments(patientid) == 0)
        return;

    printf("\nEnter Appointment ID to see payment details: ");
    scanf("%s", id);
    clearinput();

    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].id, id) == 0 &&
            appointments[i].patientid == patientid) {
            printf("\n========== PAYMENT DETAILS ==========\n");
            printf("Appointment ID     : %s\n", appointments[i].id);
            printf("Doctor             : %s\n", appointments[i].doctorname);
            printf("Consultation Fee   : Rs. %d\n", appointments[i].fee);
            printf("Payment Method     : %s\n", appointments[i].method);
            printf("Transaction Number : %s\n", appointments[i].transaction);
            printf("Payment Status     : %s\n", appointments[i].payment);
            printf("Appointment Status : %s\n", appointments[i].status);
            return;
        }
    }
    printf("Appointment not found.\n");
}

/* ===================== MAIN ===================== */

int main()
{
    int choice, patientid;

    loadappointments();

    printf("\n========== PAYMENT SYSTEM ==========\n");
    printf("Enter your Patient ID: ");
    patientid = getnumber();

    while (1) {
        printf("\n========== PAYMENT MENU ==========\n");
        printf("Patient ID: %d\n", patientid);
        printf("1. Pay for an appointment\n");
        printf("2. View payment status\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        choice = getnumber();

        switch (choice) {
            case 1: makepayment(patientid);   break;
            case 2: paymentstatus(patientid); break;
            case 0:
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
}
