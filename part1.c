/*
 * HIMS - Hospital Information Management System
 * PART 1 : PATIENT SIDE (Account + Find a doctor + Doctor profile)
 *
 * doctors.txt  -> doctor records (already written, the program only reads it)
 * patients.txt -> patient records (made when someone registers)
 */

#include <stdio.h>
#include <string.h>

/* ===================== STRUCTURES ===================== */

struct patient {
    int  id;
    char name[50];
    char password[20];
    int  age;
    int  gender;        /* 1 = Male, 2 = Female, 3 = Other */
    char phone[20];
};

struct doctor {
    char  id[5];
    char  name[50];
    char  specialization[20];
    int   experience;
    char  qualification[20];
    char  hospital[20];
    int   fee;
    float rating;
    char  days[30];
    char  timing[15];
};

struct patient patients[100];
int patientcount = 0;

struct doctor doctors[50];
int doctorcount = 0;

/* ===================== INPUT HELPERS ===================== */

/* Throws away everything left on the line the user typed (up to Enter).
 * scanf leaves the Enter key behind, which would mess up the next input. */
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
        if (feof(stdin))                /* input closed (Ctrl+Z) */
            return 0;
        printf("Please enter a number: ");
        clearinput();
    }
    clearinput();
    return n;
}

/* Reads a full line like "Ali Khan" (scanf stops at spaces, fgets doesn't) */
void getline_text(char text[], int size)
{
    int len;
    fgets(text, size, stdin);
    len = strlen(text);
    if (len > 0 && text[len - 1] == '\n')
        text[len - 1] = '\0';           /* remove the Enter at the end */
}

/* ===================== FILES ===================== */

void loaddoctors()
{
    FILE *fp = fopen("doctors.txt", "r");
    struct doctor d;
    int len;

    if (fp == NULL) {
        printf("Error: doctors.txt not found!\n");
        return;
    }

    doctorcount = 0;
    /* first 9 words of the line, then the name (which has spaces) */
    while (fscanf(fp, "%s %s %d %s %s %d %f %s %s ",
                  d.id, d.specialization, &d.experience, d.qualification,
                  d.hospital, &d.fee, &d.rating, d.days, d.timing) == 9) {
        fgets(d.name, 50, fp);          /* rest of the line is the name */
        len = strlen(d.name);
        if (len > 0 && d.name[len - 1] == '\n')
            d.name[len - 1] = '\0';
        doctors[doctorcount] = d;
        doctorcount++;
    }
    fclose(fp);
}

void loadpatients()
{
    FILE *fp = fopen("patients.txt", "r");
    struct patient p;
    int len;

    patientcount = 0;
    if (fp == NULL)
        return;                         /* no one has registered yet */

    while (fscanf(fp, "%d %s %d %d %s ",
                  &p.id, p.password, &p.age, &p.gender, p.phone) == 5) {
        fgets(p.name, 50, fp);          /* rest of the line is the name */
        len = strlen(p.name);
        if (len > 0 && p.name[len - 1] == '\n')
            p.name[len - 1] = '\0';
        patients[patientcount] = p;
        patientcount++;
    }
    fclose(fp);
}

void savepatients()
{
    FILE *fp = fopen("patients.txt", "w");
    int i;

    for (i = 0; i < patientcount; i++) {
        fprintf(fp, "%d %s %d %d %s %s\n",
                patients[i].id, patients[i].password, patients[i].age,
                patients[i].gender, patients[i].phone, patients[i].name);
    }
    fclose(fp);
}

/* ===================== CHECKS ===================== */

/* phone must be 11 digits, e.g. 03001234567 */
int isvalidphone(char phone[])
{
    int i;
    if (strlen(phone) != 11)
        return 0;
    for (i = 0; i < 11; i++) {
        if (phone[i] < '0' || phone[i] > '9')
            return 0;
    }
    return 1;
}

/* returns 1 if another patient already has this phone */
int phoneexists(char phone[])
{
    int i;
    for (i = 0; i < patientcount; i++) {
        if (strcmp(patients[i].phone, phone) == 0)
            return 1;
    }
    return 0;
}

/* password must be 4 to 10 characters */
void getpassword(char pass[])
{
    char temp[50];
    while (1) {
        printf("Enter password (4-10 characters): ");
        scanf("%s", temp);
        clearinput();
        if (strlen(temp) > 3 && strlen(temp) <= 10)
            break;
        printf("Password must be 4 to 10 characters.\n");
    }
    strcpy(pass, temp);
}

void getphone(char phone[])
{
    while (1) {
        printf("Enter phone number (11 digits): ");
        scanf("%s", phone);
        clearinput();
        if (!isvalidphone(phone))
            printf("Invalid phone number.\n");
        else if (phoneexists(phone))
            printf("This phone number is already registered.\n");
        else
            break;
    }
}

/* ===================== ACCOUNT ===================== */

