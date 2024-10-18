
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include "lim.h"
#define MAX_INPUT_SIZE 1000
#define MAX_MATRICES 100
#define MAX_INPUT_SIZE 1000 
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

void free_all(TVars *vars)
{
    TVar *current_var = vars->primera;
    TVar *next_var;

    while (current_var != NULL)
    {
        next_var = current_var->sig;
        free_matrix(current_var->mat);
        free(current_var);
        current_var = next_var;
    }

    free(vars);
}

void TextColor(int color)
{
    static int __BACKGROUND;

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

    GetConsoleScreenBufferInfo(h, &csbiInfo);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color + (__BACKGROUND << 4));
}

TMatriz *existing_matrix(TVars *vars, char *name)
{
    TVar *current_var = vars->primera;
    while (current_var != NULL)
    {
        if (strcmp(current_var->nomVar, name) == 0)
        {
            return current_var->mat;
        }
        current_var = current_var->sig;
    }
    return NULL;
}

TMatriz *copy_matrix(TMatriz *src)
{
    TMatriz *dest = NuevaTmatrizVacia(src->fil, src->col);
    for (int i = 0; i < src->fil; i++)
    {
        for (int j = 0; j < src->col; j++)
        {
            dest->m[i][j] = src->m[i][j];
        }
    }
    return dest;
}

void assign_matrix(TVars *vars, const char *var_name, TMatriz *src)
{
    TVar *var = find_var(vars, var_name);
    if (var == NULL)
    {
        var = new_named_matrix(var_name, src);
        store_named_matrix(vars, var);
    }
    else
    {
        free_matrix(var->mat);
        var->mat = copy_matrix(src);
    }
}

void trim_custom(char *input_string)
{
    int i = 0, j = 0;
    bool space = false; 

    while (input_string[i] != '\0' && isspace(input_string[i]))
        i++;

    while (input_string[i] != '\0')
    {
        if (isspace(input_string[i]))
        {
            // Si encontramos un espacio, lo reemplazamos con un solo espacio
            if (!space)
            {
                input_string[j++] = ' ';
                space = true;
            }
        }
        else
        {
            // Si encontramos un carácter que no está vacío, lo copiamos.
            input_string[j++] = input_string[i];
            space = false;
        }
        i++;
    }

    // Si la cadena termina en un espacio, elimínelo
    if (j > 0 && input_string[j - 1] == ' ')
        j--;

    // Agregar carácter de fin de cadena
    input_string[j] = '\0';
}

static int count_digits(const char *str)
{
    int count = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            count++;
        }
    }
    return count;
}

TMatriz *NuevaTmatriz(const char *cad)
{
    //Eliminar paréntesis y barras verticales de la cadena.
    char *cleaned_str = remove_chars(cad, "()|");

    // Valide la cadena de entrada y obtenga el número de filas y columnas
    MatrixInfo info = validate_and_count(cad);
    if (!info.isValid)
    {

        return NULL;
    }

    TMatriz *m = (TMatriz *)malloc(sizeof(TMatriz));
    if (m == NULL)
    {
        return NULL;
    }

    m->fil = info.rows;
    m->col = info.cols;

    // Asignar memoria para la matriz.
    m->m = NuevaMatriz(info.rows, info.cols);
    if (m->m == NULL)
    {
        free(m);
        return NULL;
    }

    // Cree una copia editable de la cadena de entrada
    char *cad_copy = strdup(cad);
    if (cad_copy == NULL)
    {
        free_matrix(m);
        free(m);
        return NULL;
    }

    char previous_char = ' ';
    // Llenar la matriz con los valores de entrada
    char *p = cad_copy; // Puntero a la posición actual en la cadena
    for (int f = 0; f < info.rows; f++)
    {
        for (int c = 0; c < info.cols; c++)
        {
            // Ignorar caracteres no numéricos

            while (*p != '\0' && !isdigit(*p) && !(*p == '-' && isdigit(*(p + 1))))
                p++;
            if (*p == '\0')
            {
                // Liberar memoria previamente asignada
                free_matrix(m);
                free(m);
                free(cad_copy);
                return NULL;
            }
            // Leer el número en la posición actual.
            double n;
            if (c == info.cols - 1)
            {
                // Última columna: lee un número seguido de un paréntesis de cierre
                n = sscanf(p, " %lf)", &m->m[f][c]);
            }
            else
            {
                // Otra columna: lee un número seguido de un espacio
                n = sscanf(p, " %lf", &m->m[f][c]);
            }
            if (n != 1)
            {
                // Error de lectura: el número no tiene el formato esperado
                // Liberar memoria previamente asignada
                free_matrix(m);
                free(m);
                free(cad_copy);
                return NULL;
            }
            // Avanzar al siguiente carácter no numérico
            while (*p != '\0' && (isdigit(*p) || *p == '.' || *p == '-'))
                p++;
        }
    }

    // Libere la memoria asignada para copiar la cadena.
    free(cad_copy);

    return m;
}

