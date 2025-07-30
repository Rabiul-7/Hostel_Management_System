#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct user
{
    char username[100];
    char password[100];
    char role[10];
} user;

typedef struct student_registration
{
    char name[100];
    char department[100];
    char semester[10];
    char id[20];
    char number[20];
    char room[10];
    char username[20];
} student;

// Function Declarations
void registration();
int login(char *logusername, char *logroll);
void studentdashboard(char *username);
void myprofile(char *username);
void payfees(char *usrname);
void complaint(char *username);
void admindashboard();
void viewallstudent();
void availableroom();
void reallocate();
void viewComplain();
void viewFeeReports();
void flushInputBuffer();

int main()
{
    int choice;
    char username[100];
    char roll[10];
    while (1)
    {
        printf("\n========== HOSTEL MANAGEMENT SYSTEM ==========\n");
        printf("1. Registration (new student)\n");
        printf("2. Login\n");
        printf("3. Exit the program\n");
        printf("---------------------------------------------\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input! Please enter a number.\n");
            flushInputBuffer();
            continue;
        }
        flushInputBuffer();

        switch (choice)
        {
        case 1:
            registration();
            break;
        case 2:
            if (login(username, roll))
            {
                printf("\n--- Login successful ---\n");
                if (strcmp(roll, "student") == 0)
                {
                    printf("\n=== Welcome student: %s ===\n\n", username);
                    studentdashboard(username);
                }
                else if (strcmp(roll, "admin") == 0)
                {
                    printf("\n=== Welcome Admin ===\n\n");
                    admindashboard();
                }
            }
            else
            {
                printf("Invalid username or password!\n");
            }
            break;
        case 3:
            printf("Exiting the program...\n");
            exit(0);
        default:
            printf("Invalid choice!\n");
            break;
        }
    }

    return 0;
}

// Helper: Flush Input Buffer (for invalid scanf or after char input)
void flushInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

// Only allow room that's vacant (validated)
void getValidRoom(char *roomOut)
{
    int valid;
    do
    {
        valid = 0;
        availableroom();
        printf("Choose a vacant Room: ");
        scanf(" %[^\n]", roomOut);

        FILE *rptr = fopen("room.txt", "r");
        if (rptr == NULL)
        {
            printf("room.txt file doesn't exist. Contact admin.\n");
            exit(1);
        }
        char rnum[20], rstatus[20];
        while (fscanf(rptr, "%[^|]|%[^\n]\n", rnum, rstatus) != EOF)
        {
            if (strcmp(rnum, roomOut) == 0 && strcmp(rstatus, "vacant") == 0)
            {
                valid = 1;
                break;
            }
        }
        fclose(rptr);

        if (!valid)
        {
            printf("Room is not vacant or invalid! Please choose a vacant room.\n");
        }
    } while (!valid);
}

