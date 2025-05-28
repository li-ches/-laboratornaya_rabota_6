/*#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>

using namespace std;

// Размеры игрового поля
const int WIDTH = 40;
const int HEIGHT = 20;

// Тип для хранения сетки
using Grid = vector<vector<int>>;

/*
 * Инициализация сетки случайным образом с заданной плотностью живых клеток.
 * @param grid - ссылка на сетку, которую нужно заполнить.
 * @param density - вероятность появления живой клетки в каждой ячейке (по умолчанию 0.2).
 *//*
void initializeRandom(Grid& grid, double density=0.2) {
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            // Генерация случайного числа и сравнение с плотностью
            grid[i][j] = ((double)rand() / RAND_MAX) < density ? 1 : 0;
        }
    }
}

/**
 * Вывод текущего состояния сетки на экран.
 * @param grid - константная ссылка на сетку для отображения.
 *//*
void printGrid(const Grid& grid) {
    system("clear");
    for (const auto& row : grid) {
        for (int cell : row) {
            if (cell)
                cout << "O"; // Живая клетка
            else
                cout << " "; // Мертвая клетка
        }
        cout << "\n";
    }
}

/**
 * Подсчет количества живых соседей вокруг клетки
 * Использует тороидальную топологию (границы соединены)
 * @param grid - текущая сетка
 * @param x - координата по вертикали
 * @param y - координата по горизонтали
 * @return число живых соседей
 *//*
int countNeighbors(const Grid& grid, int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0)
                continue; // пропускаем саму клетку
            int nx = (x + dx + HEIGHT) % HEIGHT;
            int ny = (y + dy + WIDTH) % WIDTH;
            count += grid[nx][ny];
        }
    }
    return count;
}

/**
 * Обновление состояния сетки по правилам игры "Жизнь".
 * @param grid - ссылка на текущую сетку, которая будет обновлена.
 *//*
void updateGrid(Grid& grid) {
    Grid newGrid = grid; // создаем копию для обновления

    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            int neighbors = countNeighbors(grid, i, j);
            if (grid[i][j] == 1) { // Живая клетка
                if (neighbors < 2 || neighbors > 3)
                    newGrid[i][j] = 0; // Умирает от недостатка или избытка соседей
            } else { // Мертвая клетка
                if (neighbors == 3)
                    newGrid[i][j] = 1; // Воскресает при наличии трех соседей
            }
        }
    }
    grid = newGrid;
}

int main() {
    srand(time(0));

    // Создаем игровое поле
    Grid grid(HEIGHT, vector<int>(WIDTH));

    // Инициализация с проверкой корректности плотности
    double density;
    cout << "Введите плотность начальной жизни (от 0 до 1): ";
    while (!(cin >> density) || density < 0 || density > 1) {
        cerr << "Некорректный ввод. Пожалуйста, введите число от 0 до 1: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    initializeRandom(grid, density);

    while (true) {
        printGrid(grid);
        updateGrid(grid);
        this_thread::sleep_for(chrono::milliseconds(200));
    }

    return 0;
}
*/
#include <iostream>
#include <vector>
#include <unistd.h> // для usleep

using namespace std;

// Размер поля 20x20
const int SIZE = 20;

// Игровое поле
vector<vector<bool>> grid(SIZE, vector<bool>(SIZE, false));

// Ставим глайдер в центр
void setupGlider() {
    int center = SIZE/2;
    grid[center][center+1] = true;
    grid[center+1][center+2] = true;
    grid[center+2][center] = true;
    grid[center+2][center+1] = true;
    grid[center+2][center+2] = true;
}

// Печатаем поле
void print() {
    system("clear");
    for (auto row : grid) {
        for (bool cell : row)
            cout << (cell ? "O" : " ");
        cout << endl;
    }
}

// Считаем соседей
int countNeighbors(int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)
            if (i || j) // не считаем саму клетку
                count += grid[(x+i+SIZE)%SIZE][(y+j+SIZE)%SIZE];
    return count;
}

// Обновляем состояние
void update() {
    auto newGrid = grid;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            int n = countNeighbors(i, j);
            if (grid[i][j]) newGrid[i][j] = (n == 2 || n == 3);
            else newGrid[i][j] = (n == 3);
        }
    grid = newGrid;
}

int main() {
    setupGlider();
    while (true) {
        print();
        update();
        usleep(200000); // 200ms задержка
    }
}
