// Bogdan-Constantin Rizescu, 312CA

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

typedef struct {
    unsigned char grey, red, green, blue; // campuri pentru intensitati culori
    int x1, y1, x2, y2; // campuri pentru retinerea zonei de selectie
    int color; // camp pentru img color/grayscale (1/0)
} matrix;

// returning functions
int parse_load();
int parse_select();
int parse_rotate();
int parse_crop();
int parse_grayscale();
int parse_sepia();
int parse_save();
int parse_exit();
int load();
int select_coordinates();
int rotate();
int crop();
int grayscale();
int sepia();
int save();
int check_in_matrix();

// void functions
void select_all();
void save_color_ascii();
void save_grayscale_ascii();
void save_color_bin();
void save_grayscale_bin();
void save_ascii();
void save_binary();
void free_mem();