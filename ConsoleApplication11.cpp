#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <locale.h>

void clearInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void printLabeledMatrix(int** M, int size) {
    printf("    ");
    for (int j = 0; j < size; j++) {
        printf("x%-2d", j + 1);
    }
    printf("\n");

    for (int i = 0; i < size; i++) {
        printf("x%-2d", i + 1);
        for (int j = 0; j < size; j++) {
            printf("%-3d", M[i][j]);
        }
        printf("\n");
    }
}

void printList(int** A, int n) {
    printf("Список смежности:\n");
    for (int i = 0; i < n; i++) {
        printf("x%-2d: ", i + 1);
        int first = 1;
        for (int j = 0; j < n; j++) {
            if (A[i][j] == 1) {
                if (!first) {
                    printf(", ");
                }
                printf("x%d", j + 1);
                first = 0;
            }
        }
        printf("\n");
    }
}

int** createMatrix(int n) {
    int** M = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++)
        M[i] = (int*)calloc(n, sizeof(int));
    return M;
}

void freeMatrix(int** M, int n) {
    for (int i = 0; i < n; i++) free(M[i]);
    free(M);
}

void randomFill(int** A, int n, double p) {
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++) {
            double r = (double)rand() / (double)RAND_MAX;
            if (r < p) {
                A[i][j] = 1;
                A[j][i] = 1;
            }
        }
}

int contract(int** A, int n, int u, int v) {
    int m = n - 1;
    int** B = createMatrix(m);
    int* pr = (int*)malloc(n * sizeof(int));
    for (int k = 0, idx = 0; k < n; k++) {
        if (k == v) {
            pr[k] = -1;
        }
        else pr[k] = idx++;
    }
    for (int i = 0; i < n; i++) {
        if (i == v) continue;
        for (int j = 0; j < n; j++) {
            if (j == v) continue;
            int val;
            if (i == u) {
                val = A[u][j] | A[v][j];
            }
            else if (j == u) val = A[i][u] | A[i][v];
            else val = A[i][j];
            B[pr[i]][pr[j]] = val;
        }
    }

    printf("\nМатрица после отождествления x%d и x%d:\n", u + 1, v + 1);
    printLabeledMatrix(B, m);
    printList(B, m);

    freeMatrix(B, m);
    free(pr);
    return 0;
}

int ot(int** A, int n) {
    int u, v;
    printf("Введите номера вершин u и v (от 1 до %d): ", n);
    if (scanf_s("%d %d", &u, &v) != 2) {
        printf("Ошибка ввода\n"); clearInput(); return 1;
    }
    u--; v--;
    if (u == v) {
        printf("Ошибка: вершины должны быть разными\n");
        return 1;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
        printf("Ошибка: диапазон 1..%d\n", n);
        return 1;
    }
    return contract(A, n, u, v);
}

int st(int** A, int n) {
    int u, v;
    printf("Введите номера смежных вершин u и v (от 1 до %d): ", n);
    if (scanf_s("%d %d", &u, &v) != 2) {
        printf("Ошибка ввода\n"); clearInput(); return 1;
    }
    u--; v--;
    if (u == v) {
        printf("Ошибка\n");
        return 1;
    }
    if (u < 0 || u >= n || v < 0 || v >= n) {
        printf("Ошибка: диапазон 1..%d\n", n);
        return 1;
    }
    if (A[u][v] != 1) {
        printf("Ошибка: ребра x%d-x%d не существует\n", u + 1, v + 1);
        return 1;
    }
    return contract(A, n, u, v);
}

