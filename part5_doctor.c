#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAXPATIENTS 500
#define MAXDOCTORS 100
#define MAXPENDING 50
#define MAXAPPOINTMENTS 1000
#define MAXREVIEWS 1000
#define MAXCATEGORIES 30

struct patient {
    int  id;
    char name[50];
    char password[20];
    int  age;
    int  gender;
    char phone[20];
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
    char appointmentid[10];
    char doctorid[10];
    int  patientid;
    int  stars;
    char text[200];
};

struct account {
    char id[5];
    char password[20];
};

struct pendingdoctor {
    struct doctor info;
    char password[20];
};

struct patient patients[MAXPATIENTS];
int patientcount = 0;

struct doctor doctors[MAXDOCTORS];
int doctorcount = 0;

struct appointment appointments[MAXAPPOINTMENTS];
int appointmentcount = 0;

struct review reviews[MAXREVIEWS];
int reviewcount = 0;

struct account accounts[MAXDOCTORS];
int accountcount = 0;

struct pendingdoctor pending[MAXPENDING];
int pendingcount = 0;

char categories[MAXCATEGORIES][20];
int categorycount = 0;

char shortdays[7][4] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
char fulldays[7][10] = {"Monday", "Tuesday", "Wednesday", "Thursday",
                        "Friday", "Saturday", "Sunday"};

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

void getline_text(char text[], int size)
{
    int len;
    if (fgets(text, size, stdin) == NULL) {
        text[0] = '\0';
        return;
    }
    len = strlen(text);
    if (len > 0 && text[len - 1] == '\n')
        text[len - 1] = '\0';
    else if (len > 0)
        clearinput();
}

void readname(FILE *fp, char text[], int size)
{
    int len;
    if (fgets(text, size, fp) == NULL) {
        text[0] = '\0';
        return;
    }
    len = strlen(text);
    if (len > 0 && text[len - 1] == '\n')
        text[len - 1] = '\0';
}

void touppercase(char text[])
{
    int i;
    for (i = 0; text[i] != '\0'; i++)
        text[i] = toupper(text[i]);
}

void getname(char prompt[], char text[], int size)
{
    while (1) {
        printf("%s", prompt);
        getline_text(text, size);
        if (text[0] != '\0')
            break;
        printf("This cannot be empty.\n");
    }
}

void getnospace(char prompt[], char text[], int size)
{
    int i;
    getname(prompt, text, size);
    for (i = 0; text[i] != '\0'; i++) {
        if (text[i] == ' ')
            text[i] = '_';
    }
}

void getpassword(char pass[])
{
    char temp[50];
    while (1) {
        printf("Enter password (4-10 characters): ");
        scanf("%49s", temp);
        clearinput();
        if (strlen(temp) > 3 && strlen(temp) <= 10)
            break;
        printf("Password must be 4 to 10 characters.\n");
    }
    strcpy(pass, temp);
}

int isleap(int y)
{
    return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
}

int daysinmonth(int m, int y)
{
    int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && isleap(y))
        return 29;
    return days[m - 1];
}

int datenumber(char date[])
{
    int i, d, m, y;

    if (strlen(date) != 10)
        return 0;
    for (i = 0; i < 10; i++) {
        if (i == 2 || i == 5) {
            if (date[i] != '-')
                return 0;
        } else if (date[i] < '0' || date[i] > '9') {
            return 0;
        }
    }
    d = (date[0] - '0') * 10 + (date[1] - '0');
    m = (date[3] - '0') * 10 + (date[4] - '0');
    y = (date[6] - '0') * 1000 + (date[7] - '0') * 100 +
        (date[8] - '0') * 10 + (date[9] - '0');
    if (y < 2000 || y > 2100 || m < 1 || m > 12)
        return 0;
    if (d < 1 || d > daysinmonth(m, y))
        return 0;
    return y * 10000 + m * 100 + d;
}

int todaynumber()
{
    time_t t = time(NULL);
    struct tm *now = localtime(&t);
    return (now->tm_year + 1900) * 10000 + (now->tm_mon + 1) * 100 + now->tm_mday;
}

