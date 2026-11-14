#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAXPATIENTS 500
#define MAXDOCTORS 100
#define MAXPENDING 50
#define MAXAPPOINTMENTS 1000
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

int sametext(char a[], char b[])
{
    int i;
    if (strlen(a) != strlen(b))
        return 0;
    for (i = 0; a[i] != '\0'; i++) {
        if (tolower(a[i]) != tolower(b[i]))
            return 0;
    }
    return 1;
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

int findaccountindex(char id[])
{
    int i;
    for (i = 0; i < accountcount; i++) {
        if (strcmp(accounts[i].id, id) == 0)
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
        printf("No specializations exist yet. Add one from the categories menu first.\n");
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

    printf("Select the days the doctor is available:\n");
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
        printf("At least one day must be selected.\n");
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

void showalldoctors()
{
    int i;

    printf("\n%-4s %-18s %-14s %-4s %-7s %-6s %-20s %s\n",
           "ID", "Name", "Specialization", "Exp", "Fee", "Rating", "Days", "Timing");
    printf("---------------------------------------------------------------------------------------\n");
    for (i = 0; i < doctorcount; i++) {
        printf("%-4s %-18s %-14s %-4d %-7d %-6.1f %-20s %s\n",
               doctors[i].id, doctors[i].name, doctors[i].specialization,
               doctors[i].experience, doctors[i].fee, doctors[i].rating,
               doctors[i].days, doctors[i].timing);
    }
    if (doctorcount == 0)
        printf("No doctors found.\n");
}

int askdoctor(char prompt[])
{
    char id[10];

    printf("%s", prompt);
    scanf("%9s", id);
    clearinput();
    touppercase(id);
    return finddoctorindex(id);
}

void cancelrecord(int a)
{
    strcpy(appointments[a].status, "Cancelled");
    if (strcmp(appointments[a].payment, "Verified") == 0) {
        strcpy(appointments[a].payment, "Refunded");
    }
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

void adddoctor()
{
    struct doctor d;
    char password[20];

    if (doctorcount >= MAXDOCTORS) {
        printf("The doctor list is full.\n");
        return;
    }

    printf("\n========== ADD DOCTOR ==========\n");
    getname("Enter full name: ", d.name, 50);
    if (doctornameexists(d.name)) {
        printf("A doctor with this name already exists.\n");
        return;
    }
    if (!pickcategory(d.specialization))
        return;

    printf("Years of experience: ");
    d.experience = getnumber();
    while (d.experience < 0 || d.experience > 60) {
        printf("Invalid experience. Enter again: ");
        d.experience = getnumber();
    }

    getnospace("Qualification (e.g. MBBS): ", d.qualification, 20);
    getnospace("Hospital/clinic: ", d.hospital, 20);

    printf("Consultation fee (Rs.): ");
    d.fee = getnumber();
    while (d.fee < 100 || d.fee > 100000) {
        printf("Fee must be between 100 and 100000. Enter again: ");
        d.fee = getnumber();
    }

    while (!pickdays(d.days))
        ;
    picktiming(d.timing);

    printf("Set a login password for this doctor.\n");
    getpassword(password);

    newdoctorid(d.id);
    d.rating = 0.0;
    doctors[doctorcount] = d;
    doctorcount++;
    strcpy(accounts[accountcount].id, d.id);
    strcpy(accounts[accountcount].password, password);
    accountcount++;
    savedoctors();
    saveaccounts();

    printf("\nDoctor added. Doctor ID is %s\n", d.id);
}

void editdoctor()
{
    char text[50];
    int d, choice, number;

    showalldoctors();
    d = askdoctor("\nEnter Doctor ID to edit: ");
    if (d == -1) {
        printf("Doctor not found.\n");
        return;
    }

    while (1) {
        printf("\n========== EDIT %s ==========\n", doctors[d].name);
        printf("1. Name\n");
        printf("2. Specialization\n");
        printf("3. Years of experience\n");
        printf("4. Qualification\n");
        printf("5. Hospital/clinic\n");
        printf("6. Consultation fee\n");
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
            if (pickcategory(text)) {
                strcpy(doctors[d].specialization, text);
                savedoctors();
                printf("Specialization updated.\n");
            }
        } else if (choice == 3) {
            printf("Enter years of experience: ");
            number = getnumber();
            while (number < 0 || number > 60) {
                printf("Invalid experience. Enter again: ");
                number = getnumber();
            }
            doctors[d].experience = number;
            savedoctors();
            printf("Experience updated.\n");
        } else if (choice == 4) {
            getnospace("Enter new qualification: ", doctors[d].qualification, 20);
            savedoctors();
            printf("Qualification updated.\n");
        } else if (choice == 5) {
            getnospace("Enter hospital/clinic: ", doctors[d].hospital, 20);
            savedoctors();
            printf("Hospital updated.\n");
        } else if (choice == 6) {
            printf("Enter new consultation fee (Rs.): ");
            number = getnumber();
            while (number < 100 || number > 100000) {
                printf("Fee must be between 100 and 100000. Enter again: ");
                number = getnumber();
            }
            doctors[d].fee = number;
            savedoctors();
            printf("Fee updated.\n");
        } else {
            printf("Invalid choice.\n");
        }
    }
}

int isopen(int i)
{
    return strcmp(appointments[i].status, "Pending") == 0 ||
           strcmp(appointments[i].status, "Accepted") == 0 ||
           strcmp(appointments[i].status, "Confirmed") == 0;
}

void removedoctor()
{
    int i, d, a, open = 0, choice;

    showalldoctors();
    d = askdoctor("\nEnter Doctor ID to remove: ");
    if (d == -1) {
        printf("Doctor not found.\n");
        return;
    }

    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].doctorid, doctors[d].id) == 0 && isopen(i))
            open++;
    }
    printf("\nRemove %s (%s)?\n", doctors[d].name, doctors[d].id);
    if (open > 0)
        printf("This doctor has %d open appointment(s). They will be cancelled and paid ones refunded.\n", open);
    printf("(1 = yes, 0 = no): ");
    choice = getnumber();
    if (choice != 1) {
        printf("Nothing was removed.\n");
        return;
    }

    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].doctorid, doctors[d].id) == 0 && isopen(i))
            cancelrecord(i);
    }
    saveappointments();

    a = findaccountindex(doctors[d].id);
    if (a != -1) {
        for (i = a; i < accountcount - 1; i++)
            accounts[i] = accounts[i + 1];
        accountcount--;
        saveaccounts();
    }

    for (i = d; i < doctorcount - 1; i++)
        doctors[i] = doctors[i + 1];
    doctorcount--;
    savedoctors();

    printf("Doctor removed.\n");
}