void registerpatient()
{
    struct patient p;

    printf("\n========== REGISTER ==========\n");

    printf("Enter full name: ");
    getline_text(p.name, 50);

    printf("Enter age: ");
    p.age = getnumber();
    while (p.age < 1 || p.age > 110) {
        printf("Invalid age. Enter again: ");
        p.age = getnumber();
    }

    printf("Gender: 1. Male  2. Female  3. Other\nChoose: ");
    p.gender = getnumber();
    while (p.gender < 1 || p.gender > 3) {
        printf("Invalid choice. Enter 1, 2 or 3: ");
        p.gender = getnumber();
    }

    getphone(p.phone);
    getpassword(p.password);

    p.id = 1001 + patientcount;         /* IDs: 1001, 1002, 1003 ... */
    patients[patientcount] = p;
    patientcount++;
    savepatients();

    printf("\nRegistered successfully! Welcome %s\n", p.name);
    printf("Your Patient ID is %d (you need it to login)\n", p.id);
}

/* returns the patient's position in the array, or -1 if login failed */
int loginpatient()
{
    int id, i;
    char pass[50];

    printf("\n========== LOGIN ==========\n");
    printf("Enter Patient ID: ");
    id = getnumber();
    printf("Enter password: ");
    scanf("%s", pass);
    clearinput();

    for (i = 0; i < patientcount; i++) {
        if (patients[i].id == id && strcmp(patients[i].password, pass) == 0) {
            printf("\nLogin successful! Welcome %s\n", patients[i].name);
            return i;
        }
    }
    printf("Wrong ID or password.\n");
    return -1;
}

void viewprofile(int i)
{
    printf("\n========== MY PROFILE ==========\n");
    printf("Patient ID : %d\n", patients[i].id);
    printf("Name       : %s\n", patients[i].name);
    printf("Age        : %d\n", patients[i].age);
    if (patients[i].gender == 1)      printf("Gender     : Male\n");
    else if (patients[i].gender == 2) printf("Gender     : Female\n");
    else                              printf("Gender     : Other\n");
    printf("Phone      : %s\n", patients[i].phone);
}

void changecontact(int i)
{
    char newphone[20];

    printf("\nCurrent phone: %s\n", patients[i].phone);
    getphone(newphone);
    strcpy(patients[i].phone, newphone);
    savepatients();
    printf("Phone number updated.\n");
}

void changeagegender(int i)
{
    printf("\nEnter new age: ");
    patients[i].age = getnumber();
    while (patients[i].age < 1 || patients[i].age > 110) {
        printf("Invalid age. Enter again: ");
        patients[i].age = getnumber();
    }

    printf("Gender: 1. Male  2. Female  3. Other\nChoose: ");
    patients[i].gender = getnumber();
    while (patients[i].gender < 1 || patients[i].gender > 3) {
        printf("Invalid choice. Enter 1, 2 or 3: ");
        patients[i].gender = getnumber();
    }

    savepatients();
    printf("Age and gender updated.\n");
}

void changepassword(int i)
{
    char old[50];

    printf("\nEnter current password: ");
    scanf("%s", old);
    clearinput();

    if (strcmp(old, patients[i].password) != 0) {
        printf("Wrong password.\n");
        return;
    }
    getpassword(patients[i].password);
    savepatients();
    printf("Password changed.\n");
}

/* ===================== FIND A DOCTOR ===================== */

void printheading()
{
    printf("\n%-4s %-18s %-14s %-8s %-6s %s\n",
           "ID", "Name", "Specialization", "Fee", "Rating", "Days");
    printf("--------------------------------------------------------------------\n");
}

void printdoctor(int i)
{
    printf("%-4s %-18s %-14s %-8d %-6.1f %s\n",
           doctors[i].id, doctors[i].name, doctors[i].specialization,
           doctors[i].fee, doctors[i].rating, doctors[i].days);
}

void showalldoctors()
{
    int i;
    printheading();
    for (i = 0; i < doctorcount; i++)
        printdoctor(i);
}

void searchbyname()
{
    char name[50];
    int i, found = 0;

    printf("Enter doctor name: ");
    getline_text(name, 50);

    printheading();
    for (i = 0; i < doctorcount; i++) {
        /* strstr checks if "name" appears inside the doctor's name */
        if (strstr(doctors[i].name, name) != NULL) {
            printdoctor(i);
            found++;
        }
    }
    if (found == 0)
        printf("No doctor found.\n");
}

void filterbyspecialization()
{
    char specs[9][20] = {"Cardiologist", "Dermatologist", "Pediatrician",
                         "Gynecologist", "Orthopedic", "Physician",
                         "Neurologist", "Dentist", "ENT"};
    int i, choice;

    for (i = 0; i < 9; i++)
        printf("%d. %s\n", i + 1, specs[i]);
    printf("Choose: ");
    choice = getnumber();
    if (choice < 1 || choice > 9) {
        printf("Invalid choice.\n");
        return;
    }

    printheading();
    for (i = 0; i < doctorcount; i++) {
        if (strcmp(doctors[i].specialization, specs[choice - 1]) == 0)
            printdoctor(i);
    }
}

