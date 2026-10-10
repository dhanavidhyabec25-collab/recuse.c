#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 500
#define FILE_NAME "campus_complaints.txt"

typedef struct {
    char id[20];
    char category[40];
    char problem[150];
    char priority[15];
    char location[60];
    char status[20];
    char department[40];
} Complaint;

Complaint complaints[MAX];
int count = 0;

/* Remove newline from input */
void removeNewline(char *s) {
    s[strcspn(s, "\n")] = '\0';
}

/* Read text safely */
void readText(const char *prompt, char *s, int size) {
    int ch;
    printf("%s", prompt);

    if (fgets(s, size, stdin) == NULL) {
        s[0] = '\0';
        return;
    }

    if (strchr(s, '\n') == NULL) {
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* Clear extra input */
        }
    }

    removeNewline(s);

    /* Prevent file delimiter from breaking records */
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] == '|')
            s[i] = '/';
    }
}

/* Read a menu number */
int readNumber(const char *prompt, int min, int max) {
    char line[50];
    char extra;
    int n;

    while (1) {
        printf("%s", prompt);

        if (fgets(line, sizeof(line), stdin) == NULL)
            return min;

        if (strchr(line, '\n') == NULL) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
        }

        if (sscanf(line, " %d %c", &n, &extra) == 1 &&
            n >= min && n <= max)
            return n;

        printf("Invalid input! Enter a number from %d to %d.\n",
               min, max);
    }
}

/* Convert text to lowercase for searching */
void toLowerText(char *s) {
    for (int i = 0; s[i] != '\0'; i++)
        s[i] = (char)tolower((unsigned char)s[i]);
}

/* Case-insensitive comparison */
int sameText(const char *a, const char *b) {
    char x[200], y[200];

    snprintf(x, sizeof(x), "%s", a);
    snprintf(y, sizeof(y), "%s", b);

    toLowerText(x);
    toLowerText(y);

    return strcmp(x, y) == 0;
}

/* Get priority rank */
int priorityRank(const char *priority) {
    if (sameText(priority, "High")) return 1;
    if (sameText(priority, "Medium")) return 2;
    return 3;
}

/* Save all complaints to a text file */
void saveComplaints(void) {
    FILE *fp = fopen(FILE_NAME, "w");

    if (fp == NULL) {
        printf("Error: Unable to save complaint records.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s|%s|%s|%s|%s|%s|%s\n",
                complaints[i].id,
                complaints[i].category,
                complaints[i].problem,
                complaints[i].priority,
                complaints[i].location,
                complaints[i].status,
                complaints[i].department);
    }

    fclose(fp);
}

/* Load saved complaints when the program starts */
void loadComplaints(void) {
    FILE *fp = fopen(FILE_NAME, "r");
    char line[500];

    if (fp == NULL)
        return;

    while (count < MAX && fgets(line, sizeof(line), fp)) {
        Complaint c;
        char *p;

        removeNewline(line);

        p = strtok(line, "|");
        if (!p) continue;
        snprintf(c.id, sizeof(c.id), "%s", p);

        p = strtok(NULL, "|");
        if (!p) continue;
        snprintf(c.category, sizeof(c.category), "%s", p);

        p = strtok(NULL, "|");
        if (!p) continue;
        snprintf(c.problem, sizeof(c.problem), "%s", p);

        p = strtok(NULL, "|");
        if (!p) continue;
        snprintf(c.priority, sizeof(c.priority), "%s", p);

        p = strtok(NULL, "|");
        if (!p) continue;
        snprintf(c.location, sizeof(c.location), "%s", p);

        p = strtok(NULL, "|");
        if (!p) continue;
        snprintf(c.status, sizeof(c.status), "%s", p);

        p = strtok(NULL, "|");
        if (!p) continue;
        snprintf(c.department, sizeof(c.department), "%s", p);

        complaints[count++] = c;
    }

    fclose(fp);
}

/* Generate a unique complaint ID */
void generateID(char *id, size_t size) {
    int maxID = 100;

    for (int i = 0; i < count; i++) {
        if (complaints[i].id[0] == 'C') {
            int n = atoi(complaints[i].id + 1);
            if (n > maxID)
                maxID = n;
        }
    }

    snprintf(id, size, "C%d", maxID + 1);
}