void loadpatients()
{
    FILE *fp = fopen("patients.txt", "r");
    struct patient p;

    patientcount = 0;
    if (fp == NULL)
        return;
    while (patientcount < MAXPATIENTS &&
           fscanf(fp, "%d %s %d %d %s ",
                  &p.id, p.password, &p.age, &p.gender, p.phone) == 5) {
        readname(fp, p.name, 50);
        patients[patientcount] = p;
        patientcount++;
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
    doctorcount = 0;
    while (doctorcount < MAXDOCTORS &&
           fscanf(fp, "%s %s %d %s %s %d %f %s %s ",
                  d.id, d.specialization, &d.experience, d.qualification,
                  d.hospital, &d.fee, &d.rating, d.days, d.timing) == 9) {
        readname(fp, d.name, 50);
        doctors[doctorcount] = d;
        doctorcount++;
    }
    fclose(fp);
}

void savedoctors()
{
    FILE *fp = fopen("doctors.txt", "w");
    int i;

    if (fp == NULL) {
        printf("Error: could not save doctors.txt\n");
        return;
    }
    for (i = 0; i < doctorcount; i++) {
        fprintf(fp, "%s %s %d %s %s %d %.1f %s %s %s\n",
                doctors[i].id, doctors[i].specialization, doctors[i].experience,
                doctors[i].qualification, doctors[i].hospital, doctors[i].fee,
                doctors[i].rating, doctors[i].days, doctors[i].timing,
                doctors[i].name);
    }
    fclose(fp);
}

void loadappointments()
{
    FILE *fp = fopen("appointments.txt", "r");
    struct appointment a;

    appointmentcount = 0;
    if (fp == NULL)
        return;
    while (appointmentcount < MAXAPPOINTMENTS &&
           fscanf(fp, "%s %d %s %d %s %s %s %s %s %s ",
                  a.id, &a.patientid, a.doctorid, &a.fee, a.date, a.time,
                  a.status, a.payment, a.method, a.transaction) == 10) {
        readname(fp, a.doctorname, 50);
        appointments[appointmentcount] = a;
        appointmentcount++;
    }
    fclose(fp);
}

void saveappointments()
{
    FILE *fp = fopen("appointments.txt", "w");
    int i;

    if (fp == NULL) {
        printf("Error: could not save appointments.txt\n");
        return;
    }
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

void loadreviews()
{
    FILE *fp = fopen("reviews.txt", "r");
    struct review r;

    reviewcount = 0;
    if (fp == NULL)
        return;
    while (reviewcount < MAXREVIEWS &&
           fscanf(fp, "%s %s %d %d ",
                  r.appointmentid, r.doctorid, &r.patientid, &r.stars) == 4) {
        readname(fp, r.text, 200);
        reviews[reviewcount] = r;
        reviewcount++;
    }
    fclose(fp);
}

void loadaccounts()
{
    FILE *fp = fopen("doctoracc.txt", "r");
    struct account a;

    accountcount = 0;
    if (fp == NULL)
        return;
    while (accountcount < MAXDOCTORS && fscanf(fp, "%4s %19s", a.id, a.password) == 2) {
        accounts[accountcount] = a;
        accountcount++;
    }
    fclose(fp);
}

void saveaccounts()
{
    FILE *fp = fopen("doctoracc.txt", "w");
    int i;

    if (fp == NULL) {
        printf("Error: could not save doctoracc.txt\n");
        return;
    }
    for (i = 0; i < accountcount; i++)
        fprintf(fp, "%s %s\n", accounts[i].id, accounts[i].password);
    fclose(fp);
}

void loadpending()
{
    FILE *fp = fopen("pendingdoctors.txt", "r");
    struct pendingdoctor p;

    pendingcount = 0;
    if (fp == NULL)
        return;
    while (pendingcount < MAXPENDING &&
           fscanf(fp, "%s %s %s %d %s %s %d %f %s %s ",
                  p.info.id, p.password, p.info.specialization,
                  &p.info.experience, p.info.qualification, p.info.hospital,
                  &p.info.fee, &p.info.rating, p.info.days, p.info.timing) == 10) {
        readname(fp, p.info.name, 50);
        pending[pendingcount] = p;
        pendingcount++;
    }
    fclose(fp);
}

void savepending()
{
    FILE *fp = fopen("pendingdoctors.txt", "w");
    int i;

    if (fp == NULL) {
        printf("Error: could not save pendingdoctors.txt\n");
        return;
    }
    for (i = 0; i < pendingcount; i++) {
        fprintf(fp, "%s %s %s %d %s %s %d %.1f %s %s %s\n",
                pending[i].info.id, pending[i].password,
                pending[i].info.specialization, pending[i].info.experience,
                pending[i].info.qualification, pending[i].info.hospital,
                pending[i].info.fee, pending[i].info.rating,
                pending[i].info.days, pending[i].info.timing,
                pending[i].info.name);
    }
    fclose(fp);
}

void savecategories()
{
    FILE *fp = fopen("categories.txt", "w");
    int i;

    if (fp == NULL) {
        printf("Error: could not save categories.txt\n");
        return;
    }
    for (i = 0; i < categorycount; i++)
        fprintf(fp, "%s\n", categories[i]);
    fclose(fp);
}

void loadcategories()
{
    FILE *fp = fopen("categories.txt", "r");
    char defaults[9][20] = {"Cardiologist", "Dermatologist", "Pediatrician",
                            "Gynecologist", "Orthopedic", "Physician",
                            "Neurologist", "Dentist", "ENT"};
    int i;

    categorycount = 0;
    if (fp == NULL) {
        for (i = 0; i < 9; i++)
            strcpy(categories[i], defaults[i]);
        categorycount = 9;
        savecategories();
        return;
    }
    while (categorycount < MAXCATEGORIES &&
           fscanf(fp, "%19s", categories[categorycount]) == 1)
        categorycount++;
    fclose(fp);
}

int finddoctorindex(char id[])
{
    int i;
    for (i = 0; i < doctorcount; i++) {
        if (strcmp(doctors[i].id, id) == 0)
            return i;
    }
    return -1;
}

int findpatientindex(int id)
{
    int i;
    for (i = 0; i < patientcount; i++) {
        if (patients[i].id == id)
            return i;
    }
    return -1;
}

int findaccountindex(char id[])
{
    int i;
    for (i = 0; i < accountcount; i++) {
        if (strcmp(accounts[i].id, id) == 0)
            return i;
    }
    return -1;
}

void makemissingaccounts()
{
    int i;
    int changed = 0;

    for (i = 0; i < doctorcount; i++) {
        if (findaccountindex(doctors[i].id) == -1 && accountcount < MAXDOCTORS) {
            strcpy(accounts[accountcount].id, doctors[i].id);
            strcpy(accounts[accountcount].password, "doc123");
            accountcount++;
            changed = 1;
        }
    }
    if (changed)
        saveaccounts();
}

int doctornameexists(char name[])
{
    int i;
    for (i = 0; i < doctorcount; i++) {
        if (strcmp(doctors[i].name, name) == 0)
            return 1;
    }
    for (i = 0; i < pendingcount; i++) {
        if (strcmp(pending[i].info.name, name) == 0)
            return 1;
    }
    return 0;
}

void newdoctorid(char id[])
{
    int i, n, max = 0;
    for (i = 0; i < doctorcount; i++) {
        if (sscanf(doctors[i].id, "D%d", &n) == 1 && n > max)
            max = n;
    }
    for (i = 0; i < pendingcount; i++) {
        if (sscanf(pending[i].info.id, "D%d", &n) == 1 && n > max)
            max = n;
    }
    sprintf(id, "D%d", max + 1);
}

int pickcategory(char spec[])
{
    int i, choice;

    if (categorycount == 0) {
        printf("No specializations are available. Ask the admin to add some.\n");
        return 0;
    }
    for (i = 0; i < categorycount; i++)
        printf("%d. %s\n", i + 1, categories[i]);
    printf("Choose specialization: ");
    choice = getnumber();
    while (choice < 1 || choice > categorycount) {
        printf("Invalid choice. Enter again: ");
        choice = getnumber();
    }
    strcpy(spec, categories[choice - 1]);
    return 1;
}

int pickdays(char days[])
{
    char text[30] = "";
    int i, answer, count = 0;

    printf("Select the days you are available:\n");
    for (i = 0; i < 7; i++) {
        printf("%-10s (1 = yes, 0 = no): ", fulldays[i]);
        answer = getnumber();
        while (answer != 0 && answer != 1) {
            printf("Enter 1 or 0: ");
            answer = getnumber();
        }
        if (answer == 1) {
            if (count > 0)
                strcat(text, ",");
            strcat(text, shortdays[i]);
            count++;
        }
    }
    if (count == 0) {
        printf("You must select at least one day.\n");
        return 0;
    }
    strcpy(days, text);
    return 1;
}

void picktiming(char timing[])
{
    int start, end;

    printf("Use a 24-hour clock. Each appointment slot is 30 minutes.\n");
    printf("Start hour (0 to 22): ");
    start = getnumber();
    while (start < 0 || start > 22) {
        printf("Invalid hour. Enter again: ");
        start = getnumber();
    }
    printf("End hour (%d to 23): ", start + 1);
    end = getnumber();
    while (end <= start || end > 23) {
        printf("Invalid hour. Enter again: ");
        end = getnumber();
    }
    sprintf(timing, "%02d:00-%02d:00", start, end);
}

void registerdoctor()
{
    struct pendingdoctor p;

    if (pendingcount >= MAXPENDING || doctorcount + pendingcount >= MAXDOCTORS) {
        printf("Registration is full right now. Please contact the admin.\n");
        return;
    }

    printf("\n========== DOCTOR REGISTRATION ==========\n");
    getname("Enter full name: ", p.info.name, 50);
    if (doctornameexists(p.info.name)) {
        printf("A doctor with this name is already registered.\n");
        return;
    }
    if (!pickcategory(p.info.specialization))
        return;

    printf("Years of experience: ");
    p.info.experience = getnumber();
    while (p.info.experience < 0 || p.info.experience > 60) {
        printf("Invalid experience. Enter again: ");
        p.info.experience = getnumber();
    }

    getnospace("Qualification (e.g. MBBS): ", p.info.qualification, 20);
    getnospace("Hospital/clinic: ", p.info.hospital, 20);

    printf("Consultation fee (Rs.): ");
    p.info.fee = getnumber();
    while (p.info.fee < 100 || p.info.fee > 100000) {
        printf("Fee must be between 100 and 100000. Enter again: ");
        p.info.fee = getnumber();
    }

    while (!pickdays(p.info.days))
        ;
    picktiming(p.info.timing);
    getpassword(p.password);

    newdoctorid(p.info.id);
    p.info.rating = 0.0;
    pending[pendingcount] = p;
    pendingcount++;
    savepending();

    printf("\nRegistration submitted!\n");
    printf("Your Doctor ID is %s (you need it to login)\n", p.info.id);
    printf("You can login after the admin verifies your account.\n");
}

int logindoctor()
{
    char id[10], pass[50];
    int i, a;

    printf("\n========== DOCTOR LOGIN ==========\n");
    printf("Enter Doctor ID (e.g. D1): ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);
    printf("Enter password: ");
    scanf("%49s", pass);
    clearinput();

    for (a = 0; a < accountcount; a++) {
        if (strcmp(accounts[a].id, id) == 0 && strcmp(accounts[a].password, pass) == 0) {
            i = finddoctorindex(id);
            if (i != -1) {
                printf("\nLogin successful! Welcome %s\n", doctors[i].name);
                return i;
            }
        }
    }
    for (i = 0; i < pendingcount; i++) {
        if (strcmp(pending[i].info.id, id) == 0 && strcmp(pending[i].password, pass) == 0) {
            printf("Your registration is still waiting for admin verification.\n");
            return -1;
        }
    }
    printf("Wrong ID or password.\n");
    return -1;
}

void viewprofile(int d)
{
    printf("\n========== MY PROFILE ==========\n");
    printf("Doctor ID        : %s\n", doctors[d].id);
    printf("Name             : %s\n", doctors[d].name);
    printf("Specialization   : %s\n", doctors[d].specialization);
    printf("Experience       : %d years\n", doctors[d].experience);
    printf("Qualification    : %s\n", doctors[d].qualification);
    printf("Hospital         : %s\n", doctors[d].hospital);
    printf("Consultation fee : Rs. %d\n", doctors[d].fee);
    printf("Rating           : %.1f / 5\n", doctors[d].rating);
    printf("Available days   : %s\n", doctors[d].days);
    printf("Timing           : %s\n", doctors[d].timing);
}

void updatedoctorname(int d, char newname[])
{
    int i;
    strcpy(doctors[d].name, newname);
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].doctorid, doctors[d].id) == 0)
            strcpy(appointments[i].doctorname, newname);
    }
    savedoctors();
    saveappointments();
}

