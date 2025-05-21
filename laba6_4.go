package main

import (
	"fmt"
	"os"
)

func main() {
	// Вывод подсказки для ввода K
	fmt.Println("Введите количество целевых клеток K:")
	var K int
	_, err := fmt.Scan(&K)
	if err != nil {
		fmt.Println("Ошибка ввода: введены некорректные данные")
		os.Exit(1)
	}

	// Проверка корректности K
	if K <= 0 {
		fmt.Println("Ошибка: K должно быть положительным числом")
		os.Exit(1)
	}
	if K > 10000 {
		fmt.Println("Ошибка: K не может превышать 10000")
		os.Exit(1)
	}

	// Вывод подсказки для ввода клеток
	fmt.Println("Введите клетки через пробел в порядке возрастания:")
	cells := make([]int, K)
	for i := 0; i < K; i++ {
		_, err := fmt.Scan(&cells[i])
		if err != nil {
			fmt.Println("Ошибка ввода: введены некорректные данные")
			os.Exit(1)
		}

		// Проверка корректности клеток
		if cells[i] < 1 {
			fmt.Println("Ошибка: номер клетки не может быть меньше 1")
			os.Exit(1)
		}
		if cells[i] > 10000 {
			fmt.Println("Ошибка: номер клетки не может превышать 10000")
			os.Exit(1)
		}

		// Проверка возрастания
		if i > 0 && cells[i] <= cells[i-1] {
			fmt.Println("Ошибка: номера клеток должны быть строго возрастающими")
			os.Exit(1)
		}
	}

	position := 1  // Начинаем с головы удава (клетка 1)
	speed := 0      // Начальная скорость
	moves := 0      // Счетчик ходов

	for _, target := range cells {
		for position < target {
			// Вычисляем минимальную скорость для достижения цели
			requiredSpeed := target - position

			// Оптимизируем скорость
			if speed < requiredSpeed {
				speed++
			} else if speed > requiredSpeed {
				speed = max(speed-1, 1)
			}

			// Перемещаем фишку
			position += speed
			moves++
		}
	}

	// Вывод результата
	fmt.Printf("Минимальное количество ходов: %d\n", moves)
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}