void verifydoctors()
{
    char id[10];
    int i, p, choice;

    if (pendingcount == 0) {
        printf("\nNo doctors are waiting for verification.\n");
        return;
    }

    printf("\n========== DOCTORS WAITING FOR VERIFICATION ==========\n");
    printf("%-4s %-18s %-14s %-4s %-12s %-12s %-6s %-20s %s\n",
           "ID", "Name", "Specialization", "Exp", "Qualif.", "Hospital", "Fee", "Days", "Timing");
    printf("------------------------------------------------------------------------------------------------------------\n");
    for (i = 0; i < pendingcount; i++) {
        printf("%-4s %-18s %-14s %-4d %-12s %-12s %-6d %-20s %s\n",
               pending[i].info.id, pending[i].info.name,
               pending[i].info.specialization, pending[i].info.experience,
               pending[i].info.qualification, pending[i].info.hospital,
               pending[i].info.fee, pending[i].info.days, pending[i].info.timing);
    }

    printf("\nEnter Doctor ID to review: ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);

    p = -1;
    for (i = 0; i < pendingcount; i++) {
        if (strcmp(pending[i].info.id, id) == 0)
            p = i;
    }
    if (p == -1) {
        printf("Doctor not found in the waiting list.\n");
        return;
    }

    printf("1. Verify doctor\n2. Reject registration\n0. Back\nChoose: ");
    choice = getnumber();
    if (choice == 1) {
        if (doctorcount >= MAXDOCTORS) {
            printf("The doctor list is full.\n");
            return;
        }
        doctors[doctorcount] = pending[p].info;
        doctorcount++;
        strcpy(accounts[accountcount].id, pending[p].info.id);
        strcpy(accounts[accountcount].password, pending[p].password);
        accountcount++;
        for (i = p; i < pendingcount - 1; i++)
            pending[i] = pending[i + 1];
        pendingcount--;
        savedoctors();
        saveaccounts();
        savepending();
        printf("Doctor verified. They can now login and patients can book them.\n");
    } else if (choice == 2) {
        for (i = p; i < pendingcount - 1; i++)
            pending[i] = pending[i + 1];
        pendingcount--;
        savepending();
        printf("Registration rejected.\n");
    }
}

void managecategories()
{
    char name[50];
    int i, j, choice, used, bad;

    while (1) {
        printf("\n========== MANAGE CATEGORIES ==========\n");
        printf("1. View categories\n");
        printf("2. Add category\n");
        printf("3. Remove category\n");
        printf("0. Back\n");
        printf("Enter choice: ");
        choice = getnumber();

        if (choice == 0)
            return;
        if (choice == 1) {
            if (categorycount == 0)
                printf("No categories yet.\n");
            for (i = 0; i < categorycount; i++)
                printf("%d. %s\n", i + 1, categories[i]);
        } else if (choice == 2) {
            if (categorycount >= MAXCATEGORIES) {
                printf("The category list is full.\n");
                continue;
            }
            printf("Enter new category (one word, e.g. Urologist): ");
            scanf("%19s", name);
            clearinput();
            bad = 0;
            for (i = 0; name[i] != '\0'; i++) {
                if (!isalpha(name[i]))
                    bad = 1;
            }
            if (bad) {
                printf("Use letters only, with no spaces.\n");
                continue;
            }
            used = 0;
            for (i = 0; i < categorycount; i++) {
                if (sametext(categories[i], name))
                    used = 1;
            }
            if (used) {
                printf("This category already exists.\n");
                continue;
            }
            strcpy(categories[categorycount], name);
            categorycount++;
            savecategories();
            printf("Category added.\n");
        } else if (choice == 3) {
            if (categorycount == 0) {
                printf("No categories to remove.\n");
                continue;
            }
            for (i = 0; i < categorycount; i++)
                printf("%d. %s\n", i + 1, categories[i]);
            printf("Enter number to remove (0 to go back): ");
            choice = getnumber();
            if (choice < 1 || choice > categorycount) {
                if (choice != 0)
                    printf("Invalid choice.\n");
                continue;
            }
            used = 0;
            for (i = 0; i < doctorcount; i++) {
                if (strcmp(doctors[i].specialization, categories[choice - 1]) == 0)
                    used++;
            }
            for (i = 0; i < pendingcount; i++) {
                if (strcmp(pending[i].info.specialization, categories[choice - 1]) == 0)
                    used++;
            }
            if (used > 0) {
                printf("Cannot remove: %d doctor(s) use this specialization.\n", used);
                continue;
            }
            for (j = choice - 1; j < categorycount - 1; j++)
                strcpy(categories[j], categories[j + 1]);
            categorycount--;
            savecategories();
            printf("Category removed.\n");
        } else {
            printf("Invalid choice.\n");
        }
    }
}

void viewpatients()
{
    int i, j, count;
    char gender[8];

    printf("\n========== ALL PATIENTS ==========\n");
    printf("%-6s %-20s %-4s %-7s %-12s %s\n",
           "ID", "Name", "Age", "Gender", "Phone", "Appointments");
    printf("--------------------------------------------------------------------\n");
    for (i = 0; i < patientcount; i++) {
        if (patients[i].gender == 1)
            strcpy(gender, "Male");
        else if (patients[i].gender == 2)
            strcpy(gender, "Female");
        else
            strcpy(gender, "Other");
        count = 0;
        for (j = 0; j < appointmentcount; j++) {
            if (appointments[j].patientid == patients[i].id)
                count++;
        }
        printf("%-6d %-20s %-4d %-7s %-12s %d\n",
               patients[i].id, patients[i].name, patients[i].age,
               gender, patients[i].phone, count);
    }
    if (patientcount == 0)
        printf("No patients registered yet.\n");
    else
        printf("\nTotal patients: %d\n", patientcount);
}

int listappointments(int type, char text[])
{
    int i, match, found = 0;

    printf("\n%-5s %-6s %-18s %-11s %-9s %-7s %-10s %s\n",
           "ID", "Pat.", "Doctor", "Date", "Time", "Fee", "Status", "Payment");
    printf("------------------------------------------------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        match = 0;
        if (type == 0)
            match = 1;
        else if (type == 1 && strcmp(appointments[i].status, text) == 0)
            match = 1;
        else if (type == 2 && strcmp(appointments[i].doctorid, text) == 0)
            match = 1;
        else if (type == 3 && strcmp(appointments[i].date, text) == 0)
            match = 1;
        if (match) {
            printf("%-5s %-6d %-18s %-11s %-9s %-7d %-10s %s\n",
                   appointments[i].id, appointments[i].patientid,
                   appointments[i].doctorname, appointments[i].date,
                   appointments[i].time, appointments[i].fee,
                   appointments[i].status, appointments[i].payment);
            found++;
        }
    }
    if (found == 0)
        printf("No appointments found.\n");
    else
        printf("\nTotal: %d\n", found);
    return found;
}

void viewappointments()
{
    char text[15];
    char statuses[5][10] = {"Pending", "Accepted", "Confirmed", "Completed", "Cancelled"};
    int choice, i;

    while (1) {
        printf("\n========== VIEW APPOINTMENTS ==========\n");
        printf("1. All appointments\n");
        printf("2. By status\n");
        printf("3. By doctor\n");
        printf("4. By date\n");
        printf("0. Back\n");
        printf("Enter choice: ");
        choice = getnumber();

        if (choice == 0)
            return;
        if (choice == 1) {
            listappointments(0, "");
        } else if (choice == 2) {
            for (i = 0; i < 5; i++)
                printf("%d. %s\n", i + 1, statuses[i]);
            printf("Choose status: ");
            choice = getnumber();
            if (choice < 1 || choice > 5)
                printf("Invalid choice.\n");
            else
                listappointments(1, statuses[choice - 1]);
        } else if (choice == 3) {
            printf("Enter Doctor ID: ");
            scanf("%9s", text);
            clearinput();
            touppercase(text);
            listappointments(2, text);
        } else if (choice == 4) {
            printf("Enter date (dd-mm-yyyy): ");
            scanf("%14s", text);
            clearinput();
            listappointments(3, text);
        } else {
            printf("Invalid choice.\n");
        }
    }
}

void viewcancelled()
{
    printf("\n========== CANCELLED APPOINTMENTS ==========");
    listappointments(1, "Cancelled");
}

void showschedules()
{
    int i;

    printf("\n%-4s %-18s %-14s %-20s %s\n", "ID", "Name", "Specialization", "Days", "Timing");
    printf("--------------------------------------------------------------------------\n");
    for (i = 0; i < doctorcount; i++) {
        printf("%-4s %-18s %-14s %-20s %s\n",
               doctors[i].id, doctors[i].name, doctors[i].specialization,
               doctors[i].days, doctors[i].timing);
    }
    if (doctorcount == 0)
        printf("No doctors found.\n");
}

void manageschedules()
{
    char days[30], timing[15];
    int d, choice;

    while (1) {
        printf("\n========== MANAGE DOCTOR SCHEDULES ==========\n");
        printf("1. View all schedules\n");
        printf("2. Change a doctor's available days\n");
        printf("3. Change a doctor's time slots\n");
        printf("0. Back\n");
        printf("Enter choice: ");
        choice = getnumber();

        if (choice == 0)
            return;
        if (choice == 1) {
            showschedules();
        } else if (choice == 2 || choice == 3) {
            showschedules();
            d = askdoctor("\nEnter Doctor ID: ");
            if (d == -1) {
                printf("Doctor not found.\n");
                continue;
            }
            if (choice == 2) {
                if (pickdays(days)) {
                    strcpy(doctors[d].days, days);
                    savedoctors();
                    printf("Available days updated.\n");
                }
            } else {
                picktiming(timing);
                strcpy(doctors[d].timing, timing);
                savedoctors();
                printf("Time slots updated.\n");
            }
        } else {
            printf("Invalid choice.\n");
        }
    }
}

int listpayments(char status[])
{
    int i, found = 0;

    printf("\n%-5s %-6s %-7s %-10s %-12s %-10s %s\n",
           "ID", "Pat.", "Fee", "Method", "Transaction", "Payment", "Appointment");
    printf("--------------------------------------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(status, "All") == 0 || strcmp(appointments[i].payment, status) == 0) {
            printf("%-5s %-6d %-7d %-10s %-12s %-10s %s\n",
                   appointments[i].id, appointments[i].patientid,
                   appointments[i].fee, appointments[i].method,
                   appointments[i].transaction, appointments[i].payment,
                   appointments[i].status);
            found++;
        }
    }
    if (found == 0)
        printf("No payment records found.\n");
    return found;
}

