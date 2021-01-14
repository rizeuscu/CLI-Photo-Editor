// Bogdan-Constantin Rizescu, 312CA

#include "declarations.h"

int min(int x, int y) {
    if (x < y)
        return x;
    return y;
}

void swap(int *x, int *y) {
    int aux;
    aux = *x;
    *x = *y;
    *y = aux;
}

int check_in_matrix(int x, int y, int x1, int y1, int x2, int y2){
    return (x1 >= 0 && x1 <= x && x2 >= 0 && x2 <= x && y1 >= 0 && y1 <= y && y2 >= 0 && y2 <= y && x1 < x2 && y1 < y2);
}

void skip_comments(FILE **fin) {
    char c, line[4000];
    fscanf(*fin, "%c", &c); // reading the newline character
    // c = primul caracter de pe linie
    while (fscanf(*fin, "%c", &c)) {
        if (c == '#')
            fgets(line, 4000, *fin); // citim toata linia comentata pentru a-i da skip
        else
            break; // linia nu e comentata, iesim din while
    }
    fseek(*fin, -1, SEEK_CUR); // resetam in spate cu 1 byte pentru ca am citit primul caracter de pe o linie necomentata
}

void free_mem(matrix ***m, int *lines, int *cols) {
    for (int i = 0; i < *lines; ++i) {
        free((*m)[i]);
    }
    free(*m);
    // daca nu exista memorie alocata pentru matrice
    // atat lines, cat si cols vor fi -1
    *lines = -1;
    *cols = -1;
}

int alloc_mem(matrix ***m, int *lines, int *cols, int a, int b) {
    matrix **mat;
    mat = (matrix **)malloc(b * sizeof(matrix *));
    
    if (!(mat)) {  // alocare esuata de matrice
        fprintf(stderr, "malloc() failed\n");
        mat = NULL;
        return -1;
    }

    for (int i = 0; i < b; ++i) {
        mat[i] = (matrix *)malloc(a * sizeof(matrix));
        // alocam a elemente pe fiecare linie din matrice

        if (!mat[i]) {  // alocare esuata de linie in matrice
            fprintf(stderr, "malloc() failed\n");
            for (int j = 0; j < i; ++j)  // dealocam toata matricea alocata pana acum
                free(mat[j]);
            free(mat);
            mat = NULL;
            return -1;
        }
    }
    *m = mat; // egalam matricea noastra cu cea alocata in functie
    *lines = b; // nr de linii = b
    *cols = a; // nr de coloane = a
    return 0;
}

int loadAscii(FILE *fin, matrix ***m, int a, int b, char file_type[]) {
    if (!strcmp(file_type, "P2")) {
        (*m)[0][0].color = 0; // imagine grayscale

        for (int i = 0; i < b; ++i) {
            for (int j = 0; j < a; ++j) {
                fscanf(fin, "%hhu", &(*m)[i][j].grey);
            }
        }
    } 
    else if (!strcmp(file_type, "P3")) {
        (*m)[0][0].color = 1; // imagine color

        for (int i = 0; i < b; ++i) {
            for (int j = 0; j < a; ++j) {
                fscanf(fin, "%hhu%hhu%hhu", &(*m)[i][j].red, &(*m)[i][j].green, &(*m)[i][j].blue);
            }
        }
    }
    else
        return -1;
    return 0;
}

int loadBinary(FILE *fin, matrix ***m, int a, int b, char file_type[]) {
    char nwl;
    fscanf(fin, "%c", &nwl); // citim newline-ul de dinainte de matricea in binar
    
    if (!strcmp(file_type, "P5")) {
        (*m)[0][0].color = 0; // imagine grayscale

        for (int i = 0; i < b; ++i) {
            for (int j = 0; j < a; ++j) {
                fread(&(*m)[i][j].grey, 1, sizeof(unsigned char), fin);
            }
        }
    } 
    else if (!strcmp(file_type, "P6")) {
        (*m)[0][0].color = 1; // imagine color

        for (int i = 0; i < b; ++i) {
            for (int j = 0; j < a; ++j) {
                fread(&(*m)[i][j].red, 1, sizeof(unsigned char), fin);
                fread(&(*m)[i][j].green, 1, sizeof(unsigned char), fin);
                fread(&(*m)[i][j].blue, 1, sizeof(unsigned char), fin);
            }
        }
    }
    else
        return -1;
    return 0;
}

