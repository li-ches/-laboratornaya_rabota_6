package main

import (
	"fmt"
	"math"
)

const epsilon = 1e-6

func main() {
	// Исходные данные для варианта 11
	m := 0.89
	n := 0.08
	p := 1.21

	// Матрица коэффициентов
	A := [][]float64{
		{m, -0.04, 0.21, -18},
		{0.25, -1.23, n, -0.09},
		{-0.21, n, 0.8, -0.13},
		{0.15, -1.31, 0.06, p},
	}

	// Вектор правых частей
	b := []float64{-1.24, p, 2.56, m}

	// Вывод исходной системы
	fmt.Println("Исходная система уравнений:")
	fmt.Printf("%.2fx1 - 0.04x2 + 0.21x3 - 18x4 = -1.24\n", m)
	fmt.Printf("0.25x1 - 1.23x2 + %.2fx3 - 0.09x4 = %.2f\n", n, p)
	fmt.Printf("-0.21x1 + %.2fx2 + 0.8x3 - 0.13x4 = 2.56\n", n)
	fmt.Printf("0.15x1 - 1.31x2 + 0.06x3 + %.2fx4 = %.2f\n\n", p, m)

	// LU-разложение
	L, U := LUDecomposition(A)
	printMatrix(L, "Матрица L")
	printMatrix(U, "Матрица U")

	// Решение системы
	y := forwardSubstitution(L, b)
	x := backwardSubstitution(U, y)

	// Вычисление невязки
	residual := calculateResidual(A, b, x)

	// Вывод результатов
	fmt.Println("Решение системы методом LU-разложения:")
	printResultsTable(x, residual)
}

// LUDecomposition выполняет LU-разложение матрицы
func LUDecomposition(A [][]float64) ([][]float64, [][]float64) {
	n := len(A)
	L := make([][]float64, n)
	U := make([][]float64, n)
	for i := range L {
		L[i] = make([]float64, n)
		U[i] = make([]float64, n)
	}

	for i := 0; i < n; i++ {
		// Верхняя треугольная матрица U
		for k := i; k < n; k++ {
			sum := 0.0
			for j := 0; j < i; j++ {
				sum += L[i][j] * U[j][k]
			}
			U[i][k] = A[i][k] - sum
		}

		// Нижняя треугольная матрица L
		for k := i; k < n; k++ {
			if i == k {
				L[i][i] = 1
			} else {
				sum := 0.0
				for j := 0; j < i; j++ {
					sum += L[k][j] * U[j][i]
				}
				L[k][i] = (A[k][i] - sum) / U[i][i]
			}
		}
	}

	return L, U
}

// forwardSubstitution решает систему Ly = b
func forwardSubstitution(L [][]float64, b []float64) []float64 {
	n := len(L)
	y := make([]float64, n)

	for i := 0; i < n; i++ {
		sum := 0.0
		for j := 0; j < i; j++ {
			sum += L[i][j] * y[j]
		}
		y[i] = (b[i] - sum) / L[i][i]
	}

	return y
}

// backwardSubstitution решает систему Ux = y
func backwardSubstitution(U [][]float64, y []float64) []float64 {
	n := len(U)
	x := make([]float64, n)

	for i := n - 1; i >= 0; i-- {
		sum := 0.0
		for j := i + 1; j < n; j++ {
			sum += U[i][j] * x[j]
		}
		x[i] = (y[i] - sum) / U[i][i]
	}

	return x
}

// calculateResidual вычисляет невязку решения
func calculateResidual(A [][]float64, b []float64, x []float64) []float64 {
	n := len(A)
	residual := make([]float64, n)

	for i := 0; i < n; i++ {
		sum := 0.0
		for j := 0; j < n; j++ {
			sum += A[i][j] * x[j]
		}
		residual[i] = b[i] - sum
	}

	return residual
}

// printMatrix выводит матрицу в читаемом формате
func printMatrix(matrix [][]float64, title string) {
	fmt.Printf("%s:\n", title)
	for _, row := range matrix {
		for _, val := range row {
			fmt.Printf("%8.2f ", val)
		}
		fmt.Println()
	}
	fmt.Println()
}

// printResultsTable выводит результаты в табличном формате
func printResultsTable(x []float64, residual []float64) {
	fmt.Println("+-----+--------+--------+--------+--------+--------+")
	fmt.Println("|  N  |   x1   |   x2   |   x3   |   x4   |   εn   |")
	fmt.Println("+-----+--------+--------+--------+--------+--------+")

	fmt.Printf("| %3d ", 1)
	for j := 0; j < 4; j++ {
		fmt.Printf("| %6.2f ", x[j])
	}

	// Максимальная по модулю невязка
	maxResidual := 0.0
	for _, r := range residual {
		if math.Abs(r) > maxResidual {
			maxResidual = math.Abs(r)
		}
	}
	fmt.Printf("| %6.2f |\n", maxResidual)

	fmt.Println("+-----+--------+--------+--------+--------+--------+")
}
