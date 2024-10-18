// main.c
#include <stdio.h>
#include "lim.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

// Estructura para almacenar información sobre el comando actual
typedef struct
{
    char *command;  // Nombre del comando
    char *operand1; // Primer operando (si aplica)
    char *operand2; // Segundo operando (si aplica)
    char *operand3; // Tercer operando (si aplica)
    char operator1; // Primer operador (si aplica)
    char operator2; // Segundo operador (si aplica)
} CommandInfo;

// Prototipos de funciones
static void process_input(TVars *vars, const char *input);
static CommandInfo parse_command(const char *input);
static void execute_command(TVars *vars, const CommandInfo *cmd);
static void handle_matrix_assignment(TVars *vars, const CommandInfo *cmd);
static void handle_matrix_operation(TVars *vars, const CommandInfo *cmd);
static void handle_command(TVars *vars, const CommandInfo *cmd);
static TMatriz *execute_matrix_operation(TMatriz *m1, TMatriz *m2, char operator);

int main()
{
    char input[MAX_INPUT_SIZE];
    TVars vars = {0, NULL};

    // Mostrar información del alumno al inicio
    TextColor(GREEN);
    printf("*Datos del alumno*\n");
    TextColor(WHITE);
    printf("- Nombre: AYMANE \n");
    printf("- Apellido: El khilaly\n");
    printf("- Correo electronico: aymane.khilaly@umh.es\n\n");

    while (1)
    {
        printf(":%d> ", vars.numVars);
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL)
        {
            // Manejar error de lectura de entrada
            break;
        }
        trim_custom(input);

        if (strcmp(input, "quit") == 0)
        {
            free_all(&vars);
            TextColor(GREEN);
            printf("exit ok \n");
            TextColor(WHITE);
            break;
        }

        if (strlen(input) > 0)
        {
            process_input(&vars, input);
        }
    }

    return 0;
}

// Procesa la entrada del usuario
static void process_input(TVars *vars, const char *input)
{
    CommandInfo cmd = parse_command(input);
    execute_command(vars, &cmd);
    // Liberar memoria de los operandos, si se asignaron
    free(cmd.operand1);
    free(cmd.operand2);
    free(cmd.operand3);
}

// Analiza la entrada del usuario y devuelve una estructura CommandInfo
static CommandInfo parse_command(const char *input)
{
    CommandInfo cmd = {0};
    char *input_copy = strdup(input); // Copia la entrada para no modificarla

    // Obtener el comando
    cmd.command = strtok(input_copy, " ");

    // Buscar operadores para determinar el tipo de comando
    char *operator1 = strchr(input_copy, '+');
    char *operator2 = strchr(input_copy, '*');
    char *operator3 = strchr(input_copy, '$');
    char *operator4 = strchr(input_copy, '&');
    char *operator5 = strchr(input_copy, '-');
    // Si encontramos '-' , comprobamos si es un operador o un signo negativo
    if (operator5 != NULL && isdigit(*(operator5 + 1)))
    {
        operator5 = NULL; // No es un operador
    }

    // Determinar el tipo de comando y extraer operandos
    if (strchr(cmd.command, '=') != NULL)
    {
        // Asignación de matriz
        cmd.operand1 = strtok(NULL, "=");
        cmd.operand2 = strtok(NULL, "");
        trim_custom(cmd.operand1);
        trim_custom(cmd.operand2);
    }
    else if (operator1 != NULL || operator2 != NULL || operator3 != NULL || operator4 != NULL || operator5 != NULL)
    {
        // Operación de matrices
        cmd.operand1 = strtok(NULL, "+*$&-");
        // Determinar qué operador se encontró primero
        if (operator1 != NULL && (operator1 < operator2 || operator2 == NULL) && (operator1 < operator3 || operator3 == NULL) && (operator1 < operator4 || operator4 == NULL) && (operator1 < operator5 || operator5 == NULL))
        {
            cmd.operator1 = '+';
            cmd.operand2 = strtok(NULL, " ");
        }
        else if (operator2 != NULL && (operator2 < operator1 || operator1 == NULL) && (operator2 < operator3 || operator3 == NULL) && (operator2 < operator4 || operator4 == NULL) && (operator2 < operator5 || operator5 == NULL))
        {
            cmd.operator1 = '*';
            cmd.operand2 = strtok(NULL, " ");
        }
        else if (operator3 != NULL && (operator3 < operator1 || operator1 == NULL) && (operator3 < operator2 || operator2 == NULL) && (operator3 < operator4 || operator4 == NULL) && (operator3 < operator5 || operator5 == NULL))
        {
            cmd.operator1 = '$';
            cmd.operand2 = strtok(NULL, " ");
        }
        else if (operator4 != NULL && (operator4 < operator1 || operator1 == NULL) && (operator4 < operator2 || operator2 == NULL) && (operator4 < operator3 || operator3 == NULL) && (operator4 < operator5 || operator5 == NULL))
        {
            cmd.operator1 = '&';
            cmd.operand2 = strtok(NULL, " ");
        }
        else if (operator5 != NULL && (operator5 < operator1 || operator1 == NULL) && (operator5 < operator2 || operator2 == NULL) && (operator5 < operator3 || operator3 == NULL) && (operator5 < operator4 || operator4 == NULL))
        {
            cmd.operator1 = '-';
            cmd.operand2 = strtok(NULL, " ");
        }
        // Verificar si hay un segundo operador
        if (strchr(cmd.operand2, '+') != NULL || strchr(cmd.operand2, '-') != NULL)
        {
            cmd.operator2 = *strpbrk(cmd.operand2, "+-");
            cmd.operand2 = strtok(cmd.operand2, "+-");
            cmd.operand3 = strtok(NULL, " ");
        }
    }
    else
    {
        // Comando simple (det, product, view, save, load, transp, inv)
        cmd.operand1 = strtok(NULL, " ");
        if (strcmp(cmd.command, "product") == 0)
        {
            cmd.operand2 = strtok(NULL, " ");
        }
        else if (strcmp(cmd.command, "load") == 0)
        {
            cmd.operand2 = strtok(NULL, " "); // Para "load over"
        }
    }

    free(input_copy);
    return cmd;
}