void editprofile(int d)
{
    char text[50];
    int choice, years;

    while (1) {
        printf("\n========== EDIT PROFILE ==========\n");
        printf("1. Name\n");
        printf("2. Years of experience\n");
        printf("3. Qualification\n");
        printf("4. Hospital/clinic\n");
        printf("0. Back\n");
        printf("Enter choice: ");
        choice = getnumber();

        if (choice == 0)
            return;
        if (choice == 1) {
            getname("Enter new name: ", text, 50);
            if (strcmp(text, doctors[d].name) != 0 && doctornameexists(text))
                printf("A doctor with this name already exists.\n");
            else {
                updatedoctorname(d, text);
                printf("Name updated.\n");
            }
        } else if (choice == 2) {
            printf("Enter years of experience: ");
            years = getnumber();
            while (years < 0 || years > 60) {
                printf("Invalid experience. Enter again: ");
                years = getnumber();
            }
            doctors[d].experience = years;
            savedoctors();
            printf("Experience updated.\n");
        } else if (choice == 3) {
            getnospace("Enter new qualification: ", doctors[d].qualification, 20);
            savedoctors();
            printf("Qualification updated.\n");
        } else if (choice == 4) {
            getnospace("Enter hospital/clinic: ", doctors[d].hospital, 20);
            savedoctors();
            printf("Hospital updated.\n");
        } else {
            printf("Invalid choice.\n");
        }
    }
}

