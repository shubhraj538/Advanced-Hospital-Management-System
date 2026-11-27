/*
 * HIMS - Hospital Information Management System
 * PART 11 : WAITING LIST
 * If every slot is booked the patient can join the waiting list of a slot.
 * When that slot is cancelled, the first person in the queue gets it.
 */

/* place of entry w in the queue of its slot (1 = first) */
int position(int w)
{
    int i, pos = 1;
    for (i = 0; i < w; i++) {
        if (strcmp(waitlist[i].doctorid, waitlist[w].doctorid) == 0 &&
            strcmp(waitlist[i].date, waitlist[w].date) == 0 &&
            strcmp(waitlist[i].time, waitlist[w].time) == 0)
            pos++;
    }
    return pos;
}

/* deletes entry w and moves the rest one step up */
void removewait(int w)
{
    int i;
    for (i = w; i < waitcount - 1; i++)
        waitlist[i] = waitlist[i + 1];
    waitcount--;
}

/* Goes through the waiting list from the top. If the slot someone waits for
 * is free now, he gets a Pending appointment and leaves the list.
 * This is called after every cancellation and when the program starts. */
void checkwaitlist()
{
    int i = 0, d, a, changed = 0;

    while (i < waitcount) {
        if (datenumber(waitlist[i].date) < todaynumber()) {
            removewait(i);
            changed = 1;
        } else if (!slotbooked(waitlist[i].doctorid, waitlist[i].date, waitlist[i].time, -1)) {
            d = finddoctorindex(waitlist[i].doctorid);
            if (d != -1 && appointmentcount < MAXAPPOINTMENTS &&
                !patientbusy(waitlist[i].patientid, waitlist[i].date, waitlist[i].time, -1)) {
                a = appointmentcount++;
                newappointmentid(appointments[a].id);
                appointments[a].patientid = waitlist[i].patientid;
                strcpy(appointments[a].doctorid, doctors[d].id);
                appointments[a].fee = doctors[d].fee;
                strcpy(appointments[a].date, waitlist[i].date);
                strcpy(appointments[a].time, waitlist[i].time);
                strcpy(appointments[a].status, "Pending");
                strcpy(appointments[a].payment, "Pending");
                strcpy(appointments[a].method, "None");
                strcpy(appointments[a].transaction, "None");
                strcpy(appointments[a].doctorname, doctors[d].name);
            }
            removewait(i);
            changed = 1;
        } else {
            i++;
        }
    }
    if (changed) {
        savewaitlist();
        saveappointments();
    }
}

/* One place to cancel an appointment, so every part does it the same way.
 * A paid appointment is refunded, and the free slot goes to the waiting list. */
void cancelrecord(int a)
{
    strcpy(appointments[a].status, "Cancelled");
    if (strcmp(appointments[a].payment, "Verified") == 0)
        printf("Payment of Rs. %d will be refunded.\n", appointments[a].fee);
    saveappointments();
    checkwaitlist();
}

/* Patient joins the queue of a slot that is already booked. */
void joinwaitlist(int patientid, char doctorid[], char date[], char slot[])
{
    int i;

    if (patientbusy(patientid, date, slot, -1)) {
        printf("You already have an appointment at this time.\n");
        return;
    }
    for (i = 0; i < waitcount; i++) {
        if (waitlist[i].patientid == patientid && strcmp(waitlist[i].doctorid, doctorid) == 0 &&
            strcmp(waitlist[i].date, date) == 0 && strcmp(waitlist[i].time, slot) == 0) {
            printf("You are already on the waiting list for this slot.\n");
            return;
        }
    }
    if (waitcount >= MAXWAITING) {
        printf("The waiting list is full.\n");
        return;
    }
    waitlist[waitcount].patientid = patientid;
    strcpy(waitlist[waitcount].doctorid, doctorid);
    strcpy(waitlist[waitcount].date, date);
    strcpy(waitlist[waitcount].time, slot);
    waitcount++;
    savewaitlist();
    printf("You joined the waiting list. Your position: %d\n", position(waitcount - 1));
}

/* shows everybody who is waiting for one doctor, with names */
void doctorwaitlist()
{
    char id[10];
    int i, p, found = 0;

    printf("\nEnter Doctor ID: ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);
    if (finddoctorindex(id) == -1) {
        printf("Doctor not found.\n");
        return;
    }

    printf("\n%-11s %-9s %-4s %s\n", "Date", "Time", "No.", "Patient");
    printf("------------------------------------------\n");
    for (i = 0; i < waitcount; i++) {
        if (strcmp(waitlist[i].doctorid, id) == 0) {
            p = findpatientindex(waitlist[i].patientid);
            printf("%-11s %-9s %-4d %s\n", waitlist[i].date, waitlist[i].time,
                   position(i), p == -1 ? "Unknown" : patients[p].name);
            found++;
        }
    }
    if (found == 0)
        printf("Nobody is waiting for this doctor.\n");
}