int load(char photo[], matrix ***m, int *lines, int *cols, int *maxVal){
    FILE *fin;
    char file_type[2];
    int a, b;

    // programam defensiv in cazul in care nu putem deschide imaginea / nu exista
    if ((fin = fopen(photo, "r")) == NULL) {
        printf("Failed to load %s\n", photo); // nu reuseste sa incarce imaginea
        if (*lines != -1 && *cols != -1)
            free_mem(m, lines, cols); // eliberam memoria incarcata, daca exista, la fail
        return -1;
    }

    fscanf(fin, "%s", file_type); // citim magic word-ul
    skip_comments(&fin); // sarim peste liniile comentate, deoarece pot contine numere pe care nu vrem sa le parsam
    fscanf(fin, "%d%d", &a, &b); // citim dimensiunile imaginii
    skip_comments(&fin); // sarim peste liniile comentate, deoarece pot contine numere pe care nu vrem sa le parsam
    fscanf(fin, "%d", maxVal); // citim valoarea maxima a culorii de la adresa la care pointeaza maxVal


    // incarcam imaginea in memorie
    if (*lines != -1 && *cols != -1) {
        free_mem(m, lines, cols); // eliberam memoria deja incarcata pentru a face loc noii imagini
        alloc_mem(m, lines, cols, a, b); // alocam memorie pentru noua imagine
    }
    else
        alloc_mem(m, lines, cols, a, b); // alocam memorie pentru imagine

    // verificam tipul imaginii - ASCII sau Binary
    if (!strcmp(file_type, "P2") || !strcmp(file_type, "P3")) {
        loadAscii(fin, m, a, b, file_type); // incarcam imagine ASCII
    }
    else if (!strcmp(file_type, "P5") || !strcmp(file_type, "P6")) {
        loadBinary(fin, m, a, b, file_type); // incarcam imagine Binary
    }
    
    select_all(m, lines, cols, ""); // selectam intreaga imagine la load
    printf("Loaded %s\n", photo); // afisam mesaj pentru incarcarea cu succes
    fclose(fin);
    return 0;
}

int select_coordinates(matrix ***m, int *lines, int *cols, int x1, int y1, int x2, int y2) {
    // le inversam daca nu respecta ordinea
    if (x1 > x2)
        swap(&x1, &x2);
    // le inversam daca nu respecta ordinea
    if (y1 > y2)
        swap(&y1, &y2);

    // verificam daca coordonatele sunt in cadrul imaginii din memorie
    if (!check_in_matrix(*cols, *lines, x1, y1, x2, y2)) {
        printf("Invalid set of coordinates\n");
        return -1;
    }
    else {
        // retinem zona selectata, in primul element al matricei
        (*m)[0][0].x1 = x1;
        (*m)[0][0].y1 = y1;
        (*m)[0][0].x2 = x2;
        (*m)[0][0].y2 = y2;
        
        printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
    }
    return 0;
}

void select_all(matrix ***m, int *lines, int *cols, char *op) {
    (*m)[0][0].x1 = 0;
    (*m)[0][0].y1 = 0;
    (*m)[0][0].x2 = *cols;
    (*m)[0][0].y2 = *lines;

    if (!strcmp(op, "SELECTED ALL"))
        printf("Selected ALL\n");
}

int check_selection(matrix **m, int lines, int cols) {
    int x1 = m[0][0].x1, x2 = m[0][0].x2, y1 = m[0][0].y1, y2 = m[0][0].y2;
    int selection_ok = 0;
    if (!x1 && !y1 && x2 == cols && y2 == lines)
        selection_ok = 1; // selectia e intreaga imagine
    if (x2 - x1 == y2 - y1)
        selection_ok = 1; // selectia e submatrice patratica
    return selection_ok;
}

