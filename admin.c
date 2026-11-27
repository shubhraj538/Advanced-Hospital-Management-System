/*
 * HIMS - Hospital Information Management System
 * PART 6 : HOSPITAL / ADMIN SIDE
 * Admin login: username admin, password admin123
 */

/* ===================== DOCTORS ===================== */

/* full doctor table for the admin */
void adminshowdoctors()
{
    int i;

    printf("\n%-4s %-18s %-14s %-4s %-7s %-6s %-20s %s\n",
           "ID", "Name", "Specialization", "Exp", "Fee", "Rating", "Days", "Timing");
    printf("---------------------------------------------------------------------------------------\n");
    for (i = 0; i < doctorcount; i++)
        printf("%-4s %-18s %-14s %-4d %-7d %-6.1f %-20s %s\n", doctors[i].id, doctors[i].name,
               doctors[i].specialization, doctors[i].experience, doctors[i].fee,
               doctors[i].rating, doctors[i].days, doctors[i].timing);
    if (doctorcount == 0)
        printf("No doctors found.\n");
}

/* asks for a doctor id and gives his position (-1 if not found) */
int askdoctor(char prompt[])
{
    char id[10];

    printf("%s", prompt);
    scanf("%9s", id);
    clearinput();
    touppercase(id);
    return finddoctorindex(id);
}

/* Admin adds a doctor directly, so no verification is needed. */
void adddoctor()
{
    struct doctor d;

    if (doctorcount >= MAXDOCTORS) {
        printf("The doctor list is full.\n");
        return;
    }
    printf("\n========== ADD DOCTOR ==========\n");
    d = getdoctordetails();
    if (d.id[0] == '\0')
        return;
    doctors[doctorcount++] = d;
    savedoctors();
    printf("\nDoctor added. Doctor ID is %s\n", d.id);
}

void editdoctor()
{
    char text[50];
    int d, choice;

    adminshowdoctors();
    d = askdoctor("\nEnter Doctor ID to edit: ");
    if (d == -1) {
        printf("Doctor not found.\n");
        return;
    }
    while (1) {
        printf("\n========== EDIT %s ==========\n", doctors[d].name);
        printf("1. Name\n2. Specialization\n3. Years of experience\n");
        printf("4. Qualification\n5. Hospital/clinic\n6. Consultation fee\n0. Back\n");
        choice = getrange("Enter choice: ", 0, 6);

        if (choice == 0)
            return;
        if (choice == 1) {
            getname("Enter new name: ", text, 50);
            if (strcmp(text, doctors[d].name) != 0 && doctornameexists(text)) {
                printf("A doctor with this name already exists.\n");
                continue;
            }
            updatedoctorname(d, text);
        } else if (choice == 2) {
            if (!pickcategory(doctors[d].specialization))
                continue;
        } else if (choice == 3)
            doctors[d].experience = getrange("Enter years of experience: ", 0, 60);
        else if (choice == 4)
            getnospace("Enter new qualification: ", doctors[d].qualification, 20);
        else if (choice == 5)
            getnospace("Enter hospital/clinic: ", doctors[d].hospital, 20);
        else
            doctors[d].fee = getrange("Enter new consultation fee (Rs.): ", 100, 100000);
        savedoctors();
        printf("Doctor updated.\n");
    }
}

/* The open appointments of the doctor are cancelled (paid ones refunded),
 * then the doctor is deleted. */
void removedoctor()
{
    char id[10];
    int i, d, open = 0;

    adminshowdoctors();
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
    if (getrange("(1 = yes, 0 = no): ", 0, 1) != 1)
        return;

    /* the doctor is deleted first, so the waiting list cannot give his slots to someone else */
    strcpy(id, doctors[d].id);
    for (i = d; i < doctorcount - 1; i++)
        doctors[i] = doctors[i + 1];
    doctorcount--;
    savedoctors();
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].doctorid, id) == 0 && isopen(i))
            cancelrecord(i);
    }
    printf("Doctor removed.\n");
}

