// Bogdan-Constantin Rizescu, 312CA

#include "declarations.h"

int image_loaded(int lines, int cols) {
    if (lines == -1 && cols == -1) {
        printf("No image loaded\n");
        return -1;
    }
    return 0;
}
int check_numeric(char *ptr) { 
    // verificam daca un string e numar
	for (unsigned int i = 0; i < strlen(ptr); ++i) {
        if (!isdigit(ptr[i]) && !(ptr[i] == '-' && !i))
            return 0;
    }
    return 1;
}

int check_angle(int angle) {
	if (angle%90 == 0)
		return 1; // unghi valid
	return 0; // unghi invalid
}

int parse_load(char **ptr, matrix ***m, int *lines, int *cols, int *maxVal) {
    int img_loaded = 0;
    while (*ptr != NULL) {
        *ptr = strtok (NULL, " ");
        if (*ptr != NULL && !img_loaded) { // daca nu am incarcat deja imaginea, o incarcam
            load(*ptr, m, lines, cols, maxVal);
            img_loaded = 1; // marcam ca am incarcat imaginea in memorie
        }
    }
    return 0;
}

int parse_select(char **ptr, matrix ***m, int *lines, int *cols) {
    if (image_loaded(*lines, *cols) == -1)
        return -1; // nu avem imagine incarcata in memorie

    int words = -1, numbers = -1, v[4] = {0}, it = -1;
    int all_selected = 0;

    while (*ptr != NULL) {
        *ptr = strtok(NULL, " ");
        if (*ptr != NULL) {
            words++; // creste numarul de cuvinte
            if (check_numeric(*ptr)) { // verificam daca e numar
                numbers++; // creste counterul numerelor
                if (numbers == words) {
                    v[++it] = atoi(*ptr); // punem coordonata intr-un array
                }
            }
        }
        if (*ptr != NULL && !strcmp(*ptr, "ALL") && !words) {
            select_all(m, lines, cols, "SELECTED ALL"); // daca avem ALL ca prim cuvant selectam toata imaginea
            all_selected = 1; // marcam ca am selectat toata imaginea
        }
    }

    // in cazul in care nu avem ALL / nu apare pe pozitia potrivita, verificam coordonatele
    if (!all_selected) {
        if (it == 3 && it == numbers){
            select_coordinates(m, lines, cols, v[0], v[1], v[2], v[3]);
        }
        else {
            printf("Invalid command\n");
            return -1;
        }
    }
    return 0;
}

int parse_rotate(char **ptr, matrix ***m, int *lines, int *cols) {
	if (image_loaded(*lines, *cols) == -1)
		return -1; // nu avem imagine incarcata in memorie

	int words = -1, angle_ok = 0, angle;
	while (*ptr != NULL) {
		*ptr = strtok(NULL, " ");
		if (*ptr != NULL) {
			words++;
			if (!words && check_numeric(*ptr)) {
				angle_ok = 1;
				angle = atoi(*ptr);
			}
		}
	}
	if (!angle_ok || (angle_ok && !check_angle(angle))) {
		printf("Unsupported rotation angle\n");
		return -1;
	}
	rotate(m, lines, cols, angle);
	return 0;
}

int parse_crop(char **ptr, matrix ***m, int *lines, int *cols) {
	if (image_loaded(*lines, *cols) == -1)
        return -1; // nu avem imagine incarcata in memorie
	while (*ptr != NULL) // parsam garbage-ul comenzii
		*ptr = strtok(NULL, " ");
	crop(m, lines, cols);
	return 0;
}

int parse_grayscale(char **ptr, matrix ***m, int *lines, int *cols) {
	if (image_loaded(*lines, *cols) == -1)
        return -1; // nu avem imagine incarcata in memorie
	while (*ptr != NULL) // parsam garbage-ul comenzii
		*ptr = strtok(NULL, " ");
	grayscale(m);
	return 0;
}

int parse_sepia(char **ptr, matrix ***m, int *lines, int *cols, int *maxVal) {
	if (image_loaded(*lines, *cols) == -1)
        return -1; // nu avem imagine incarcata in memorie
	while (*ptr != NULL) // parsam garbage-ul comenzii
		*ptr = strtok(NULL, " ");
	sepia(m, maxVal);
	return 0;
}

int parse_save(char **ptr, matrix ***m, int *lines, int *cols) {
    if (image_loaded(*lines, *cols) == -1)
        return -1; // nu avem imagine incarcata in memorie
    
    int words = -1, file = 0, ascii = 0;
    char output_file[4000];

    while (*ptr != NULL) {
        *ptr = strtok(NULL, " ");
        if (*ptr != NULL) {
            words++;
            if (!words) {
                strcpy(output_file, *ptr); // salvam numele fisierului
                file = 1;
            }
            if (words == 1 && !strcmp(*ptr, "ascii"))
                ascii = 1; // retinem ca trebuie sa scriem in format ASCII
        }
    }
    if (words == -1) {
        printf("Invalid command\n");
        return -1; // words = -1 ==> dupa SAVE nu mai avem nimic ==> returnam -1 = eroare
    }
    else if (file && !ascii)
        save_binary(m, lines, cols, output_file); // salvam binar
    else if (file && ascii)
        save_ascii(m, lines, cols, output_file); // salvam in format ASCII
    return 0;
}

int parse_exit(int *lines, int *cols) {
    if (image_loaded(*lines, *cols) == -1)
        return -1; // nu avem imagine incarcata in memorie
    return 0;
}