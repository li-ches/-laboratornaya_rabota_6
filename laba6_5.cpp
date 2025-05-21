#include <iostream>
#include <vector>
#include <climits>
#include <random>

using namespace std;

// Функция для проверки, является ли доска шахматной
bool isChessboard(const vector<vector<int>>& board, int pattern) {
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[0].size(); ++j) {
            int expected = (i + j + pattern) % 2;
            if (board[i][j] != expected) {
                return false;
            }
        }
    }
    return true;
}

// Функция для переворота монеты
void flipCoin(vector<vector<int>>& board, int i, int j) {
    if (i >= 0 && i < board.size() && j >= 0 && j < board[0].size()) {
        board[i][j] ^= 1; // Переворачиваем монету
    }
}

// Функция для подсчета минимального количества ходов
int minFlips(vector<vector<int>>& board) {
    int n = board.size();
    if (n == 0) return 0;
    int m = board[0].size();

    // Два возможных шахматных паттерна
    int min_flips = INT_MAX;

    for (int pattern = 0; pattern < 2; ++pattern) {
        vector<vector<int>> temp = board;
        int flips = 0;

        // Пытаемся преобразовать доску в текущий паттерн
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                int expected = (i + j + pattern) % 2;
                if (temp[i][j] != expected) {
                    // Переворачиваем текущую монету и соседнюю (правую или нижнюю)
                    flipCoin(temp, i, j);
                    if (j + 1 < m) {
                        flipCoin(temp, i, j + 1);
                    } else if (i + 1 < n) {
                        flipCoin(temp, i + 1, j);
                    } else {
                        // Невозможно сделать ход - паттерн недостижим
                        flips = INT_MAX;
                        break;
                    }
                    flips++;
                }
            }
            if (flips == INT_MAX) break;
        }

        if (flips < min_flips) {
            min_flips = flips;
        }
    }

    return (min_flips == INT_MAX) ? -1 : min_flips;
}

int main() {
    // Инициализация генератора случайных чисел
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1);

    // Размеры доски
    int n, m;
    cout << "Введите размеры доски N и M: ";
    cin >> n >> m;

    // Создание и заполнение доски случайными значениями
    vector<vector<int>> board(n, vector<int>(m));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            board[i][j] = dis(gen);
        }
    }

    // Вывод исходной доски
    cout << "Исходная доска:" << endl;
    for (const auto& row : board) {
        for (int coin : row) {
            cout << coin << " ";
        }
        cout << endl;
    }

    // Подсчет минимального количества ходов
    int result = minFlips(board);

    if (result == -1) {
        cout << "Невозможно преобразовать доску в шахматный узор" << endl;
    } else {
        cout << "Минимальное количество ходов: " << result << endl;
    }

    return 0;
}