// Ejecuta el comando especificado en la estructura CommandInfo
static void execute_command(TVars *vars, const CommandInfo *cmd)
{
    if (strchr(cmd->command, '=') != NULL)
    {
        // Asignación de matriz
        handle_matrix_assignment(vars, cmd);
    }
    else if (
        strcmp(cmd->command, "det") == 0 ||
        strcmp(cmd->command, "product") == 0 ||
        strcmp(cmd->command, "view") == 0 ||
        strcmp(cmd->command, "save") == 0 ||
        strcmp(cmd->command, "load") == 0 ||
        strcmp(cmd->command, "transp") == 0 ||
        strcmp(cmd->command, "inv") == 0)
    {
        handle_command(vars, cmd);
    }
    else
    {
        // Operación de matrices
        handle_matrix_operation(vars, cmd);
    }
}

// Maneja la asignación de matrices (p. ej., A = (1 2|3 4))
static void handle_matrix_assignment(TVars *vars, const CommandInfo *cmd)
{
    char *var_name = cmd->command;
    char *matrix_str = cmd->operand1;

    // Eliminar el signo '=' del nombre de la variable
    *strchr(var_name, '=') = '\0';
    trim_custom(var_name);

    if (!is_valid_matrix_name(var_name))
    {
        TextColor(RED);
        printf("ERROR: Nombre de variable no válido.\n");
        TextColor(WHITE);
        return;
    }

    TMatriz *m;

    // Verificar si 'matrix_str' es el nombre de una matriz existente
    if (!is_matrix(matrix_str))
    {
        m = find_matrix(vars, matrix_str);
        if (m == NULL)
        {
            TextColor(RED);
            printf("ERROR: Matriz '%s' no encontrada.\n", matrix_str);
            TextColor(WHITE);
            return;
        }
    }
    else
    {
        // Si no es una matriz existente, crear una nueva matriz
        m = NuevaTmatriz(matrix_str);
        if (m == NULL)
        {
            TextColor(RED);
            printf("ERROR: Formato de matriz no válido.\n");
            TextColor(WHITE);
            return;
        }
    }

    // Asignar la matriz a la variable
    if (!store_named_matrix(vars, new_named_matrix(var_name, m)))
    {
        TextColor(RED);
        printf("ERROR: No se pudo asignar la matriz.\n");
        TextColor(WHITE);
    }
}

