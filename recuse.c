v#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100
#define FILE_NAME "rescue_records.txt"

typedef struct {
    char id[20];
    char location[60];
    char emergency[20];
    char resources[50];
    char team[40];
    char status[20];
    int people;
    int priority;
} Rescue;

Rescue requests[MAX];
int count = 0;

/* Read text input */
void readText(const char *msg, char *s, int size) {
    printf("%s", msg);

    if (fgets(s, size, stdin) != NULL) {
        size_t len = strlen(s);

        if (len > 0 && s[len - 1] == '\n') {
            s[len - 1] = '\0';
        } else {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
                /* Clear remaining input */
            }
        }
    } else {
        s[0] = '\0';
    }
}

/* Read a valid integer */
int readNumber(const char *msg) {
    char line[40];
    char extra;
    int number;

    while (1) {
        readText(msg, line, sizeof(line));

        if (sscanf(line, "%d %c", &number, &extra) == 1) {
            return number;
        }

        printf("Invalid input! Enter a number.\n");
    }
}

/* Case-insensitive matching */
int contains(const char *s, const char *word) {
    char a[100];
    char b[100];
    int i;

    snprintf(a, sizeof(a), "%s", s);
    snprintf(b, sizeof(b), "%s", word);

    for (i = 0; a[i] != '\0'; i++) {
        a[i] = (char)tolower((unsigned char)a[i]);
    }

    for (i = 0; b[i] != '\0'; i++) {
        b[i] = (char)tolower((unsigned char)b[i]);
    }

    return strstr(a, b) != NULL;
}

/* Calculate demonstration priority score */
int calculatePriority(const char *level, int people) {
    int score;

    if (strcmp(level, "Critical") == 0) {
        score = 400;
    } else if (strcmp(level, "High") == 0) {
        score = 300;
    } else if (strcmp(level, "Medium") == 0) {
        score = 200;
    } else {
        score = 100;
    }

    if (people >= 50) {
        score += 40;
    } else if (people >= 20) {
        score += 30;
    } else if (people >= 10) {
        score += 20;
    } else if (people >= 5) {
        score += 10;
    }

    return score;
}

const char *priorityName(int score) {
    if (score >= 400) return "CRITICAL";
    if (score >= 300) return "HIGH";
    if (score >= 200) return "MEDIUM";
    return "LOW";
}

/* Suggest a team */
void assignTeam(Rescue *r) {
    if (contains(r->resources, "medical") ||
        contains(r->resources, "ambulance")) {
        strcpy(r->team, "Medical Team");
    } else if (contains(r->resources, "fire")) {
        strcpy(r->team, "Fire and Rescue");
    } else if (contains(r->resources, "flood") ||
               contains(r->resources, "boat") ||
               contains(r->resources, "water")) {
        strcpy(r->team, "Flood Rescue");
    } else if (contains(r->resources, "earthquake") ||
               contains(r->resources, "debris")) {
        strcpy(r->team, "Disaster Rescue");
    } else {
        strcpy(r->team, "General Rescue");
    }
}

/* Find a request by ID */
int findRequest(const char *id) {
    int i;

    for (i = 0; i < count; i++) {
        if (strcmp(requests[i].id, id) == 0) {
            return i;
        }
    }

    return -1;
}

/* Save all records */
void saveRecords(void) {
    FILE *fp = fopen(FILE_NAME, "w");
    int i;

    if (fp == NULL) {
        printf("Warning: Could not save records.\n");
        return;
    }

    for (i = 0; i < count; i++) {
        fprintf(fp, "%s|%s|%d|%s|%s|%s|%s\n",
                requests[i].id,
                requests[i].location,
                requests[i].people,
                requests[i].emergency,
                requests[i].resources,
                requests[i].team,
                requests[i].status);
    }

    fclose(fp);
}

/* Load saved records */
void loadRecords(void) {
    FILE *fp = fopen(FILE_NAME, "r");
    char line[300];
    Rescue r;

    if (fp == NULL) {
        return;
    }

    while (count < MAX && fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line,
                   "%19[^|]|%59[^|]|%d|%19[^|]|%49[^|]|%39[^|]|%19[^\n]",
                   r.id,
                   r.location,
                   &r.people,
                   r.emergency,
                   r.resources,
                   r.team,
                   r.status) == 7) {

            if (r.people < 1 || findRequest(r.id) != -1) {
                continue;
            }

            r.priority = calculatePriority(r.emergency, r.people);
            requests[count++] = r;
        }
    }

    fclose(fp);
}

