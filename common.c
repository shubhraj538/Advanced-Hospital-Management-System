/*
 * HIMS - Hospital Information Management System
 * HELPER FUNCTIONS USED EVERYWHERE
 * input, dates, reading/saving the text files, and a few shared checks
 */

/* ===================== INPUT HELPERS ===================== */

/* Throws away everything left on the line (up to Enter).
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
        if (feof(stdin))
            return 0;
        printf("Please enter a number: ");
        clearinput();
    }
    clearinput();
    return n;
}

/* Shows the prompt and keeps asking until the number is between min and max */
int getrange(char prompt[], int min, int max)
{
    int n;
    printf("%s", prompt);
    n = getnumber();
    while (n < min || n > max) {
        if (feof(stdin))
            return min;
        printf("Enter a number from %d to %d: ", min, max);
        n = getnumber();
    }
    return n;
}

/* Reads a full line like "Ali Khan" (scanf stops at spaces, fgets does not). */
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

/* Same as above but reads from a file. A name is always the last thing
 * on a line in our files because it can have spaces. */
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

/* makes the text capital so d1 and D1 both work */
void touppercase(char text[])
{
    int i;
    for (i = 0; text[i] != '\0'; i++)
        text[i] = toupper(text[i]);
}

/* keeps asking until the user types something */
void getname(char prompt[], char text[], int size)
{
    do {
        printf("%s", prompt);
        getline_text(text, size);
    } while (text[0] == '\0' && !feof(stdin));
}

/* for qualification or hospital: spaces become _ because the files are read word by word */
void getnospace(char prompt[], char text[], int size)
{
    int i;
    getname(prompt, text, size);
    for (i = 0; text[i] != '\0'; i++) {
        if (text[i] == ' ')
            text[i] = '_';
    }
}

/* Reads a password. On Windows it shows * instead of the letters. */
void readpassword(char pass[])
{
#ifdef _WIN32
    int i = 0, c = getch();
    while (c != '\r' && c != '\n' && i < 49) {
        if (c == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            pass[i++] = c;
            printf("*");
        }
        c = getch();
    }
    pass[i] = '\0';
    printf("\n");
#else
    scanf("%49s", pass);
    clearinput();
#endif
}

/* password must be 4 to 10 characters */
void getpassword(char pass[])
{
    char temp[50];
    while (1) {
        printf("Enter password (4-10 characters): ");
        readpassword(temp);
        if (strlen(temp) > 3 && strlen(temp) <= 10)
            break;
        printf("Password must be 4 to 10 characters.\n");
        if (feof(stdin))
            return;
    }
    strcpy(pass, temp);
}

/* ===================== DATES AND TIME SLOTS ===================== */

/* how many days this month has (February is 29 in a leap year) */
int daysinmonth(int m, int y)
{
    int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0))
        return 29;
    return days[m - 1];
}

/* Checks a date like 25-11-2026 and gives back 20261125 (0 if the date is wrong).
 * A number like this is easy to compare (bigger number = later date). */
int datenumber(char date[])
{
    int i, d, m, y;

    if (strlen(date) != 10 || date[2] != '-' || date[5] != '-')
        return 0;
    for (i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && (date[i] < '0' || date[i] > '9'))
            return 0;
    }
    sscanf(date, "%d-%d-%d", &d, &m, &y);
    if (y < 2000 || y > 2100 || m < 1 || m > 12 || d < 1 || d > daysinmonth(m, y))
        return 0;
    return y * 10000 + m * 100 + d;
}

/* date number of today + daysahead (1 = tomorrow), taken from the computer clock */
int daynumber(int daysahead)
{
    time_t t = time(NULL) + daysahead * 86400;
    struct tm *now = localtime(&t);
    return (now->tm_year + 1900) * 10000 + (now->tm_mon + 1) * 100 + now->tm_mday;
}

int todaynumber()
{
    return daynumber(0);
}

