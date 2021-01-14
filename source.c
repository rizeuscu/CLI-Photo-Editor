// Bogdan-Constantin Rizescu, 312CA

#include "declarations.h"

int main(void) {
    // declarari structuri de date pentru main
    char command[4000];
    matrix **m;
    int lines = -1, cols = -1; // -1 = memorie goala / nu avem imagine incarcata
    int maxVal; // valoarea maxima a unei culori

    // logica parsarii comenzilor
    while (fgets(command, 4000, stdin)) {
        
        if (strcmp(command, "EXIT") && strlen(command) > 0)
            command[strlen(command) - 1] = '\0'; // eliminam newline

        char *ptr; // pointer ca sa splituim comenzile
        ptr = strtok(command, " "); // splituim comanda

        if (ptr == NULL) { // in cazul in care comanda e NULL, protejam functia strcmp de pointerul NULL
            printf("Invalid command\n");
            continue;
        }

        if (!strcmp(ptr, "LOAD")) {
            parse_load(&ptr, &m, &lines, &cols, &maxVal);
        }
        else if (!strcmp(ptr, "SELECT")) {
            parse_select(&ptr, &m, &lines, &cols);
        }
        else if (!strcmp(ptr, "ROTATE")) {
            parse_rotate(&ptr, &m, &lines, &cols);
        }
        else if (!strcmp(ptr, "CROP")) {
            parse_crop(&ptr, &m, &lines, &cols);
        }
        else if (!strcmp(ptr, "GRAYSCALE")) {
            parse_grayscale(&ptr, &m, &lines, &cols);
        }
        else if (!strcmp(ptr, "SEPIA")) {
            parse_sepia(&ptr, &m, &lines, &cols, &maxVal);
        }
        else if (!strcmp(ptr, "SAVE")) {
            parse_save(&ptr, &m, &lines, &cols);
        }
        else {
            if (!strcmp(ptr, "EXIT")) {
                parse_exit(&lines, &cols); // parsam exitul ca sa vedem daca avem imagine incarcata sau nu
                break;
            }
            else
                printf("Invalid command\n");
        }
    }

    if (lines != -1 && cols != -1) // verificam daca avem memorie incarcata
        free_mem(&m, &lines, &cols); // dealocare memorie daca exista, terminare program
    return 0;
}