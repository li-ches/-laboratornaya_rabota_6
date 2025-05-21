package main

import (
	"bufio"
	"fmt"
	"math/rand"
	"os"
	"strconv"
	"time"
)

func main() {
	var N int

	// Создаем буфер для чтения ввода
	reader := bufio.NewReader(os.Stdin)

	fmt.Print("Введите порядок матрицы N: ")
	input, err := reader.ReadString('\n')
	if err != nil {
		fmt.Println("Ошибка чтения ввода:", err)
		return
	}

	input = input[:len(input)-1] // Удаляем символ новой строки

	N, err = strconv.Atoi(input)
	if err != nil {
		fmt.Println("Ошибка преобразования в число:", err)
		return
	}

	if N <= 0 {
		fmt.Println("Порядок матрицы должен быть положительным числом.")
		return
	}

	rand.Seed(time.Now().UnixNano())

	// Создаем матрицу NxN
	matrix := make([][]int, N)
	for i := range matrix {
		matrix[i] = make([]int, N)
	}

	// Заполняем матрицу случайными числами [-100, 100]
	for i := 0; i < N; i++ {
		for j := 0; j < N; j++ {
			matrix[i][j] = -100 + rand.Intn(201) // от -100 до 100 включительно
		}
	}

	// Выводим матрицу
	fmt.Println("Матрица:")
	for _, row := range matrix {
		for _, val := range row {
			fmt.Printf("%d\t", val)
		}
		fmt.Println()
	}

	maxPosCount := -1
	indexOfMaxRow := -1

	// Находим строку с максимальным количеством положительных чисел
	for i, row := range matrix {
		countPos := 0
		for _, val := range row {
			if val > 0 {
				countPos++
			}
		}

        if countPos > maxPosCount {
			maxPosCount = countPos
			indexOfMaxRow = i
        }
    }

    // Обработка случая, если нет положительных чисел
    if indexOfMaxRow == -1 || maxPosCount == 0 {
        fmt.Println("Нет строки с положительными числами.")
        return
    }

    resultRow := matrix[indexOfMaxRow]

    fmt.Printf("Строка с наибольшим количеством положительных чисел (%d):\n", maxPosCount)
    for _, val := range resultRow {
        fmt.Printf("%d ", val)
    }
    fmt.Println()
}
