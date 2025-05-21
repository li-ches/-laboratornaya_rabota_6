package main

import (
	"fmt"
	"math/rand"
	"time"
)

// Функция для проверки, является ли доска шахматной
func isChessboard(board [][]int, pattern int) bool {
	for i := 0; i < len(board); i++ {
		for j := 0; j < len(board[0]); j++ {
			expected := (i + j + pattern) % 2
			if board[i][j] != expected {
				return false
			}
		}
	}
	return true
}

// Функция для переворота монеты
func flipCoin(board [][]int, i, j int) {
	if i >= 0 && i < len(board) && j >= 0 && j < len(board[0]) {
		board[i][j] ^= 1 // Переворачиваем монету
	}
}

// Функция для подсчета минимального количества ходов
func minFlips(board [][]int) int {
	n := len(board)
	if n == 0 {
		return 0
	}
	m := len(board[0])

	minFlips := 1<<31 - 1 // Аналог INT_MAX в C++

	for pattern := 0; pattern < 2; pattern++ {
		// Создаем копию доски
		temp := make([][]int, n)
		for i := range board {
			temp[i] = make([]int, m)
			copy(temp[i], board[i])
		}

		flips := 0

		// Пытаемся преобразовать доску в текущий паттерн
		for i := 0; i < n; i++ {
			for j := 0; j < m; j++ {
				expected := (i + j + pattern) % 2
				if temp[i][j] != expected {
					// Переворачиваем текущую монету и соседнюю (правую или нижнюю)
					flipCoin(temp, i, j)
					if j+1 < m {
						flipCoin(temp, i, j+1)
					} else if i+1 < n {
						flipCoin(temp, i+1, j)
					} else {
						// Невозможно сделать ход - паттерн недостижим
						flips = 1<<31 - 1
						break
					}
					flips++
				}
			}
			if flips == 1<<31-1 {
				break
			}
		}

		if flips < minFlips {
			minFlips = flips
		}
	}

	if minFlips == 1<<31-1 {
		return -1
	}
	return minFlips
}

func main() {
	// Инициализация генератора случайных чисел
	rand.Seed(time.Now().UnixNano())

	// Размеры доски
	var n, m int
	fmt.Print("Введите размеры доски N и M: ")
	fmt.Scan(&n, &m)

	// Создание и заполнение доски случайными значениями
	board := make([][]int, n)
	for i := 0; i < n; i++ {
		board[i] = make([]int, m)
		for j := 0; j < m; j++ {
			board[i][j] = rand.Intn(2)
		}
	}

	// Вывод исходной доски
	fmt.Println("Исходная доска:")
	for _, row := range board {
		for _, coin := range row {
			fmt.Printf("%d ", coin)
		}
		fmt.Println()
	}

	// Подсчет минимального количества ходов
	result := minFlips(board)

	if result == -1 {
		fmt.Println("Невозможно преобразовать доску в шахматный узор")
	} else {
		fmt.Printf("Минимальное количество ходов: %d\n", result)
	}
}
