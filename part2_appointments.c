#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAXPATIENTS 500
#define MAXDOCTORS 100
#define MAXAPPOINTMENTS 1000

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

struct patient patients[MAXPATIENTS];
int patientcount = 0;

struct doctor doctors[MAXDOCTORS];
int doctorcount = 0;

struct appointment appointments[MAXAPPOINTMENTS];
int appointmentcount = 0;

char shortdays[7][4] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
char fulldays[7][10] = {"Monday", "Tuesday", "Wednesday", "Thursday",
                        "Friday", "Saturday", "Sunday"};

int timepos = 0;
int slotstart = 0;
int slotend = 0;

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

int weekday(int datenum)
{
    int t[12] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    int y = datenum / 10000;
    int m = (datenum / 100) % 100;
    int d = datenum % 100;
    int w;

    if (m < 3)
        y = y - 1;
    w = (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
    return (w + 6) % 7;
}

int finddayposition(char days[], char name[])
{
    int j, len = strlen(days);
    for (j = 0; j + 2 < len; j++) {
        if (days[j] == name[0] && days[j + 1] == name[1] && days[j + 2] == name[2])
            return j;
    }
    return -1;
}

int doctorworks(char days[], int d)
{
    int i, pos, len = strlen(days);
    int hasrange = 0;
    int startday = -1, endday = -1, startpos = 1000, endpos = -1;

    for (i = 0; i < len; i++) {
        if (days[i] == '-')
            hasrange = 1;
    }
    if (!hasrange)
        return finddayposition(days, shortdays[d]) != -1;

    for (i = 0; i < 7; i++) {
        pos = finddayposition(days, shortdays[i]);
        if (pos != -1) {
            if (pos < startpos) {
                startpos = pos;
                startday = i;
            }
            if (pos > endpos) {
                endpos = pos;
                endday = i;
            }
        }
    }
    if (startday == -1)
        return 0;
    if (startday <= endday)
        return d >= startday && d <= endday;
    return d >= startday || d <= endday;
}

int readtime(char s[])
{
    int h = 0, m = 0;

    while (s[timepos] >= '0' && s[timepos] <= '9') {
        h = h * 10 + (s[timepos] - '0');
        timepos++;
    }
    if (s[timepos] == ':') {
        timepos++;
        while (s[timepos] >= '0' && s[timepos] <= '9') {
            m = m * 10 + (s[timepos] - '0');
            timepos++;
        }
    }
    if (s[timepos] == 'P' || s[timepos] == 'p') {
        if (h < 12)
            h = h + 12;
        timepos++;
    } else if (s[timepos] == 'A' || s[timepos] == 'a') {
        if (h == 12)
            h = 0;
        timepos++;
    }
    if (s[timepos] == 'M' || s[timepos] == 'm')
        timepos++;
    return h * 60 + m;
}

int readtiming(char timing[])
{
    timepos = 0;
    slotstart = readtime(timing);
    while (timing[timepos] != '\0' && (timing[timepos] < '0' || timing[timepos] > '9'))
        timepos++;
    if (timing[timepos] == '\0')
        return 0;
    slotend = readtime(timing);
    if (slotend <= slotstart && slotend < 12 * 60)
        slotend = slotend + 12 * 60;
    if (slotstart >= slotend || slotend > 24 * 60)
        return 0;
    return 1;
}

void timetext(int minutes, char out[])
{
    int h = minutes / 60;
    int m = minutes % 60;
    int h12 = h % 12;

    if (h12 == 0)
        h12 = 12;
    if (h >= 12)
        sprintf(out, "%02d:%02d%s", h12, m, "PM");
    else
        sprintf(out, "%02d:%02d%s", h12, m, "AM");
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

int isupcoming(int i)
{
    int n;

    if (strcmp(appointments[i].status, "Pending") != 0 &&
        strcmp(appointments[i].status, "Accepted") != 0 &&
        strcmp(appointments[i].status, "Confirmed") != 0)
        return 0;
    n = datenumber(appointments[i].date);
    if (n != 0 && n < todaynumber())
        return 0;
    return 1;
}

void cancelexpired()
{
    int i, n, changed = 0;
    int today = todaynumber();

    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].status, "Pending") == 0 ||
            strcmp(appointments[i].status, "Accepted") == 0) {
            n = datenumber(appointments[i].date);
            if (n != 0 && n < today) {
                strcpy(appointments[i].status, "Cancelled");
                strcpy(appointments[i].payment, "Failed");
                changed = 1;
            }
        }
    }
    if (changed)
        saveappointments();
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

