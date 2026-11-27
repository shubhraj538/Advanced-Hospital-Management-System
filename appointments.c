/*
 * HIMS - Hospital Information Management System
 * PART 2 : APPOINTMENT SYSTEM
 * Doctor -> Date -> Available slot -> Confirm. The status starts as Pending
 * and becomes Confirmed when the payment is verified.
 */

/* Asks for a date like 25-11-2026. Gives the date number,
 * or 0 if the user wants to go back. */
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
        if (feof(stdin))
            return 0;
    }
}

/* Shows all slots of the doctor on that date (Booked / Available) and lets the patient pick one.
 * ignore = the appointment that is being rescheduled, its own old slot counts as free.
 * Returns 0 = went back, 1 = a free slot was chosen, 2 = a booked slot was chosen. */
int chooseslot(int d, char date[], int ignore, char chosen[])
{
    char slots[48][10], text[100];
    int i, count, choice;

    if (!readtiming(doctors[d].timing)) {
        printf("This doctor's timing is not set properly.\n");
        return 0;
    }
    count = (slotend - slotstart) / 30;     /* every slot is 30 minutes */
    if (count > 48)
        count = 48;

    printf("\nSlots on %s with %s:\n", date, doctors[d].name);
    printf("%-4s %-9s %s\n-------------------------\n", "No.", "Time", "Status");
    for (i = 0; i < count; i++) {
        timetext(slotstart + i * 30, slots[i]);
        printf("%-4d %-9s %s\n", i + 1, slots[i],
               slotbooked(doctors[d].id, date, slots[i], ignore) ? "Booked" : "Available");
    }
    choice = getrange("\nChoose slot number (0 to go back): ", 0, count);
    if (choice == 0)
        return 0;

    strcpy(chosen, slots[choice - 1]);
    if (slotbooked(doctors[d].id, date, chosen, ignore)) {
        sprintf(text, "The %s slot with %s\non %s is already booked.", chosen, doctors[d].name, date);
        shownotification("Slot unavailable", text, "");
        return 2;
    }
    return 1;
}

void bookappointment(int p)
{
    char id[10], date[15], slot[10];
    int d, a, datenum, r;

    if (appointmentcount >= MAXAPPOINTMENTS) {
        printf("The system cannot take more appointments right now.\n");
        return;
    }
    printf("\n========== BOOK AN APPOINTMENT ==========\n");
    showalldoctors();
    printf("\nEnter Doctor ID (e.g. D1): ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);
    d = finddoctorindex(id);
    if (d == -1) {
        printf("Doctor not found.\n");
        return;
    }
    printf("\nDoctor : %s (%s)\nFee    : Rs. %d\nDays   : %s\n",
           doctors[d].name, doctors[d].specialization, doctors[d].fee, doctors[d].days);

    datenum = askdate(date);
    if (datenum == 0)
        return;
    if (!doctorworks(doctors[d].days, weekday(datenum))) {
        printf("%s is not available on %s.\n", doctors[d].name, fulldays[weekday(datenum)]);
        return;
    }

    r = chooseslot(d, date, -1, slot);
    if (r == 0)
        return;
    if (r == 2) {
        if (getrange("Join the waiting list for this slot? (1 = yes, 0 = no): ", 0, 1) == 1)
            joinwaitlist(patients[p].id, doctors[d].id, date, slot);
        return;
    }
    if (patientbusy(patients[p].id, date, slot, -1)) {
        printf("You already have another appointment on %s at %s.\n", date, slot);
        return;
    }

    a = appointmentcount++;
    newappointmentid(appointments[a].id);
    appointments[a].patientid = patients[p].id;
    strcpy(appointments[a].doctorid, doctors[d].id);
    appointments[a].fee = doctors[d].fee;
    strcpy(appointments[a].date, date);
    strcpy(appointments[a].time, slot);
    strcpy(appointments[a].status, "Pending");      /* Pending until the payment is verified */
    strcpy(appointments[a].payment, "Pending");
    strcpy(appointments[a].method, "None");
    strcpy(appointments[a].transaction, "None");
    strcpy(appointments[a].doctorname, doctors[d].name);
    saveappointments();

    printf("\n========== APPOINTMENT BOOKED ==========\n");
    printf("Appointment ID : %s\nDoctor         : %s\n", appointments[a].id, doctors[d].name);
    printf("Date / Time    : %s  %s\nFee            : Rs. %d\n", date, slot, doctors[d].fee);
    printf("Status         : Pending\n\nPay from the payment menu to confirm it.\n");
}

/* upcoming = still active (Pending or Confirmed) and the date is not over */
int isupcoming(int i)
{
    return (strcmp(appointments[i].status, "Pending") == 0 ||
            strcmp(appointments[i].status, "Confirmed") == 0) &&
           datenumber(appointments[i].date) >= todaynumber();
}

/* shows the upcoming appointments of one patient, returns how many */
int showupcoming(int patientid)
{
    int i, found = 0;

    printf("\n%-5s %-18s %-11s %-9s %-7s %-10s %s\n",
           "ID", "Doctor", "Date", "Time", "Fee", "Status", "Payment");
    printf("---------------------------------------------------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (appointments[i].patientid == patientid && isupcoming(i)) {
            printf("%-5s %-18s %-11s %-9s %-7d %-10s %s\n",
                   appointments[i].id, appointments[i].doctorname, appointments[i].date,
                   appointments[i].time, appointments[i].fee, appointments[i].status,
                   appointments[i].payment);
            found++;
        }
    }
    if (found == 0)
        printf("You have no upcoming appointments.\n");
    return found;
}

/* position of this patient's appointment with this id, or -1 */
int findappointment(char id[], int patientid)
{
    int i;
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].id, id) == 0 && appointments[i].patientid == patientid)
            return i;
    }
    return -1;
}

/* patient cancels his own appointment */
void patientcancel(int patientid)
{
    char id[10];
    int a;

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
    if (getrange("Are you sure? (1 = yes, 0 = no): ", 0, 1) == 1) {
        cancelrecord(a);        /* refund and waiting list are handled inside cancelrecord */
        printf("Appointment %s has been cancelled.\n", appointments[a].id);
    }
}

/* Same doctor, new date and slot. The appointment id, status and payment stay the same. */
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
    printf("\nCurrent booking: %s at %s with %s\n", appointments[a].date,
           appointments[a].time, appointments[a].doctorname);

    datenum = askdate(date);
    if (datenum == 0)
        return;
    if (!doctorworks(doctors[d].days, weekday(datenum))) {
        printf("%s is not available on %s.\n", doctors[d].name, fulldays[weekday(datenum)]);
        return;
    }
    if (chooseslot(d, date, a, slot) != 1)      /* a is sent so the old slot counts as free */
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
