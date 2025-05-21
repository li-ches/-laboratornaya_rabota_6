#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    // Вывод подсказки для ввода K
    cout << "Введите количество целевых клеток K:" << endl;
    int K;
    cin >> K;

    // Проверка корректности K
    if (K <= 0) {
        cout << "Ошибка: K должно быть положительным числом" << endl;
        return 1;
    }
    if (K > 1e4) {
        cout << "Ошибка: K не может превышать 10000" << endl;
        return 1;
    }

    // Вывод подсказки для ввода клеток
    cout << "Введите клетки через пробел в порядке возрастания:" << endl;
    vector<int> cells(K);
    for (int i = 0; i < K; ++i) {
        cin >> cells[i];
        // Проверка корректности клеток
        if (cells[i] < 1) {
            cout << "Ошибка: номер клетки не может быть меньше 1" << endl;
            return 1;
        }
        if (cells[i] > 1e4) {
            cout << "Ошибка: номер клетки не может превышать 10000" << endl;
            return 1;
        }
        // Проверка возрастания
        if (i > 0 && cells[i] <= cells[i-1]) {
            cout << "Ошибка: номера клеток должны быть строго возрастающими" << endl;
            return 1;
        }
    }

    // Проверка на ошибки ввода
    if (cin.fail()) {
        cout << "Ошибка ввода: введены некорректные данные" << endl;
        return 1;
    }

    int position = 1; // Начинаем с головы удава (клетка 1)
    int speed = 0;     // Начальная скорость
    int moves = 0;     // Счетчик ходов

    for (int target : cells) {
        while (position < target) {
            // Вычисляем минимальную скорость для достижения цели
            int required_speed = target - position;

            // Оптимизируем скорость
            if (speed < required_speed) {
                speed++;
            } else if (speed > required_speed) {
                speed = max(speed - 1, 1);
            }

            // Перемещаем фишку
            position += speed;
            moves++;
        }
    }

    // Вывод результата
    cout << "Минимальное количество ходов: " << moves << endl;
    return 0;
}
