/*
 * HIMS - Hospital Information Management System
 * PART 1 : PATIENT SIDE (Account + Find a doctor + Doctor profile)
 */

/* ===================== ACCOUNT ===================== */

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

/* returns 1 if another patient already has this phone
 * (skip = this patient's own position, so his own number is not counted) */
int phoneexists(char phone[], int skip)
{
    int i;
    for (i = 0; i < patientcount; i++) {
        if (i != skip && strcmp(patients[i].phone, phone) == 0)
            return 1;
    }
    return 0;
}

/* keeps asking until the phone is valid and not used by someone else */
void getphone(char phone[], int skip)
{
    while (1) {
        printf("Enter phone number (11 digits): ");
        scanf("%19s", phone);
        clearinput();
        if (!isvalidphone(phone))
            printf("Invalid phone number.\n");
        else if (phoneexists(phone, skip))
            printf("This phone number is already registered.\n");
        else
            return;
        if (feof(stdin))
            return;
    }
}

void registerpatient()
{
    struct patient p;

    if (patientcount >= MAXPATIENTS) {
        printf("The system cannot take more patients right now.\n");
        return;
    }
    printf("\n========== REGISTER ==========\n");
    getname("Enter full name: ", p.name, 50);
    p.age = getrange("Enter age: ", 1, 110);
    p.gender = getrange("Gender: 1. Male  2. Female  3. Other\nChoose: ", 1, 3);
    getphone(p.phone, -1);
    getpassword(p.password);

    p.id = 1001 + patientcount;     /* ids are 1001, 1002, 1003 ... */
    patients[patientcount++] = p;
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
    readpassword(pass);

    i = findpatientindex(id);
    if (i != -1 && strcmp(patients[i].password, pass) == 0) {
        printf("\nLogin successful! Welcome %s\n", patients[i].name);
        return i;
    }
    printf("Wrong ID or password.\n");
    return -1;
}

void patientprofile(int i)
{
    char gender[3][8] = {"Male", "Female", "Other"};

    printf("\n========== MY PROFILE ==========\n");
    printf("Patient ID : %d\n", patients[i].id);
    printf("Name       : %s\n", patients[i].name);
    printf("Age        : %d\n", patients[i].age);
    printf("Gender     : %s\n", gender[patients[i].gender - 1]);
    printf("Phone      : %s\n", patients[i].phone);
}

void changecontact(int i)
{
    char newphone[20];

    printf("\nCurrent phone: %s\n", patients[i].phone);
    getphone(newphone, i);
    strcpy(patients[i].phone, newphone);
    savepatients();
    printf("Phone number updated.\n");
}

void changeagegender(int i)
{
    patients[i].age = getrange("\nEnter new age: ", 1, 110);
    patients[i].gender = getrange("Gender: 1. Male  2. Female  3. Other\nChoose: ", 1, 3);
    savepatients();
    printf("Age and gender updated.\n");
}

void changepassword(int i)
{
    char old[50];

    printf("\nEnter current password: ");
    readpassword(old);
    if (strcmp(old, patients[i].password) != 0) {
        printf("Wrong password.\n");
        return;
    }
    getpassword(patients[i].password);
    savepatients();
    printf("Password changed.\n");
}

void accountmenu(int i)
{
    int choice;

    while (1) {
        printf("\n========== MY ACCOUNT ==========\n");
        printf("1. View my profile\n2. Change contact information\n");
        printf("3. Change age/gender\n4. Change password\n0. Back\n");
        choice = getrange("Enter choice: ", 0, 4);

        switch (choice) {
            case 1: patientprofile(i);   break;
            case 2: changecontact(i);    break;
            case 3: changeagegender(i);  break;
            case 4: changepassword(i);   break;
            case 0: return;
        }
    }
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

/* the filters below all do the same: print the doctors that match and say if none did */
void showalldoctors()
{
    int i;
    printheading();
    for (i = 0; i < doctorcount; i++)
        printdoctor(i);
}

/* part of the name is enough, capital letters don't matter */
void searchbyname()
{
    char name[50];
    int i, found = 0;

    printf("Enter doctor name: ");
    getline_text(name, 50);
    printheading();
    for (i = 0; i < doctorcount; i++) {
        if (findtext(doctors[i].name, name)) {
            printdoctor(i);
            found++;
        }
    }
    if (found == 0)
        printf("No doctor found.\n");
}

/* the list comes from categories.txt, so when the admin adds one it shows here too */
void filterbyspecialization()
{
    int i, choice, found = 0;

    if (categorycount == 0) {
        printf("No specializations are available yet.\n");
        return;
    }
    for (i = 0; i < categorycount; i++)
        printf("%d. %s\n", i + 1, categories[i]);
    choice = getrange("Choose: ", 1, categorycount);
    printheading();
    for (i = 0; i < doctorcount; i++) {
        if (strcmp(doctors[i].specialization, categories[choice - 1]) == 0) {
            printdoctor(i);
            found++;
        }
    }
    if (found == 0)
        printf("No doctor found.\n");
}

/* doctors with this rating or more */
void filterbyrating()
{
    float min;
    int i, found = 0;

    printf("Show doctors with rating of at least (e.g. 4.5): ");
    while (scanf("%f", &min) != 1) {
        if (feof(stdin))
            return;
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

/* doctors whose fee is not more than the patient's budget */
void filterbyfee()
{
    int max, i, found = 0;

    max = getrange("Enter your maximum budget (Rs.): ", 0, 1000000);
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

/* availability = doctors who work on the chosen day */
void filterbyday()
{
    int i, choice, found = 0;

    printf("1. Mon  2. Tue  3. Wed  4. Thu  5. Fri  6. Sat  7. Sun\n");
    choice = getrange("Choose day: ", 1, 7);
    printheading();
    for (i = 0; i < doctorcount; i++) {
        if (doctorworks(doctors[i].days, choice - 1)) {
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
    scanf("%9s", id);
    clearinput();
    touppercase(id);
    i = finddoctorindex(id);
    if (i == -1) {
        printf("Doctor not found.\n");
        return;
    }
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
}

void finddoctormenu()
{
    int choice;

    while (1) {
        printf("\n========== FIND A DOCTOR ==========\n");
        printf("1. View all doctors\n2. Search by name\n3. Filter by specialization\n");
        printf("4. Filter by rating\n5. Filter by consultation fee\n");
        printf("6. Filter by availability\n7. View doctor profile\n0. Back\n");
        choice = getrange("Enter choice: ", 0, 7);

        switch (choice) {
            case 1: showalldoctors();          break;
            case 2: searchbyname();            break;
            case 3: filterbyspecialization();  break;
            case 4: filterbyrating();          break;
            case 5: filterbyfee();             break;
            case 6: filterbyday();             break;
            case 7: doctorprofile();           break;
            case 0: return;
        }
    }
}
