#include <omp.h>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>  

// Função de comparação para usar com qsort em um array de doubles.
int compare_doubles(const void *a, const void *b) {
    double da = *(const double *)a;
    double db = *(const double *)b;
    if (da > db) return 1;
    if (da < db) return -1;
    return 0;
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

int main() {
    int n_inicial = 32;
    int n_final = 16384;
    int n_passo = 2;

    double tempo_mediana_linhas, tempo_mediana_colunas;
    double fator_lentidao; 

    // Cabeçalho do CSV
    printf("Tamanho_N,Tempo_Mediana_Linhas_s,Tempo_Mediana_Colunas_s,Fator_Lentidao\n");

    for (int N = n_inicial; N <= n_final; N *= n_passo) {
        
        int M = N;
        
        double **A = create_matrix(M, N);
        double *x = (double *)malloc(N * sizeof(double));
        double *y = (double *)malloc(M * sizeof(double));

        if (A == NULL || x == NULL || y == NULL) {
            fprintf(stderr, "Falha ao alocar para N=%d\n", N);
            continue;
        }

        fill_random_data(M, N, A, x);
        
        int repeticoes = 1001;
        if (N >= 512) repeticoes = 51;
        if (N >= 1024) repeticoes = 11;
        if (N >= 2048) repeticoes = 5;
        
        //Alocar arrays para armazenar os tempos de cada repetição ---
        double *tempos_linhas = (double *)malloc(repeticoes * sizeof(double));
        double *tempos_colunas = (double *)malloc(repeticoes * sizeof(double));
        if (tempos_linhas == NULL || tempos_colunas == NULL) {
             fprintf(stderr, "Falha ao alocar arrays de tempo para N=%d\n", N);
             free_matrix(M, A); free(x); free(y);
             continue;
        }

        // Teste 1: Acesso por Linhas (Coletando tempos individuais)
        for(int r = 0; r < repeticoes; r++) {
            double start_time = omp_get_wtime();
            multiply_matrix_vector(M, N, A, x, y);
            double end_time = omp_get_wtime();
            tempos_linhas[r] = end_time - start_time;
        }

        // Teste 2: Acesso por Colunas (Coletando tempos individuais)
        for(int r = 0; r < repeticoes; r++) {
            double start_time = omp_get_wtime();
            multiply_matrix_vector_cols_outer(M, N, A, x, y);
            double end_time = omp_get_wtime();
            tempos_colunas[r] = end_time - start_time;
        }
 
        // Ordena os tempos do acesso por linhas
        qsort(tempos_linhas, repeticoes, sizeof(double), compare_doubles);
        
        // Ordena os tempos do acesso por colunas
        qsort(tempos_colunas, repeticoes, sizeof(double), compare_doubles);

        // Calcula a mediana (pegando o elemento do meio do array ordenado)
        tempo_mediana_linhas = tempos_linhas[repeticoes / 2];
        tempo_mediana_colunas = tempos_colunas[repeticoes / 2];

        // Calcula o fator de lentidão
        if (tempo_mediana_linhas > 0) {
            fator_lentidao = tempo_mediana_colunas / tempo_mediana_linhas;
        } else {
            fator_lentidao = 1.0;
        }

        // Imprime a linha de dados CSV
        printf("%d,%.12f,%.12f,%.3f\n", N, tempo_mediana_linhas, tempo_mediana_colunas, fator_lentidao);
        
        // --- MODIFICAÇÃO 3: Liberar a memória dos arrays de tempo ---
        free_matrix(M, A);
        free(x);
        free(y);
        free(tempos_linhas);
        free(tempos_colunas);
    }

    return 0;
}