void paymentsummary()
{
    int i;
    int verified = 0, refunded = 0, pendingamount = 0;

    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].payment, "Verified") == 0)
            verified = verified + appointments[i].fee;
        else if (strcmp(appointments[i].payment, "Refunded") == 0)
            refunded = refunded + appointments[i].fee;
        else if (strcmp(appointments[i].payment, "Pending") == 0 &&
                 strcmp(appointments[i].status, "Cancelled") != 0)
            pendingamount = pendingamount + appointments[i].fee;
    }
    printf("\n========== PAYMENT SUMMARY ==========\n");
    printf("Verified (revenue) : Rs. %d\n", verified);
    printf("Refunded           : Rs. %d\n", refunded);
    printf("Waiting to be paid : Rs. %d\n", pendingamount);
}

void changepayment()
{
    char id[10];
    int i, a = -1, choice;

    printf("\nPayments that are still pending:\n");
    printf("%-5s %-6s %-7s %-10s %s\n", "ID", "Pat.", "Fee", "Payment", "Appointment");
    printf("------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].payment, "Pending") == 0 &&
            strcmp(appointments[i].status, "Cancelled") != 0 &&
            strcmp(appointments[i].status, "Completed") != 0) {
            printf("%-5s %-6d %-7d %-10s %s\n",
                   appointments[i].id, appointments[i].patientid,
                   appointments[i].fee, appointments[i].payment,
                   appointments[i].status);
            a = i;
        }
    }
    if (a == -1) {
        printf("No pending payments.\n");
        return;
    }

    printf("\nEnter Appointment ID: ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);

    a = -1;
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].id, id) == 0 &&
            strcmp(appointments[i].payment, "Pending") == 0 &&
            strcmp(appointments[i].status, "Cancelled") != 0 &&
            strcmp(appointments[i].status, "Completed") != 0)
            a = i;
    }
    if (a == -1) {
        printf("Appointment not found in the pending list.\n");
        return;
    }

    printf("1. Mark payment as Verified (appointment becomes Confirmed)\n");
    printf("2. Mark payment as Failed (appointment is cancelled)\n");
    printf("0. Back\nChoose: ");
    choice = getnumber();
    if (choice == 1) {
        strcpy(appointments[a].payment, "Verified");
        strcpy(appointments[a].method, "Counter");
        strcpy(appointments[a].transaction, "ByAdmin");
        strcpy(appointments[a].status, "Confirmed");
        saveappointments();
        printf("Payment verified. Appointment %s is now Confirmed.\n", appointments[a].id);
    } else if (choice == 2) {
        strcpy(appointments[a].payment, "Failed");
        strcpy(appointments[a].status, "Cancelled");
        saveappointments();
        printf("Payment marked as failed. Appointment %s is cancelled.\n", appointments[a].id);
    }
}