int findappointment(char id[], int patientid)
{
    int i;
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].id, id) == 0 &&
            appointments[i].patientid == patientid)
            return i;
    }
    return -1;
}

void newappointmentid(char id[])
{
    int i, n, max = 0;
    for (i = 0; i < appointmentcount; i++) {
        if (sscanf(appointments[i].id, "A%d", &n) == 1 && n > max)
            max = n;
    }
    sprintf(id, "A%d", max + 1);
}

int slotbooked(char doctorid[], char date[], char slot[], int ignore)
{
    int i;
    for (i = 0; i < appointmentcount; i++) {
        if (i == ignore)
            continue;
        if (strcmp(appointments[i].doctorid, doctorid) == 0 &&
            strcmp(appointments[i].date, date) == 0 &&
            strcmp(appointments[i].time, slot) == 0 &&
            strcmp(appointments[i].status, "Cancelled") != 0)
            return 1;
    }
    return 0;
}

int patientbusy(int patientid, char date[], char slot[], int ignore)
{
    int i;
    for (i = 0; i < appointmentcount; i++) {
        if (i == ignore)
            continue;
        if (appointments[i].patientid == patientid &&
            strcmp(appointments[i].date, date) == 0 &&
            strcmp(appointments[i].time, slot) == 0 &&
            strcmp(appointments[i].status, "Cancelled") != 0)
            return 1;
    }
    return 0;
}

