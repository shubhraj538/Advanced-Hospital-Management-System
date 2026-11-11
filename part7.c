/*
 * HIMS - Hospital Information Management System
 * PART 7 : ADMIN STATISTICS
 *
 * Reads patients.txt, doctors.txt and appointments.txt and shows:
 *   - total patients, doctors, appointments today, completed,
 *     cancelled, pending, total revenue
 *   - most booked doctor, most popular specialization,
 *     most cancelled doctor, highest rated doctor,
 *     total appointments per specialization
 *
 * Admin login: username admin, password admin123
 */

#include <stdio.h>
#include <string.h>

/* ===================== STRUCTURES ===================== */

struct appointment {
    char id[10];
    int  patientid;
    char doctorid[10];
    int  fee;
    char date[15];
    char time[10];
    char status[15];
    char payment[15];
    char method[15];
    char transaction[20];
    char doctorname[50];
};

struct doctor {
    char  id[5];
    char  specialization[20];
    int   experience;
    char  qualification[20];
    char  hospital[20];
    int   fee;
    float rating;
    char  days[30];
    char  timing[15];
    char  name[50];
};

struct appointment appointments[100];
int appointmentcount = 0;

struct doctor doctors[50];
int doctorcount = 0;

int patientcount = 0;

char specs[9][20] = {"Cardiologist", "Dermatologist", "Pediatrician",
                     "Gynecologist", "Orthopedic", "Physician",
                     "Neurologist", "Dentist", "ENT"};

/* ===================== INPUT HELPERS ===================== */

void clearinput()
{
    int c = getchar();
    while (c != '\n' && c != EOF)
        c = getchar();
}

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

void removenewline(char text[])
{
    int len = strlen(text);
    if (len > 0 && text[len - 1] == '\n')
        text[len - 1] = '\0';
}

/* ===================== FILES ===================== */

/* we only need HOW MANY patients there are, so just count the lines */
void countpatients()
{
    FILE *fp = fopen("patients.txt", "r");
    char line[200];

    patientcount = 0;
    if (fp == NULL)
        return;
    while (fgets(line, 200, fp) != NULL)
        patientcount++;
    fclose(fp);
}

void loaddoctors()
{
    FILE *fp = fopen("doctors.txt", "r");
    struct doctor d;

    if (fp == NULL) {
        printf("Error: doctors.txt not found!\n");
        return;
    }
    while (fscanf(fp, "%s %s %d %s %s %d %f %s %s ",
                  d.id, d.specialization, &d.experience, d.qualification,
                  d.hospital, &d.fee, &d.rating, d.days, d.timing) == 9) {
        fgets(d.name, 50, fp);
        removenewline(d.name);
        doctors[doctorcount] = d;
        doctorcount++;
    }
    fclose(fp);
}

void loadappointments()
{
    FILE *fp = fopen("appointments.txt", "r");
    struct appointment a;

    if (fp == NULL) {
        printf("Error: appointments.txt not found!\n");
        return;
    }
    while (fscanf(fp, "%s %d %s %d %s %s %s %s %s %s ",
                  a.id, &a.patientid, a.doctorid, &a.fee, a.date, a.time,
                  a.status, a.payment, a.method, a.transaction) == 10) {
        fgets(a.doctorname, 50, fp);
        removenewline(a.doctorname);
        appointments[appointmentcount] = a;
        appointmentcount++;
    }
    fclose(fp);
}

/* ===================== HELPERS ===================== */

/* returns the position of the doctor with this ID, or -1 */
int finddoctor(char id[])
{
    int i;
    for (i = 0; i < doctorcount; i++) {
        if (strcmp(doctors[i].id, id) == 0)
            return i;
    }
    return -1;
}

/* how many appointments have this status (Completed, Cancelled ...) */
int countstatus(char status[])
{
    int i, count = 0;
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].status, status) == 0)
            count++;
    }
    return count;
}

/* how many appointments one doctor has.
 * if status is "All" it counts every appointment */
int countfordoctor(char doctorid[], char status[])
{
    int i, count = 0;
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].doctorid, doctorid) == 0) {
            if (strcmp(status, "All") == 0 ||
                strcmp(appointments[i].status, status) == 0)
                count++;
        }
    }
    return count;
}

