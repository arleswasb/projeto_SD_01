# Análise de Performance: Impacto do Cache na Multiplicação de Matrizes em C

Este projeto explora de forma prática o impacto da hierarquia de cache da CPU no desempenho de algoritmos computacionais. Através de um experimento controlado, demonstramos como diferentes padrões de acesso à memória em C podem levar a uma divergência drástica no tempo de execução para a mesma operação matemática: a multiplicação de matriz por vetor (MxV).

## Descrição do Projeto

Foram implementadas duas versões do algoritmo MxV:
1.  **Acesso por Linhas (Cache-Friendly):** Um algoritmo que itera sobre a matriz de forma sequencial, alinhada com seu layout na memória (*row-major*), maximizando a eficiência do cache.
2.  **Acesso por Colunas (Cache-Unfriendly):** Um algoritmo que itera sobre a matriz de forma não sequencial, acessando elementos distantes na memória e resultando em um grande número de *cache misses*.

O projeto inclui um programa em C (`mxv_teste_grafico_v1.c`) que executa ambos os algoritmos em matrizes de tamanhos variados, mede seus tempos de execução com alta precisão e gera os dados para análise. Scripts em Python são fornecidos para visualizar os resultados.

## Metodologia

O núcleo do experimento consiste em:
- **Execução Comparativa:** Rodar as duas versões do algoritmo para matrizes quadradas `N x N`, com `N` variando de 32 a 2048.
- **Medição de Precisão:** Utilizar a função `omp_get_wtime()` para medição de tempo (wall-clock time) e a técnica de repetição com média para garantir resultados precisos, mesmo em operações muito rápidas.
- **Coleta de Dados:** O programa em C gera um arquivo `results.csv` contendo o tamanho da matriz, os tempos de execução de ambos os métodos e o "Fator de Lentidão" (a razão entre o tempo da versão lenta e da rápida).

## Como Compilar e Executar

**Pré-requisitos:**
* Um compilador C (GCC com MinGW-w64 no Windows).
* Git instalado.

**Passos:**

1.  **Clone o repositório (opcional, se já tiver os arquivos):**
    ```bash
    git clone [https://github.com/seu-usuario/seu-repositorio.git](https://github.com/seu-usuario/seu-repositorio.git)
    cd seu-repositorio
    ```

2.  **Compile o programa de teste:**
    O código deve ser compilado com as flags de otimização (`-O2`) e do OpenMP (`-fopenmp`).
    ```bash
    gcc mxv_teste_grafico_v1.c -o mxv_teste -O2 -fopenmp
    ```

3.  **Gere os dados de análise:**
    Execute o programa compilado, redirecionando a saída para um arquivo `results.csv`.
    ```bash
    ./mxv_teste > results.csv
    ```

## Visualização dos Resultados

Para gerar os gráficos a partir dos dados, utilize os scripts Python fornecidos.

**Pré-requisitos:**
* Python 3.
* Bibliotecas Pandas e Matplotlib.

1.  **Instale as bibliotecas:**
    ```bash
    pip install pandas matplotlib
    ```

2.  **Execute os scripts de plotagem:**
    ```bash
    # Para gerar o gráfico de tempos de execução
    python gerar_grafico.py

    # Para gerar o gráfico do Fator de Lentidão
    python gerar_grafico_fator_lentidao.py
    ```
    Isso criará os arquivos `grafico_performance_cache.png` e `grafico_fator_lentidao.png`.

## Análise e Conclusões

Os resultados do experimento demonstram de forma conclusiva o impacto da arquitetura de cache no desempenho do software.

![Gráfico de Performance](caminho/para/o/grafico_performance_cache.png)
*(Substitua pelo seu gráfico gerado)*

-   **Para matrizes pequenas (N ≤ 256):** O desempenho de ambos os métodos é praticamente idêntico, pois o conjunto de dados cabe confortavelmente nos caches L1/L2 da CPU.
-   **O Ponto de Inflexão (N ≈ 512):** Quando o tamanho da matriz começa a saturar a capacidade do cache L2, o método de acesso por colunas (cache-unfriendly) torna-se significativamente mais lento.
-   **Colapso da Performance (N ≥ 1024):** Quando a matriz excede a capacidade do cache L3, forçando acessos constantes à lenta memória RAM, o desempenho do método ineficiente entra em colapso. O Fator de Lentidão dispara, chegando a quase **4,5x** para uma matriz de 2048x2048 nos testes realizados.

Este projeto valida o princípio fundamental de que o design de algoritmos de alta performance deve, necessariamente, considerar a arquitetura do hardware, especialmente a hierarquia de memória, para evitar gargalos de desempenho.