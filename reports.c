/*
 * HIMS - Hospital Information Management System
 * PART 12 : REPORTS (monthly appointments, revenue, patient registration)
 */

char monthnames[12][10] = {"January", "February", "March", "April", "May", "June",
                           "July", "August", "September", "October", "November", "December"};

/* month (which = 1) or year (which = 2) of a date like 25-11-2026 */
int datepart(char date[], int which)
{
    int d, m, y;
    if (sscanf(date, "%d-%d-%d", &d, &m, &y) != 3)
        return 0;
    return which == 1 ? m : y;
}

/* how many appointments of this month have this status */
int countstatusmonth(char status[], int month, int year)
{
    int i, count = 0;
    for (i = 0; i < appointmentcount; i++) {
        if (datepart(appointments[i].date, 1) == month && datepart(appointments[i].date, 2) == year &&
            strcmp(appointments[i].status, status) == 0)
            count++;
    }
    return count;
}

void monthlyreport()
{
    char names[4][10] = {"Pending", "Confirmed", "Completed", "Cancelled"};
    int i, j, month, year, total = 0, count, revenue = 0;

    month = getrange("\nEnter month (1-12): ", 1, 12);
    year = getrange("Enter year (e.g. 2026): ", 2000, 2100);
    for (i = 0; i < appointmentcount; i++) {
        if (datepart(appointments[i].date, 1) == month && datepart(appointments[i].date, 2) == year) {
            total++;
            if (isrevenue(i))
                revenue = revenue + appointments[i].fee;
        }
    }
    printf("\n===== MONTHLY APPOINTMENT REPORT: %s %d =====\n\n", monthnames[month - 1], year);
    printf("Total appointments : %d\n", total);
    for (i = 0; i < 4; i++)
        printf("%-19s: %d\n", names[i], countstatusmonth(names[i], month, year));
    printf("Revenue            : Rs. %d\n\nAppointments per doctor:\n", revenue);
    for (i = 0; i < doctorcount; i++) {
        count = 0;
        for (j = 0; j < appointmentcount; j++) {
            if (strcmp(appointments[j].doctorid, doctors[i].id) == 0 &&
                datepart(appointments[j].date, 1) == month && datepart(appointments[j].date, 2) == year)
                count++;
        }
        if (count > 0)
            printf("%-20s : %d\n", doctors[i].name, count);
    }
}

/* month by month revenue of one year, and the revenue of each doctor */
void revenuereport()
{
    int revenue[12] = {0};      /* one place for every month */
    int i, j, year, m, total = 0, sum;

    year = getrange("\nEnter year (e.g. 2026): ", 2000, 2100);
    for (i = 0; i < appointmentcount; i++) {
        m = datepart(appointments[i].date, 1);
        if (datepart(appointments[i].date, 2) == year && m >= 1 && m <= 12 && isrevenue(i)) {
            revenue[m - 1] = revenue[m - 1] + appointments[i].fee;
            total = total + appointments[i].fee;
        }
    }
    printf("\n===== REVENUE REPORT: %d =====\n\n", year);
    for (i = 0; i < 12; i++)
        printf("%-10s : Rs. %d\n", monthnames[i], revenue[i]);
    printf("-----------------------\n%-10s : Rs. %d\n\nRevenue per doctor:\n", "Total", total);
    for (i = 0; i < doctorcount; i++) {
        sum = 0;
        for (j = 0; j < appointmentcount; j++) {
            if (strcmp(appointments[j].doctorid, doctors[i].id) == 0 &&
                datepart(appointments[j].date, 2) == year && isrevenue(j))
                sum = sum + appointments[j].fee;
        }
        if (sum > 0)
            printf("%-20s : Rs. %d\n", doctors[i].name, sum);
    }
}

/* We do not save the registration date, so this report counts by gender and age group. */
void patientreport()
{
    int i, gender[3] = {0, 0, 0}, age[4] = {0, 0, 0, 0};

    for (i = 0; i < patientcount; i++) {
        gender[patients[i].gender - 1]++;
        if (patients[i].age < 18)
            age[0]++;
        else if (patients[i].age <= 30)
            age[1]++;
        else if (patients[i].age <= 50)
            age[2]++;
        else
            age[3]++;
    }
    printf("\n===== PATIENT REGISTRATION REPORT =====\n\n");
    printf("Total registered patients : %d\n\nBy gender:\n", patientcount);
    printf("  Male    : %d\n  Female  : %d\n  Other   : %d\n\n", gender[0], gender[1], gender[2]);
    printf("By age group:\n  Under 18 : %d\n  18 - 30  : %d\n", age[0], age[1]);
    printf("  31 - 50  : %d\n  Above 50 : %d\n", age[2], age[3]);
}

void reportsmenu()
{
    int choice;

    while (1) {
        printf("\n========== REPORTS ==========\n");
        printf("1. Monthly appointment report\n2. Revenue report\n");
        printf("3. Patient registration report\n0. Back\n");
        choice = getrange("Enter choice: ", 0, 3);

        switch (choice) {
            case 1: monthlyreport();  break;
            case 2: revenuereport();  break;
            case 3: patientreport();  break;
            case 0: return;
        }
    }
}
