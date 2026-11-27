/*
 * HIMS - Hospital Information Management System
 * PART 7 : ADMIN STATISTICS
 */

/* how many appointments have this status (Completed, Cancelled ...) */
int countstatus(char status[])
{
    int i, count = 0;
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].status, status) == 0)
            count++;
    }
    return count;
}

/* how many appointments one doctor has.
 * if status is "All" it counts every appointment */
int countfordoctor(char doctorid[], char status[])
{
    int i, count = 0;
    for (i = 0; i < appointmentcount; i++) {
        if (strcmp(appointments[i].doctorid, doctorid) == 0 &&
            (strcmp(status, "All") == 0 || strcmp(appointments[i].status, status) == 0))
            count++;
    }
    return count;
}

/* how many appointments were with doctors of this specialization */
int countforspecialization(char spec[])
{
    int i, d, count = 0;
    for (i = 0; i < appointmentcount; i++) {
        d = finddoctorindex(appointments[i].doctorid);
        if (d != -1 && strcmp(doctors[d].specialization, spec) == 0)
            count++;
    }
    return count;
}

/* same as countstatus but only for the appointments of today */
int counttoday(char status[])
{
    int i, count = 0;
    for (i = 0; i < appointmentcount; i++) {
        if (datenumber(appointments[i].date) == todaynumber() &&
            strcmp(appointments[i].status, status) == 0)
            count++;
    }
    return count;
}

/* numbers for today and for all appointments, plus revenue (verified payments, not cancelled) */
void systemstatistics()
{
    char names[4][10] = {"Pending", "Confirmed", "Completed", "Cancelled"};
    int i, todaycount = 0, revenue = 0;

    for (i = 0; i < appointmentcount; i++) {
        if (datenumber(appointments[i].date) == todaynumber())
            todaycount++;
        if (isrevenue(i))
            revenue = revenue + appointments[i].fee;
    }
    printf("\n===== SYSTEM STATISTICS =====\n\n");
    printf("Total Patients       : %d\nTotal Doctors        : %d\n", patientcount, doctorcount);
    printf("Doctors Waiting      : %d\nTotal Appointments   : %d\n", pendingcount, appointmentcount);
    printf("Appointments Today   : %d\n", todaycount);
    for (i = 0; i < 4; i++)
        printf("  %-18s : %d\n", names[i], counttoday(names[i]));
    printf("\nAll appointments:\n");
    for (i = 0; i < 4; i++)
        printf("  %-18s : %d\n", names[i], countstatus(names[i]));
    printf("\nTotal Revenue        : Rs. %d\n", revenue);
}

/* most booked doctor, most cancelled doctor, highest rated doctor and most popular specialization */
void detailedstatistics()
{
    int i, count;
    int booked = 0, bookedcount = -1, cancelled = 0, cancelledcount = -1;
    int best = 0, popular = 0, popularcount = -1;

    if (doctorcount == 0) {
        printf("\nThere are no doctors yet.\n");
        return;
    }
    for (i = 0; i < doctorcount; i++) {
        count = countfordoctor(doctors[i].id, "All");
        if (count > bookedcount) {
            bookedcount = count;
            booked = i;
        }
        count = countfordoctor(doctors[i].id, "Cancelled");
        if (count > cancelledcount) {
            cancelledcount = count;
            cancelled = i;
        }
        if (doctors[i].rating > doctors[best].rating)
            best = i;
    }
    for (i = 0; i < categorycount; i++) {
        count = countforspecialization(categories[i]);
        if (count > popularcount) {
            popularcount = count;
            popular = i;
        }
    }

    printf("\n===== DETAILED STATISTICS =====\n\n");
    if (bookedcount == 0)
        printf("Most booked doctor         : None\n");
    else
        printf("Most booked doctor         : %s (%d appointments)\n", doctors[booked].name, bookedcount);
    if (popularcount <= 0)
        printf("Most popular specialization: None\n");
    else
        printf("Most popular specialization: %s (%d appointments)\n", categories[popular], popularcount);
    if (cancelledcount == 0)
        printf("Most cancelled doctor      : None\n");
    else
        printf("Most cancelled doctor      : %s (%d cancelled)\n", doctors[cancelled].name, cancelledcount);
    printf("Highest rated doctor       : %s (%.1f)\n", doctors[best].name, doctors[best].rating);
    printf("\nAppointments per specialization:\n--------------------------------\n");
    for (i = 0; i < categorycount; i++)
        printf("%-16s : %d\n", categories[i], countforspecialization(categories[i]));
}
