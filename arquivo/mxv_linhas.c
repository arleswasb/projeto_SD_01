#include <stdio.h>

// --- Dimensões da Matriz e Vetores ---
// Matriz A tem M linhas e N colunas
#define M 3
// Vetor x tem N elementos, Vetor y tem M elementos
#define N 4

/**
 * @brief Imprime os elementos de uma matriz formatada.
 *
 * @param rows O número de linhas da matriz.
 * @param cols O número de colunas da matriz.
 * @param matrix A matriz a ser impressa.
 */
void print_matrix(int rows, int cols, double matrix[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        printf("| ");
        for (int j = 0; j < cols; j++) {
            printf("%8.2f ", matrix[i][j]);
        }
        printf("|\n");
    }
}

/**
 * @brief Imprime os elementos de um vetor.
 *
 * @param size O número de elementos no vetor.
 * @param vector O vetor a ser impresso.
 */
void print_vector(int size, double vector[size]) {
    for (int i = 0; i < size; i++) {
        printf("| %8.2f |\n", vector[i]);
    }
}


int main() {
    // --- 1. Inicialização ---
    // Matriz A (M x N)
    double A[M][N] = {
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        {9.0, 1.0, 2.0, 3.0}
    };

    // Vetor de entrada x (N elementos)
    double x[N] = {1.0, 2.0, 3.0, 4.0};

    // Vetor de resultado y (M elementos), inicializado com zeros
    double y[M] = {0.0};

    // --- 2. Exibição dos Dados de Entrada ---
    printf("Matriz A (%d x %d):\n", M, N);
    print_matrix(M, N, A);
    printf("\nVetor x (%d x 1):\n", N);
    print_vector(N, x);

    // --- 3. Lógica da Multiplicação (Acesso por Linhas) ---
    // O laço externo itera sobre as LINHAS da matriz A.
    for (int i = 0; i < M; i++) {
        // Para cada linha i, calculamos o elemento y[i] do vetor resultado.
        // Inicializamos a soma para esta linha.
        double sum = 0.0;

        // O laço interno itera sobre as COLUNAS da matriz A (e os elementos do vetor x).
        for (int j = 0; j < N; j++) {
            // Acessa A[i][j] e multiplica pelo elemento correspondente de x
            sum += A[i][j] * x[j];
        }

        // Armazena a soma final no vetor resultado.
        y[i] = sum;
    }

    // --- 4. Exibição do Resultado ---
    printf("\nVetor Resultante y = A * x (%d x 1):\n", M);
    print_vector(M, y);

    return 0;
}