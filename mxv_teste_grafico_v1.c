#include <omp.h>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>  

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

//Aloca dinamicamente uma matriz 2D.

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

// Libera a memória de uma matriz alocada dinamicamente.

void free_matrix(int rows, double **matrix) {
    if (matrix == NULL) return;
    // Primeiro, libera a memória de cada linha
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    // Finalmente, libera o array de ponteiros
    free(matrix);
}

// Preenche uma matriz e um vetor com valores aleatórios.

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

// Executa a multiplicação de matriz por vetor (y = A * x).

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
            y[i] += A[i][j] * x[j];
        }
    }
}

// --- Função Principal para o Experimento Final ---
int main() {
    int n_inicial = 32;
    int n_final = 16384;
    int n_passo = 2;

    double start_time, end_time;
    double tempo_linhas, tempo_colunas;
    double fator_lentidao; 

    // 2. Cabeçalho do CSV
    printf("Tamanho_N,Tempo_Linhas_s,Tempo_Colunas_s,Fator_Lentidao\n");

    for (int N = n_inicial; N <= n_final; N *= n_passo) {
        
        int M = N;
        
        double **A = create_matrix(M, N);
        double *x = (double *)malloc(N * sizeof(double));
        double *y = (double *)malloc(M * sizeof(double));

        if (A == NULL || x == NULL || y == NULL) {
            fprintf(stderr, "Falha ao alocar para N=%d\n", N);
            continue;
        }
        // Inicialização de dados na matriz e no vetor

        fill_random_data(M, N, A, x);
        
        int repeticoes = 1000;
        if (N >= 512) repeticoes = 50;
        if (N >= 1024) repeticoes = 10;
        if (N >= 2048) repeticoes = 3;
        
        // Teste 1: Acesso por Linhas
        start_time = omp_get_wtime();
        for(int r = 0; r < repeticoes; r++) {
            multiply_matrix_vector(M, N, A, x, y);
        }
        end_time = omp_get_wtime();
        tempo_linhas = (end_time - start_time) / repeticoes;

        // Teste 2: Acesso por Colunas
        start_time = omp_get_wtime();
        for(int r = 0; r < repeticoes; r++) {
            multiply_matrix_vector_cols_outer(M, N, A, x, y);
        }
        end_time = omp_get_wtime();
        tempo_colunas = (end_time - start_time) / repeticoes;

        // 3. Calcula o fator de lentidão
        if (tempo_linhas > 0) {
            fator_lentidao = tempo_colunas / tempo_linhas;
        } else {
            fator_lentidao = 1.0; // Se o tempo base for 0, não há diferença
        }

        // Imprime a linha de dados CSV, agora com 4 colunas
        printf("%d,%.12f,%.12f,%.3f\n", N, tempo_linhas, tempo_colunas, fator_lentidao);

        free_matrix(M, A);
        free(x);
        free(y);
    }

    return 0;
}