// Maneja operaciones entre matrices (p. ej., A + B, A * B)
static void handle_matrix_operation(TVars *vars, const CommandInfo *cmd)
{
    TMatriz *m1 = NULL;
    TMatriz *m2 = NULL;
    TMatriz *m3 = NULL;
    TMatriz *result = NULL;

    m1 = is_matrix(cmd->operand1) ? NuevaTmatriz(cmd->operand1) : find_matrix(vars, cmd->operand1);
    m2 = is_matrix(cmd->operand2) ? NuevaTmatriz(cmd->operand2) : find_matrix(vars, cmd->operand2);

    if (cmd->operand3 != NULL)
    {
        m3 = is_matrix(cmd->operand3) ? NuevaTmatriz(cmd->operand3) : find_matrix(vars, cmd->operand3);
    }

    if (m1 == NULL || m2 == NULL || (cmd->operand3 != NULL && m3 == NULL))
    {
        TextColor(RED);
        printf("ERROR: Una o más matrices no existen.\n");
        TextColor(WHITE);
        // Liberar matrices en caso de error
        if (is_matrix(cmd->operand1) && m1 != NULL)
            free_matrix(m1);
        if (is_matrix(cmd->operand2) && m2 != NULL)
            free_matrix(m2);
        if (is_matrix(cmd->operand3) && m3 != NULL)
            free_matrix(m3);
        return;
    }

    if (cmd->operand3 != NULL)
    {
        // Operaciones con tres operandos
        TMatriz *temp = execute_matrix_operation(m1, m2, cmd->operator1);
        if (temp == NULL)
        {
            // Manejar error en la operación
            TextColor(RED);
            printf("ERROR: No se pudo realizar la operación.\n");
            TextColor(WHITE);
            // Liberar matrices en caso de error
            if (is_matrix(cmd->operand1) && m1 != NULL)
                free_matrix(m1);
            if (is_matrix(cmd->operand2) && m2 != NULL)
                free_matrix(m2);
            if (is_matrix(cmd->operand3) && m3 != NULL)
                free_matrix(m3);
            return;
        }
        result = execute_matrix_operation(temp, m3, cmd->operator2);
        free_matrix(temp);
    }
    else
    {
        // Operaciones con dos operandos
        result = execute_matrix_operation(m1, m2, cmd->operator1);
    }

    if (result != NULL)
    {
        TextColor(GREEN);
        print_matrix(result, result->fil, result->col);
        TextColor(WHITE);
        free_matrix(result);
    }
    else
    {
        if (cmd->operator1 != '$')
        {
            TextColor(RED);
            printf("ERROR: No se pudo realizar la operación entre las matrices.\n");
            TextColor(WHITE);
        }
    }
    // Liberar matrices en caso de que se hayan creado al inicio
    if (is_matrix(cmd->operand1) && m1 != NULL)
        free_matrix(m1);
    if (is_matrix(cmd->operand2) && m2 != NULL)
        free_matrix(m2);
    if (is_matrix(cmd->operand3) && m3 != NULL)
        free_matrix(m3);
}

// Ejecuta la operación de matriz especificada por el operador
static TMatriz *execute_matrix_operation(TMatriz *m1, TMatriz *m2, char operator)
{
    switch (operator)
    {
    case '+':
        return add_matrices(m1, m2);
    case '*':
        return multiply_matrices(m1, m2);
    case '-':
        return subtract_matrices(m1, m2);
    case '&':
        return unir_matrices(m1, m2);
    case '$':
    {
        double product = scalar_product(m1, m2);
        if (!isnan(product))
        {
            TextColor(GREEN);
            printf("%.2f\n", product);
            TextColor(WHITE);
        }
        else
        {
            TextColor(RED);
            printf("ERROR: No se pudo calcular el producto escalar.\n");
            TextColor(WHITE);
        }
        return NULL; // No se devuelve una matriz para el producto escalar
    }
    default:
        TextColor(RED);
        printf("ERROR: Operador no válido.\n");
        TextColor(WHITE);
        return NULL;
    }
}