/* Doctors who registered by themselves wait here. Verify moves them to the real list. */
void verifydoctors()
{
    char id[10];
    int i, p = -1, choice;

    if (pendingcount == 0) {
        printf("\nNo doctors are waiting for verification.\n");
        return;
    }
    printf("\n========== DOCTORS WAITING FOR VERIFICATION ==========\n");
    printf("%-4s %-18s %-14s %-4s %-12s %-12s %s\n", "ID", "Name", "Specialization", "Exp", "Qualif.", "Hospital", "Fee");
    printf("-----------------------------------------------------------------------------------\n");
    for (i = 0; i < pendingcount; i++)
        printf("%-4s %-18s %-14s %-4d %-12s %-12s %d\n", pending[i].id, pending[i].name,
               pending[i].specialization, pending[i].experience, pending[i].qualification,
               pending[i].hospital, pending[i].fee);

    printf("\nEnter Doctor ID to review: ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);
    for (i = 0; i < pendingcount; i++) {
        if (strcmp(pending[i].id, id) == 0)
            p = i;
    }
    if (p == -1) {
        printf("Doctor not found in the waiting list.\n");
        return;
    }
    choice = getrange("1. Verify doctor\n2. Reject registration\n0. Back\nChoose: ", 0, 2);
    if (choice == 0 || (choice == 1 && doctorcount >= MAXDOCTORS))
        return;
    if (choice == 1) {
        doctors[doctorcount++] = pending[p];    /* move the doctor from the pending list to the real list */
        savedoctors();
    }
    for (i = p; i < pendingcount - 1; i++)
        pending[i] = pending[i + 1];
    pendingcount--;
    savepending();
    printf(choice == 1 ? "Doctor verified. He can login now and patients can book him.\n"
                       : "Registration rejected.\n");
}

/* ===================== CATEGORIES ===================== */

/* add or remove specializations (saved in categories.txt) */
void managecategories()
{
    char name[50];
    int i, choice, bad;

    while (1) {
        printf("\n========== MANAGE CATEGORIES ==========\n");
        for (i = 0; i < categorycount; i++)
            printf("%d. %s\n", i + 1, categories[i]);
        printf("\n1. Add category\n2. Remove category\n0. Back\n");
        choice = getrange("Enter choice: ", 0, 2);

        if (choice == 0)
            return;
        if (choice == 1) {
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
            for (i = 0; i < categorycount; i++) {
                if (findtext(categories[i], name) && findtext(name, categories[i]))
                    bad = 2;    /* each one is inside the other, so they are the same word */
            }
            if (bad == 1)
                printf("Use letters only, with no spaces.\n");
            else if (bad == 2)
                printf("This category already exists.\n");
            else {
                strcpy(categories[categorycount++], name);
                savecategories();
                printf("Category added.\n");
            }
        } else if (categorycount > 0) {
            choice = getrange("Enter number to remove (0 to go back): ", 0, categorycount);
            if (choice == 0)
                continue;
            bad = 0;    /* a category cannot be removed while a doctor uses it */
            for (i = 0; i < doctorcount; i++)
                bad += strcmp(doctors[i].specialization, categories[choice - 1]) == 0;
            for (i = 0; i < pendingcount; i++)
                bad += strcmp(pending[i].specialization, categories[choice - 1]) == 0;
            if (bad > 0)
                printf("Cannot remove: %d doctor(s) use this specialization.\n", bad);
            else {
                for (i = choice - 1; i < categorycount - 1; i++)
                    strcpy(categories[i], categories[i + 1]);
                categorycount--;
                savecategories();
                printf("Category removed.\n");
            }
        }
    }
}

/* ===================== PATIENTS AND APPOINTMENTS ===================== */

void viewpatients()
{
    char gender[3][8] = {"Male", "Female", "Other"};
    int i, j, count;

    printf("\n========== ALL PATIENTS ==========\n");
    printf("%-6s %-20s %-4s %-7s %-12s %s\n", "ID", "Name", "Age", "Gender", "Phone", "Appointments");
    printf("--------------------------------------------------------------------\n");
    for (i = 0; i < patientcount; i++) {
        count = 0;
        for (j = 0; j < appointmentcount; j++) {
            if (appointments[j].patientid == patients[i].id)
                count++;
        }
        printf("%-6d %-20s %-4d %-7s %-12s %d\n", patients[i].id, patients[i].name,
               patients[i].age, gender[patients[i].gender - 1], patients[i].phone, count);
    }
    printf("\nTotal patients: %d\n", patientcount);
}

/* status can be Pending, Confirmed, Completed, Cancelled or All */
void listappointments(char status[])
{
    int i, found = 0;

    printf("\n%-5s %-6s %-18s %-11s %-9s %-7s %-10s %s\n",
           "ID", "Pat.", "Doctor", "Date", "Time", "Fee", "Status", "Payment");
    printf("------------------------------------------------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(status, "All") == 0 || strcmp(appointments[i].status, status) == 0) {
            printf("%-5s %-6d %-18s %-11s %-9s %-7d %-10s %s\n", appointments[i].id,
                   appointments[i].patientid, appointments[i].doctorname, appointments[i].date,
                   appointments[i].time, appointments[i].fee, appointments[i].status,
                   appointments[i].payment);
            found++;
        }
    }
    if (found == 0)
        printf("No appointments found.\n");
    else
        printf("\nTotal: %d\n", found);
}

void adminappointments()
{
    printf("\n========== ALL APPOINTMENTS ==========");
    listappointments("All");
}

void viewcancelled()
{
    printf("\n========== CANCELLED APPOINTMENTS ==========");
    listappointments("Cancelled");
}

/* ===================== SCHEDULES ===================== */

