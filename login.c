#include <stdio.h>
#include <string.h>

int main() {
    int choice, age, gender;
    char username[50], password[50], fname[50];

    printf("=====HOSPITAL MANAGEMENT SYSTEM=====\n");
    printf("            ACCOUNT\n");

printf("====================================\n");
printf("1.Login: \n");
printf("2.Register: \n");
printf("====================================\n");

printf("Enter your choice: ");
scanf("%d", &choice);
printf("====================================\n");

switch(choice) {
    case 1: // LOGINS
        printf("Enter your credentials.\n");
        scanf("%s", &username); //CHECK AGAINTS A .txt file (haven't figured that out yet.)

        printf("Welcome, %s!\n", username);

        printf("Enter your password: ");
        scanf("%s", &password); //same thing for here
// can use if and strcmp to check dk tbh
        break;

// STILL HAVE TO COMPELTE THIS 
    case 2: // REGISTER
        printf("Enter your details to register;\n");
        printf("====================================\n");
        printf("Enter your Full Name: ");
        scanf("%s", &fname);
        if(fname[0] == '\0') {
            printf("Full Name cannot be empty. Please try again.\n");
            return 0;
        } else if(fname[0] == ' ') {
            printf("Full Name cannot start with a space. Please try again.\n");
            return 0;
        } else {}

printf("====================================\n");
        printf("Enter your age: \n");
        scanf("%d", &age);
        if(age < 0) {
            printf("Age cannot be negative. Please try again.\n");
            return 0;
        } else if (age > 110) {
            printf("Invalid age input. Please try again.\n");
            return 0;
        }

printf("====================================\n");
        printf("Enter your gender: \n");
        printf("1. Male\n2. Female\n3. Other\n");
        scanf("%d", &gender);
        if(gender < 1 || gender > 3) {
            printf("Invalid gender input. Please try again.\n");
            return 0;
        }

printf("====================================\n");
        printf("Enter your password: \n");
        scanf("%s", password);
        if (password[0] == '\0') {
            printf("Password cannot be empty. Please try again.\n");
            return 0;
        } else if(password[0] == ' ') {
            printf("Password cannot start with a space. Please try again.\n");
            return 0;
        } else {}
//lenght requiremtn
        if (strlen(password) < 8) {
            printf("Password must be at least 8 characters long. Please try again.\n");
            return 0;
        } //should also add number & other requirements tbh
printf("====================================\n");
        break;

    default:
        printf("Invalid choice. Please try again.\n");
        return 0;
}

printf("Welcome %s! You have successfully registered.\n", fname);
printf("Your age is %d.\n", age);

















    return 0;
}