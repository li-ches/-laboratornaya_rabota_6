package main

import (
	"fmt"
	"math/rand"
	"time"
)

// Размеры игрового поля
const (
	WIDTH  = 40
	HEIGHT = 20
)

// Grid — тип для хранения сетки (двумерный срез)
type Grid [][]int

// initializeRandom заполняет сетку случайными значениями с заданной плотностью живых клеток
func initializeRandom(grid Grid, density float64) {
	for i := 0; i < HEIGHT; i++ {
		for j := 0; j < WIDTH; j++ {
			if rand.Float64() < density {
				grid[i][j] = 1 // Живая клетка
			} else {
				grid[i][j] = 0 // Мертвая клетка
			}
		}
	}
}

// printGrid выводит текущее состояние сетки на экран
func printGrid(grid Grid) {
	fmt.Print("\033[H\033[2J") // ANSI-коды для очистки экрана и возврата курсора в начало
	for _, row := range grid {
		for _, cell := range row {
			if cell == 1 {
				fmt.Print("O")
			} else {
				fmt.Print(" ")
			}
		}
		fmt.Println()
	}
}

// countNeighbors подсчитывает количество живых соседей вокруг клетки (тороидальная топология)
func countNeighbors(grid Grid, x, y int) int {
	count := 0
	for dx := -1; dx <= 1; dx++ {
		for dy := -1; dy <= 1; dy++ {
			if dx == 0 && dy == 0 {
				continue // пропускаем саму клетку
			}
			nx := (x + dx + HEIGHT) % HEIGHT // оборачиваем по вертикали
			ny := (y + dy + WIDTH) % WIDTH   // оборачиваем по горизонтали
			count += grid[nx][ny]
		}
	}
	return count
}

// updateGrid обновляет состояние сетки по правилам "Жизни"
func updateGrid(grid Grid) {
	newGrid := make(Grid, HEIGHT)
	for i := range newGrid {
		newGrid[i] = make([]int, WIDTH)
	}

	for i := 0; i < HEIGHT; i++ {
		for j := 0; j < WIDTH; j++ {
			nNeighbors := countNeighbors(grid, i, j)
			if grid[i][j] == 1 { // Живая клетка
				if nNeighbors < 2 || nNeighbors > 3 {
					newGrid[i][j] = 0 // Умирает от недостатка или избытка соседей
				} else {
					newGrid[i][j] = 1 // Остается живой
				}
			} else { // Мертвая клетка
				if nNeighbors == 3 {
					newGrid[i][j] = 1 // Воскресает при наличии трех соседей
				} else {
					newGrid[i][j] = 0 // Остается мертвой
				}
			}
		}
	}

	copy(grid, newGrid) // копируем обновленное состояние обратно в исходную сетку
}

func main() {
	rand.Seed(time.Now().UnixNano())

	grid := make(Grid, HEIGHT)
	for i := range grid {
		grid[i] = make([]int, WIDTH)
	}

	var density float64

	fmt.Print("Введите плотность начальной жизни (от 0 до 1): ")
	fmt.Scanf("%f", &density)
	if density < 0 || density > 1 {
		fmt.Println("Некорректный ввод. Используем значение по умолчанию: 0.2")
        density = 0.2
    }

	initializeRandom(grid, density)

	for {
		printGrid(grid)

        updateGrid(grid)

        time.Sleep(200 * time.Millisecond)
    }
}