/* Register a complaint */
void registerComplaint(void) {
    if (count >= MAX) {
        printf("\nComplaint storage is full!\n");
        return;
    }

    Complaint c;
    int choice;

    printf("\n========== REGISTER COMPLAINT ==========\n");

    generateID(c.id, sizeof(c.id));
    printf("Generated Complaint ID: %s\n", c.id);

    printf("\nChoose category:\n");
    printf("1. Laboratory\n");
    printf("2. Classroom\n");
    printf("3. Hostel\n");
    printf("4. Transport\n");
    printf("5. Network Services\n");
    printf("6. Electricity\n");
    printf("7. Water / Sanitation\n");
    printf("8. Other\n");

    choice = readNumber("Enter category (1-8): ", 1, 8);

    const char *categories[] = {
        "Laboratory", "Classroom", "Hostel", "Transport",
        "Network Services", "Electricity",
        "Water / Sanitation", "Other"
    };

    snprintf(c.category, sizeof(c.category), "%s",
             categories[choice - 1]);

    readText("Enter problem description: ",
             c.problem, sizeof(c.problem));

    readText("Enter exact location: ",
             c.location, sizeof(c.location));

    readText("Enter responsible department/team: ",
             c.department, sizeof(c.department));

    printf("\nSelect severity:\n");
    printf("1. High   - Safety risk / major disruption\n");
    printf("2. Medium - Important service affected\n");
    printf("3. Low    - Minor issue\n");

    choice = readNumber("Enter severity (1-3): ", 1, 3);

    const char *priorities[] = {"High", "Medium", "Low"};

    snprintf(c.priority, sizeof(c.priority), "%s",
             priorities[choice - 1]);

    /* New complaints start as Pending */
    snprintf(c.status, sizeof(c.status), "Pending");

    complaints[count++] = c;
    saveComplaints();

    printf("\nComplaint registered successfully!\n");
    printf("Complaint ID : %s\n", c.id);
    printf("Priority     : %s\n", c.priority);
    printf("Status       : %s\n", c.status);
}

/* Display one complaint */
void displayOne(const Complaint *c) {
    printf("\n----------------------------------------\n");
    printf("Complaint ID : %s\n", c->id);
    printf("Category     : %s\n", c->category);
    printf("Problem      : %s\n", c->problem);
    printf("Priority     : %s\n", c->priority);
    printf("Location     : %s\n", c->location);
    printf("Department   : %s\n", c->department);
    printf("Status       : %s\n", c->status);
}

/* Display all complaints */
void displayAll(void) {
    if (count == 0) {
        printf("\nNo complaints registered yet.\n");
        return;
    }

    printf("\n========== ALL COMPLAINTS (%d) ==========\n",
           count);

    for (int i = 0; i < count; i++)
        displayOne(&complaints[i]);
}

/* Find complaint index using ID */
int findByID(const char *id) {
    for (int i = 0; i < count; i++) {
        if (sameText(complaints[i].id, id))
            return i;
    }
    return -1;
}

/* Search by ID, category, location or problem */
void searchComplaint(void) {
    char query[150];
    int found = 0;

    if (count == 0) {
        printf("\nNo complaints available to search.\n");
        return;
    }

    readText("\nEnter complaint ID, category, location or keyword: ",
             query, sizeof(query));

    char q[150];
    snprintf(q, sizeof(q), "%s", query);
    toLowerText(q);

    for (int i = 0; i < count; i++) {
        char id[30], category[50], location[70], problem[160];

        snprintf(id, sizeof(id), "%s", complaints[i].id);
        snprintf(category, sizeof(category), "%s",
                 complaints[i].category);
        snprintf(location, sizeof(location), "%s",
                 complaints[i].location);
        snprintf(problem, sizeof(problem), "%s",
                 complaints[i].problem);

        toLowerText(id);
        toLowerText(category);
        toLowerText(location);
        toLowerText(problem);

        if (strstr(id, q) || strstr(category, q) ||
            strstr(location, q) || strstr(problem, q)) {
            displayOne(&complaints[i]);
            found = 1;
        }
    }

    if (!found)
        printf("\nNo matching complaint found.\n");
}

/* Sort complaints by priority: High, Medium, Low */
void sortByPriority(void) {
    if (count == 0) {
        printf("\nNo complaints available to sort.\n");
        return;
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (priorityRank(complaints[j].priority) >
                priorityRank(complaints[j + 1].priority)) {
                Complaint temp = complaints[j];
                complaints[j] = complaints[j + 1];
                complaints[j + 1] = temp;
            }
        }
    }

    saveComplaints();

    printf("\nComplaints sorted by priority:\n");
    displayAll();
}

/* Update complaint status */
void updateStatus(void) {
    char id[30];
    int index, choice;

    readText("\nEnter complaint ID: ", id, sizeof(id));

    index = findByID(id);

    if (index == -1) {
        printf("\nComplaint ID not found.\n");
        return;
    }

    displayOne(&complaints[index]);

    printf("\nSelect new status:\n");
    printf("1. Pending\n");
    printf("2. In Progress\n");
    printf("3. Resolved\n");

    choice = readNumber("Enter status (1-3): ", 1, 3);

    const char *statuses[] = {
        "Pending", "In Progress", "Resolved"
    };

    snprintf(complaints[index].status,
             sizeof(complaints[index].status),
             "%s", statuses[choice - 1]);

    saveComplaints();

    printf("\nStatus updated successfully to: %s\n",
           complaints[index].status);
}

/* Display pending complaints */
void displayPending(void) {
    int found = 0;

    printf("\n========== PENDING COMPLAINTS ==========\n");

    for (int i = 0; i < count; i++) {
        if (sameText(complaints[i].status, "Pending")) {
            displayOne(&complaints[i]);
            found = 1;
        }
    }

    if (!found)
        printf("No pending complaints.\n");
}