double **NuevaMatriz(int fil, int col)
{
    double **mat;
    int i;

    mat = (double **)malloc(sizeof(double *) * fil);

    if (mat == NULL)
        return NULL;

    for (i = 0; i < fil; i++)
    {
        mat[i] = (double *)malloc(sizeof(double) * col);
        if (mat[i] == NULL)
        {
            for (int j = 0; j < i; j++)
                free(mat[j]);
            free(mat);
            return NULL;
        }
    }
    return mat;
}

MatrixInfo validate_and_count(const char *matrix_str)
{
    MatrixInfo info = {0, 0, true};
    bool new_number = true;
    int current_cols = 0;

    // Encuentra la posición del primer carácter '('
    char *p = strchr(matrix_str, '(');
    if (p == NULL)
    {
        info.isValid = false;
        return info;
    }

    // Comprobamos si la matriz termina con paréntesis
    int length = strlen(matrix_str);
    if (matrix_str[length - 1] != ')')
    {
        info.isValid = false;
        return info;
    }

    // Recorre la cadena comenzando desde el carácter '('
    for (int i = p - matrix_str + 1; matrix_str[i] != '\0'; i++)
    {
        if (isdigit(matrix_str[i]) || matrix_str[i] == '.' || matrix_str[i] == '-')
        {
            if (new_number)
            {
                current_cols++;
                new_number = false;
            }
        }
        else if (matrix_str[i] == '|')
        {
            if (info.cols == 0)
            {
                info.cols = current_cols;
            }
            else if (current_cols != info.cols)
            {
                info.isValid = false;
                return info;
            }
            info.rows++;
            new_number = true;
            current_cols = 0; // Restablecer el contador de columnas para cada nueva fila
        }
        else if (matrix_str[i] != ' ' && matrix_str[i] != ')')
        {
            info.isValid = false;
        }

        if (matrix_str[i] == ' ')
        {
            new_number = true;
        }
    }

    if (info.cols == 0)
    {
        info.cols = current_cols;
    }
    else if (current_cols != info.cols)
    {
        info.isValid = false;
        return info;
    }

    info.rows++; // Agrega 1 para contar la última línea

    return info;
}

void print_matrix(TMatriz *m, int fil, int col)
{
    if (m == NULL)
    {
        printf("Error: matriz nula pasada a print_matrix\n");
        return;
    }
    for (int i = 0; i < fil; i++)
    {
        for (int j = 0; j < col; j++)
        {
            printf("%10.2f ", m->m[i][j]);
        }
        printf("\n");
    }
}

TMatriz *add_matrices(TMatriz *m1, TMatriz *m2)
{
    //Comprobar que las matrices tengan las mismas dimensiones.
    if (m1->fil != m2->fil || m1->col != m2->col)
    {
        TextColor(RED);
        fprintf(stderr, "ERROR : Las dimensiones no son compatibles\n");
        TextColor(WHITE);
        return NULL;
    }

    // Crea una nueva matriz para el resultado.
    TMatriz *result = malloc(sizeof(TMatriz));
    result->fil = m1->fil;
    result->col = m1->col;
    result->m = malloc(result->fil * sizeof(double *));
    for (int i = 0; i < result->fil; i++)
    {
        result->m[i] = malloc(result->col * sizeof(double));
    }

    // Añadir las matrices
    for (int i = 0; i < result->fil; i++)
    {
        for (int j = 0; j < result->col; j++)
        {
            result->m[i][j] = m1->m[i][j] + m2->m[i][j];
        }
    }

    return result;
}

