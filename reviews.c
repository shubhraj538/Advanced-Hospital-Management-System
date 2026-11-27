/*
 * HIMS - Hospital Information Management System
 * PART 4 : RATINGS & REVIEWS
 * After an appointment is Completed, the patient can rate the doctor (1 to 5 stars)
 * and write a review. The doctor's rating becomes the average of all his reviews.
 */

/* returns 1 if this appointment already has a review */
int alreadyreviewed(char appointmentid[])
{
    int i;
    for (i = 0; i < reviewcount; i++) {
        if (strcmp(reviews[i].appointmentid, appointmentid) == 0)
            return 1;
    }
    return 0;
}

/* prints something like ***-- for 3 stars */
void printstars(int stars)
{
    int i;
    for (i = 1; i <= 5; i++)
        printf("%c", i <= stars ? '*' : '-');
}

/* new rating = average of all the stars this doctor got */
void updaterating(char doctorid[])
{
    int i, d, total = 0, count = 0;

    for (i = 0; i < reviewcount; i++) {
        if (strcmp(reviews[i].doctorid, doctorid) == 0) {
            total = total + reviews[i].stars;
            count++;
        }
    }
    d = finddoctorindex(doctorid);
    if (d == -1 || count == 0)
        return;
    printf("\n%s's rating: %.1f", doctors[d].name, doctors[d].rating);
    doctors[d].rating = (float)total / count;
    printf("  ->  %.1f  (%d reviews)\n", doctors[d].rating, count);
    savedoctors();
}

void rateadoctor(int patientid)
{
    char id[10];
    int i, found = 0;

    if (reviewcount >= MAXREVIEWS) {
        printf("The system cannot take more reviews right now.\n");
        return;
    }
    printf("\nAppointments you can review:\n%-5s %-18s %-11s\n", "ID", "Doctor", "Date");
    printf("-----------------------------------\n");
    for (i = 0; i < appointmentcount; i++) {
        if (appointments[i].patientid == patientid &&
            strcmp(appointments[i].status, "Completed") == 0 &&
            !alreadyreviewed(appointments[i].id)) {
            printf("%-5s %-18s %-11s\n", appointments[i].id,
                   appointments[i].doctorname, appointments[i].date);
            found++;
        }
    }
    if (found == 0) {
        printf("No completed appointments to review.\n");
        return;
    }

    printf("\nEnter Appointment ID: ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);
    i = findappointment(id, patientid);
    if (i == -1) {
        printf("Appointment not found.\n");
        return;
    }
    if (strcmp(appointments[i].status, "Completed") != 0) {     /* only completed appointments can be reviewed */
        printf("You can only review a completed appointment.\n");
        return;
    }
    if (alreadyreviewed(id)) {
        printf("You already reviewed this appointment.\n");
        return;
    }

    strcpy(reviews[reviewcount].appointmentid, id);
    strcpy(reviews[reviewcount].doctorid, appointments[i].doctorid);
    reviews[reviewcount].patientid = patientid;
    reviews[reviewcount].stars = getrange("\nRate the doctor (1 to 5): ", 1, 5);
    printf("You gave: ");
    printstars(reviews[reviewcount].stars);
    printf("\nWrite review: ");
    getline_text(reviews[reviewcount].text, 200);
    if (reviews[reviewcount].text[0] == '\0')
        strcpy(reviews[reviewcount].text, "No comment.");
    reviewcount++;
    savereviews();

    printf("\nThank you! Your review has been saved.");
    updaterating(appointments[i].doctorid);
}

/* patient side: pick a doctor and read his reviews */
void viewreviews()
{
    char id[10];
    int i, found = 0;

    printf("\n%-4s %-18s %s\n------------------------------\n", "ID", "Doctor", "Rating");
    for (i = 0; i < doctorcount; i++)
        printf("%-4s %-18s %.1f\n", doctors[i].id, doctors[i].name, doctors[i].rating);

    printf("\nEnter Doctor ID to see reviews (e.g. D1): ");
    scanf("%9s", id);
    clearinput();
    touppercase(id);

    printf("\n========== REVIEWS ==========\n");
    for (i = 0; i < reviewcount; i++) {
        if (strcmp(reviews[i].doctorid, id) == 0) {
            printstars(reviews[i].stars);
            printf("  %s\n", reviews[i].text);
            found++;
        }
    }
    if (found == 0)
        printf("No reviews for this doctor.\n");
}