void rotate_image(matrix ***m, int *lines, int *cols, char *position) {
    int x1 = (*m)[0][0].x1, x2 = (*m)[0][0].x2, y1 = (*m)[0][0].y1, y2 = (*m)[0][0].y2;

    matrix **mat;
    int a = *cols, b = *lines;
    alloc_mem(&mat, &b, &a, a, b); // alocam memorie pentru copia imaginii

    // copiem imaginea in mat[][]
    for (int i = 0; i < *lines; ++i) {
        for (int j = 0; j < *cols; ++j)
            mat[i][j] = (*m)[i][j];
    }

    if (!(x2 - x1 == y2 - y1)) {
        // daca intra in if selectia e dreptunghiulara, deci pe toata imaginea
        // astfel, dealocam memoria si alocam din nou cu nr de linii = nr de coloane si invers
        free_mem(m, lines, cols);// dealocare memorie matrice
        alloc_mem(m, lines, cols, b, a);// alocare memorie cu numarul de linii inversat cu cel al coloanelor
    }

    if (!strcmp(position, "right")) {
        // rotire 90 grade dreapta
        int l = y1, c = x1;
        for (int i = x1; i < x2; ++i) {
            c = x1;
            for (int j = y2 - 1; j >= y1; --j) {
                (*m)[l][c] = mat[j][i];
                c++; // creste numarul coloanei pe care scriem
            }
            l++; // creste numarul liniei pe care scriem
        }
    }
    else if (!strcmp(position, "left")) {
        // rotire 90 grade stanga
        int l = y1, c = x1;
        for (int i = x2 - 1; i >= x1; --i) {
            c = x1;
            for (int j = y1; j < y2; ++j) {
                (*m)[l][c] = mat[j][i];
                c++; // creste numarul coloanei pe care scriem
            }
            l++; // creste numarul liniei pe care scriem
        }
    }

    (*m)[0][0].color = mat[0][0].color; // retinem colorizarea
    if (!(x2 - x1 == y2 - y1))
        select_all(m, lines, cols, ""); // reselectam total daca selectia precedenta a fost toata imaginea
    else {
        // retinem din nou selectia, in cazul in care submatricea rotita a fost patratica
        // datele se pot pierde in cazul in care pixelul (0,0) face parte din selectia care trebuie rotita
        (*m)[0][0].x1 = mat[0][0].x1;
        (*m)[0][0].y1 = mat[0][0].y1;
        (*m)[0][0].x2 = mat[0][0].x2;
        (*m)[0][0].y2 = mat[0][0].y2;
    }
    free_mem(&mat, &b, &a); // dealocare memorie copie
}

int rotate(matrix ***m, int *lines, int *cols, int angle) {
    if (!check_selection(*m, *lines, *cols)) {
        printf("The selection must be square\n");
        return -1;
    }
    switch(angle) {
        case 0:
            break;
        case 360:
            break;
        case -360:
            break;
        case 90:
            rotate_image(m, lines, cols, "right");
            break;
        case 180:
            rotate_image(m, lines, cols, "right");
            rotate_image(m, lines, cols, "right");
            break;
        case 270:
            rotate_image(m, lines, cols, "right");
            rotate_image(m, lines, cols, "right");
            rotate_image(m, lines, cols, "right");
            break;
        case -90:
            rotate_image(m, lines, cols, "left");
            break;
        case -180:
            rotate_image(m, lines, cols, "left");
            rotate_image(m, lines, cols, "left");
            break;
        case -270:
            rotate_image(m, lines, cols, "left");
            rotate_image(m, lines, cols, "left");
            rotate_image(m, lines, cols, "left");
            break;
        default:
            printf("Invalid parameters\n"); // daca sunt multipli de 90, dar nu cazurile mentionate
            break;
    }
    printf("Rotated %d\n", angle);
    return 0;
}

int crop(matrix ***m, int *lines, int *cols) {
    int x1 = (*m)[0][0].x1, x2 = (*m)[0][0].x2, y1 = (*m)[0][0].y1, y2 = (*m)[0][0].y2;
    matrix **mat;
    int a = x2 - x1, b = y2 - y1;
    alloc_mem(&mat, &b, &a, a, b); // alocam memorie pentru copia selectiei din imagine

    for (int i = y1; i < y2; ++i) {
        for (int j = x1; j < x2; ++j) {
            mat[i - y1][j - x1] = (*m)[i][j]; // copiem selectia in mat[][]
        }
    }
    mat[0][0].color = (*m)[0][0].color; // retinem colorizarea imaginii initiale
    
    free_mem(m, lines, cols); // dealocam matricea pentru ca vom egala pointerul la matrice cu copia selectiei

    *m = mat; // facem pointerul m sa pointeze la copia selectiei imaginii
    *lines = b; // retinem numarul de linii ca fiind egal cu cel al selectiei imaginii
    *cols = a;  // retinem numarul de coloane ca fiind egal cu cel al selectiei imaginii

    select_all(m, lines, cols, ""); // reselectam intreaga imagine

    printf("Image cropped\n");
    return 0;
}

int grayscale(matrix ***m) {
    // imagine grayscale
    if (!(*m)[0][0].color) {
        printf("Grayscale filter not available\n");
        return -1;
    }

    // imagine color
    int x1 = (*m)[0][0].x1, x2 = (*m)[0][0].x2, y1 = (*m)[0][0].y1, y2 = (*m)[0][0].y2;
    for (int i = y1; i < y2; ++i) {
        for (int j = x1; j < x2; ++j) {
            double nr = round(((*m)[i][j].red + (*m)[i][j].green + (*m)[i][j].blue) / 3.0);
            (*m)[i][j].red = (unsigned char)nr;
            (*m)[i][j].green = (unsigned char)nr;
            (*m)[i][j].blue = (unsigned char)nr;
        }
    }
    printf("Grayscale filter applied\n");
    return 0;
}