TMatriz *multiply_matrices(TMatriz *m1, TMatriz *m2)
{
    //Verificar que las matrices se puedan multiplicar.
    if (m1->col != m2->fil)
    {
        TextColor(RED);
        fprintf(stderr, "ERROR : Las dimensiones no son compatibles\n");
        TextColor(WHITE);
        return NULL;
    }
    // Crea una nueva matriz para el resultado.
    TMatriz *result = malloc(sizeof(TMatriz));
    result->fil = m1->fil;
    result->col = m2->col;
    result->m = malloc(result->fil * sizeof(double *));
    for (int i = 0; i < result->fil; i++)
    {
        result->m[i] = malloc(result->col * sizeof(double));
    }

    //Multiplicar matrices
    for (int i = 0; i < result->fil; i++)
    {
        for (int j = 0; j < result->col; j++)
        {
            result->m[i][j] = 0;
            for (int k = 0; k < m1->col; k++)
            {
                result->m[i][j] += m1->m[i][k] * m2->m[k][j];
            }
        }
    }

    return result;
}

TMatriz *subtract_matrices(TMatriz *m1, TMatriz *m2)
{
    if (m1->fil != m2->fil || m1->col != m2->col)
    {
        return NULL;
    }

    TMatriz *result = NuevaTmatrizVacia(m1->fil, m1->col);
    for (int i = 0; i < m1->fil; i++)
    {
        for (int j = 0; j < m1->col; j++)
        {
            result->m[i][j] = m1->m[i][j] - m2->m[i][j];
        }
    }

    return result;
}

TMatriz *find_matrix(TVars *vars, const char *name)
{
    TVar *var = vars->primera;
    while (var != NULL)
    {
        if (strcmp(var->nomVar, name) == 0)
        {
            return var->mat;
        }
        var = var->sig;
    }
    return NULL; // Matriz no encontrada
}

TVar *find_var(TVars *vars, const char *name)
{
    TVar *var = vars->primera;
    while (var != NULL)
    {
        if (strcmp(var->nomVar, name) == 0)
        {
            return var;
        }
        var = var->sig;
    }
    return NULL; // Variable no encontrada
}

void free_matrix(TMatriz *matrix)
{
    for (int i = 0; i < matrix->fil; i++)
    {
        free(matrix->m[i]);
    }
    free(matrix->m);
    free(matrix);
}

bool store_named_matrix(TVars *vars, TVar *var)
{
    if (!is_valid_matrix_name(var->nomVar))
    {
        return false;
    }

    // Verificar si ya existe una matriz con el mismo nombre
    TVar *current_var = vars->primera;
    while (current_var != NULL)
    {
        if (strcmp(current_var->nomVar, var->nomVar) == 0)
        {
            // Si es así, libera la matriz existente y reemplázala con una copia de la nueva.
            free_matrix(current_var->mat);
            current_var->mat = copy_matrix(var->mat);
            // Libera la nueva estructura de variable, ya que estamos reutilizando la existente
            free(var);
            return true;
        }
        current_var = current_var->sig;
    }

    // Si no existe una matriz con el mismo nombre, agregue la nueva matriz
    if (vars->numVars >= MAX_MATRICES)
    {
        return false; // The array is full
    }

    //Agrega la nueva variable al inicio de la lista
    var->sig = vars->primera;
    if (vars->primera != NULL)
    {
        vars->primera->ant = var;
    }
    vars->primera = var;
    vars->numVars++;

    return true;
}

TVar *new_named_matrix(const char *name, TMatriz *matrix)
{

    if (!is_valid_matrix_name(name))
    {
        return NULL;
    }

    //Asignar memoria para la estructura TVar
    TVar *var = (TVar *)malloc(sizeof(TVar));
    if (var == NULL)
    {
        return NULL;
    }
    strncpy(var->nomVar, name, 15);
    var->nomVar[15] = '\0';
    var->mat = matrix;
    var->sig = NULL;
    var->ant = NULL;
    return var;
}

bool is_valid_matrix_name(const char *name)
{
    //Comprueba que el nombre no sea demasiado largo
    if (strlen(name) > 15)
    {
        return false;
    }

    // Verificar si todos los caracteres son alfanuméricos
    if (!isalpha(name[0]))
    {
        return false;
    }

    // Verificar si todos los caracteres son alfanuméricos
    for (int i = 0; name[i] != '\0'; i++)
    {
        if (!isalnum(name[i]))
        {
            return false;
        }
    }

    return true;
}