// Student Registration (with robust input and vacant room validation)
void registration()
{
    user newuser;
    student newstudent;

    printf("\n======= Student Registration =======\n");
    printf("Enter Your Information:\n");

    printf("Username: ");
    scanf(" %[^\n]", newuser.username);
    printf("Password: ");
    scanf(" %[^\n]", newuser.password);
    strcpy(newuser.role, "student");
    strcpy(newstudent.username, newuser.username);

    printf("Full Name: ");
    scanf(" %[^\n]", newstudent.name);
    printf("Department: ");
    scanf(" %[^\n]", newstudent.department);
    printf("Semester: ");
    scanf(" %[^\n]", newstudent.semester);
    printf("ID: ");
    scanf(" %[^\n]", newstudent.id);
    printf("Contact Number: ");
    scanf(" %[^\n]", newstudent.number);

    // Vacant Room Only
    getValidRoom(newstudent.room);

    // Save to files (All file open error-checked)
    FILE *fee = fopen("fee.txt", "a");
    if (fee == NULL)
    {
        printf("fee.txt open error!\n");
        return;
    }
    fprintf(fee, "%s|UNPAID\n", newstudent.id);
    fclose(fee);

    FILE *fuser = fopen("user.txt", "a");
    if (fuser == NULL)
    {
        printf("user.txt open error!\n");
        return;
    }
    fprintf(fuser, "%s|%s|%s\n", newuser.username, newuser.password, newuser.role);
    fclose(fuser);

    FILE *fstudent = fopen("student.txt", "a");
    if (fstudent == NULL)
    {
        printf("student.txt open error!\n");
        return;
    }
    fprintf(fstudent, "%s|%s|%s|%s|%s|%s|%s\n", newstudent.name, newstudent.department, newstudent.semester, newstudent.id, newstudent.number, newstudent.room, newstudent.username);
    fclose(fstudent);

    // Update room.txt (make that room occupied)
    FILE *ptr = fopen("room.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (ptr == NULL || temp == NULL)
    {
        printf("room.txt open error!\n");
        if (ptr)
            fclose(ptr);
        if (temp)
            fclose(temp);
        return;
    }
    char room[20], status[20];
    while (fscanf(ptr, "%[^|]|%[^\n]\n", room, status) != EOF)
    {
        if (strcmp(room, newstudent.room) == 0)
            fprintf(temp, "%s|occupied\n", room);
        else
            fprintf(temp, "%s|%s\n", room, status);
    }
    fclose(ptr);
    fclose(temp);
    remove("room.txt");
    rename("temp.txt", "room.txt");

    printf("\n--- Registration successful! ---\n");
}

// Login function (returns 1 on success, else 0)
int login(char *logusername, char *logroll)
{
    char username[100], password[100];
    printf("\n\n===== Login =====\n");
    printf("Enter Your Username: ");
    scanf(" %[^\n]", username);
    printf("Enter Your Password: ");
    scanf(" %[^\n]", password);

    FILE *ptr = fopen("user.txt", "r");
    if (ptr == NULL)
    {
        printf("user.txt file doesn't exist!\n");
        return 0;
    }

    user u;
    int found = 0;
    while (fscanf(ptr, "%[^|]|%[^|]|%s\n", u.username, u.password, u.role) != EOF)
    {
        if (strcmp(username, u.username) == 0 && strcmp(password, u.password) == 0)
        {
            strcpy(logusername, u.username);
            strcpy(logroll, u.role);
            found = 1;
            break;
        }
    }
    fclose(ptr);
    return found;
}

void studentdashboard(char *username)
{
    int choice;
    while (1)
    {
        printf("\n===== Student Dashboard =====\n");
        printf("1. View My Profile\n");
        printf("2. Pay Fees\n");
        printf("3. Submit Complaint\n");
        printf("4. Log Out\n");
        printf("---------------------------------\n");
        printf("Enter Your Choice: ");
        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input! Enter number only.\n");
            flushInputBuffer();
            continue;
        }
        flushInputBuffer();

        switch (choice)
        {
        case 1:
            myprofile(username);
            break;
        case 2:
            payfees(username);
            break;
        case 3:
            complaint(username);
            break;
        case 4:
            printf("Log Out Successful\n");
            return;
        default:
            printf("Invalid Choice!\n");
        }
    }
}

