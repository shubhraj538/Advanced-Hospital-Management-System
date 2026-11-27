/*
 * HIMS - Hospital Information Management System
 * PART 10 : NOTIFICATION SYSTEM
 * Nothing extra is saved. The notifications are made from the appointments.
 */

/* prints one notification inside lines */
void shownotification(char title[], char text[], char id[])
{
    printf("\n--------------------------------------\n");
    printf("NOTIFICATION - %s\n%s\n", title, text);
    if (id[0] != '\0')
        printf("Appointment ID: %s\n", id);
    printf("--------------------------------------\n");
}

/* payment verified, appointment confirmed, reminder (today or tomorrow), cancelled */
void mynotifications(int patientid)
{
    char text[200];
    int i, count = 0, date;

    for (i = 0; i < appointmentcount; i++) {
        if (appointments[i].patientid != patientid)
            continue;

        if (strcmp(appointments[i].status, "Confirmed") == 0 ||
            strcmp(appointments[i].status, "Completed") == 0) {
            sprintf(text, "Your payment of Rs. %d has been verified.", appointments[i].fee);
            shownotification("Payment verified", text, appointments[i].id);
            count++;
        }
        if (strcmp(appointments[i].status, "Confirmed") == 0) {
            sprintf(text, "Your appointment with %s\nis confirmed for %s on %s.",
                    appointments[i].doctorname, appointments[i].time, appointments[i].date);
            shownotification("Appointment confirmed", text, appointments[i].id);
            date = datenumber(appointments[i].date);
            if (date == daynumber(0) || date == daynumber(1)) {
                sprintf(text, "Reminder: you have an appointment with %s\n%s at %s.",
                        appointments[i].doctorname,
                        date == daynumber(0) ? "today" : "tomorrow", appointments[i].time);
                shownotification("Appointment reminder", text, appointments[i].id);
                count++;
            }
            count++;
        }
        if (strcmp(appointments[i].status, "Cancelled") == 0) {
            sprintf(text, "Your appointment with %s\non %s at %s was cancelled.%s",
                    appointments[i].doctorname, appointments[i].date, appointments[i].time,
                    strcmp(appointments[i].payment, "Verified") == 0 ?
                    "\nYour payment will be refunded." : "");
            shownotification("Appointment cancelled", text, appointments[i].id);
            count++;
        }
    }
    if (count == 0)
        printf("\nYou have no notifications.\n");
    else
        printf("\nYou have %d notification(s).\n", count);
}
