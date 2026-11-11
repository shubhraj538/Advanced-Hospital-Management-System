/*
 * HIMS - Hospital Information Management System
 * PART 4 : RATINGS & REVIEWS
 *
 * After an appointment is Completed, the patient can rate the doctor
 * (1 to 5 stars) and write a review. The doctor's rating is then
 * updated to the average of all their reviews.
 *
 * appointments.txt -> appointments (only "Completed" ones can be reviewed)
 * reviews.txt      -> every review ever written
 *                     each line: appointmentID doctorID patientID stars review text
 * doctors.txt      -> the doctor's rating gets updated here
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

struct review {
    char appointmentid[10];   /* "old" for reviews that came with the file */
    char doctorid[10];
    int  patientid;
    int  stars;
    char text[200];
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

struct review reviews[500];
int reviewcount = 0;

struct doctor doctors[50];
int doctorcount = 0;

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

/* removes the Enter that fgets keeps at the end of the text */
void removenewline(char text[])
{
    int len = strlen(text);
    if (len > 0 && text[len - 1] == '\n')
        text[len - 1] = '\0';
}

/* ===================== FILES ===================== */

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

void loadreviews()
{
    FILE *fp = fopen("reviews.txt", "r");
    struct review r;

    if (fp == NULL)
        return;                     /* no reviews yet */

    while (fscanf(fp, "%s %s %d %d ",
                  r.appointmentid, r.doctorid, &r.patientid, &r.stars) == 4) {
        fgets(r.text, 200, fp);     /* rest of the line is the review */
        removenewline(r.text);
        reviews[reviewcount] = r;
        reviewcount++;
    }
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

void savedoctors()
{
    FILE *fp = fopen("doctors.txt", "w");
    int i;

    for (i = 0; i < doctorcount; i++) {
        fprintf(fp, "%s %s %d %s %s %d %.1f %s %s %s\n",
                doctors[i].id, doctors[i].specialization, doctors[i].experience,
                doctors[i].qualification, doctors[i].hospital, doctors[i].fee,
                doctors[i].rating, doctors[i].days, doctors[i].timing,
                doctors[i].name);
    }
    fclose(fp);
}

/* "a" = append mode: adds the new review at the end of the file */
void savenewreview(struct review r)
{
    FILE *fp = fopen("reviews.txt", "a");
    fprintf(fp, "%s %s %d %d %s\n",
            r.appointmentid, r.doctorid, r.patientid, r.stars, r.text);
    fclose(fp);
}

/* ===================== HELPERS ===================== */

/* returns 1 if this appointment already has a review */
int alreadyreviewed(char appointmentid[])
{
    int i;
    for (i = 0; i < reviewcount; i++) {
        if (strcmp(reviews[i].appointmentid, appointmentid) == 0)
            return 1;
    }
    return 0;
}

void printstars(int stars)
{
    int i;
    for (i = 1; i <= 5; i++) {
        if (i <= stars)
            printf("*");
        else
            printf("-");
    }
}

/* new rating = average of all the stars this doctor got */
void updaterating(char doctorid[])
{
    int i, total = 0, count = 0;

    for (i = 0; i < reviewcount; i++) {
        if (strcmp(reviews[i].doctorid, doctorid) == 0) {
            total = total + reviews[i].stars;
            count++;
        }
    }

    for (i = 0; i < doctorcount; i++) {
        if (strcmp(doctors[i].id, doctorid) == 0) {
            printf("\n%s's rating: %.1f", doctors[i].name, doctors[i].rating);
            doctors[i].rating = (float)total / count;
            printf("  ->  %.1f  (%d reviews)\n", doctors[i].rating, count);
        }
    }
    savedoctors();
}

/* ===================== RATE A DOCTOR ===================== */

void rateadoctor(int patientid)
{
    char id[10];
    int i, found = 0;
    struct review r;

    /* show completed appointments that are not reviewed yet */
    printf("\nAppointments you can review:\n");
    printf("%-5s %-18s %-11s\n", "ID", "Doctor", "Date");
    printf("-----------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (appointments[i].patientid == patientid &&
            strcmp(appointments[i].status, "Completed") == 0 &&
            !alreadyreviewed(appointments[i].id)) {
            printf("%-5s %-18s %-11s\n", appointments[i].id,
                   appointments[i].doctorname, appointments[i].date);
            found++;
        }
    }
    if (found == 0) {
        printf("No completed appointments to review.\n");
        return;
    }

    printf("\nEnter Appointment ID: ");
    scanf("%s", id);
    clearinput();

    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].id, id) == 0 &&
            appointments[i].patientid == patientid)
            break;
    }
    if (i == appointmentcount) {
        printf("Appointment not found.\n");
        return;
    }
    if (strcmp(appointments[i].status, "Completed") != 0) {
        printf("You can only review a completed appointment.\n");
        return;
    }
    if (alreadyreviewed(id)) {
        printf("You already reviewed this appointment.\n");
        return;
    }

    /* stars */
    printf("\nRate %s (1 to 5): ", appointments[i].doctorname);
    r.stars = getnumber();
    while (r.stars < 1 || r.stars > 5) {
        printf("Rating must be 1 to 5: ");
        r.stars = getnumber();
    }
    printf("You gave: ");
    printstars(r.stars);
    printf("\n");

    /* review text */
    printf("Write review: ");
    fgets(r.text, 200, stdin);
    removenewline(r.text);
    if (r.text[0] == '\0')
        strcpy(r.text, "No comment.");

    strcpy(r.appointmentid, appointments[i].id);
    strcpy(r.doctorid, appointments[i].doctorid);
    r.patientid = patientid;

    reviews[reviewcount] = r;
    reviewcount++;
    savenewreview(r);

    printf("\nThank you! Your review has been saved.");
    updaterating(r.doctorid);
}

/* ===================== VIEW REVIEWS ===================== */

void viewreviews()
{
    char id[10];
    int i, found = 0;

    printf("\n%-4s %-18s %s\n", "ID", "Doctor", "Rating");
    printf("------------------------------\n");
    for (i = 0; i < doctorcount; i++)
        printf("%-4s %-18s %.1f\n", doctors[i].id, doctors[i].name, doctors[i].rating);

    printf("\nEnter Doctor ID to see reviews (e.g. D1): ");
    scanf("%s", id);
    clearinput();

    printf("\n========== REVIEWS ==========\n");
    for (i = 0; i < reviewcount; i++) {
        if (strcmp(reviews[i].doctorid, id) == 0) {
            printstars(reviews[i].stars);
            printf("  %s\n", reviews[i].text);
            found++;
        }
    }
    if (found == 0)
        printf("No reviews for this doctor.\n");
}

/* ===================== MAIN ===================== */

int main()
{
    int choice, patientid;

    loadappointments();
    loadreviews();
    loaddoctors();

    printf("\n========== RATINGS & REVIEWS ==========\n");
    printf("Enter your Patient ID: ");
    patientid = getnumber();

    while (1) {
        printf("\n========== REVIEW MENU ==========\n");
        printf("1. Rate a doctor\n");
        printf("2. View doctor reviews\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        choice = getnumber();

        switch (choice) {
            case 1: rateadoctor(patientid); break;
            case 2: viewreviews();          break;
            case 0:
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
}