bool is_matrix(const char *str)
{

    while (*str == ' ' || (*str >= '0' && *str <= '9'))
    {
        str++;
    }
    // Una matriz válida debe comenzar con '(' y terminar con ')'
    if (str[0] != '(' || str[strlen(str) - 1] != ')')
    {
        return false;
    }

    // Verifica el resto de la cadena
    for (int i = 1; i < strlen(str) - 1; i++)
    {
        // Una matriz válida solo puede contener dígitos, espacios, puntos (para números decimales), signos menos (para números negativos) y '|'
        if (!(isdigit(str[i]) || str[i] == ' ' || str[i] == '.' || str[i] == '-' || str[i] == '|'))
        {
            return false;
        }

        // Si el carácter actual es '-', verifique que el siguiente carácter sea un dígito
        if (str[i] == '-' && (i + 1 < strlen(str) - 1) && !isdigit(str[i + 1]))
        {
            return false;
        }
    }

    return true;
}

TMatriz *NuevaTmatriz_or_find(TVars *vars, const char *str)
{

    // Comprobar si la entrada es un nombre de variable
    if (!is_matrix(str))
    {
        // Si es así, busca la matriz correspondiente en la memoria
        TMatriz *matrix = find_matrix(vars, str);
        if (matrix != NULL)
        {
            return matrix;
        }
        else
        {

            return NULL;
        }
    }

    // De lo contrario, intenta convertir la entrada a una matriz
    TMatriz *matrix = NuevaTmatriz(str);
    if (matrix != NULL)
    {
        return matrix;
    }
    else
    {
        printf("Erreur : l'entrée %s ne peut pas être convertie en une matrice.\n", str);
        return NULL;
    }
}

double determinant(TMatriz *matrix, int n)
{
    if (matrix->fil != matrix->col)
    {
        TextColor(RED);
        printf("Error: El determinante solo se puede calcular para matrices cuadradas.\n");
        TextColor(WHITE);
        return -1; 
    }

    double det = 0;
    int sign = 1;
    int p, q, j, i, r;
    TMatriz *temp = NuevaTmatrizVacia(n, n);

    if (n == 1)
    {
        return matrix->m[0][0];
    }
    else
    {
        for (p = 0; p < n; p++)
        {
            int h = 0;
            int k = 0;
            for (q = 0; q < n; q++)
            {
                for (j = 0; j < n; j++)
                {
                    if (q != 0 && j != p)
                    {
                        temp->m[h][k] = matrix->m[q][j];
                        if (k < (n - 2))
                            k++;
                        else
                        {
                            k = 0;
                            h++;
                        }
                    }
                }
            }
            det = det + sign * (matrix->m[0][p] * determinant(temp, n - 1));
            sign = -sign;
        }
        return det;
    }
    free(temp);
}

TMatriz *NuevaTmatrizVacia(int fil, int col)
{
    TMatriz *m = (TMatriz *)malloc(sizeof(TMatriz));
    m->fil = fil;
    m->col = col;
    m->m = (double **)malloc(fil * sizeof(double *));
    for (int i = 0; i < fil; i++)
    {
        m->m[i] = (double *)calloc(col, sizeof(double));
    }
    return m;
}

TMatriz *multiply_matrix_by_number(TMatriz *m, double num)
{
    TMatriz *result = NuevaTmatrizVacia(m->fil, m->col);
    for (int i = 0; i < m->fil; i++)
    {
        for (int j = 0; j < m->col; j++)
        {
            result->m[i][j] = m->m[i][j] * num;
        }
    }
    return result;
}

char *remove_chars(const char *str, char *chars_to_remove)
{
    char *cleaned_str = malloc(strlen(str) + 1);
    char *p = cleaned_str;
    for (int i = 0; i < strlen(str); i++)
    {
        if (strchr(chars_to_remove, str[i]) == NULL)
        {
            *p++ = str[i];
        }
    }
    *p = '\0';
    return cleaned_str;
}

char *find_operator(char *str)
{
    for (char *p = str; *p; p++)
    {
        if (*p == '+' || *p == '*' || *p == '$' || *p == '&')
        {
            // Si el carácter es '+' o '*', devuelve el puntero al operador
            return p;
        }
        else if (*p == '-' && !isdigit(*(p + 1)))
        {
            // Si el carácter es un '-' y el carácter siguiente no es un dígito, devolver el puntero al operador
            return p;
        }
    }    
    return NULL;
}