void setspecialization(int d)
{
    char spec[20];

    printf("\nCurrent specialization: %s\n", doctors[d].specialization);
    if (pickcategory(spec)) {
        strcpy(doctors[d].specialization, spec);
        savedoctors();
        printf("Specialization updated.\n");
    }
}

void setfee(int d)
{
    int fee;

    printf("\nCurrent fee: Rs. %d\n", doctors[d].fee);
    printf("Enter new consultation fee (Rs.): ");
    fee = getnumber();
    while (fee < 100 || fee > 100000) {
        printf("Fee must be between 100 and 100000. Enter again: ");
        fee = getnumber();
    }
    doctors[d].fee = fee;
    savedoctors();
    printf("Fee updated. New bookings will use this fee.\n");
}

void setdays(int d)
{
    char days[30];

    printf("\nCurrent days: %s\n", doctors[d].days);
    if (pickdays(days)) {
        strcpy(doctors[d].days, days);
        savedoctors();
        printf("Available days updated.\n");
    }
}

void settiming(int d)
{
    char timing[15];

    printf("\nCurrent timing: %s\n", doctors[d].timing);
    picktiming(timing);
    strcpy(doctors[d].timing, timing);
    savedoctors();
    printf("Time slots updated.\n");
}

int inmode(int i, int mode)
{
    if (mode == 0)
        return 1;
    if (mode == 1)
        return strcmp(appointments[i].status, "Pending") == 0;
    if (mode == 2)
        return strcmp(appointments[i].status, "Pending") == 0 ||
               strcmp(appointments[i].status, "Accepted") == 0 ||
               strcmp(appointments[i].status, "Confirmed") == 0;
    return strcmp(appointments[i].status, "Confirmed") == 0 &&
           datenumber(appointments[i].date) <= todaynumber();
}