void manageschedules()
{
    int i, d, choice;

    printf("\n========== DOCTOR SCHEDULES ==========\n");
    printf("%-4s %-18s %-30s %s\n", "ID", "Name", "Days", "Timing");
    for (i = 0; i < doctorcount; i++)
        printf("%-4s %-18s %-30s %s\n", doctors[i].id, doctors[i].name, doctors[i].days, doctors[i].timing);

    d = askdoctor("\nEnter Doctor ID to change (or any other text to go back): ");
    if (d == -1)
        return;
    choice = getrange("1. Change available days\n2. Change time slots\n0. Back\nChoose: ", 0, 2);
    if (choice == 1 && pickdays(doctors[d].days))
        savedoctors();
    else if (choice == 2) {
        picktiming(doctors[d].timing);
        savedoctors();
    }
}

/* ===================== PAYMENTS ===================== */

/* all payment records and the total revenue */
void listpayments()
{
    int i, revenue = 0;

    printf("\n%-5s %-6s %-7s %-10s %-12s %-10s %s\n",
           "ID", "Pat.", "Fee", "Method", "Transaction", "Payment", "Appointment");
    printf("--------------------------------------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        printf("%-5s %-6d %-7d %-10s %-12s %-10s %s\n", appointments[i].id,
               appointments[i].patientid, appointments[i].fee, appointments[i].method,
               appointments[i].transaction, appointments[i].payment, appointments[i].status);
        if (isrevenue(i))
            revenue = revenue + appointments[i].fee;
    }
    printf("\nTotal revenue (verified, not cancelled): Rs. %d\n", revenue);
}

/* Admin can verify or fail a pending payment by hand. */
void changepayment()
{
    char id[10];
    int i, a = -1, choice;

    printf("\nAppointments waiting for payment:\n");
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].status, "Pending") == 0) {
            printf("%-5s patient %-6d Rs. %d\n", appointments[i].id, appointments[i].patientid, appointments[i].fee);
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
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].id, id) == 0 && strcmp(appointments[i].status, "Pending") == 0)
            a = i;
    }
    if (strcmp(appointments[a].id, id) != 0) {
        printf("Appointment not found in the list.\n");
        return;
    }
    choice = getrange("1. Verify payment (appointment becomes Confirmed)\n2. Mark payment as Failed (appointment is cancelled)\n0. Back\nChoose: ", 0, 2);
    if (choice == 1) {
        strcpy(appointments[a].payment, "Verified");
        strcpy(appointments[a].method, "Counter");
        strcpy(appointments[a].transaction, "ByAdmin");     /* verified by hand, so no real transaction number */
        strcpy(appointments[a].status, "Confirmed");
        saveappointments();
        printf("Payment verified. Appointment %s is now Confirmed.\n", id);
    } else if (choice == 2) {
        strcpy(appointments[a].payment, "Failed");
        cancelrecord(a);
        printf("Payment failed. Appointment %s is cancelled.\n", id);
    }
}

void managepayments()
{
    int choice;

    while (1) {
        printf("\n========== MANAGE PAYMENT RECORDS ==========\n");
        printf("1. View all payment records\n2. Verify or fail a pending payment\n0. Back\n");
        choice = getrange("Enter choice: ", 0, 2);

        if (choice == 1)
            listpayments();
        else if (choice == 2)
            changepayment();
        else
            return;
    }
}

/* ===================== LOGIN AND MENU ===================== */

/* 3 tries for the admin password */
int adminlogin()
{
    char username[30], password[50];
    int attempt;

    printf("\n========== ADMIN LOGIN ==========\n");
    for (attempt = 1; attempt <= 3; attempt++) {
        printf("Username: ");
        scanf("%29s", username);
        clearinput();
        printf("Password: ");
        readpassword(password);
        if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {  /* fixed admin account */
            printf("\nLogin successful! Welcome Admin\n");
            return 1;
        }
        printf("Wrong username or password.\n");
        if (feof(stdin))
            break;
    }
    return 0;
}

void admindashboard()
{
    int choice;

    while (1) {
        printf("\n========== ADMIN DASHBOARD ==========\n");
        printf("1. Add doctor\n2. Edit doctor\n3. Remove doctor\n4. Verify doctors\n");
        printf("5. Manage categories\n6. View patients\n7. View appointments\n");
        printf("8. View cancelled appointments\n9. Manage doctor schedules\n");
        printf("10. Manage payment records\n11. System statistics\n");
        printf("12. Detailed statistics\n13. Reports\n0. Logout\n");
        choice = getrange("Enter choice: ", 0, 13);

        switch (choice) {
            case 1:  adddoctor();           break;
            case 2:  editdoctor();          break;
            case 3:  removedoctor();        break;
            case 4:  verifydoctors();       break;
            case 5:  managecategories();    break;
            case 6:  viewpatients();        break;
            case 7:  adminappointments();   break;
            case 8:  viewcancelled();       break;
            case 9:  manageschedules();     break;
            case 10: managepayments();      break;
            case 11: systemstatistics();    break;
            case 12: detailedstatistics();  break;
            case 13: reportsmenu();         break;
            case 0:
                printf("Logged out.\n");
                return;
        }
    }
}