// View student's own profile
void myprofile(char *username)
{
    char name[50], department[50], semester[20], id[20], number[20], room[10], uname[20];
    FILE *ptr = fopen("student.txt", "r");
    if (!ptr)
    {
        printf("student.txt file not found!\n");
        return;
    }

    int found = 0;
    while (fscanf(ptr, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]\n", name, department, semester, id, number, room, uname) != EOF)
    {
        if (strcmp(username, uname) == 0)
        {
            printf("\n===== Student Profile =====\n");
            printf("Name        : %s\n", name);
            printf("Department  : %s\n", department);
            printf("Semester    : %s\n", semester);
            printf("ID          : %s\n", id);
            printf("Number      : %s\n", number);
            printf("Room        : %s\n\n", room);
            found = 1;
            break;
        }
    }
    fclose(ptr);
    if (!found)
        printf("Profile not found!\n");
}

// Pay fees (only if not already paid)
void payfees(char *usrname)
{
    char line[300], username[20], id[20];
    int found = 0;

    FILE *ptr = fopen("student.txt", "r");
    if (!ptr)
    {
        printf("student.txt not found!\n");
        return;
    }
    while (fgets(line, sizeof(line), ptr))
    {
        sscanf(line, "%*[^|]|%*[^|]|%*[^|]|%[^|]|%*[^|]|%*[^|]|%[^\n]\n", id, username);
        if (strcmp(username, usrname) == 0)
        {
            found = 1;
            break;
        }
    }
    fclose(ptr);
    if (!found)
    {
        printf("Student not found\n");
        return;
    }

    char ID[20], status[20];
    FILE *fee = fopen("fee.txt", "r");
    FILE *tempt = fopen("tempt.txt", "w");
    if (!fee || !tempt)
    {
        printf("fee.txt file error!\n");
        if (fee)
            fclose(fee);
        if (tempt)
            fclose(tempt);
        return;
    }
    int paid = 0;
    while (fscanf(fee, "%[^|]|%[^\n]\n", ID, status) != EOF)
    {
        if (strcmp(ID, id) == 0)
        {
            printf("Your Fee Status: %s\n", status);
            if (strcmp(status, "PAID") == 0)
            {
                printf("Fee already paid\n");
                fprintf(tempt, "%s|%s\n", ID, status);
                paid = 1;
            }
            else
            {
                char ch;
                printf("Do you want to pay (y/n): ");
                scanf(" %c", &ch);
                flushInputBuffer();
                if (ch == 'y' || ch == 'Y')
                {
                    printf("Payment successful\n");
                    fprintf(tempt, "%s|PAID\n", ID);
                    paid = 1;
                }
                else
                {
                    fprintf(tempt, "%s|%s\n", ID, status);
                }
            }
        }
        else
        {
            fprintf(tempt, "%s|%s\n", ID, status);
        }
    }
    fclose(fee);
    fclose(tempt);
    remove("fee.txt");
    rename("tempt.txt", "fee.txt");
    if (!paid)
        printf("Fee not paid.\n");
}

// Student submits a complaint (attached to student ID)
void complaint(char *username)
{
    char msg[300], uname[20], line[200], id[20];
    int found = 0;

    FILE *ptr = fopen("student.txt", "r");
    if (!ptr)
    {
        printf("student.txt not found!\n");
        return;
    }
    while (fgets(line, sizeof(line), ptr))
    {
        sscanf(line, "%*[^|]|%*[^|]|%*[^|]|%[^|]|%*[^|]|%*[^|]|%[^\n]\n", id, uname);
        if (strcmp(username, uname) == 0)
        {
            found = 1;
            break;
        }
    }
    fclose(ptr);
    if (!found)
    {
        printf("Student not found\n");
        return;
    }

    printf("Enter your complaint: ");
    scanf(" %[^\n]", msg);

    FILE *complaint = fopen("complaint.txt", "a");
    if (!complaint)
    {
        printf("complaint.txt open error!\n");
        return;
    }
    fprintf(complaint, "%s|%s\n", id, msg);
    fclose(complaint);

    printf("Complaint saved successfully.\n");
}

// ADMIN Dashboard (with all options)
void admindashboard()
{
    int choice;
    while (1)
    {
        printf("\n====== Admin Dashboard ======\n");
        printf("1. View All Students\n");
        printf("2. Reallocate Room\n");
        printf("3. View Room Availability\n");
        printf("4. View Complaints\n");
        printf("5. View Fee Reports\n");
        printf("6. Logout\n");
        printf("--------------------------------\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input! Enter number only.\n");
            flushInputBuffer();
            continue;
        }
        flushInputBuffer();

        switch (choice)
        {
        case 1:
            viewallstudent();
            break;
        case 2:
            reallocate();
            break;
        case 3:
            availableroom();
            break;
        case 4:
            viewComplain();
            break;
        case 5:
            viewFeeReports();
            break;
        case 6:
            printf("Logging out from Admin Dashboard...\n");
            return;
        default:
            printf("Invalid choice! Try again.\n");
        }
    }
}