int showlist(int d, int mode)
{
    int i, p, found = 0;
    char patientname[50];

    printf("\n%-5s %-6s %-18s %-11s %-9s %-10s %s\n",
           "ID", "Pat.", "Patient", "Date", "Time", "Status", "Payment");
    printf("-----------------------------------------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].doctorid, doctors[d].id) == 0 && inmode(i, mode)) {
            p = findpatientindex(appointments[i].patientid);
            if (p == -1)
                strcpy(patientname, "Unknown");
            else
                strcpy(patientname, patients[p].name);
            printf("%-5s %-6d %-18s %-11s %-9s %-10s %s\n",
                   appointments[i].id, appointments[i].patientid, patientname,
                   appointments[i].date, appointments[i].time,
                   appointments[i].status, appointments[i].payment);
            found++;
        }
    }
    if (found == 0)
        printf("No appointments found.\n");
    return found;
}

int pickappointment(int d, int mode, char action[])
{
    char id[10];
    int i;

    if (showlist(d, mode) == 0)
        return -1;
    printf("\nEnter Appointment ID to %s: ", action);
    scanf("%9s", id);
    clearinput();
    touppercase(id);

    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].id, id) == 0 &&
            strcmp(appointments[i].doctorid, doctors[d].id) == 0 &&
            inmode(i, mode))
            return i;
    }
    printf("Appointment not found in the list.\n");
    return -1;
}

