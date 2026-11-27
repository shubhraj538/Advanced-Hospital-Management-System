/*
 * HIMS - Hospital Information Management System
 * MAIN MENU + PATIENT DASHBOARD
 * The program starts here. Compile only this file:  gcc main.c -o hims
 */

#include "HIMS.h"

/* everything a patient can do after login */
void patientdashboard(int p)
{
    int choice;

    while (1) {
        printf("\n========== PATIENT DASHBOARD ==========\n");
        printf("Logged in as: %s\n", patients[p].name);
        printf("1. Find a doctor\n2. Book an appointment\n3. My upcoming appointments\n");
        printf("4. My past appointments\n5. Cancel an appointment\n6. Reschedule an appointment\n");
        printf("7. Pay for an appointment\n8. View payment status\n9. Rate a doctor\n");
        printf("10. View doctor reviews\n11. Waiting list\n12. Notifications\n");
        printf("13. Get a receipt\n14. My record and history\n15. My account\n0. Logout\n");
        choice = getrange("Enter choice: ", 0, 15);

        switch (choice) {
            case 1:  finddoctormenu();                              break;
            case 2:  bookappointment(p);                            break;
            case 3:  showupcoming(patients[p].id);                  break;
            case 4:  viewhistory(patients[p].id, 1);                break;
            case 5:  patientcancel(patients[p].id);                 break;
            case 6:  rescheduleappointment(patients[p].id);         break;
            case 7:  makepayment(patients[p].id);                   break;
            case 8:  paymentstatus(patients[p].id);                 break;
            case 9:  rateadoctor(patients[p].id);                   break;
            case 10: viewreviews();                                 break;
            case 11: doctorwaitlist();                              break;
            case 12: mynotifications(patients[p].id);               break;
            case 13: getreceipt(p);                                 break;
            case 14: viewrecord(p); viewhistory(patients[p].id, 0); break;
            case 15: accountmenu(p);                                break;
            case 0:
                printf("Logged out.\n");
                return;
        }
    }
}

int main()
{
    int choice, i;

    loadall();              /* read all the text files */
    cancelexpired();        /* cancel old appointments that were never paid */
    checkwaitlist();        /* give free slots to people on the waiting list */

    while (1) {
        printf("\n===== HOSPITAL INFORMATION MANAGEMENT SYSTEM =====\n");
        printf("1. Patient login\n2. Patient registration\n3. Doctor login\n");
        printf("4. Doctor registration\n5. Admin login\n0. Exit\n");
        choice = getrange("Enter choice: ", 0, 5);

        switch (choice) {
            case 1:
                i = loginpatient();
                if (i != -1)
                    patientdashboard(i);
                break;
            case 2:
                registerpatient();
                break;
            case 3:
                i = logindoctor();
                if (i != -1)
                    doctordashboard(i);
                break;
            case 4:
                registerdoctor();
                break;
            case 5:
                if (adminlogin())
                    admindashboard();
                break;
            case 0:
                printf("Goodbye!\n");
                return 0;
        }
    }
}