/* day of the week for a date number: 0 = Monday ... 6 = Sunday */
int weekday(int datenum)
{
    int t[12] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    int y = datenum / 10000, m = (datenum / 100) % 100, d = datenum % 100;
    int w;

    if (m < 3)
        y = y - 1;
    w = (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
    return (w + 6) % 7;
}

/* 1 if the doctor works on this day (d = 0 for Monday ... 6 for Sunday) */
int doctorworks(char days[], int d)
{
    return strstr(days, shortdays[d]) != NULL;
}

/* Splits a timing like 09:00-17:00 into slotstart and slotend (in minutes).
 * Returns 0 if the timing is not readable. */
int readtiming(char timing[])
{
    int h1, m1, h2, m2;

    if (sscanf(timing, "%d:%d-%d:%d", &h1, &m1, &h2, &m2) != 4)
        return 0;
    slotstart = h1 * 60 + m1;
    slotend = h2 * 60 + m2;
    return slotstart < slotend && slotend <= 24 * 60;
}

/* minutes -> text like 10:30AM (this is how a slot time is saved in the file) */
void timetext(int minutes, char out[])
{
    int h = minutes / 60 % 12;
    if (h == 0)
        h = 12;
    sprintf(out, "%02d:%02d%s", h, minutes % 60, minutes / 60 >= 12 ? "PM" : "AM");
}

/* ===================== FILES ===================== */

/* doctors.txt line:  id specialization experience qualification hospital fee rating days timing password name
 * The same two functions are used for doctors.txt and pendingdoctors.txt */
int readdoctors(char file[], struct doctor list[], int max)
{
    FILE *fp = fopen(file, "r");
    int n = 0;

    if (fp == NULL)
        return 0;
    while (n < max && fscanf(fp, "%s %s %d %s %s %d %f %s %s %s ",
           list[n].id, list[n].specialization, &list[n].experience,
           list[n].qualification, list[n].hospital, &list[n].fee, &list[n].rating,
           list[n].days, list[n].timing, list[n].password) == 10) {
        readname(fp, list[n].name, 50);
        n++;
    }
    fclose(fp);
    return n;
}

void writedoctors(char file[], struct doctor list[], int n)
{
    FILE *fp = fopen(file, "w");
    int i;

    if (fp == NULL)
        return;
    for (i = 0; i < n; i++)
        fprintf(fp, "%s %s %d %s %s %d %.1f %s %s %s %s\n",
                list[i].id, list[i].specialization, list[i].experience,
                list[i].qualification, list[i].hospital, list[i].fee, list[i].rating,
                list[i].days, list[i].timing, list[i].password, list[i].name);
    fclose(fp);
}

void loaddoctors()  { doctorcount = readdoctors("doctors.txt", doctors, MAXDOCTORS); }
void savedoctors()  { writedoctors("doctors.txt", doctors, doctorcount); }
void loadpending()  { pendingcount = readdoctors("pendingdoctors.txt", pending, MAXPENDING); }
void savepending()  { writedoctors("pendingdoctors.txt", pending, pendingcount); }

/* patients.txt line:  id password age gender phone name */
void loadpatients()
{
    FILE *fp = fopen("patients.txt", "r");
    int n = 0;

    if (fp != NULL) {
        while (n < MAXPATIENTS && fscanf(fp, "%d %s %d %d %s ", &patients[n].id,
               patients[n].password, &patients[n].age, &patients[n].gender,
               patients[n].phone) == 5) {
            readname(fp, patients[n].name, 50);
            n++;
        }
        fclose(fp);
    }
    patientcount = n;
}

void savepatients()
{
    FILE *fp = fopen("patients.txt", "w");
    int i;

    if (fp == NULL)
        return;
    for (i = 0; i < patientcount; i++)
        fprintf(fp, "%d %s %d %d %s %s\n", patients[i].id, patients[i].password,
                patients[i].age, patients[i].gender, patients[i].phone, patients[i].name);
    fclose(fp);
}

/* appointments.txt line:
 * id patientID doctorID fee date time status payment method transaction doctorName */
void loadappointments()
{
    FILE *fp = fopen("appointments.txt", "r");
    int n = 0;

    if (fp != NULL) {
        while (n < MAXAPPOINTMENTS && fscanf(fp, "%s %d %s %d %s %s %s %s %s %s ",
               appointments[n].id, &appointments[n].patientid, appointments[n].doctorid,
               &appointments[n].fee, appointments[n].date, appointments[n].time,
               appointments[n].status, appointments[n].payment, appointments[n].method,
               appointments[n].transaction) == 10) {
            readname(fp, appointments[n].doctorname, 50);
            n++;
        }
        fclose(fp);
    }
    appointmentcount = n;
}

void saveappointments()
{
    FILE *fp = fopen("appointments.txt", "w");
    int i;

    if (fp == NULL)
        return;
    for (i = 0; i < appointmentcount; i++)
        fprintf(fp, "%s %d %s %d %s %s %s %s %s %s %s\n", appointments[i].id,
                appointments[i].patientid, appointments[i].doctorid, appointments[i].fee,
                appointments[i].date, appointments[i].time, appointments[i].status,
                appointments[i].payment, appointments[i].method,
                appointments[i].transaction, appointments[i].doctorname);
    fclose(fp);
}

/* reviews.txt line:  appointmentID doctorID patientID stars text */
void loadreviews()
{
    FILE *fp = fopen("reviews.txt", "r");
    int n = 0;

    if (fp != NULL) {
        while (n < MAXREVIEWS && fscanf(fp, "%s %s %d %d ", reviews[n].appointmentid,
               reviews[n].doctorid, &reviews[n].patientid, &reviews[n].stars) == 4) {
            readname(fp, reviews[n].text, 200);
            n++;
        }
        fclose(fp);
    }
    reviewcount = n;
}

void savereviews()
{
    FILE *fp = fopen("reviews.txt", "w");
    int i;

    if (fp == NULL)
        return;
    for (i = 0; i < reviewcount; i++)
        fprintf(fp, "%s %s %d %d %s\n", reviews[i].appointmentid, reviews[i].doctorid,
                reviews[i].patientid, reviews[i].stars, reviews[i].text);
    fclose(fp);
}

/* waitlist.txt line:  patientID doctorID date time  (top line = first in the queue) */
void loadwaitlist()
{
    FILE *fp = fopen("waitlist.txt", "r");
    int n = 0;

    if (fp != NULL) {
        while (n < MAXWAITING && fscanf(fp, "%d %s %s %s", &waitlist[n].patientid,
               waitlist[n].doctorid, waitlist[n].date, waitlist[n].time) == 4)
            n++;
        fclose(fp);
    }
    waitcount = n;
}

void savewaitlist()
{
    FILE *fp = fopen("waitlist.txt", "w");
    int i;

    if (fp == NULL)
        return;
    for (i = 0; i < waitcount; i++)
        fprintf(fp, "%d %s %s %s\n", waitlist[i].patientid, waitlist[i].doctorid,
                waitlist[i].date, waitlist[i].time);
    fclose(fp);
}

void savecategories()
{
    FILE *fp = fopen("categories.txt", "w");
    int i;

    if (fp == NULL)
        return;
    for (i = 0; i < categorycount; i++)
        fprintf(fp, "%s\n", categories[i]);
    fclose(fp);
}

/* categories.txt = the list of specializations. If the file is missing
 * we make it with the default list. */
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
    while (categorycount < MAXCATEGORIES && fscanf(fp, "%19s", categories[categorycount]) == 1)
        categorycount++;
    fclose(fp);
}