void managepayments()
{
    char statuses[4][10] = {"Pending", "Verified", "Failed", "Refunded"};
    int choice, i;

    while (1) {
        printf("\n========== MANAGE PAYMENT RECORDS ==========\n");
        printf("1. View all payments\n");
        printf("2. View payments by status\n");
        printf("3. Verify or fail a pending payment\n");
        printf("4. Payment summary\n");
        printf("0. Back\n");
        printf("Enter choice: ");
        choice = getnumber();

        if (choice == 0)
            return;
        if (choice == 1) {
            listpayments("All");
        } else if (choice == 2) {
            for (i = 0; i < 4; i++)
                printf("%d. %s\n", i + 1, statuses[i]);
            printf("Choose status: ");
            choice = getnumber();
            if (choice < 1 || choice > 4)
                printf("Invalid choice.\n");
            else
                listpayments(statuses[choice - 1]);
        } else if (choice == 3) {
            changepayment();
        } else if (choice == 4) {
            paymentsummary();
        } else {
            printf("Invalid choice.\n");
        }
    }
}

int counttoday(char status[], int today)
{
    int i, count = 0;
    for (i = 0; i < appointmentcount; i++) {
        if (datenumber(appointments[i].date) == today &&
            strcmp(appointments[i].status, status) == 0)
            count++;
    }
    return count;
}

