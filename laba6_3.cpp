#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>

using namespace std;

const double EPSILON = 1e-6;

// Функция для вывода матрицы
void printMatrix(const vector<vector<double>>& matrix, const string& title) {
    cout << title << ":\n";
    for (const auto& row : matrix) {
        for (double val : row) {
            cout << setw(8) << fixed << setprecision(2) << val << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// LU-разложение матрицы
void LUDecomposition(const vector<vector<double>>& A,
                    vector<vector<double>>& L,
                    vector<vector<double>>& U) {
    int n = A.size();
    L = vector<vector<double>>(n, vector<double>(n, 0));
    U = vector<vector<double>>(n, vector<double>(n, 0));

    for (int i = 0; i < n; i++) {
        // Верхняя треугольная матрица U
        for (int k = i; k < n; k++) {
            double sum = 0;
            for (int j = 0; j < i; j++) {
                sum += L[i][j] * U[j][k];
            }
            U[i][k] = A[i][k] - sum;
        }

        // Нижняя треугольная матрица L
        for (int k = i; k < n; k++) {
            if (i == k) {
                L[i][i] = 1;
            } else {
                double sum = 0;
                for (int j = 0; j < i; j++) {
                    sum += L[k][j] * U[j][i];
                }
                L[k][i] = (A[k][i] - sum) / U[i][i];
            }
        }
    }
}

// Прямая подстановка Ly = b
vector<double> ForwardSubstitution(const vector<vector<double>>& L,
                                  const vector<double>& b) {
    int n = L.size();
    vector<double> y(n, 0);

    for (int i = 0; i < n; i++) {
        double sum = 0;
        for (int j = 0; j < i; j++) {
            sum += L[i][j] * y[j];
        }
        y[i] = (b[i] - sum) / L[i][i];
    }

    return y;
}

// Обратная подстановка Ux = y
vector<double> BackwardSubstitution(const vector<vector<double>>& U,
                                   const vector<double>& y) {
    int n = U.size();
    vector<double> x(n, 0);

    for (int i = n - 1; i >= 0; i--) {
        double sum = 0;
        for (int j = i + 1; j < n; j++) {
            sum += U[i][j] * x[j];
        }
        x[i] = (y[i] - sum) / U[i][i];
    }

    return x;
}

// Вычисление невязки
vector<double> calculateResidual(const vector<vector<double>>& A,
                               const vector<double>& b,
                               const vector<double>& x) {
    int n = A.size();
    vector<double> residual(n, 0);

    for (int i = 0; i < n; i++) {
        double sum = 0;
        for (int j = 0; j < n; j++) {
            sum += A[i][j] * x[j];
        }
        residual[i] = b[i] - sum;
    }

    return residual;
}

// Вывод результатов в таблицу
void printResultsTable(const vector<double>& x,
                      const vector<double>& residual) {
    cout << "+-----+--------+--------+--------+--------+--------+\n";
    cout << "|  N  |   x1   |   x2   |   x3   |   x4   |   εn   |\n";
    cout << "+-----+--------+--------+--------+--------+--------+\n";

    cout << "| " << setw(3) << 1;
    for (int j = 0; j < 4; j++) {
        cout << " | " << fixed << setprecision(2) << setw(6) << x[j];
    }

    // Максимальная по модулю невязка
    double max_residual = 0;
    for (double r : residual) {
        if (fabs(r) > max_residual) {
            max_residual = fabs(r);
        }
    }
    cout << " | " << fixed << setprecision(2) << setw(6) << max_residual << " |\n";

    cout << "+-----+--------+--------+--------+--------+--------+\n";
}

int main() {
    // Исходные данные
    const double M = 0.89;
    const double N = 0.08;
    const double P = 1.21;

    // Матрица коэффициентов
    vector<vector<double>> A = {
        {M, -0.04, 0.21, -18},    // Изменено N на M в первом уравнении
        {0.25, -1.23, N, -0.09},
        {-0.21, N, 0.8, -0.13},
        {0.15, -1.31, 0.06, P}
    };

    // Вектор правых частей
    vector<double> b = {-1.24, P, 2.56, M};

    // Вывод исходной системы
    cout << "Исходная система уравнений:\n";
    cout << M << "x1 - 0.04x2 + 0.21x3 - 18x4 = -1.24\n";
    cout << "0.25x1 - 1.23x2 + " << N << "x3 - 0.09x4 = " << P << "\n";
    cout << "-0.21x1 + " << N << "x2 + 0.8x3 - 0.13x4 = 2.56\n";
    cout << "0.15x1 - 1.31x2 + 0.06x3 + " << P << "x4 = " << M << "\n\n";

    // LU-разложение
    vector<vector<double>> L, U;
    LUDecomposition(A, L, U);

    printMatrix(L, "Матрица L");
    printMatrix(U, "Матрица U");

    // Решение системы
    vector<double> y = ForwardSubstitution(L, b);
    vector<double> x = BackwardSubstitution(U, y);

    // Вычисление невязки
    vector<double> residual = calculateResidual(A, b, x);

    // Вывод результатов
    cout << "Решение системы методом LU-разложения:\n";
    printResultsTable(x, residual);

    return 0;
}