/* Register a request */
void registerRequest(void) {
    Rescue r = {0};

    if (count >= MAX) {
        printf("Maximum request limit reached!\n");
        return;
    }

    readText("Enter Request ID: ", r.id, sizeof(r.id));

    if (r.id[0] == '\0' || findRequest(r.id) != -1) {
        printf("ID is empty or already exists!\n");
        return;
    }

    readText("Enter Location: ", r.location, sizeof(r.location));

    if (r.location[0] == '\0' || strchr(r.location, '|') != NULL) {
        printf("Enter a valid location without the | character.\n");
        return;
    }

    r.people = readNumber("People affected: ");

    if (r.people < 1) {
        printf("People affected must be at least 1.\n");
        return;
    }

    readText("Emergency (Critical/High/Medium/Low): ",
             r.emergency, sizeof(r.emergency));

    if (contains(r.emergency, "critical")) {
        strcpy(r.emergency, "Critical");
    } else if (contains(r.emergency, "high")) {
        strcpy(r.emergency, "High");
    } else if (contains(r.emergency, "medium")) {
        strcpy(r.emergency, "Medium");
    } else if (contains(r.emergency, "low")) {
        strcpy(r.emergency, "Low");
    } else {
        printf("Invalid emergency level!\n");
        return;
    }

    readText("Required resources: ",
             r.resources, sizeof(r.resources));

    if (strchr(r.id, '|') != NULL ||
        strchr(r.resources, '|') != NULL ||
        r.resources[0] == '\0') {
        printf("Enter a valid ID and resource description.\n");
        return;
    }

    r.priority = calculatePriority(r.emergency, r.people);
    assignTeam(&r);
    strcpy(r.status, "Pending");

    requests[count++] = r;
    saveRecords();

    printf("\nRequest registered successfully!\n");
    printf("Priority: %s (Score: %d)\n",
           priorityName(r.priority), r.priority);
    printf("Suggested team: %s\n", r.team);
}

/* Display requests in priority order without changing stored order */
void displayRequests(void) {
    int order[MAX];
    int i, j, temp;

    if (count == 0) {
        printf("\nNo rescue requests registered.\n");
        return;
    }

    for (i = 0; i < count; i++) {
        order[i] = i;
    }

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (requests[order[j]].priority <
                requests[order[j + 1]].priority) {
                temp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = temp;
            }
        }
    }

    printf("\n========== RESCUE PRIORITY BOARD ==========\n");

    for (i = 0; i < count; i++) {
        Rescue *r = &requests[order[i]];

        printf("\nRank: %d | ID: %s\n", i + 1, r->id);
        printf("Priority: %s (%d)\n",
               priorityName(r->priority), r->priority);
        printf("Location: %s | People: %d\n",
               r->location, r->people);
        printf("Emergency: %s\n", r->emergency);
        printf("Resources: %s\n", r->resources);
        printf("Team: %s | Status: %s\n", r->team, r->status);
        printf("-------------------------------------------\n");
    }
}

/* Search for a request */
void searchRequest(void) {
    char id[20];
    int i;

    readText("Enter Request ID to search: ", id, sizeof(id));
    i = findRequest(id);

    if (i == -1) {
        printf("Request not found!\n");
        return;
    }

    printf("\nID: %s\n", requests[i].id);
    printf("Location: %s\n", requests[i].location);
    printf("People affected: %d\n", requests[i].people);
    printf("Emergency: %s\n", requests[i].emergency);
    printf("Priority: %s (%d)\n",
           priorityName(requests[i].priority),
           requests[i].priority);
    printf("Resources: %s\n", requests[i].resources);
    printf("Team: %s\n", requests[i].team);
    printf("Status: %s\n", requests[i].status);
}

/* Update status */
void updateStatus(void) {
    char id[20];
    int i, choice;

    readText("Enter Request ID: ", id, sizeof(id));
    i = findRequest(id);

    if (i == -1) {
        printf("Request not found!\n");
        return;
    }

    printf("\n1. Pending\n");
    printf("2. In Progress\n");
    printf("3. Resolved\n");

    choice = readNumber("Select new status: ");

    switch (choice) {
        case 1:
            strcpy(requests[i].status, "Pending");
            break;
        case 2:
            strcpy(requests[i].status, "In Progress");
            break;
        case 3:
            strcpy(requests[i].status, "Resolved");
            break;
        default:
            printf("Invalid status choice!\n");
            return;
    }

    saveRecords();
    printf("Status updated to %s.\n", requests[i].status);
}

/* Display emergency summary */
void emergencySummary(void) {
    int i;
    int people = 0, critical = 0;
    int pending = 0, active = 0, resolved = 0;

    for (i = 0; i < count; i++) {
        people += requests[i].people;

        if (strcmp(requests[i].emergency, "Critical") == 0) {
            critical++;
        }

        if (strcmp(requests[i].status, "Pending") == 0) {
            pending++;
        } else if (strcmp(requests[i].status, "In Progress") == 0) {
            active++;
        } else if (strcmp(requests[i].status, "Resolved") == 0) {
            resolved++;
        }
    }

    printf("\n========== EMERGENCY SUMMARY ==========\n");
    printf("Total requests       : %d\n", count);
    printf("People affected      : %d\n", people);
    printf("Critical emergencies : %d\n", critical);
    printf("Pending requests     : %d\n", pending);
    printf("In-progress requests : %d\n", active);
    printf("Resolved requests    : %d\n", resolved);
    printf("=======================================\n");
}

int main(void) {
    int choice;

    loadRecords();

    do {
        printf("\n=====================================\n");
        printf("    SMART EMERGENCY RESCUE SYSTEM\n");
        printf("=====================================\n");
        printf("1. Register rescue request\n");
        printf("2. Display requests by priority\n");
        printf("3. Search request\n");
        printf("4. Update request status\n");
        printf("5. Emergency summary\n");
        printf("6. Exit\n");

        choice = readNumber("Enter your choice: ");

        switch (choice) {
            case 1:
                registerRequest();
                break;
            case 2:
                displayRequests();
                break;
            case 3:
                searchRequest();
                break;
            case 4:
                updateStatus();
                break;
            case 5:
                emergencySummary();
                break;
            case 6:
                saveRecords();
                printf("Records saved. Goodbye!\n");
                break;
            default:
                printf("Invalid choice! Select 1 to 6.\n");
        }

    } while (choice != 6);

    return 0;
}