/* how many appointments were with doctors of this specialization */
int countforspecialization(char spec[])
{
    int i, d, count = 0;
    for (i = 0; i < appointmentcount; i++) {
        d = finddoctor(appointments[i].doctorid);
        if (d != -1 && strcmp(doctors[d].specialization, spec) == 0)
            count++;
    }
    return count;
}

/* ===================== STATISTICS ===================== */

void systemstatistics()
{
    char today[15];
    int i, todaycount = 0, revenue = 0;

    printf("Enter today's date (dd-mm-yyyy): ");
    scanf("%s", today);
    clearinput();

    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].date, today) == 0)
            todaycount++;
        /* only payments that were verified count as revenue */
        if (strcmp(appointments[i].payment, "Verified") == 0)
            revenue = revenue + appointments[i].fee;
    }

    printf("\n===== SYSTEM STATISTICS =====\n\n");
    printf("Total Patients       : %d\n", patientcount);
    printf("Total Doctors        : %d\n", doctorcount);
    printf("Total Appointments   : %d\n", appointmentcount);
    printf("Appointments Today   : %d\n", todaycount);
    printf("Completed            : %d\n", countstatus("Completed"));
    printf("Confirmed            : %d\n", countstatus("Confirmed"));
    printf("Cancelled            : %d\n", countstatus("Cancelled"));
    printf("Pending              : %d\n", countstatus("Pending"));
    printf("\nTotal Revenue        : Rs. %d\n", revenue);
}

void detailedstatistics()
{
    int i, count;
    int mostbooked = 0, mostbookedcount = -1;
    int mostcancelled = 0, mostcancelledcount = -1;
    int highestrated = 0;
    int popularspec = 0, popularcount = -1;

    for (i = 0; i < doctorcount; i++) {
        /* most booked doctor */
        count = countfordoctor(doctors[i].id, "All");
        if (count > mostbookedcount) {
            mostbookedcount = count;
            mostbooked = i;
        }

        /* most cancelled doctor */
        count = countfordoctor(doctors[i].id, "Cancelled");
        if (count > mostcancelledcount) {
            mostcancelledcount = count;
            mostcancelled = i;
        }

        /* highest rated doctor */
        if (doctors[i].rating > doctors[highestrated].rating)
            highestrated = i;
    }

    /* most popular specialization */
    for (i = 0; i < 9; i++) {
        count = countforspecialization(specs[i]);
        if (count > popularcount) {
            popularcount = count;
            popularspec = i;
        }
    }

    printf("\n===== DETAILED STATISTICS =====\n\n");
    printf("Most booked doctor         : %s (%d appointments)\n",
           doctors[mostbooked].name, mostbookedcount);
    printf("Most popular specialization: %s (%d appointments)\n",
           specs[popularspec], popularcount);
    if (mostcancelledcount == 0)
        printf("Most cancelled doctor      : None\n");
    else
        printf("Most cancelled doctor      : %s (%d cancelled)\n",
               doctors[mostcancelled].name, mostcancelledcount);
    printf("Highest rated doctor       : %s (%.1f)\n",
           doctors[highestrated].name, doctors[highestrated].rating);

    printf("\nAppointments per specialization:\n");
    printf("--------------------------------\n");
    for (i = 0; i < 9; i++)
        printf("%-16s : %d\n", specs[i], countforspecialization(specs[i]));
}

/* ===================== MAIN ===================== */

int main()
{
    char username[30], password[30];
    int choice;

    printf("\n========== ADMIN LOGIN ==========\n");
    printf("Username: ");
    scanf("%s", username);
    clearinput();
    printf("Password: ");
    scanf("%s", password);
    clearinput();

    if (strcmp(username, "admin") != 0 || strcmp(password, "admin123") != 0) {
        printf("Wrong username or password.\n");
        return 0;
    }

    countpatients();
    loaddoctors();
    loadappointments();

    while (1) {
        printf("\n========== ADMIN STATISTICS ==========\n");
        printf("1. System statistics\n");
        printf("2. Detailed statistics\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        choice = getnumber();

        switch (choice) {
            case 1: systemstatistics();   break;
            case 2: detailedstatistics(); break;
            case 0:
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
}
