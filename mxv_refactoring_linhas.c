#include <omp.h>
#include <stdio.h>
#include <stdlib.h> // Necessário para malloc, free e rand
#include <time.h>   // Necessário para srand

// As funções de impressão 
void print_matrix(int rows, int cols, double **matrix) {
    for (int i = 0; i < rows; i++) {
        printf("| ");
        for (int j = 0; j < cols; j++) {
            printf("%8.2f ", matrix[i][j]);
        }
        printf("|\n");
    }
}

void print_vector(int size, double *vector) {
    for (int i = 0; i < size; i++) {
        printf("| %8.2f |\n", vector[i]);
    }
}

/**
 * @brief Aloca dinamicamente uma matriz 2D.
 * @param rows O número de linhas.
 * @param cols O número de colunas.
 * @return Um ponteiro para a matriz alocada, ou NULL se a alocação falhar.
 */
double **create_matrix(int rows, int cols) {
    // Aloca um array de ponteiros (um para cada linha)
    double **matrix = (double **)malloc(rows * sizeof(double *));
    if (matrix == NULL) return NULL;

    // Para cada ponteiro de linha, aloca a memória para as colunas
    for (int i = 0; i < rows; i++) {
        matrix[i] = (double *)malloc(cols * sizeof(double));
        if (matrix[i] == NULL) {
            // Se falhar no meio, libera o que já foi alocado
            for(int k = 0; k < i; k++) free(matrix[k]);
            free(matrix);
            return NULL;
        }
    }
    return matrix;
}

/**
 * @brief Libera a memória de uma matriz alocada dinamicamente.
 * @param matrix A matriz a ser liberada.
 * @param rows O número de linhas da matriz.
 */
void free_matrix(int rows, double **matrix) {
    if (matrix == NULL) return;
    // Primeiro, libera a memória de cada linha
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    // Finalmente, libera o array de ponteiros
    free(matrix);
}

/**
 * @brief Preenche uma matriz e um vetor com valores aleatórios.
 */
void fill_random_data(int rows, int cols, double **matrix, double *vector) {
    // Usa o tempo atual como semente para o gerador de números aleatórios
    srand(time(NULL));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = (double)(rand() % 10); // Números aleatórios de 0 a 9
        }
    }
    for (int i = 0; i < cols; i++) {
        vector[i] = (double)(rand() % 10);
    }
}

/**
 * @brief Executa a multiplicação de matriz por vetor (y = A * x).
 */
void multiply_matrix_vector(int rows, int cols, double **A, double *x, double *y) {
    for (int i = 0; i < rows; i++) {
        y[i] = 0.0; // Garante que o valor inicial seja zero
        for (int j = 0; j < cols; j++) {
            y[i] += A[i][j] * x[j];
        }
    }
}

void multiply_matrix_vector_cols_outer(int rows, int cols, double **A, double *x, double *y) {
    for (int i = 0; i < rows; i++) {
        y[i] = 0.0;
    }

    // 2. O laço externo agora itera sobre as COLUNAS (j) da matriz A.
    for (int j = 0; j < cols; j++) {
        // O laço interno itera sobre as LINHAS (i).
        for (int i = 0; i < rows; i++) {
            // Para cada elemento da coluna j, sua contribuição é adicionada
            // ao elemento correspondente do vetor resultado y.
            y[i] += A[i][j] * x[j];
        }
    }
}

// --- Função Principal Orquestradora ---
// Inclua a nova função no seu código e modifique a main assim:

int main() {
    int M = 2000;
    int N = 2000;
    double start_time, end_time;

    printf("Criando matriz %d x %d e vetores...\n", M, N);

    double **A = create_matrix(M, N);
    double *x = (double *)malloc(N * sizeof(double));
    double *y = (double *)malloc(M * sizeof(double));

    if (A == NULL || x == NULL || y == NULL) { /* ... código de erro ... */ return 1; }

    fill_random_data(M, N, A, x);

    // --- Teste 1: Acesso por Linhas (Método Rápido) ---
    printf("\nTestando metodo com acesso por LINHAS (cache-friendly)...\n");
    start_time = omp_get_wtime();
    multiply_matrix_vector(M, N, A, x, y); // A nossa função original e paralela
    end_time = omp_get_wtime();
    printf("Tempo de execucao: %f segundos\n", end_time - start_time);
    printf("--------------------------------------------------\n");

    // --- Teste 2: Acesso por Colunas (Método Lento) ---
    printf("\nTestando metodo com acesso por COLUNAS (cache-unfriendly)...\n");
    start_time = omp_get_wtime();
    multiply_matrix_vector_cols_outer(M, N, A, x, y); // A nossa nova função
    end_time = omp_get_wtime();
    printf("Tempo de execucao: %f segundos\n", end_time - start_time);
    printf("--------------------------------------------------\n");

    // Liberação da memória...
    printf("\nLiberando memoria...\n");
    free_matrix(M, A);
    free(x);
    free(y);
    printf("Memoria liberada com sucesso.\n");

    return 0;
}