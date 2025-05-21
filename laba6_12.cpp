#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    int M, N;

    // Ввод размеров матрицы с проверкой
    cout << "Введите размеры матрицы M и N: ";
    if (!(cin >> M >> N)) {
        cerr << "Ошибка ввода. Пожалуйста, введите целые числа.\n";
        return 1; // Завершение программы с ошибкой
    }

    if (M <= 0 || N <= 0) {
        cerr << "Размеры матрицы должны быть положительными числами.\n";
        return 1;
    }

    // Инициализация генератора случайных чисел
    srand(time(nullptr));

    // Создание матрицы M x N со случайными числами в диапазоне [100, 150]
    vector<vector<int>> matrix(M, vector<int>(N));
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = 100 + rand() % 51; // [100,150]
        }
    }

    // Вывод матрицы
    cout << "Матрица:\n";
    for (const auto& row : matrix) {
        for (int val : row)
            cout << val << "\t";
        cout << "\n";
    }

    // Подсчет частоты каждого числа
    unordered_map<int, int> frequency;
    for (const auto& row : matrix) {
        for (int val : row)
            ++frequency[val];
    }

    // Поиск числа с максимальной частотой
    int maxCount = 0;
    int numberWithMaxCount = 0;
    for (const auto& kv : frequency) {
        if (kv.second > maxCount) {
            maxCount = kv.second;
            numberWithMaxCount = kv.first;
        }
    }

    // Вывод результата
    if (maxCount == 1) {
        cout << "Нет числа, которое встречается более одного раза.\n";
    } else {
        cout << "Число, встречающееся наибольшее количество раз: "
             << numberWithMaxCount
             << " (" << maxCount << " раз)\n";
    }

    return 0;
}