// View all students (admin only)
void viewallstudent()
{
    FILE *ptr = fopen("student.txt", "r");
    if (ptr == NULL)
    {
        printf("student.txt doesn't exist\n");
        return;
    }
    char name[100], department[100], semester[10], id[20], number[20], room[10], username[20];
    printf("\n======= All Student List =======\n");
    printf("Name\t\t\tDepartment\tSemester\tID\t\tNumber\t\tRoom\n");
    printf("---------------------------------------------------------------------------------------------------\n");
    while (fscanf(ptr, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]\n", name, department, semester, id, number, room, username) != EOF)
    {
        printf("%-20s\t%-10s\t%-10s\t%-10s\t%-10s\t%-7s\n", name, department, semester, id, number, room);
    }
    fclose(ptr);
    printf("---------------------------------------------------------------------------------------------------\n");
}

// Show available rooms only
void availableroom()
{
    FILE *ptr = fopen("room.txt", "r");
    if (ptr == NULL)
    {
        printf("room.txt doesn't exist\n");
        return;
    }
    char room[10], status[20];
    int count = 0;
    printf("\n======= Available Room List =======\n");
    printf("-----------------------------------\n");
    while (fscanf(ptr, "%[^|]|%[^\n]\n", room, status) != EOF)
    {
        if (strcmp(status, "vacant") == 0)
        {
            printf("%s\n", room);
            count++;
        }
    }
    fclose(ptr);
    if (!count)
    {
        printf("There are no available rooms\n");
    }
    printf("-----------------------------------\n");
}

// Reallocate room for student (admin only)
void reallocate()
{
    char name[100], department[100], semester[10], id[20], number[20], room[10], username[20];
    char ID[20], status[20], newroom[20], oldroom[20];
    int found = 0;

    printf("Enter student ID: ");
    scanf(" %[^\n]", ID);

    FILE *student = fopen("student.txt", "r");
    if (!student)
    {
        printf("student.txt not found!\n");
        return;
    }
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        fclose(student);
        printf("temp.txt error!\n");
        return;
    }

    int valid_room = 0;
    // First find student, ask for valid room
    while (fscanf(student, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]\n", name, department, semester, id, number, room, username) != EOF)
    {
        if (strcmp(ID, id) == 0)
        {
            strcpy(oldroom, room);
            // Vacant room validation
            getValidRoom(newroom);
            fprintf(temp, "%s|%s|%s|%s|%s|%s|%s\n", name, department, semester, id, number, newroom, username);
            found = 1;
        }
        else
        {
            fprintf(temp, "%s|%s|%s|%s|%s|%s|%s\n", name, department, semester, id, number, room, username);
        }
    }
    fclose(student);
    fclose(temp);

    if (!found)
    {
        printf("ID not found\n");
        remove("temp.txt");
        return;
    }

    remove("student.txt");
    rename("temp.txt", "student.txt");

    // Update room.txt (vacant old, occupied new)
    FILE *ptr = fopen("room.txt", "r");
    FILE *tempt = fopen("tempt.txt", "w");
    if (!ptr || !tempt)
    {
        if (ptr)
            fclose(ptr);
        if (tempt)
            fclose(tempt);
        printf("room.txt/tempt.txt error!\n");
        return;
    }

    while (fscanf(ptr, "%[^|]|%[^\n]\n", room, status) != EOF)
    {
        if (strcmp(room, oldroom) == 0)
            fprintf(tempt, "%s|vacant\n", oldroom);
        else if (strcmp(room, newroom) == 0)
            fprintf(tempt, "%s|occupied\n", newroom);
        else
            fprintf(tempt, "%s|%s\n", room, status);
    }

    fclose(ptr);
    fclose(tempt);
    remove("room.txt");
    rename("tempt.txt", "room.txt");
    printf("Room has been reallocated from %s to %s\n", oldroom, newroom);
}

// Admin: View all complaints
void viewComplain()
{
    FILE *com = fopen("complaint.txt", "r");
    if (com == NULL)
    {
        printf("complaint.txt doesn't exist\n");
        return;
    }

    char complain[200], id[20];
    int count = 0;
    printf("\n======= All Complaints =======\n");
    printf("ID\t\tComplaint\n");
    printf("------------------------------------------\n");
    while (fscanf(com, "%[^|]|%[^\n]\n", id, complain) != EOF)
    {
        printf("%-15s\t--->%s\n", id, complain);
        count++;
    }
    if (!count)
        printf("There are no complaints found.\n");
    printf("------------------------------------------\n");
    fclose(com);
}

// Admin: Fee Reports
void viewFeeReports()
{
    FILE *fee = fopen("fee.txt", "r");
    if (fee == NULL)
    {
        printf("fee.txt file not found!\n");
        return;
    }
    char id[20], status[20];
    printf("\n======= Fee Reports =======\n");
    printf("StudentID\tStatus\n");
    printf("---------------------------\n");
    while (fscanf(fee, "%[^|]|%[^\n]\n", id, status) != EOF)
    {
        printf("%-10s\t--->%s\n", id, status);
    }
    fclose(fee);
    printf("---------------------------\n");
}