int sepia(matrix ***m, int *maxVal) {
    // imagine grayscale
    if (!(*m)[0][0].color) {
        printf("Sepia filter not available\n");
        return -1;
    }

    // imagine color
    int x1 = (*m)[0][0].x1, x2 = (*m)[0][0].x2, y1 = (*m)[0][0].y1, y2 = (*m)[0][0].y2;
    for (int i = y1; i < y2; ++i) {
        for (int j = x1; j < x2; ++j) {
            double newRed = round(0.393 * (*m)[i][j].red + 0.769 * (*m)[i][j].green + 0.189 * (*m)[i][j].blue);
            double newGreen = round(0.349 * (*m)[i][j].red + 0.686 * (*m)[i][j].green + 0.168 * (*m)[i][j].blue);
            double newBlue = round(0.272 * (*m)[i][j].red + 0.534 * (*m)[i][j].green + 0.131 * (*m)[i][j].blue);
            (*m)[i][j].red = min(*maxVal, newRed);
            (*m)[i][j].green = min(*maxVal, newGreen);
            (*m)[i][j].blue = min(*maxVal, newBlue);
        }
    }
    printf("Sepia filter applied\n");
    return 0;
}

void save_color_ascii(FILE *fout, matrix ***m, int *lines, int *cols) {
    for (int i = 0; i < *lines; ++i) {
        for (int j = 0; j < *cols; ++j) 
            fprintf(fout, "%hhu %hhu %hhu ", (*m)[i][j].red, (*m)[i][j].green, (*m)[i][j].blue);
        fprintf(fout, "\n");
    }
}

void save_grayscale_ascii(FILE *fout, matrix ***m, int *lines, int *cols) {
    for (int i = 0; i < *lines; ++i) {
        for (int j = 0; j < *cols; ++j)
            fprintf(fout, "%hhu ", (*m)[i][j].grey);
        fprintf(fout, "\n");
    }
}

void save_color_bin(FILE *fout, matrix ***m, int *lines, int *cols) {
    for (int i = 0; i < *lines; ++i) {
        for (int j = 0; j < *cols; ++j) {
            fwrite(&(*m)[i][j].red, 1, sizeof(unsigned char), fout);
            fwrite(&(*m)[i][j].green, 1, sizeof(unsigned char), fout);
            fwrite(&(*m)[i][j].blue, 1, sizeof(unsigned char), fout);
        }
    }
}

void save_grayscale_bin(FILE *fout, matrix ***m, int *lines, int *cols) {
    for (int i = 0; i < *lines; ++i) {
        for (int j = 0; j < *cols; ++j)
            fwrite(&(*m)[i][j].grey, 1, sizeof(unsigned char), fout);
    }
}

void save_binary(matrix ***m, int *lines, int *cols, char output_file[]) {
    FILE *fout;
    if ((fout = fopen(output_file, "w")) == NULL) {
        fprintf(stderr, "Failed to open / create file"); // nu reuseste sa deschisa / creeze fisierul
    }

    // scriem identificarea imaginii in output_file
    if ((*m)[0][0].color) // imagine color
        fprintf(fout, "P6\n");
    else // imagine grayscale
        fprintf(fout, "P5\n");
    fprintf(fout, "%d %d\n255\n", *cols, *lines);
    
    // salvam matricea in output_file
    if ((*m)[0][0].color)
        save_color_bin(fout, m, lines, cols);
    else
        save_grayscale_bin(fout, m, lines, cols);

    printf("Saved %s\n", output_file);
    fclose(fout);
}

void save_ascii(matrix ***m, int *lines, int *cols, char output_file[]) {
    FILE *fout;
    if ((fout = fopen(output_file, "w")) == NULL) {
        fprintf(stderr, "Failed to open / create file"); // nu reuseste sa deschisa / creeze fisierul
    }

    // scriem identificarea imaginii in output_file
    if ((*m)[0][0].color) // imagine color
        fprintf(fout, "P3\n");
    else // imagine grayscale
        fprintf(fout, "P2\n");
    fprintf(fout, "%d %d\n255\n", *cols, *lines);
    
    // salvam matricea in output_file
    if ((*m)[0][0].color)
        save_color_ascii(fout, m, lines, cols);
    else
        save_grayscale_ascii(fout, m, lines, cols);

    printf("Saved %s\n", output_file);
    fclose(fout);
}