/* Filter by category, location or status */
void filterComplaints(void) {
    int choice, found = 0;
    char query[100];

    printf("\n========== FILTER COMPLAINTS ==========\n");
    printf("1. By category\n");
    printf("2. By location\n");
    printf("3. By status\n");

    choice = readNumber("Choose filter (1-3): ", 1, 3);

    readText("Enter filter value: ", query, sizeof(query));

    for (int i = 0; i < count; i++) {
        int match = 0;

        if (choice == 1)
            match = sameText(complaints[i].category, query);
        else if (choice == 2)
            match = (strstr(complaints[i].location, query) != NULL);
        else
            match = sameText(complaints[i].status, query);

        if (match) {
            displayOne(&complaints[i]);
            found = 1;
        }
    }

    if (!found)
        printf("\nNo complaints match that filter.\n");
}

/* Generate summary and department-wise statistics */
void generateSummary(void) {
    int high = 0, medium = 0, low = 0;
    int pending = 0, progress = 0, resolved = 0;

    printf("\n========== CAMPUS COMPLAINT SUMMARY ==========\n");
    printf("Total complaints: %d\n", count);

    for (int i = 0; i < count; i++) {
        if (sameText(complaints[i].priority, "High"))
            high++;
        else if (sameText(complaints[i].priority, "Medium"))
            medium++;
        else
            low++;

        if (sameText(complaints[i].status, "Pending"))
            pending++;
        else if (sameText(complaints[i].status, "In Progress"))
            progress++;
        else if (sameText(complaints[i].status, "Resolved"))
            resolved++;
    }

    printf("\n--- Priority Statistics ---\n");
    printf("High   : %d\n", high);
    printf("Medium : %d\n", medium);
    printf("Low    : %d\n", low);

    printf("\n--- Status Statistics ---\n");
    printf("Pending     : %d\n", pending);
    printf("In Progress : %d\n", progress);
    printf("Resolved    : %d\n", resolved);

    printf("\n--- Category-wise Statistics ---\n");

    for (int i = 0; i < count; i++) {
        int alreadyCounted = 0;

        for (int j = 0; j < i; j++) {
            if (sameText(complaints[i].category,
                         complaints[j].category)) {
                alreadyCounted = 1;
                break;
            }
        }

        if (!alreadyCounted) {
            int total = 0;

            for (int j = 0; j < count; j++) {
                if (sameText(complaints[i].category,
                             complaints[j].category))
                    total++;
            }

            printf("%-22s : %d\n",
                   complaints[i].category, total);
        }
    }

    printf("\n--- Department-wise Statistics ---\n");

    for (int i = 0; i < count; i++) {
        int alreadyCounted = 0;

        for (int j = 0; j < i; j++) {
            if (sameText(complaints[i].department,
                         complaints[j].department)) {
                alreadyCounted = 1;
                break;
            }
        }

        if (!alreadyCounted) {
            int total = 0, done = 0;

            for (int j = 0; j < count; j++) {
                if (sameText(complaints[i].department,
                             complaints[j].department)) {
                    total++;
                    if (sameText(complaints[j].status, "Resolved"))
                        done++;
                }
            }

            printf("%-22s : %d total, %d resolved\n",
                   complaints[i].department, total, done);
        }
    }

    if (count > 0) {
        double rate = (double)resolved * 100.0 / count;

        printf("\nOverall resolution rate: %.2f%%\n", rate);
    } else {
        printf("\nOverall resolution rate: 0.00%%\n");
    }
}

/* Main menu */
int main(void) {
    int choice;

    loadComplaints();

    printf("============================================\n");
    printf("       SMART CAMPUS COMPLAINT SYSTEM\n");
    printf("============================================\n");
    printf("Loaded %d saved complaint(s).\n", count);

    do {
        printf("\n=============== MAIN MENU ===============\n");
        printf("1. Register New Complaint\n");
        printf("2. Display All Complaints\n");
        printf("3. Search Complaint\n");
        printf("4. Sort by Priority\n");
        printf("5. Update Complaint Status\n");
        printf("6. Display Pending Complaints\n");
        printf("7. Filter Complaints\n");
        printf("8. Generate Complaint Summary\n");
        printf("9. Exit\n");
        printf("=========================================\n");

        choice = readNumber("Enter your choice (1-9): ", 1, 9);

        switch (choice) {
            case 1:
                registerComplaint();
                break;
            case 2:
                displayAll();
                break;
            case 3:
                searchComplaint();
                break;
            case 4:
                sortByPriority();
                break;
            case 5:
                updateStatus();
                break;
            case 6:
                displayPending();
                break;
            case 7:
                filterComplaints();
                break;
            case 8:
                generateSummary();
                break;
            case 9:
                saveComplaints();
                printf("\nAll records saved successfully.\n");
                printf("Thank you for using Smart Campus Complaint System!\n");
                break;
        }

    } while (choice != 9);

    return 0;
}