void save_variables_to_file(TVars *vars, char *file_name)
{
    // Paso 2: Verificar si hay variables
    if (vars->numVars == 0)
    {
        TextColor(RED);
        printf("Error: No hay variables para guardar.\n");
        TextColor(WHITE);
        return;
    }

    // Paso 3: Abrir el archivo
    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        TextColor(RED);
        printf("Error: No se puede abrir el archivo '%s'.\n", file_name);
        TextColor(WHITE);
        return;
    }

    TVar *var = vars->primera;
    // Paso 4: Escribir las variables en el archivo
    for (int i = 0; i < vars->numVars; i++)
    {

        fprintf(file, "%s\n", var->nomVar);
        fprintf(file, "(");
        print_matrix_to_file(file, var->mat);
        fprintf(file, ")\n");
        var = var->sig;
    }

    // Paso 5: Cerrar el archivo
    fclose(file);
}

void print_matrix_to_file(FILE *file, TMatriz *m)
{
    for (int i = 0; i < m->fil; i++)
    {
        for (int j = 0; j < m->col; j++)
        {
            fprintf(file, "%10.2f ", m->m[i][j]);
        }
        if (i < m->fil - 1)
        {
            fprintf(file, "| ");
        }
    }
}

void load_variables_from_file(TVars *vars, char *file_name, bool overlap)
{
    TCopiedMatrices *copied_matrices = (TCopiedMatrices *)malloc(sizeof(TCopiedMatrices));
    copied_matrices->matrices = NULL;
    copied_matrices->count = 0;

    FILE *file = fopen(file_name, "r");

    if (file == NULL)
    {
        TextColor(RED);
        printf("Error: No se puede abrir el archivo %s.\n", file_name);
        TextColor(WHITE);
        return;
    }

    if (!overlap)
    {
        TVar *current = vars->primera;
        while (current != NULL)
        {
            TVar *next = current->sig;
            free_matrix(current->mat);
            free(current);
            current = next;
        }
        vars->primera = NULL;
        vars->numVars = 0;
    }
    else
    {
        // Solo liberar variables que también están en el archivo
        char line[MAX_INPUT_SIZE];
        while (fgets(line, sizeof(line), file))
        {
            trim_custom(line);
            if (line[0] != '(')
            {
                // Esta línea contiene un nombre de variable
                TVar *existing_var = find_var(vars, line);
                if (existing_var != NULL)
                {
                    // Liberar la matriz de la variable existente
                    free_matrix(existing_var->mat);
                    existing_var->mat = NULL;
                }
            }
        }

        // Restablecer el puntero del archivo al principio
        rewind(file);
    }

        // Continúa con el resto de la función
    char line[MAX_INPUT_SIZE];
    TVar *last_var = find_last_var(vars);
    TMatriz *mat = NULL;
    char var_name[MAX_INPUT_SIZE];
    bool is_matrix = false;
    char matrix_data[MAX_INPUT_SIZE * 10] = ""; // Búfer para almacenar datos de matrices de varias líneas
    while (fgets(line, sizeof(line), file))
    {
        trim_custom(line);
        if (line[0] == '(')
        {
            // Esta línea inicia una matriz
            is_matrix = true;
        }
        if (is_matrix)
        {
            // Esta línea es parte de una matriz
            if (matrix_data[0] != '\0')
            {
                strcat(matrix_data, "|"); // Agregue un '|' antes de cada línea excepto la primera
            }
            strcat(matrix_data, line); // Añade la línea a matrix_data
            if (strchr(line, ')') != NULL)
            {
                // Esta línea finaliza la matriz.
                mat = NuevaTmatriz(matrix_data);
                is_matrix = false;
                matrix_data[0] = '\0'; // Restablecer matrix_data para la siguiente matriz
            }
        }
        else
        {
            // Esta línea contiene un nombre de variable
            strncpy(var_name, line, sizeof(var_name));
        }
        if (!is_matrix && mat != NULL)
        {

            if (overlap)
            {
                TVar *existing_var = find_var(vars, var_name);

                if (existing_var != NULL)
                {

                    bool was_copied = false;
                    for (int i = 0; i < copied_matrices->count; i++)
                    {
                        if (copied_matrices->matrices[i] == existing_var->mat)
                        {
                            was_copied = true;
                            break;
                        }
                    }

                    if (was_copied)
                    {
                        // Si se copió la matriz, mantenga la variable antigua y cree una nueva
                        TVar *new_var = new_named_matrix(var_name, mat);
                        store_named_matrix(vars, new_var);
                    }
                    else
                    {
                        // Si no se copió la matriz, reemplace la matriz de la variable existente con la nueva
                        existing_var->mat = copy_matrix(mat);
                    }

                    mat = NULL; // Restablecer la matriz para la siguiente variable
                    continue;
                }
            }

            //Se acaba de leer una matriz, así que crea una nueva variable con ella
            TVar *var = (TVar *)malloc(sizeof(TVar));
            strncpy(var->nomVar, var_name, sizeof(var->nomVar));
            var->mat = copy_matrix(mat);
            var->sig = NULL;
            var->ant = last_var;
            if (last_var != NULL)
            {
                last_var->sig = var;
            }
            last_var = var;
            if (vars->primera == NULL)
            {
                vars->primera = var;
            }
            vars->numVars++;
            mat = NULL; // Restablecer la matriz para la siguiente variable
        }
    }
    free(copied_matrices->matrices);
    free(copied_matrices);

    fclose(file);

    if (vars->numVars == 0)
    {
        TextColor(RED);
        printf("Error: No hay variables cargadas.\n");
        TextColor(WHITE);
    }
}

