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
            cout << setw(10) << fixed << setprecision(4) << val << " ";
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
                for (int j = 0; j < i; j++){
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

// Метод Зейделя для решения системы линейных уравнений
vector<double> SeidelMethod(const vector<vector<double>>& A,
                           const vector<double>& b,
                           double epsilon = EPSILON,
                           int maxIterations = 1000) {
    int n = A.size();
    vector<double> x(n, 0.0);//текущее приближение
    vector<double> x_prev(n, 0.0);//предыдущее приближение
    int iteration = 0;//счетчик итераций
    double error;

    do {
        x_prev = x;
        for (int i = 0; i < n; i++) {
            double sum = 0.0;
            for (int j = 0; j < n; j++) {
                if (j != i) {
                    sum += A[i][j] * x[j];
                }
            }
            x[i] = (b[i] - sum) / A[i][i];
        }

        // Вычисление ошибки
        error = 0.0;
        for (int i = 0; i < n; i++) {
            error = max(error, abs(x[i] - x_prev[i]));
        }
        iteration++;
    } while (error > epsilon && iteration < maxIterations); // Условие продолжения

    cout << "Метод Зейделя сошелся за " << iteration << " итераций\n";
    return x;
}

// Вывод результатов в таблицу
void printResultsTable(const vector<double>& x,
                       const vector<double>& residual,
                       const string& methodName) {
    cout << "\nРезультаты решения методом " << methodName << ":\n";
    cout << "+-----+-----------+-----------+-----------+-----------+\n";
    cout << "|  N  |    x1     |    x2     |    x3     |    x4     |\n";
    cout << "+-----+-----------+-----------+-----------+-----------+\n";

    cout << "| " << setw(3) << 1;
    for (int j = 0; j < 4; j++) {
        cout << " | " << setw(9) << fixed << setprecision(6) << x[j];
    }
    cout << " |\n";

    // Максимальная по модулю невязка
    double max_residual = 0;
    for(double r : residual) {
        if (fabs(r) > max_residual) {
            max_residual = fabs(r);
        }
    }
    cout << "+-----+-----------+-----------+-----------+-----------+\n";
    cout << "Максимальная невязка: " << scientific << setprecision(2) << max_residual << "\n\n";
}

int main() {
    // Преобразованная система с диагональным преобладанием
    vector<vector<double>> A = {
        {18.0,  -0.04,  0.21,  -0.89},
        {0.25,  -1.23,  0.08,  -0.09},
        {-0.21,  0.08,  1.80,  -0.13},
        {0.15,  -1.31,  0.06,   2.42}
    };

    vector<double> b = {1.24, 1.21, 2.56, 1.78};

    // Вывод преобразованной системы
    cout << "Преобразованная система уравнений:\n";
    cout << "18.0x4 - 0.04x2 + 0.21x3 - 0.89x1 = 1.24\n";
    cout << "0.25x1 - 1.23x2 + 0.08x3 - 0.09x4 = 1.21\n";
    cout << "-0.21x1 + 0.08x2 + 1.8x3 - 0.13x4 = 2.56\n";
    cout << "0.15x1 - 1.31x2 + 0.06x3 + 2.42x4 = 1.78\n\n";

    // 1. Решение методом LU-разложения
    vector<vector<double>> L, U;
    LUDecomposition(A, L, U);

    printMatrix(L, "Матрица L");
    printMatrix(U, "Матрица U");

    vector<double> y = ForwardSubstitution(L, b);
    vector<double> x_lu = BackwardSubstitution(U, y);
    vector<double> residual_lu = calculateResidual(A, b, x_lu);
    printResultsTable(x_lu, residual_lu, "LU-разложения");

    // 2. Решение методом Зейделя
    vector<double> x_seidel = SeidelMethod(A, b);
    vector<double> residual_seidel = calculateResidual(A, b, x_seidel);
    printResultsTable(x_seidel, residual_seidel, "Зейделя");

    return 0;
}
