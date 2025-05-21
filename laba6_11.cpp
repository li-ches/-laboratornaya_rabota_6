#include <iostream>
#include <vector>
#include <cstdlib>  // для использования случайных чисел
#include <ctime>    // для использования случайных чисел

using namespace std;

int main() {
    int N;

    // Проверка ввода размера матрицы
    cout << "Введите порядок матрицы N: ";
    if (!(cin >> N)) {
        cerr << "Ошибка ввода. Пожалуйста, введите целое число.\n";
        return 1;
    }

    if (N <= 0) {
        cerr << "Порядок матрицы должен быть положительным числом.\n";
        return 1;
    }

    srand(time(nullptr)); // Инициализация генератора случайных чисел текущим временем

    vector<vector<int>> matrix(N, vector<int>(N));

    // Заполнение матрицы случайными числами в диапазоне [-100, 100]
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = -100 + rand() % 201;
        }
    }

    // Вывод матрицы
    cout << "Матрица:\n";
    for (const auto& row : matrix) { // Итерация по каждой строке
        for (int val : row) // Итерация по элементам строки
            cout << val << "\t"; // Вывод элемента с табуляцией для форматирования
        cout << "\n";
    }

    int maxPosCount = -1; // Инициализация переменной для хранения максимального количества положительных чисел
    int indexOfMaxRow = -1; // Индекс строки с максимальным количеством положительных чисел

    // Поиск строки с наибольшим количеством положительных чисел
    for (int i = 0; i < N; ++i) {
        int countPos = 0;
        for (int val : matrix[i]) {
            if (val > 0)
                ++countPos;
        }
        if (countPos > maxPosCount) {
            maxPosCount = countPos;
            indexOfMaxRow = i;
        }
    }

    // Обработка ситуации, если все числа равны нулю или отрицательны
    if (indexOfMaxRow == -1 || maxPosCount == 0) {
        cout << "Нет строки с положительными числами.\n";
        return 0;
    }

    vector<int> resultRow = matrix[indexOfMaxRow];

    cout << "Строка с наибольшим количеством положительных чисел (" << maxPosCount << "):\n";
    for (int val : resultRow)
        cout << val << " ";
    cout << "\n";

    return 0;
}