int loginpatient()
{
    int id, i;
    char pass[50];

    printf("\n========== PATIENT LOGIN ==========\n");
    printf("Enter Patient ID: ");
    id = getnumber();
    printf("Enter password: ");
    scanf("%49s", pass);
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

void showdoctors()
{
    int i;

    printf("\n%-4s %-18s %-14s %-7s %-6s %-20s %s\n",
           "ID", "Name", "Specialization", "Fee", "Rating", "Days", "Timing");
    printf("-------------------------------------------------------------------------------------\n");
    for (i = 0; i < doctorcount; i++) {
        printf("%-4s %-18s %-14s %-7d %-6.1f %-20s %s\n",
               doctors[i].id, doctors[i].name, doctors[i].specialization,
               doctors[i].fee, doctors[i].rating, doctors[i].days,
               doctors[i].timing);
    }
}

int askdate(char date[])
{
    int num;

    while (1) {
        printf("Enter date (dd-mm-yyyy, 0 to go back): ");
        scanf("%14s", date);
        clearinput();
        if (strcmp(date, "0") == 0)
            return 0;
        num = datenumber(date);
        if (num == 0)
            printf("Invalid date. Use the format dd-mm-yyyy, for example 25-11-2026.\n");
        else if (num < todaynumber())
            printf("You cannot choose a past date.\n");
        else
            return num;
    }
}

int chooseslot(int d, char date[], int ignore, char chosen[])
{
    char slots[48][10];
    int booked[48];
    int i, count, choice;
    int available = 0;

    if (!readtiming(doctors[d].timing)) {
        printf("This doctor's timing is not set properly.\n");
        return 0;
    }
    count = (slotend - slotstart) / 30;
    if (count > 48)
        count = 48;
    if (count < 1) {
        printf("This doctor's timing is not set properly.\n");
        return 0;
    }

    printf("\nSlots on %s with %s:\n", date, doctors[d].name);
    printf("%-4s %-9s %s\n", "No.", "Time", "Status");
    printf("-------------------------\n");
    for (i = 0; i < count; i++) {
        timetext(slotstart + i * 30, slots[i]);
        booked[i] = slotbooked(doctors[d].id, date, slots[i], ignore);
        if (booked[i]) {
            printf("%-4d %-9s Booked\n", i + 1, slots[i]);
        } else {
            printf("%-4d %-9s Available\n", i + 1, slots[i]);
            available++;
        }
    }
    if (available == 0) {
        printf("\nAll slots are booked on this date.\n");
        return 0;
    }

    printf("\nChoose slot number (0 to go back): ");
    choice = getnumber();
    while (choice < 0 || choice > count || (choice > 0 && booked[choice - 1])) {
        if (choice > 0 && choice <= count)
            printf("That slot is already booked. Choose another: ");
        else
            printf("Invalid choice. Enter again: ");
        choice = getnumber();
    }
    if (choice == 0)
        return 0;
    strcpy(chosen, slots[choice - 1]);
    return 1;
}

void bookappointment(int p)
{
    char id[10], date[15], slot[10];
    int d, a, datenum;

    if (appointmentcount >= MAXAPPOINTMENTS) {
        printf("The system cannot take more appointments right now.\n");
        return;
    }

    printf("\n========== BOOK AN APPOINTMENT ==========\n");
    showdoctors();
    printf("\nEnter Doctor ID (e.g. D1): ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);

    d = finddoctorindex(id);
    if (d == -1) {
        printf("Doctor not found.\n");
        return;
    }
    printf("\nDoctor : %s (%s)\n", doctors[d].name, doctors[d].specialization);
    printf("Fee    : Rs. %d\n", doctors[d].fee);
    printf("Days   : %s\n", doctors[d].days);

    datenum = askdate(date);
    if (datenum == 0)
        return;
    if (!doctorworks(doctors[d].days, weekday(datenum))) {
        printf("%s is not available on %s.\n", doctors[d].name, fulldays[weekday(datenum)]);
        printf("Available days: %s\n", doctors[d].days);
        return;
    }

    if (!chooseslot(d, date, -1, slot))
        return;
    if (patientbusy(patients[p].id, date, slot, -1)) {
        printf("You already have another appointment on %s at %s.\n", date, slot);
        return;
    }

    a = appointmentcount;
    newappointmentid(appointments[a].id);
    appointments[a].patientid = patients[p].id;
    strcpy(appointments[a].doctorid, doctors[d].id);
    appointments[a].fee = doctors[d].fee;
    strcpy(appointments[a].date, date);
    strcpy(appointments[a].time, slot);
    strcpy(appointments[a].status, "Pending");
    strcpy(appointments[a].payment, "Pending");
    strcpy(appointments[a].method, "None");
    strcpy(appointments[a].transaction, "None");
    strcpy(appointments[a].doctorname, doctors[d].name);
    appointmentcount++;
    saveappointments();

    printf("\n========== APPOINTMENT BOOKED ==========\n");
    printf("Appointment ID : %s\n", appointments[a].id);
    printf("Doctor         : %s\n", appointments[a].doctorname);
    printf("Date / Time    : %s  %s\n", appointments[a].date, appointments[a].time);
    printf("Fee            : Rs. %d\n", appointments[a].fee);
    printf("Status         : %s\n", appointments[a].status);
    printf("Payment        : %s\n", appointments[a].payment);
    printf("\nThe doctor will review your request. After that, pay from the payment menu.\n");
}

int showupcoming(int patientid)
{
    int i, found = 0;

    printf("\n%-5s %-18s %-11s %-9s %-7s %-10s %s\n",
           "ID", "Doctor", "Date", "Time", "Fee", "Status", "Payment");
    printf("---------------------------------------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (appointments[i].patientid == patientid && isupcoming(i)) {
            printf("%-5s %-18s %-11s %-9s %-7d %-10s %s\n",
                   appointments[i].id, appointments[i].doctorname,
                   appointments[i].date, appointments[i].time,
                   appointments[i].fee, appointments[i].status,
                   appointments[i].payment);
            found++;
        }
    }
    if (found == 0)
        printf("You have no upcoming appointments.\n");
    return found;
}

void showpast(int patientid)
{
    int i, d, found = 0;
    char spec[20];

    printf("\n%-5s %-11s %-9s %-18s %-14s %-10s %s\n",
           "ID", "Date", "Time", "Doctor", "Specialization", "Status", "Fee");
    printf("---------------------------------------------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (appointments[i].patientid == patientid && !isupcoming(i)) {
            d = finddoctorindex(appointments[i].doctorid);
            if (d == -1)
                strcpy(spec, "-");
            else
                strcpy(spec, doctors[d].specialization);
            printf("%-5s %-11s %-9s %-18s %-14s %-10s %d\n",
                   appointments[i].id, appointments[i].date, appointments[i].time,
                   appointments[i].doctorname, spec, appointments[i].status,
                   appointments[i].fee);
            found++;
        }
    }
    if (found == 0)
        printf("You have no past appointments.\n");
}