int ras(int** A, int n) {
    int u;
    printf("Введите номер вершины для расщепления (от 1 до %d): ", n);
    if (scanf_s("%d", &u) != 1) {
        printf("Ошибка ввода\n"); clearInput(); return 1;
    }
    u--;
    if (u < 0 || u >= n) {
        printf("Ошибка: диапазон 1..%d\n", n); return 1;
    }

    int m = n + 1;
    int** B = createMatrix(m);
    int* pr = (int*)malloc(n * sizeof(int));

    for (int k = 0; k < n; k++) {
        if (k < u)      pr[k] = k;
        else if (k > u) pr[k] = k + 1;
        else            pr[k] = -1;
    }

    for (int i = 0; i < n; i++) {
        if (i == u) continue;
        for (int j = 0; j < n; j++) {
            if (j == u) continue;
            B[pr[i]][pr[j]] = A[i][j];
        }
    }

    for (int x = 0; x < n; x++) {
        if (x == u) continue;
        if (A[u][x] != 1) continue;

        int newX = pr[x];

        B[u][newX] = 1;
        B[newX][u] = 1;

        B[u + 1][newX] = 1;
        B[newX][u + 1] = 1;
    }

    B[u][u + 1] = 1;
    B[u + 1][u] = 1;

    B[u][u] = 0;
    B[u + 1][u + 1] = 0;

    printf("\nМатрица после расщепления вершины x%d:\n", u + 1);
    printLabeledMatrix(B, m);
    printList(B, m);

    freeMatrix(B, m);
    free(pr);
    return 0;
}

void orr(int** A1, int n1, int** A2, int n2, int** R) {
    int N = (n1 > n2) ? n1 : n2;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int val1 = (i < n1 && j < n1) ? A1[i][j] : 0;
            int val2 = (i < n2 && j < n2) ? A2[i][j] : 0;
            R[i][j] = val1 | val2;
        }
    }
    printf("\nОбъединение G1 U G2 (размер %dx%d):\n", N, N);
    printLabeledMatrix(R, N);
}

void andd(int** A1, int n1, int** A2, int n2, int** R) {
    int N = (n1 > n2) ? n1 : n2;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int val1 = (i < n1 && j < n1) ? A1[i][j] : 0;
            int val2 = (i < n2 && j < n2) ? A2[i][j] : 0;
            R[i][j] = val1 & val2;
        }
    }
    printf("\nПересечение G1 n G2 (размер %dx%d):\n", N, N);
    printLabeledMatrix(R, N);
}

void summ(int** A1, int n1, int** A2, int n2, int** R) {
    int N = (n1 > n2) ? n1 : n2;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int val1 = (i < n1 && j < n1) ? A1[i][j] : 0;
            int val2 = (i < n2 && j < n2) ? A2[i][j] : 0;
            R[i][j] = val1 ^ val2;
        }
    }
    printf("\nКольцевая сумма G1 (+) G2 (размер %dx%d):\n", N, N);
    printLabeledMatrix(R, N);
}

