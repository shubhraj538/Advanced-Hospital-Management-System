#include <stdio.h>
#include <string.h>

int main() {

    int choice, role;
    int age, gender, special;
    int loggedin = 0;
    int menu;

    char username[50];
    char password[50];
    char fname[100];

    char doctorSpecialization[50];
    char qualification[100];
    float fee;

    /* Login information for the current user */
    char loginUsername[50];
    char loginPassword[50];

    /* Admin account */
    char adminUsername[] = "admin";
    char adminPassword[] = "admin123";

    /* =====================================================
                       MAIN ACCOUNT MENU
       ===================================================== */

    printf("========================================\n");
    printf("       HOSPITAL MANAGEMENT SYSTEM\n");
    printf("========================================\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("========================================\n");

    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();

    /* =====================================================
                          LOGIN
       ===================================================== */

    if (choice == 1) {

        printf("\n========================================\n");
        printf("              LOGIN AS\n");
        printf("========================================\n");
        printf("1. Patient\n");
        printf("2. Doctor\n");
        printf("3. Admin\n");
        printf("========================================\n");

        printf("Enter your choice: ");
        scanf("%d", &role);
        getchar();

        if (role < 1 || role > 3) {
            printf("Invalid choice. Please try again.\n");
            return 0;
        }

        printf("\n========================================\n");
        printf("Enter your username: ");
        scanf("%49s", loginUsername);

        printf("Enter your password: ");
        scanf("%49s", loginPassword);

        /* ---------------- ADMIN LOGIN ---------------- */

        if (role == 3) {

            if (strcmp(loginUsername, adminUsername) == 0 &&
                strcmp(loginPassword, adminPassword) == 0) {

                loggedin = 1;

                printf("\n========================================\n");
                printf("Admin login successful!\n");
                printf("Welcome, Admin!\n");
                printf("========================================\n");

                while (loggedin == 1) {

                    printf("\n========== ADMIN MENU ==========\n");
                    printf("1. View Doctors\n");
                    printf("2. View Patients\n");
                    printf("3. Hospital Statistics\n");
                    printf("0. Logout\n");
                    printf("================================\n");

                    printf("Enter your choice: ");
                    scanf("%d", &menu);

                    if (menu == 1) {

                        printf("\nDisplaying doctors...\n");

                    }

                    else if (menu == 2) {

                        printf("\nDisplaying patients...\n");

                    }

                    else if (menu == 3) {

                        printf("\nDisplaying hospital statistics...\n");

                    }

                    else if (menu == 0) {

                        printf("\nLogging out...\n");
                        loggedin = 0;

                    }

                    else {

                        printf("Invalid choice. Please try again.\n");

                    }
                }

            }

            else {

                printf("\nInvalid admin username or password.\n");
                return 0;

            }
        }

        /* ---------------- PATIENT LOGIN ---------------- */

        else if (role == 1) {

            /*
               At this stage we do not have file storage yet.
               Therefore patient login is temporarily demonstrated
               using the username and password entered during
               registration in the same program run.
            */

            printf("\nPatient login selected.\n");
            printf("Patient login system will be connected to registered\n");
            printf("accounts when file storage is added.\n");

            return 0;
        }

        /* ---------------- DOCTOR LOGIN ---------------- */

        else if (role == 2) {

            printf("\nDoctor login selected.\n");
            printf("Doctor login system will be connected to registered\n");
            printf("accounts when file storage is added.\n");

            return 0;
        }

    }

    /* =====================================================
                         REGISTRATION
       ===================================================== */

    else if (choice == 2) {

        printf("\n========================================\n");
        printf("             REGISTER AS\n");
        printf("========================================\n");
        printf("1. Patient\n");
        printf("2. Doctor\n");
        printf("========================================\n");

        printf("Enter your choice: ");
        scanf("%d", &role);
        getchar();

        if (role < 1 || role > 2) {

            printf("Invalid choice. Please try again.\n");
            return 0;

        }

        /* =================================================
                         PATIENT REGISTRATION
           ================================================= */

        if (role == 1) {

            printf("\n========================================\n");
            printf("          PATIENT REGISTRATION\n");
            printf("========================================\n");

            printf("Enter your Full Name: ");
            fgets(fname, sizeof(fname), stdin);

            /* Remove newline */
            fname[strcspn(fname, "\n")] = '\0';

            if (strlen(fname) == 0) {

                printf("Full Name cannot be empty.\n");
                return 0;

            }

            if (fname[0] == ' ') {

                printf("Full Name cannot start with a space.\n");
                return 0;

            }

            printf("\n========================================\n");

            printf("Enter your age: ");
            scanf("%d", &age);

            if (age < 0 || age > 110) {

                printf("Invalid age. Please try again.\n");
                return 0;

            }

            printf("\n========================================\n");

            printf("Enter your gender:\n");
            printf("1. Male\n");
            printf("2. Female\n");
            printf("3. Other\n");

            printf("Enter your choice: ");
            scanf("%d", &gender);

            if (gender < 1 || gender > 3) {

                printf("Invalid gender choice.\n");
                return 0;

            }

            printf("\n========================================\n");

            printf("Create a username: ");
            scanf("%49s", username);

            printf("Create a password: ");
            scanf("%49s", password);

            if (strlen(password) < 8) {

                printf("Password must be at least 8 characters long.\n");
                return 0;

            }

            printf("\n========================================\n");
            printf("      PATIENT REGISTRATION COMPLETE\n");
            printf("========================================\n");

            printf("Welcome, %s!\n", fname);
            printf("Your account has been created successfully.\n");

            printf("\nYour details:\n");
            printf("Name: %s\n", fname);
            printf("Age: %d\n", age);

            if (gender == 1)
                printf("Gender: Male\n");
            else if (gender == 2)
                printf("Gender: Female\n");
            else
                printf("Gender: Other\n");

            printf("Username: %s\n", username);

            printf("========================================\n");

        }

        /* =================================================
                         DOCTOR REGISTRATION
           ================================================= */

        else if (role == 2) {

            printf("\n========================================\n");
            printf("           DOCTOR REGISTRATION\n");
            printf("========================================\n");

            printf("Enter your Full Name: ");
            fgets(fname, sizeof(fname), stdin);

            /* Remove newline */
            fname[strcspn(fname, "\n")] = '\0';

            if (strlen(fname) == 0) {

                printf("Full Name cannot be empty.\n");
                return 0;

            }

            if (fname[0] == ' ') {

                printf("Full Name cannot start with a space.\n");
                return 0;

            }

            printf("\n========================================\n");

            printf("Enter your age: ");
            scanf("%d", &age);

            if (age < 18 || age > 110) {

                printf("Invalid age. Doctor must be at least 18 years old.\n");
                return 0;

            }

            printf("\n========================================\n");

            printf("Enter your gender:\n");
            printf("1. Male\n");
            printf("2. Female\n");
            printf("3. Other\n");

            printf("Enter your choice: ");
            scanf("%d", &gender);

            if (gender < 1 || gender > 3) {

                printf("Invalid gender choice.\n");
                return 0;

            }

            printf("\n========================================\n");

            printf("Select your specialization:\n");
            printf("1. Cardiologist\n");
            printf("2. Dermatologist\n");
            printf("3. Orthopedic Surgeon\n");
            printf("4. Pediatrician\n");
            printf("5. General Practitioner\n");

            printf("Enter your choice: ");
            scanf("%d", &special);
            getchar();

            if (special < 1 || special > 5) {

                printf("Invalid specialization choice.\n");
                return 0;

            }

            if (special == 1)
                strcpy(doctorSpecialization, "Cardiologist");

            else if (special == 2)
                strcpy(doctorSpecialization, "Dermatologist");

            else if (special == 3)
                strcpy(doctorSpecialization, "Orthopedic Surgeon");

            else if (special == 4)
                strcpy(doctorSpecialization, "Pediatrician");

            else
                strcpy(doctorSpecialization, "General Practitioner");


            printf("\n========================================\n");

            printf("Enter your qualification: ");
            fgets(qualification, sizeof(qualification), stdin);

            qualification[strcspn(qualification, "\n")] = '\0';

            if (strlen(qualification) == 0) {

                printf("Qualification cannot be empty.\n");
                return 0;

            }

            printf("\n========================================\n");

            printf("Enter consultation fee: ");
            scanf("%f", &fee);

            if (fee < 0) {

                printf("Fee cannot be negative.\n");
                return 0;

            }

            printf("\n========================================\n");

            printf("Create a username: ");
            scanf("%49s", username);

            printf("Create a password: ");
            scanf("%49s", password);

            if (strlen(password) < 8) {

                printf("Password must be at least 8 characters long.\n");
                return 0;

            }

            printf("\n========================================\n");
            printf("       DOCTOR REGISTRATION COMPLETE\n");
            printf("========================================\n");

            printf("Welcome, Dr. %s!\n", fname);
            printf("Your account has been created successfully.\n");

            printf("\nYour details:\n");
            printf("Name: %s\n", fname);
            printf("Age: %d\n", age);

            if (gender == 1)
                printf("Gender: Male\n");
            else if (gender == 2)
                printf("Gender: Female\n");
            else
                printf("Gender: Other\n");

            printf("Specialization: %s\n", doctorSpecialization);
            printf("Qualification: %s\n", qualification);
            printf("Consultation Fee: %.2f\n", fee);
            printf("Username: %s\n", username);

            printf("========================================\n");

        }

    }

    /* =====================================================
                         INVALID MAIN CHOICE
       ===================================================== */

    else {

        printf("Invalid choice. Please try again.\n");
        return 0;

    }

    return 0;
}