void cancelappointment(int patientid)
{
    char id[10];
    int a, choice;

    if (showupcoming(patientid) == 0)
        return;

    printf("\nEnter Appointment ID to cancel: ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);

    a = findappointment(id, patientid);
    if (a == -1 || !isupcoming(a)) {
        printf("Appointment not found in your upcoming list.\n");
        return;
    }

    printf("Cancel appointment %s with %s on %s at %s? (1 = yes, 0 = no): ",
           appointments[a].id, appointments[a].doctorname,
           appointments[a].date, appointments[a].time);
    choice = getnumber();
    if (choice != 1) {
        printf("Cancelled nothing. Appointment is still active.\n");
        return;
    }

    strcpy(appointments[a].status, "Cancelled");
    if (strcmp(appointments[a].payment, "Verified") == 0) {
        strcpy(appointments[a].payment, "Refunded");
        printf("Your payment of Rs. %d will be refunded.\n", appointments[a].fee);
    }
    saveappointments();
    printf("Appointment %s has been cancelled. The slot is free again.\n", appointments[a].id);
}

void rescheduleappointment(int patientid)
{
    char id[10], date[15], slot[10];
    int a, d, datenum;

    if (showupcoming(patientid) == 0)
        return;

    printf("\nEnter Appointment ID to reschedule: ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);

    a = findappointment(id, patientid);
    if (a == -1 || !isupcoming(a)) {
        printf("Appointment not found in your upcoming list.\n");
        return;
    }
    d = finddoctorindex(appointments[a].doctorid);
    if (d == -1) {
        printf("This doctor is no longer available. Please cancel the appointment instead.\n");
        return;
    }

    printf("\nCurrent booking: %s at %s with %s\n",
           appointments[a].date, appointments[a].time, appointments[a].doctorname);
    printf("Doctor's available days: %s\n", doctors[d].days);

    datenum = askdate(date);
    if (datenum == 0)
        return;
    if (!doctorworks(doctors[d].days, weekday(datenum))) {
        printf("%s is not available on %s.\n", doctors[d].name, fulldays[weekday(datenum)]);
        return;
    }
    if (!chooseslot(d, date, a, slot))
        return;
    if (patientbusy(patientid, date, slot, a)) {
        printf("You already have another appointment on %s at %s.\n", date, slot);
        return;
    }

    strcpy(appointments[a].date, date);
    strcpy(appointments[a].time, slot);
    saveappointments();
    printf("\nAppointment %s moved to %s at %s.\n", appointments[a].id, date, slot);
}

void dashboard(int p)
{
    int choice;

    while (1) {
        printf("\n========== MY APPOINTMENTS ==========\n");
        printf("Logged in as: %s\n", patients[p].name);
        printf("1. Book appointment\n");
        printf("2. Cancel appointment\n");
        printf("3. Reschedule appointment\n");
        printf("4. View upcoming appointments\n");
        printf("5. View past appointments\n");
        printf("0. Logout\n");
        printf("Enter choice: ");
        choice = getnumber();

        switch (choice) {
            case 1: bookappointment(p);                    break;
            case 2: cancelappointment(patients[p].id);     break;
            case 3: rescheduleappointment(patients[p].id); break;
            case 4: showupcoming(patients[p].id);          break;
            case 5: showpast(patients[p].id);              break;
            case 0:
                printf("Logged out.\n");
                return;
            default: printf("Invalid choice.\n");
        }
    }
}

int main()
{
    int choice, p;

    loaddoctors();
    loadpatients();
    loadappointments();
    cancelexpired();

    while (1) {
        printf("\n===== HOSPITAL INFORMATION MANAGEMENT SYSTEM =====\n");
        printf("1. Patient login\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        choice = getnumber();

        switch (choice) {
            case 1:
                p = loginpatient();
                if (p != -1)
                    dashboard(p);
                break;
            case 0:
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
}
