#include <stdio.h>
#include <string.h>

int main() {
    int choice, age, gender, loggedin = 0, menu, special;
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

loggedin = 1; //if logged in =1 means logged in, if =0 means not logged in
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

if(gender == 1) {
    printf("Your gender is Male.\n");}
    else if(gender == 2) {
    printf("Your gender is Female.\n");
} else if(gender == 3) {
    printf("Your gender is Others.\n");
}
printf("====================================\n");
//if logged in ( =1 from login )
while (loggedin == 1){
    printf("\n=====MENU=====\n");
    printf("1. Find a doctor\n");
    printf("2. My info\n");
    printf("3. Change password\n");
    printf("0. Logout\n");
    printf("Enter your choice: ");
    scanf("%d", &menu);

if(menu == 1) {
    printf("Finding a doctor...\n");
    printf("====================================\n");
    printf("Select specialization:\n");
    printf("1. Cardiologist\n");
    printf("2. Dermatologist\n");
    printf("3. Orthopedic Surgeon\n");
    printf("4. Pediatrician\n");
    printf("5. General Practitioner\n");
    printf("====================================\n");
    scanf("%d", &special);
switch(special) {
    case 1:
        printf("You have selected Cardiologist.\n");
        printf("Available doctors: \n");
            printf("1. Dr. John Smith -- ⭐⭐⭐\n");
            printf("2. Dr. Emily Johnson -- ⭐⭐⭐⭐⭐\n");
            printf("3. Dr. Michael Brown -- ⭐⭐⭐⭐\n");
        break;
    case 2:
        printf("You have selected Dermatologist.\n");
        printf("Available doctors: \n");
            printf("1. Dr. Sarah Davis -- ⭐⭐⭐⭐\n");
            printf("2. Dr. David Wilson -- ⭐⭐⭐⭐⭐\n");
            printf("3. Dr. Jessica Lee -- ⭐⭐⭐\n");
        break;
    case 3:
        printf("You have selected Orthopedic Surgeon.\n");
        printf("Available doctors: \n");
            printf("1. Dr. James Anderson -- ⭐⭐⭐⭐\n");
            printf("2. Dr. Jennifer Martinez -- ⭐⭐⭐\n");
            printf("3. Dr. William Taylor -- ⭐⭐⭐\n");
        break;
    case 4:
        printf("You have selected Pediatrician.\n");
        printf("Available doctors: \n");
            printf("1. Dr. Elizabeth Thomas -- ⭐⭐⭐\n");
            printf("2. Dr. Christopher Jackson -- ⭐\n");
            printf("3. Dr. Amanda White -- ⭐⭐⭐\n");
        break;
    case 5:
        printf("You have selected General Practitioner.\n");
        printf("Available doctors: \n");
            printf("1. Dr. Matthew Harris -- ⭐⭐⭐\n");
            printf("2. Dr. Ashley Clark -- ⭐⭐⭐⭐⭐\n");
            printf("3. Dr. Joshua Lewis -- ⭐⭐\n");
        break;
    default:
        printf("Invalid specialization choice. Please try again.\n");
        return 0;
}
}

else if(menu == 2) {
    printf("Displaying your info...\n");
} 

else if(menu == 3) {
    printf("Changing your password...\n");
} 

else if(menu == 0) {
    printf("Logging out...\n");
    loggedin = 0;}















}















    return 0;
}
