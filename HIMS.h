/*
 * HIMS - Hospital Information Management System
 * structures, arrays and the other files (compile only main.c)
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifdef _WIN32
#include <conio.h>
#endif

#define MAXPATIENTS 500
#define MAXDOCTORS 100
#define MAXPENDING 50
#define MAXAPPOINTMENTS 1000
#define MAXREVIEWS 1000
#define MAXWAITING 200
#define MAXCATEGORIES 30

struct patient {
    int  id;
    char name[50];
    char password[20];
    int  age;
    int  gender;        /* 1 = Male, 2 = Female, 3 = Other */
    char phone[20];
};

struct doctor {
    char  id[5];
    char  specialization[20];
    int   experience;
    char  qualification[20];
    char  hospital[20];
    int   fee;
    float rating;
    char  days[30];     /* like Mon,Wed,Fri */
    char  timing[15];   /* like 09:00-17:00 */
    char  password[20];
    char  name[50];
};

struct appointment {
    char id[10];
    int  patientid;
    char doctorid[10];
    int  fee;
    char date[15];      /* dd-mm-yyyy */
    char time[10];      /* like 10:30AM */
    char status[15];    /* Pending / Confirmed / Completed / Cancelled */
    char payment[15];   /* Pending / Verified / Failed */
    char method[15];    /* JazzCash / EasyPaisa / Bank / Card */
    char transaction[20];
    char doctorname[50];
};

struct review {
    char appointmentid[10];
    char doctorid[10];
    int  patientid;
    int  stars;
    char text[200];
};

struct waiting {
    int  patientid;
    char doctorid[10];
    char date[15];
    char time[10];
};

/* everything is read from the text files into these arrays when the program starts */
struct patient patients[MAXPATIENTS];
int patientcount = 0;

struct doctor doctors[MAXDOCTORS];
int doctorcount = 0;

/* doctors who registered but are not verified by the admin yet */
struct doctor pending[MAXPENDING];
int pendingcount = 0;

struct appointment appointments[MAXAPPOINTMENTS];
int appointmentcount = 0;

struct review reviews[MAXREVIEWS];
int reviewcount = 0;

struct waiting waitlist[MAXWAITING];
int waitcount = 0;

char categories[MAXCATEGORIES][20];
int categorycount = 0;

char shortdays[7][4] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
char fulldays[7][10] = {"Monday", "Tuesday", "Wednesday", "Thursday",
                        "Friday", "Saturday", "Sunday"};

/* start and end of a doctor's timing in minutes (set by readtiming) */
int slotstart = 0;
int slotend = 0;

/* the other files, the order matters because a function must come before it is used */
#include "common.c"
#include "waitlist.c"
#include "notifications.c"
#include "patient.c"
#include "appointments.c"
#include "payment.c"
#include "reviews.c"
#include "receipt.c"
#include "records.c"
#include "doctor.c"
#include "stats.c"
#include "reports.c"
#include "admin.c"
