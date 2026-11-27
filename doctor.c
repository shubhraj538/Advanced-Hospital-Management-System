/*
 * HIMS - Hospital Information Management System
 * PART 5 : DOCTOR SIDE
 * The payment of the patient confirms the appointment, so the doctor
 * can view his appointments, reject (cancel) one, and mark it as completed.
 */

/* ===================== ACCOUNT ===================== */

/* asks for all the details of a new doctor (used by the registration and by the admin).
 * d.id stays empty if it did not work. */
struct doctor getdoctordetails()
{
    struct doctor d;

    d.id[0] = '\0';
    getname("Enter full name: ", d.name, 50);
    if (doctornameexists(d.name)) {
        printf("A doctor with this name already exists.\n");
        return d;
    }
    if (!pickcategory(d.specialization))
        return d;
    d.experience = getrange("Years of experience: ", 0, 60);
    getnospace("Qualification (e.g. MBBS): ", d.qualification, 20);
    getnospace("Hospital/clinic: ", d.hospital, 20);
    d.fee = getrange("Consultation fee (Rs.): ", 100, 100000);
    while (!pickdays(d.days) && !feof(stdin))
        ;
    picktiming(d.timing);
    getpassword(d.password);
    newdoctorid(d.id);
    d.rating = 0.0;
    return d;
}

/* A new doctor goes to the pending list. The admin has to verify him before he can login. */
void registerdoctor()
{
    struct doctor d;

    if (pendingcount >= MAXPENDING) {
        printf("Registration is full right now. Please contact the admin.\n");
        return;
    }
    printf("\n========== DOCTOR REGISTRATION ==========\n");
    d = getdoctordetails();
    if (d.id[0] == '\0')
        return;
    pending[pendingcount++] = d;
    savepending();
    printf("\nRegistration submitted!\nYour Doctor ID is %s (you need it to login)\n", d.id);
    printf("You can login after the admin verifies your account.\n");
}

/* returns the doctor's position in the array, or -1 if login failed */
int logindoctor()
{
    char id[10], pass[50];
    int i;

    printf("\n========== DOCTOR LOGIN ==========\n");
    printf("Enter Doctor ID (e.g. D1): ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);
    printf("Enter password: ");
    readpassword(pass);

    i = finddoctorindex(id);
    if (i != -1 && strcmp(doctors[i].password, pass) == 0) {
        printf("\nLogin successful! Welcome %s\n", doctors[i].name);
        return i;
    }
    for (i = 0; i < pendingcount; i++) {    /* maybe he registered but is still waiting */
        if (strcmp(pending[i].id, id) == 0 && strcmp(pending[i].password, pass) == 0) {
            printf("Your registration is still waiting for admin verification.\n");
            return -1;
        }
    }
    printf("Wrong ID or password.\n");
    return -1;
}

/* ===================== PROFILE ===================== */

void doctorviewprofile(int d)
{
    printf("\n========== MY PROFILE ==========\n");
    printf("Doctor ID        : %s\nName             : %s\n", doctors[d].id, doctors[d].name);
    printf("Specialization   : %s\n", doctors[d].specialization);
    printf("Experience       : %d years\n", doctors[d].experience);
    printf("Qualification    : %s\nHospital         : %s\n", doctors[d].qualification, doctors[d].hospital);
    printf("Consultation fee : Rs. %d\nRating           : %.1f / 5\n", doctors[d].fee, doctors[d].rating);
    printf("Available days   : %s\nTiming           : %s\n", doctors[d].days, doctors[d].timing);
}

void editprofile(int d)
{
    char text[50];
    int choice;

    while (1) {
        printf("\n========== EDIT PROFILE ==========\n");
        printf("1. Name\n2. Years of experience\n3. Qualification\n4. Hospital/clinic\n0. Back\n");
        choice = getrange("Enter choice: ", 0, 4);

        if (choice == 0)
            return;
        if (choice == 1) {
            getname("Enter new name: ", text, 50);
            if (strcmp(text, doctors[d].name) != 0 && doctornameexists(text))
                printf("A doctor with this name already exists.\n");
            else
                updatedoctorname(d, text);
        } else if (choice == 2)
            doctors[d].experience = getrange("Enter years of experience: ", 0, 60);
        else if (choice == 3)
            getnospace("Enter new qualification: ", doctors[d].qualification, 20);
        else
            getnospace("Enter hospital/clinic: ", doctors[d].hospital, 20);
        savedoctors();
        printf("Profile updated.\n");
    }
}

void setspecialization(int d)
{
    printf("\nCurrent specialization: %s\n", doctors[d].specialization);
    if (pickcategory(doctors[d].specialization)) {
        savedoctors();
        printf("Specialization updated.\n");
    }
}

/* the new fee is used for new bookings only */
void setfee(int d)
{
    printf("\nCurrent fee: Rs. %d\n", doctors[d].fee);
    doctors[d].fee = getrange("Enter new consultation fee (Rs.): ", 100, 100000);
    savedoctors();
    printf("Fee updated.\n");
}

void setdays(int d)
{
    printf("\nCurrent days: %s\n", doctors[d].days);
    if (pickdays(doctors[d].days)) {
        savedoctors();
        printf("Available days updated.\n");
    }
}

void settiming(int d)
{
    printf("\nCurrent timing: %s\n", doctors[d].timing);
    picktiming(doctors[d].timing);
    savedoctors();
    printf("Time slots updated.\n");
}

/* ===================== APPOINTMENTS ===================== */

/* open = Pending or Confirmed */
int isopen(int i)
{
    return strcmp(appointments[i].status, "Pending") == 0 ||
           strcmp(appointments[i].status, "Confirmed") == 0;
}

/* filter for the doctor's appointment list:
 * 0 = all, 1 = can be cancelled (open), 2 = can be completed (paid and the date has come) */
int inmode(int i, int mode)
{
    if (mode == 0)
        return 1;
    if (mode == 1)
        return isopen(i);
    return strcmp(appointments[i].status, "Confirmed") == 0 &&
           datenumber(appointments[i].date) <= todaynumber();
}

/* shows this doctor's appointments that match the mode, returns how many */
int showlist(int d, int mode)
{
    int i, p, found = 0;

    printf("\n%-5s %-6s %-18s %-11s %-9s %-10s %s\n",
           "ID", "Pat.", "Patient", "Date", "Time", "Status", "Payment");
    printf("-----------------------------------------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].doctorid, doctors[d].id) == 0 && inmode(i, mode)) {
            p = findpatientindex(appointments[i].patientid);
            printf("%-5s %-6d %-18s %-11s %-9s %-10s %s\n", appointments[i].id,
                   appointments[i].patientid, p == -1 ? "Unknown" : patients[p].name,
                   appointments[i].date, appointments[i].time, appointments[i].status,
                   appointments[i].payment);
            found++;
        }
    }
    if (found == 0)
        printf("No appointments found.\n");
    return found;
}