TVar *find_last_var(TVars *vars)
{
    if (vars->primera == NULL)
    {
        return NULL;
    }
    TVar *current = vars->primera;
    while (current->sig != NULL)
    {
        current = current->sig;
    }
    return current;
}

bool is_number(char *str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (!isdigit(str[i]) && str[i] != '.' && str[i] != '-')
        {
            return false;
        }
    }
    return true;
}

void product_command(TVars *vars, char **args)
{
    // Extraer los argumentos después de 'producto'
    char *arg1 = args[1];
    char *arg2 = args[2];

    TMatriz *matrix;
    double number;

    // Comprueba si el primer argumento es una matriz
    if (is_matrix(arg1))
    {
        matrix = NuevaTmatriz_or_find(vars, arg1);
        number = atof(arg2);
    }
    else
    {
        matrix = NuevaTmatriz_or_find(vars, arg2);
        number = atof(arg1);
    }

    //Realizar la multiplicación
    TMatriz *result = multiply_matrix_by_number(matrix, number);

    // Imprima o devuelva el resultado según sea necesario
    print_matrix(result, result->fil, result->col);
}

char *substr(const char *str, int start, int end)
{
    int n = end - start;
    char *new_str = malloc(n + 1);
    if (new_str == NULL)
    {
        TextColor(RED);
        printf("Error: No se puede asignar memoria para la nueva cadena.\n");
        TextColor(WHITE);
        return NULL;
    }
    strncpy(new_str, str + start, n);
    new_str[n] = '\0';
    return new_str;
}

double scalar_product(TMatriz *m1, TMatriz *m2)
{
    if (m1->fil != 1 || m2->fil != 1 || m1->col != m2->col)
    {
        TextColor(RED);
        printf("Error:  Ambas matrices deben ser matrices de 1xn.\n");
        TextColor(WHITE);
        return NAN;
    }

    double product = 0;
    for (int i = 0; i < m1->col; i++)
    {
        product += m1->m[0][i] * m2->m[0][i]; 
    }

    return product;
}

TMatriz * inversa_matrix(TMatriz *matrix)
{
    int n = matrix->fil;
    double det = determinant(matrix, n);

    if (det == 0 )
    {
        printf("La matriz no tiene inversa porque su determinante es cero.\n");
        return NULL;
    }
    else if (det == -1)
    {
        TextColor(RED);
        printf("No se pude calcular la inversa.\n");
        TextColor(WHITE);

        return NULL;
    }

    TMatriz *inv = NuevaTmatrizVacia(n, n);
    TMatriz *copy = copy_matrix(matrix);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i == j)
            {
                inv->m[i][j] = 1.0;
            }
            else
            {
                inv->m[i][j] = 0.0;
            }
        }
    }

    for (int i = 0; i < n; i++)
    {
        double diag = copy->m[i][i];
        for (int j = 0; j < n; j++)
        {
            copy->m[i][j] /= diag;
            inv->m[i][j] /= diag;
        }

        for (int j = 0; j < n; j++)
        {
            if (i != j)
            {
                double ratio = copy->m[j][i];

                for (int k = 0; k < n; k++)
                {
                    copy->m[j][k] -= ratio * copy->m[i][k];
                    inv->m[j][k] -= ratio * inv->m[i][k];
                }
            }
        }
    }

    free_matrix(copy);
    return inv;
}