/* reads every text file when the program starts */
void loadall()
{
    loadpatients();
    loaddoctors();
    loadpending();
    loadappointments();
    loadreviews();
    loadwaitlist();
    loadcategories();
}

/* ===================== SHARED CHECKS ===================== */

/* position of the doctor in the array, or -1 if not found */
int finddoctorindex(char id[])
{
    int i;
    for (i = 0; i < doctorcount; i++) {
        if (strcmp(doctors[i].id, id) == 0)
            return i;
    }
    return -1;
}

/* position of the patient in the array, or -1 if not found */
int findpatientindex(int id)
{
    int i;
    for (i = 0; i < patientcount; i++) {
        if (patients[i].id == id)
            return i;
    }
    return -1;
}

/* makes the next id like D11 (biggest number + 1, pending doctors count too) */
void newdoctorid(char id[])
{
    int i, n, max = 0;
    for (i = 0; i < doctorcount; i++) {
        if (sscanf(doctors[i].id, "D%d", &n) == 1 && n > max)
            max = n;
    }
    for (i = 0; i < pendingcount; i++) {
        if (sscanf(pending[i].id, "D%d", &n) == 1 && n > max)
            max = n;
    }
    sprintf(id, "D%d", max + 1);
}

/* next id like A5 (biggest number + 1) */
void newappointmentid(char id[])
{
    int i, n, max = 0;
    for (i = 0; i < appointmentcount; i++) {
        if (sscanf(appointments[i].id, "A%d", &n) == 1 && n > max)
            max = n;
    }
    sprintf(id, "A%d", max + 1);
}