int** cartesian(int** A1, int n1, int** A2, int n2) {
    int N = n1 * n2;
    int** R = createMatrix(N);

    for (int u1 = 0; u1 < n1; u1++) {
        for (int v1 = 0; v1 < n2; v1++) {
            int p1 = u1 * n2 + v1;
            for (int u2 = 0; u2 < n1; u2++) {
                for (int v2 = 0; v2 < n2; v2++) {
                    int p2 = u2 * n2 + v2;
                    if (p1 >= p2) continue;

                    int hasEdge = 0;
                    if (v1 == v2 && A1[u1][u2] == 1) hasEdge = 1;
                    if (u1 == u2 && A2[v1][v2] == 1) hasEdge = 1;

                    if (hasEdge) {
                        R[p1][p2] = 1;
                        R[p2][p1] = 1;
                    }
                }
            }
        }
    }
    return R;
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand((unsigned)time(NULL));

    printf("1 - один граф (список смежности + операции)\n");
    printf("2 - два графа (объединение/пересечение/сумма)\n");
    printf("3 - декартово произведение G1 x G2\n");
    printf("Ваш выбор: ");
    int mode;
    if (scanf_s("%d", &mode) != 1) { printf("Ошибка ввода\n"); return 1; }

    if (mode == 1) {
        int n;
        double p;
        printf("Введите число вершин: ");
        if (scanf_s("%d", &n) != 1) { printf("Ошибка ввода\n"); return 1; }
        printf("Введите вероятность ребра: ");
        if (scanf_s("%lf", &p) != 1) { printf("Ошибка ввода\n"); return 1; }

        if (n <= 0 || p < 0 || p > 1) { printf("Ошибка ввода\n"); return 1; }

        int** A = createMatrix(n);
        randomFill(A, n, p);

        printf("\nМатрица смежности:\n"); printLabeledMatrix(A, n);
        printf("\n"); printList(A, n);

        printf("\nОперации:\n");
        printf("  a - отождествление\n");
        printf("  b - стягивание ребра\n");
        printf("  c - расщепление вершины\n");
        printf("Ваш выбор: ");

        clearInput();

        char op;
        if (scanf_s("%c", &op, 1) != 1) {
            printf("Ошибка ввода\n"); return 1;
        }

        switch (op) {
        case 'a': case 'A': case 'а': case 'А': ot(A, n);  break;
        case 'b': case 'B': case 'б': case 'Б': st(A, n);  break;
        case 'c': case 'C': case 'в': case 'В': ras(A, n); break;
        default: printf("Неизвестная операция\n");
        }
        freeMatrix(A, n);
    }
    else if (mode == 2) {
        int n1, n2;
        double p1, p2;

        printf("Введите число вершин G1: ");
        if (scanf_s("%d", &n1) != 1) { printf("Ошибка ввода\n"); return 1; }
        printf("Введите вероятность ребра G1: ");
        if (scanf_s("%lf", &p1) != 1) { printf("Ошибка ввода\n"); return 1; }

        printf("Введите число вершин G2: ");
        if (scanf_s("%d", &n2) != 1) { printf("Ошибка ввода\n"); return 1; }
        printf("Введите вероятность ребра G2: ");
        if (scanf_s("%lf", &p2) != 1) { printf("Ошибка ввода\n"); return 1; }

        if (n1 <= 0 || n2 <= 0 || p1 < 0 || p1 > 1 || p2 < 0 || p2 > 1) {
            printf("Ошибка ввода\n"); return 1;
        }

        int** A1 = createMatrix(n1);
        int** A2 = createMatrix(n2);
        int N = (n1 > n2) ? n1 : n2;
        int** R = createMatrix(N);

        randomFill(A1, n1, p1);
        randomFill(A2, n2, p2);

        printf("\nG1:\n"); printLabeledMatrix(A1, n1); printList(A1, n1);
        printf("\nG2:\n"); printLabeledMatrix(A2, n2); printList(A2, n2);

        int op;
        printf("\nВыберите операцию:\n");
        printf("  1 - объединение\n");
        printf("  2 - пересечение\n");
        printf("  3 - кольцевая сумма\n");
        printf("Ваш выбор: ");
        if (scanf_s("%d", &op) != 1) { printf("Ошибка ввода\n"); return 1; }

        switch (op) {
        case 1: orr(A1, n1, A2, n2, R);  break;
        case 2: andd(A1, n1, A2, n2, R); break;
        case 3: summ(A1, n1, A2, n2, R); break;
        default: printf("Неизвестная операция\n");
        }

        freeMatrix(A1, n1);
        freeMatrix(A2, n2);
        freeMatrix(R, N);
    }
    else if (mode == 3) {
        int n1, n2;
        double p1, p2;

        printf("Введите число вершин G1: ");
        if (scanf_s("%d", &n1) != 1) { printf("Ошибка ввода\n"); return 1; }
        printf("Введите вероятность ребра G1: ");
        if (scanf_s("%lf", &p1) != 1) { printf("Ошибка ввода\n"); return 1; }

        printf("Введите число вершин G2: ");
        if (scanf_s("%d", &n2) != 1) { printf("Ошибка ввода\n"); return 1; }
        printf("Введите вероятность ребра G2: ");
        if (scanf_s("%lf", &p2) != 1) { printf("Ошибка ввода\n"); return 1; }

        if (n1 <= 0 || n2 <= 0 || p1 < 0 || p1 > 1 || p2 < 0 || p2 > 1) {
            printf("Ошибка ввода\n"); return 1;
        }

        int** A1 = createMatrix(n1);
        int** A2 = createMatrix(n2);
        randomFill(A1, n1, p1);
        randomFill(A2, n2, p2);

        printf("\nG1:\n"); printLabeledMatrix(A1, n1); printList(A1, n1);
        printf("\nG2:\n"); printLabeledMatrix(A2, n2); printList(A2, n2);

        int N = n1 * n2;
        int** R = cartesian(A1, n1, A2, n2);

        printf("\nДекартово произведение G1 x G2 (размер %dx%d):\n", N, N);
        printLabeledMatrix(R, N);
        printList(R, N);

        freeMatrix(A1, n1);
        freeMatrix(A2, n2);
        freeMatrix(R, N);
    }
    else {
        printf("Неизвестный режим\n");
    }

    return 0;
}
       