TMatriz *transpose_matrix(TMatriz *matrix)
{
    int rows = matrix->fil;
    int cols = matrix->col;
    TMatriz *transposed = NuevaTmatrizVacia(cols, rows);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            transposed->m[j][i] = matrix->m[i][j];
        }
    }

    return transposed;
}

void display_variables(TVar *var)
{
    if (var == NULL)
    {
        TextColor(RED);
        printf("ERROR\n");
        TextColor(WHITE);
        return;
    }
    if (var->sig != NULL)
    {
        display_variables(var->sig);
    }
    TextColor(GREEN);
    printf("%s (%d x %d) : (", var->nomVar, var->mat->fil, var->mat->col);
    TextColor(WHITE);
    for (int j = 0; j < var->mat->col; j++)
    {
        TextColor(GREEN);
        printf("%.2f ", var->mat->m[0][j]);
        TextColor(WHITE);
    }
    TextColor(GREEN);
    printf(")\n");
    TextColor(WHITE);
}

// funcion para mostrar variables ordenadas alfabticamente - EJERCICIO 2

int compare_vars(const void *a, const void *b)
{
    TVar *varA = *(TVar **)a;
    TVar *varB = *(TVar **)b;
    return strcmp(varA->nomVar, varB->nomVar);
}

void display_vars_alfab(TVar *var)
{
    if (var == NULL)
    {
        TextColor(RED);
        printf("ERROR: No hay variables \n");
        TextColor(WHITE);
        return;
    }

    // Contar el número de variables
    int count = 0;
    TVar *temp = var;
    while (temp != NULL)
    {
        count++;
        temp = temp->sig;
    }

    // Crear un array para almacenar las variables
    TVar **varArray = (TVar **)malloc(count * sizeof(TVar *));
    temp = var;
    for (int i = 0; i < count; i++)
    {
        varArray[i] = temp;
        temp = temp->sig;
    }

    // Ordenar el array de variables por nombre
    qsort(varArray, count, sizeof(TVar *), compare_vars);

    // Mostrar las variables ordenadas
    for (int i = 0; i < count; i++)
    {
        TVar *currentVar = varArray[i];
        TextColor(GREEN);
        printf("%s (%d x %d) : (", currentVar->nomVar, currentVar->mat->fil, currentVar->mat->col);
        TextColor(WHITE);
        for (int j = 0; j < currentVar->mat->col; j++)
        {
            TextColor(GREEN);
            printf("%.2f ", currentVar->mat->m[0][j]);
            TextColor(WHITE);
        }
        TextColor(GREEN);
        printf(")\n");
        TextColor(WHITE);
    }

    // Liberar la memoria del array
    free(varArray);
}

// EJERCICIO 4

TMatriz *unir_matrices(TMatriz *mat1, TMatriz *mat2)
{
    if (mat1->fil != mat2->fil)
    {
        TextColor(RED);
        printf("Las matrices deben tener el mismo número de filas para ser unidas.\n");
        TextColor(WHITE);
        return NULL;
    }

    int nuevas_col = mat1->col + mat2->col;
    TMatriz *nueva_mat = (TMatriz *)malloc(sizeof(TMatriz));
    nueva_mat->fil = mat1->fil;
    nueva_mat->col = nuevas_col;
    nueva_mat->m = (double **)malloc(nueva_mat->fil * sizeof(double *));

    for (int i = 0; i < nueva_mat->fil; i++)
    {
        nueva_mat->m[i] = (double *)malloc(nuevas_col * sizeof(double));
        for (int j = 0; j < mat1->col; j++)
        {
            nueva_mat->m[i][j] = mat1->m[i][j];
        }
        for (int j = 0; j < mat2->col; j++)
        {
            nueva_mat->m[i][j + mat1->col] = mat2->m[i][j];
        }
    }

    return nueva_mat;
}