void filterbyrating()
{
    float min;
    int i, found = 0;

    printf("Show doctors with rating above (e.g. 4.5): ");
    while (scanf("%f", &min) != 1) {
        printf("Please enter a number: ");
        clearinput();
    }
    clearinput();

    printheading();
    for (i = 0; i < doctorcount; i++) {
        if (doctors[i].rating >= min) {
            printdoctor(i);
            found++;
        }
    }
    if (found == 0)
        printf("No doctor found.\n");
}

void filterbyfee()
{
    int max, i, found = 0;

    printf("Enter your maximum budget (Rs.): ");
    max = getnumber();

    printheading();
    for (i = 0; i < doctorcount; i++) {
        if (doctors[i].fee <= max) {
            printdoctor(i);
            found++;
        }
    }
    if (found == 0)
        printf("No doctor found.\n");
}

void filterbyday()
{
    char days[7][4] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    int i, choice, found = 0;

    printf("1. Mon  2. Tue  3. Wed  4. Thu  5. Fri  6. Sat  7. Sun\nChoose day: ");
    choice = getnumber();
    if (choice < 1 || choice > 7) {
        printf("Invalid choice.\n");
        return;
    }

    printheading();
    for (i = 0; i < doctorcount; i++) {
        if (strstr(doctors[i].days, days[choice - 1]) != NULL) {
            printdoctor(i);
            found++;
        }
    }
    if (found == 0)
        printf("No doctor available on this day.\n");
}

void doctorprofile()
{
    char id[10];
    int i;

    printf("Enter Doctor ID (e.g. D1): ");
    scanf("%s", id);
    clearinput();

    for (i = 0; i < doctorcount; i++) {
        if (strcmp(doctors[i].id, id) == 0) {
            printf("\n========== DOCTOR PROFILE ==========\n");
            printf("Name             : %s\n", doctors[i].name);
            printf("Specialization   : %s\n", doctors[i].specialization);
            printf("Experience       : %d years\n", doctors[i].experience);
            printf("Qualification    : %s\n", doctors[i].qualification);
            printf("Hospital         : %s\n", doctors[i].hospital);
            printf("Consultation fee : Rs. %d\n", doctors[i].fee);
            printf("Rating           : %.1f / 5\n", doctors[i].rating);
            printf("Available days   : %s\n", doctors[i].days);
            printf("Timing           : %s\n", doctors[i].timing);
            return;
        }
    }
    printf("Doctor not found.\n");
}

void finddoctor()
{
    int choice;

    while (1) {
        printf("\n========== FIND A DOCTOR ==========\n");
        printf("1. View all doctors\n");
        printf("2. Search by name\n");
        printf("3. Filter by specialization\n");
        printf("4. Filter by rating\n");
        printf("5. Filter by consultation fee\n");
        printf("6. Filter by availability\n");
        printf("7. View doctor profile\n");
        printf("0. Back\n");
        printf("Enter choice: ");
        choice = getnumber();

        switch (choice) {
            case 1: showalldoctors();          break;
            case 2: searchbyname();            break;
            case 3: filterbyspecialization();  break;
            case 4: filterbyrating();          break;
            case 5: filterbyfee();             break;
            case 6: filterbyday();             break;
            case 7: doctorprofile();           break;
            case 0: return;
            default: printf("Invalid choice.\n");
        }
    }
}

/* ===================== MAIN ===================== */

void dashboard(int i)
{
    int choice;

    while (1) {
        printf("\n========== PATIENT DASHBOARD ==========\n");
        printf("Logged in as: %s\n", patients[i].name);
        printf("1. Find a doctor\n");
        printf("2. View my profile\n");
        printf("3. Change contact information\n");
        printf("4. Change age/gender\n");
        printf("5. Change password\n");
        printf("0. Logout\n");
        printf("Enter choice: ");
        choice = getnumber();

        switch (choice) {
            case 1: finddoctor();          break;
            case 2: viewprofile(i);        break;
            case 3: changecontact(i);      break;
            case 4: changeagegender(i);    break;
            case 5: changepassword(i);     break;
            case 0:
                printf("Logged out.\n");
                return;
            default: printf("Invalid choice.\n");
        }
    }
}

int main()
{
    int choice, i;

    loaddoctors();
    loadpatients();

    while (1) {
        printf("\n===== HOSPITAL INFORMATION MANAGEMENT SYSTEM =====\n");
        printf("1. Login\n");
        printf("2. Register\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        choice = getnumber();

        switch (choice) {
            case 1:
                i = loginpatient();
                if (i != -1)
                    dashboard(i);
                break;
            case 2:
                registerpatient();
                break;
            case 0:
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
}