void cancelrecord(int a)
{
    strcpy(appointments[a].status, "Cancelled");
    if (strcmp(appointments[a].payment, "Verified") == 0) {
        strcpy(appointments[a].payment, "Refunded");
        printf("Payment of Rs. %d will be refunded to the patient.\n", appointments[a].fee);
    }
    saveappointments();
}

void viewappointments(int d)
{
    printf("\n========== MY APPOINTMENTS ==========");
    showlist(d, 0);
}

void acceptreject(int d)
{
    int a, choice;

    printf("\n========== NEW APPOINTMENT REQUESTS ==========");
    a = pickappointment(d, 1, "accept or reject");
    if (a == -1)
        return;

    printf("1. Accept\n2. Reject\n0. Back\nChoose: ");
    choice = getnumber();
    if (choice == 1) {
        strcpy(appointments[a].status, "Accepted");
        saveappointments();
        printf("Appointment %s accepted. The patient can now pay.\n", appointments[a].id);
    } else if (choice == 2) {
        cancelrecord(a);
        printf("Appointment %s rejected.\n", appointments[a].id);
    }
}

void cancelappointment(int d)
{
    int a, choice;

    printf("\n========== CANCEL AN APPOINTMENT ==========");
    a = pickappointment(d, 2, "cancel");
    if (a == -1)
        return;

    printf("Cancel appointment %s? (1 = yes, 0 = no): ", appointments[a].id);
    choice = getnumber();
    if (choice != 1) {
        printf("Nothing was cancelled.\n");
        return;
    }
    cancelrecord(a);
    printf("Appointment %s cancelled.\n", appointments[a].id);
}

void markcompleted(int d)
{
    int a;

    printf("\n========== MARK AS COMPLETED ==========");
    a = pickappointment(d, 3, "mark as completed");
    if (a == -1)
        return;

    strcpy(appointments[a].status, "Completed");
    saveappointments();
    printf("Appointment %s is completed. The patient can now rate you.\n", appointments[a].id);
}