// Maneja comandos como "det", "product", "view", "save", "load", "transp", "inv"
static void handle_command(TVars *vars, const CommandInfo *cmd)
{
    if (strcmp(cmd->command, "det") == 0)
    {
        TMatriz *m = is_matrix(cmd->operand1) ? NuevaTmatriz(cmd->operand1) : find_matrix(vars, cmd->operand1);
        if (m != NULL)
        {
            if (m->fil != m->col)
            {
                TextColor(RED);
                printf("Error: El determinante solo se puede calcular para matrices cuadradas.\n");
                TextColor(WHITE);
            }
            else
            {
                double det = determinant(m, m->fil);
                TextColor(GREEN);
                printf("%.2f\n", det);
                TextColor(WHITE);
            }
            if (is_matrix(cmd->operand1))
            {
                free_matrix(m);
            }
        }
        else
        {
            TextColor(RED);
            printf("ERROR:  Cadena de matriz no válida o matriz no encontrada\n");
            TextColor(WHITE);
        }
    }
    else if (strcmp(cmd->command, "product") == 0)
    {
        char *matrix_str;
        double num;
        if (is_matrix(cmd->operand1))
        {
            matrix_str = cmd->operand1;
            num = strtod(cmd->operand2, NULL);
        }
        else
        {
            matrix_str = cmd->operand2;
            num = strtod(cmd->operand1, NULL);
        }

        TMatriz *m = is_matrix(matrix_str) ? NuevaTmatriz(matrix_str) : find_matrix(vars, matrix_str);

        if (m != NULL)
        {
            TMatriz *result = multiply_matrix_by_number(m, num);
            if (result != NULL)
            {
                TextColor(GREEN);
                print_matrix(result, result->fil, result->col);
                TextColor(WHITE);
                free_matrix(result);
            }
            else
            {
                TextColor(RED);
                printf("ERROR: No se pudo multiplicar la matriz por el número.\n");
                TextColor(WHITE);
            }
            if (is_matrix(matrix_str))
            {
                free_matrix(m);
            }
        }
        else
        {
            TextColor(RED);
            printf("ERROR: La matriz no existe.\n");
            TextColor(WHITE);
        }
    }
    else if (strcmp(cmd->command, "view") == 0)
    {
        display_vars_alfab(vars->primera);
    }
    else if (strcmp(cmd->command, "save") == 0)
    {
        save_variables_to_file(vars, cmd->operand1);
    }
    else if (strcmp(cmd->command, "load") == 0)
    {
        if (cmd->operand2 != NULL && strcmp(cmd->operand2, "over") == 0)
        {
            load_variables_from_file(vars, cmd->operand1, true);
        }
        else
        {
            load_variables_from_file(vars, cmd->operand1, false);
        }
    }
    else if (strcmp(cmd->command, "transp") == 0)
    {
        TMatriz *m = is_matrix(cmd->operand1) ? NuevaTmatriz(cmd->operand1) : find_matrix(vars, cmd->operand1);
        if (m != NULL)
        {
            TMatriz *transp = transpose_matrix(m);
            TextColor(GREEN);
            print_matrix(transp, transp->fil, transp->col);
            TextColor(WHITE);
            free_matrix(transp);
            if (is_matrix(cmd->operand1))
            {
                free_matrix(m);
            }
        }
        else
        {
            TextColor(RED);
            printf("ERROR: No se pudo encontrar la matriz.\n");
            TextColor(WHITE);
        }
    }
    else if (strcmp(cmd->command, "inv") == 0)
    {
        TMatriz *m = is_matrix(cmd->operand1) ? NuevaTmatriz(cmd->operand1) : find_matrix(vars, cmd->operand1);
        if (m != NULL)
        {
            TMatriz *inv = inversa_matrix(m);
            if (inv != NULL)
            {
                TextColor(GREEN);
                print_matrix(inv, inv->fil, inv->col);
                TextColor(WHITE);
                free_matrix(inv);
            }
            if (is_matrix(cmd->operand1))
            {
                free_matrix(m);
            }
        }
        else
        {
            TextColor(RED);
            printf("ERROR: No se pudo encontrar la matriz.\n");
            TextColor(WHITE);
        }
    }
    else
    {
        TextColor(RED);
        printf("ERROR: Comando no válido.\n");
        TextColor(WHITE);
    }
}