/* stops the same doctor from being added twice */
int doctornameexists(char name[])
{
    int i;
    for (i = 0; i < doctorcount; i++) {
        if (strcmp(doctors[i].name, name) == 0)
            return 1;
    }
    for (i = 0; i < pendingcount; i++) {
        if (strcmp(pending[i].name, name) == 0)
            return 1;
    }
    return 0;
}

/* 1 if word is somewhere inside text, capital letters do not matter (used in doctor search) */
int findtext(char text[], char word[])
{
    char a[100], b[100];
    int i;

    strncpy(a, text, 99);
    strncpy(b, word, 99);
    a[99] = b[99] = '\0';
    for (i = 0; a[i] != '\0'; i++)
        a[i] = tolower(a[i]);
    for (i = 0; b[i] != '\0'; i++)
        b[i] = tolower(b[i]);
    return strstr(a, b) != NULL;
}

/* Money counts as revenue only if the payment is verified and the appointment
 * was not cancelled (a cancelled paid appointment is refunded). */
int isrevenue(int i)
{
    return strcmp(appointments[i].payment, "Verified") == 0 &&
           strcmp(appointments[i].status, "Cancelled") != 0;
}

/* returns 1 if someone already has this doctor at this date and time
 * (cancelled ones don't count, so the slot becomes free again).
 * ignore = position of an appointment to skip (used when rescheduling), or -1 */
int slotbooked(char doctorid[], char date[], char slot[], int ignore)
{
    int i;
    for (i = 0; i < appointmentcount; i++) {
        if (i != ignore && strcmp(appointments[i].doctorid, doctorid) == 0 &&
            strcmp(appointments[i].date, date) == 0 &&
            strcmp(appointments[i].time, slot) == 0 &&
            strcmp(appointments[i].status, "Cancelled") != 0)
            return 1;
    }
    return 0;
}

/* returns 1 if this patient already has another appointment at the same time */
int patientbusy(int patientid, char date[], char slot[], int ignore)
{
    int i;
    for (i = 0; i < appointmentcount; i++) {
        if (i != ignore && appointments[i].patientid == patientid &&
            strcmp(appointments[i].date, date) == 0 &&
            strcmp(appointments[i].time, slot) == 0 &&
            strcmp(appointments[i].status, "Cancelled") != 0)
            return 1;
    }
    return 0;
}

/* Appointments that were never paid and whose date is already over
 * get cancelled automatically (payment becomes Failed). */
void cancelexpired()
{
    int i, changed = 0;
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].status, "Pending") == 0 &&
            datenumber(appointments[i].date) < todaynumber()) {
            strcpy(appointments[i].status, "Cancelled");
            strcpy(appointments[i].payment, "Failed");
            changed = 1;
        }
    }
    if (changed)
        saveappointments();
}

/* ===================== DOCTOR DETAILS ===================== */

/* shows the specializations and the user picks one, returns 0 if there are none */
int pickcategory(char spec[])
{
    int i;
    if (categorycount == 0) {
        printf("No specializations exist yet. The admin must add some first.\n");
        return 0;
    }
    for (i = 0; i < categorycount; i++)
        printf("%d. %s\n", i + 1, categories[i]);
    strcpy(spec, categories[getrange("Choose specialization: ", 1, categorycount) - 1]);
    return 1;
}

/* Asks yes/no for every day and makes a text like Mon,Wed,Fri.
 * Returns 0 if no day was chosen. */
int pickdays(char days[])
{
    char text[30] = "", prompt[40];
    int i;

    printf("Select the available days:\n");
    for (i = 0; i < 7; i++) {
        sprintf(prompt, "%-10s (1 = yes, 0 = no): ", fulldays[i]);
        if (getrange(prompt, 0, 1) == 1) {
            if (text[0] != '\0')
                strcat(text, ",");
            strcat(text, shortdays[i]);
        }
    }
    if (text[0] == '\0') {
        printf("Select at least one day.\n");
        return 0;
    }
    strcpy(days, text);
    return 1;
}

/* Doctor works from a start hour to an end hour, slots are 30 minutes each. */
void picktiming(char timing[])
{
    int start, end;
    char prompt[40];

    printf("Use a 24-hour clock. Each appointment slot is 30 minutes.\n");
    start = getrange("Start hour (0 to 22): ", 0, 22);
    sprintf(prompt, "End hour (%d to 23): ", start + 1);
    end = getrange(prompt, start + 1, 23);
    sprintf(timing, "%02d:00-%02d:00", start, end);
}

/* when a doctor changes his name, his old appointments must show the new name too */
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