/* shows the list and lets the doctor type an appointment id. Gives its position, or -1. */
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
            strcmp(appointments[i].doctorid, doctors[d].id) == 0 && inmode(i, mode))
            return i;
    }
    printf("Appointment not found in the list.\n");
    return -1;
}

void viewappointments(int d)
{
    printf("\n========== MY APPOINTMENTS ==========");
    showlist(d, 0);
}

/* the doctor rejects (cancels) an appointment, a paid one is refunded */
void cancelappointment(int d)
{
    int a;

    printf("\n========== REJECT / CANCEL AN APPOINTMENT ==========");
    a = pickappointment(d, 1, "cancel");
    if (a != -1 && getrange("Cancel this appointment? (1 = yes, 0 = no): ", 0, 1) == 1) {
        cancelrecord(a);
        printf("Appointment %s cancelled.\n", appointments[a].id);
    }
}

/* only paid appointments whose date has come can be completed */
void markcompleted(int d)
{
    int a;

    printf("\n========== MARK AS COMPLETED ==========");
    a = pickappointment(d, 2, "mark as completed");
    if (a != -1) {
        strcpy(appointments[a].status, "Completed");    /* now the patient can rate the doctor */
        saveappointments();
        printf("Appointment %s is completed. The patient can now rate you.\n", appointments[a].id);
    }
}

/* shows one patient's appointments with this doctor only */
void patienthistory(int d)
{
    int i, p, found = 0;
    char gender[3][8] = {"Male", "Female", "Other"};

    p = findpatientindex(getrange("\nEnter Patient ID: ", 1, 999999));
    if (p == -1) {
        printf("Patient not found.\n");
        return;
    }
    printf("\n========== PATIENT HISTORY ==========\n");
    printf("Name   : %s\nAge    : %d\nGender : %s\n", patients[p].name, patients[p].age,
           gender[patients[p].gender - 1]);
    printf("\n%-5s %-11s %-9s %-10s %-9s %s\n", "ID", "Date", "Time", "Status", "Fee", "Payment");
    printf("----------------------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (appointments[i].patientid == patients[p].id &&
            strcmp(appointments[i].doctorid, doctors[d].id) == 0) {
            printf("%-5s %-11s %-9s %-10s %-9d %s\n", appointments[i].id, appointments[i].date,
                   appointments[i].time, appointments[i].status, appointments[i].fee,
                   appointments[i].payment);
            found++;
        }
    }
    if (found == 0)
        printf("This patient has no appointments with you.\n");
}

/* ===================== REVIEWS AND MENU ===================== */

void doctorreviews(int d)
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
}

void doctordashboard(int d)
{
    int choice;

    while (1) {
        printf("\n========== DOCTOR DASHBOARD ==========\n");
        printf("Logged in as: %s\n", doctors[d].name);
        printf("1. View my profile\n2. Edit profile\n3. Set specialization\n");
        printf("4. Set consultation fee\n5. Set available days\n6. Set available time slots\n");
        printf("7. View my appointments\n8. Reject / cancel an appointment\n");
        printf("9. Mark appointment as completed\n10. View patient appointment history\n");
        printf("11. View my ratings and reviews\n0. Logout\n");
        choice = getrange("Enter choice: ", 0, 11);

        switch (choice) {
            case 1:  doctorviewprofile(d);   break;
            case 2:  editprofile(d);         break;
            case 3:  setspecialization(d);   break;
            case 4:  setfee(d);              break;
            case 5:  setdays(d);             break;
            case 6:  settiming(d);           break;
            case 7:  viewappointments(d);    break;
            case 8:  cancelappointment(d);   break;
            case 9:  markcompleted(d);       break;
            case 10: patienthistory(d);      break;
            case 11: doctorreviews(d);       break;
            case 0:
                printf("Logged out.\n");
                return;
        }
    }
}