void systemstatistics()
{
    int i, today = todaynumber();
    int todaycount = 0, revenue = 0;

    for (i = 0; i < appointmentcount; i++) {
        if (datenumber(appointments[i].date) == today)
            todaycount++;
        if (strcmp(appointments[i].payment, "Verified") == 0)
            revenue = revenue + appointments[i].fee;
    }

    printf("\n===== SYSTEM STATISTICS =====\n\n");
    printf("Total Patients       : %d\n", patientcount);
    printf("Total Doctors        : %d\n", doctorcount);
    printf("Doctors Waiting      : %d\n", pendingcount);
    printf("Total Appointments   : %d\n", appointmentcount);
    printf("Appointments Today   : %d\n", todaycount);
    printf("  Pending            : %d\n", counttoday("Pending", today));
    printf("  Accepted           : %d\n", counttoday("Accepted", today));
    printf("  Confirmed          : %d\n", counttoday("Confirmed", today));
    printf("  Completed          : %d\n", counttoday("Completed", today));
    printf("  Cancelled          : %d\n", counttoday("Cancelled", today));
    printf("\nTotal Revenue        : Rs. %d\n", revenue);
}

int adminlogin()
{
    char username[30], password[30];
    int attempt;

    printf("\n========== ADMIN LOGIN ==========\n");
    for (attempt = 1; attempt <= 3; attempt++) {
        printf("Username: ");
        scanf("%29s", username);
        clearinput();
        printf("Password: ");
        scanf("%29s", password);
        clearinput();
        if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
            printf("\nLogin successful! Welcome Admin\n");
            return 1;
        }
        printf("Wrong username or password.\n");
    }
    return 0;
}

