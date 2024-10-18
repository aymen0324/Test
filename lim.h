// libreria.h

#ifndef LIM_H
#define LIM_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_INPUT_SIZE 1000 // or whatever value you want
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHTGRAY 7
#define DARKGRAY 8
#define LIGHTBLUE 9
#define LIGHTGREEN 10
#define LIGHTCYAN 11
#define LIGHTRED 12
#define LIGHTMAGENTA 13
#define YELLOW 14
#define WHITE 15

void TextColor(int color);

typedef struct
{
    double **m;
    int fil;
    int col;
} TMatriz;

typedef struct variable
{
    char nomVar[16];
    TMatriz *mat;
    struct variable *sig, *ant;
} TVar;

typedef struct
{
    int numVars;
    TVar *primera;
} TVars;

typedef struct
{
    int rows;
    int cols;
    bool isValid;
} MatrixInfo;

// EJERCICIO 2
void display_vars_alfab(TVar *var);

// 4
TMatriz *unir_matrices(TMatriz *mat1, TMatriz *mat2);

void free_all(TVars *vars);

TMatriz *NuevaTmatriz(const char *cad);

double **NuevaMatriz(int fil, int col);

void print_matrix(TMatriz *m, int fil, int col);

void trim_custom(char *input_string);

MatrixInfo validate_and_count(const char *matrix_str);

TMatriz *add_matrices(TMatriz *m1, TMatriz *m2);

TMatriz *subtract_matrices(TMatriz *m1, TMatriz *m2);

TMatriz *multiply_matrix_by_number(TMatriz *m, double num);

void free_matrix(TMatriz *m);

TVar *find_var(TVars *vars, const char *name);

TMatriz *multiply_matrices(TMatriz *m1, TMatriz *m2);

bool store_named_matrix(TVars *vars, TVar *var);

TMatriz *find_matrix(TVars *vars, const char *name);

TVar *new_named_matrix(const char *name, TMatriz *matrix);

bool is_matrix(const char *str);

TMatriz *NuevaTmatriz_or_find(TVars *vars, const char *str);

double determinant(TMatriz *matrix, int n);

TMatriz *NuevaTmatrizVacia(int fil, int col);

char *remove_chars(const char *str, char *chars_to_remove);

char *strtok_custom(char *str, const char *delimiters);

char *find_operator(char *str);

bool is_valid_matrix_name(const char *name);

TMatriz *existing_matrix(TVars *vars, char *name);

void save_variables_to_file(TVars *vars, char *file_name);

void print_matrix_to_file(FILE *file, TMatriz *m);

void load_variables_from_file(TVars *vars, char *file_name, bool overlap);

bool is_number(char *str);

void product_command(TVars *vars, char **args);

void display_variables(TVar *var);

char *substr(const char *str, int start, int end);

double scalar_product(TMatriz *m1, TMatriz *m2);

TVar *find_last_var(TVars *vars);

TMatriz *copy_matrix(TMatriz *src);

typedef struct
{
    TMatriz **matrices;
    int count;
} TCopiedMatrices;

TMatriz *transpose_matrix(TMatriz *matrix);

void assign_matrix(TVars *vars, const char *var_name, TMatriz *src);

TMatriz *inversa_matrix(TMatriz *matrix);

#endif
