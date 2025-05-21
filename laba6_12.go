package main

import (
    "bufio"
    "fmt"
    "math/rand"
    "os"
    "strconv"
    "strings"
    "time"
)

func main() {
    var M, N int

    // Создаем буфер для чтения ввода
    reader := bufio.NewReader(os.Stdin)

    // Запрос размеров матрицы
    fmt.Print("Введите размеры матрицы M и N: ")
    inputLine, err := reader.ReadString('\n')
    if err != nil {
        fmt.Println("Ошибка чтения ввода:", err)
        return
    }

    // Разделяем ввод по пробелам
    fields := strings.Fields(inputLine)
    if len(fields) < 2 {
        fmt.Println("Пожалуйста, введите два числа.")
        return
    }

    // Преобразуем введенные строки в целые числа
    M, err = strconv.Atoi(fields[0])
    if err != nil {
        fmt.Println("Ошибка преобразования M:", err)
        return
    }
    N, err = strconv.Atoi(fields[1])
    if err != nil {
        fmt.Println("Ошибка преобразования N:", err)
        return
    }

    // Проверка положительности размеров матрицы
    if M <= 0 || N <= 0 {
        fmt.Println("Размеры матрицы должны быть положительными числами.")
        return
    }

    // Инициализация генератора случайных чисел
    rand.Seed(time.Now().UnixNano())

    // Создание матрицы M x N
    matrix := make([][]int, M)
    for i := range matrix {
        matrix[i] = make([]int, N)
    }

    // Заполнение матрицы случайными числами в диапазоне [100, 150]
    for i := 0; i < M; i++ {
        for j := 0; j < N; j++ {
            matrix[i][j] = 100 + rand.Intn(51) // [100..150]
        }
    }

    // Вывод матрицы
    fmt.Println("Матрица:")
    for _, row := range matrix {
        for _, val := range row {
            fmt.Printf("%d\t", val)
        }
        fmt.Println()
    }

    // Подсчет частоты каждого числа с помощью map
    frequency := make(map[int]int)

    for _, row := range matrix {
        for _, val := range row {
            frequency[val]++
            // Используйте frequency[val]++, а не ++frequency[val]
        }
    }

    // Поиск числа с максимальной частотой
   maxCount := 0
   numberWithMaxCount := 0

   for num, count := range frequency {
       if count > maxCount {
           maxCount = count
           numberWithMaxCount = num
       }
   }

   // Вывод результата: если все числа уникальны (частота равна 1), сообщить об этом
   if maxCount == 1 {
       fmt.Println("Нет числа, которое встречается более одного раза.")
   } else {
       fmt.Printf("Число, встречающееся наибольшее количество раз: %d (%d раз)\n", numberWithMaxCount, maxCount)
   }
}