int main()
{
    int choice;

    loadpatients();
    loaddoctors();
    loadappointments();
    loadaccounts();
    loadpending();
    loadcategories();
    makemissingaccounts();

    if (!adminlogin())
        return 0;

    while (1) {
        printf("\n========== ADMIN DASHBOARD ==========\n");
        printf("1. Add doctor\n");
        printf("2. Edit doctor\n");
        printf("3. Remove doctor\n");
        printf("4. Verify doctors\n");
        printf("5. Manage categories\n");
        printf("6. View patients\n");
        printf("7. View appointments\n");
        printf("8. View cancelled appointments\n");
        printf("9. Manage doctor schedules\n");
        printf("10. Manage payment records\n");
        printf("11. View system statistics\n");
        printf("0. Logout\n");
        printf("Enter choice: ");
        choice = getnumber();

        switch (choice) {
            case 1:  adddoctor();         break;
            case 2:  editdoctor();        break;
            case 3:  removedoctor();      break;
            case 4:  verifydoctors();     break;
            case 5:  managecategories();  break;
            case 6:  viewpatients();      break;
            case 7:  viewappointments();  break;
            case 8:  viewcancelled();     break;
            case 9:  manageschedules();   break;
            case 10: managepayments();    break;
            case 11: systemstatistics();  break;
            case 0:
                printf("Logged out.\n");
                return 0;
            default: printf("Invalid choice.\n");
        }
    }
}