void patienthistory(int d)
{
    int i, p, id, found = 0;

    printf("\nEnter Patient ID: ");
    id = getnumber();
    p = findpatientindex(id);
    if (p == -1) {
        printf("Patient not found.\n");
        return;
    }

    printf("\n========== PATIENT HISTORY ==========\n");
    printf("Name   : %s\n", patients[p].name);
    printf("Age    : %d\n", patients[p].age);
    if (patients[p].gender == 1)
        printf("Gender : Male\n");
    else if (patients[p].gender == 2)
        printf("Gender : Female\n");
    else
        printf("Gender : Other\n");

    printf("\n%-5s %-11s %-9s %-10s %-9s %s\n",
           "ID", "Date", "Time", "Status", "Fee", "Payment");
    printf("----------------------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (appointments[i].patientid == id &&
            strcmp(appointments[i].doctorid, doctors[d].id) == 0) {
            printf("%-5s %-11s %-9s %-10s %-9d %s\n",
                   appointments[i].id, appointments[i].date, appointments[i].time,
                   appointments[i].status, appointments[i].fee,
                   appointments[i].payment);
            found++;
        }
    }
    if (found == 0)
        printf("This patient has no appointments with you.\n");
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

void viewreviews(int d)
{
    int i, found = 0;

    printf("\n========== MY RATINGS & REVIEWS ==========\n");
    printf("Current rating: %.1f / 5\n\n", doctors[d].rating);
    for (i = 0; i < reviewcount; i++) {
        if (strcmp(reviews[i].doctorid, doctors[d].id) == 0) {
            printstars(reviews[i].stars);
            printf("  %s\n", reviews[i].text);
            found++;
        }
    }
    if (found == 0)
        printf("You have no reviews yet.\n");
    else
        printf("\nTotal reviews: %d\n", found);
}

void changepassword(int d)
{
    char old[50];
    int a = findaccountindex(doctors[d].id);

    printf("\nEnter current password: ");
    scanf("%49s", old);
    clearinput();

    if (a == -1 || strcmp(old, accounts[a].password) != 0) {
        printf("Wrong password.\n");
        return;
    }
    getpassword(accounts[a].password);
    saveaccounts();
    printf("Password changed.\n");
}

void dashboard(int d)
{
    int choice;

    while (1) {
        printf("\n========== DOCTOR DASHBOARD ==========\n");
        printf("Logged in as: %s\n", doctors[d].name);
        printf("1. View my profile\n");
        printf("2. Edit profile\n");
        printf("3. Set specialization\n");
        printf("4. Set consultation fee\n");
        printf("5. Set available days\n");
        printf("6. Set available time slots\n");
        printf("7. View my appointments\n");
        printf("8. Accept / reject appointment requests\n");
        printf("9. Cancel an appointment\n");
        printf("10. Mark appointment as completed\n");
        printf("11. View patient appointment history\n");
        printf("12. View my ratings and reviews\n");
        printf("13. Change password\n");
        printf("0. Logout\n");
        printf("Enter choice: ");
        choice = getnumber();

        switch (choice) {
            case 1:  viewprofile(d);        break;
            case 2:  editprofile(d);        break;
            case 3:  setspecialization(d);  break;
            case 4:  setfee(d);             break;
            case 5:  setdays(d);            break;
            case 6:  settiming(d);          break;
            case 7:  viewappointments(d);   break;
            case 8:  acceptreject(d);       break;
            case 9:  cancelappointment(d);  break;
            case 10: markcompleted(d);      break;
            case 11: patienthistory(d);     break;
            case 12: viewreviews(d);        break;
            case 13: changepassword(d);     break;
            case 0:
                printf("Logged out.\n");
                return;
            default: printf("Invalid choice.\n");
        }
    }
}

int main()
{
    int choice, d;

    loadpatients();
    loaddoctors();
    loadappointments();
    loadreviews();
    loadaccounts();
    loadpending();
    loadcategories();
    makemissingaccounts();

    while (1) {
        printf("\n===== HOSPITAL INFORMATION MANAGEMENT SYSTEM =====\n");
        printf("1. Doctor login\n");
        printf("2. Doctor registration\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        choice = getnumber();

        switch (choice) {
            case 1:
                d = logindoctor();
                if (d != -1)
                    dashboard(d);
                break;
            case 2:
                registerdoctor();
                break;
            case 0:
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
}
