package main

import (
	"bytes"
	"crypto/rand"
	"fmt"
	"log"
	"os"
)

// Константы AES
const (
	BlockSize = 16 // Размер блока AES (128 бит)
	Nk        = 4  // Количество 32-битных слов в ключе для AES-128
	Nb        = 4  // Количество столбцов в State
	Nr        = 10 // Количество раундов для AES-128
)

// Полная таблица S-box (рисунок 2.2)
var S_box = [256]byte{
	0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
	0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
	0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
	0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
	0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
	0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
	0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
	0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
	0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
	0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
	0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
	0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
	0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
	0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
	0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
	0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16,
}

// Полная таблица обратного S-box (рисунок 2.3)
var Inv_S_box = [256]byte{
	0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
	0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
	0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
	0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
	0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
	0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
	0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
	0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
	0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
	0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
	0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
	0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
	0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
	0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
	0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
	0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D,
}

// Константы для расширения ключа (Rcon)
var Rcon = [11]byte{
	0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36,
}

func main() {
	// 1. Генерация случайного ключа и вектора инициализации
	key := make([]byte, 16)
	if _, err := rand.Read(key); err != nil {
		log.Fatal("Ошибка генерации ключа:", err)
	}

	iv := make([]byte, BlockSize)
	if _, err := rand.Read(iv); err != nil {
		log.Fatal("Ошибка генерации IV:", err)
	}

	// 2. Сохранение ключа в файл
	if err := os.WriteFile("aes_key.bin", key, 0644); err != nil {
		log.Fatal("Ошибка записи ключа:", err)
	}

	// 3. Пример данных для шифрования
	plaintext := "Hello, AES! Привет, AES! 1234"
	fmt.Println("=== AES-128 CBC Mode Implementation ===")
	fmt.Printf("Original Text: %s\n", plaintext)
	fmt.Printf("Original Bytes: %x\n", []byte(plaintext))
	fmt.Printf("\nKey: %x\n", key)
	fmt.Printf("IV: %x\n\n", iv)

	// 4. Шифрование
	fmt.Println("=== ENCRYPTION PROCESS ===")
	ciphertext, err := encryptAES_CBC([]byte(plaintext), key, iv)
	if err != nil {
		log.Fatal("Ошибка шифрования:", err)
	}

	fmt.Printf("\nCiphertext: %x\n", ciphertext)

	// 5. Дешифрование
	fmt.Println("\n=== DECRYPTION PROCESS ===")
	decrypted, err := decryptAES_CBC(ciphertext, key, iv)
	if err != nil {
		log.Fatal("Ошибка дешифрования:", err)
	}

	fmt.Printf("\nDecrypted Text: %s\n", decrypted)
	fmt.Printf("Decrypted Bytes: %x\n", decrypted)

	// 6. Проверка
	if string(decrypted) == plaintext {
		fmt.Println("\nSuccess! Decrypted text matches original.")
	} else {
		fmt.Println("\nError! Decrypted text does not match original.")
	}
}

// Шифрование AES-CBC
func encryptAES_CBC(plaintext, key, iv []byte) ([]byte, error) {
	//  Добавление PKCS#7 padding
	padded := padPKCS7(plaintext, BlockSize)

	//  Расширение ключа
	expandedKey := keyExpansion(key)
	printKeySchedule(expandedKey, "Expanded Key Schedule")
	
	//  Шифрование каждого блока
	ciphertext := make([]byte, len(padded))
	prevBlock := iv
	
	for i := 0; i < len(padded); i += BlockSize {
		// XOR с предыдущим блоком (или IV для первого блока)
		block := make([]byte, BlockSize)
		for j := 0; j < BlockSize; j++ {
			block[j] = padded[i+j] ^ prevBlock[j]
		}
		
		printState(block, fmt.Sprintf("Block %d after XOR", i/BlockSize))
		
		// Шифрование блока
		encrypted := encryptBlock(block, expandedKey)
		copy(ciphertext[i:], encrypted)
		prevBlock = encrypted
	}
	
	return ciphertext, nil
}

// Дешифрование AES-CBC
func decryptAES_CBC(ciphertext, key, iv []byte) ([]byte, error) {
	// 1. Проверка размера данных
	if len(ciphertext)%BlockSize != 0 {
		return nil, fmt.Errorf("неверный размер ciphertext")
	}
	
	// 2. Расширение ключа
	expandedKey := keyExpansion(key)
	printKeySchedule(expandedKey, "Expanded Key Schedule")
	
	// 3. Дешифрование каждого блока
	plaintext := make([]byte, len(ciphertext))
	prevBlock := iv
	
	for i := 0; i < len(ciphertext); i += BlockSize {
		// Дешифрование блока
		block := ciphertext[i : i+BlockSize]
		decrypted := decryptBlock(block, expandedKey)
		
		// XOR с предыдущим зашифрованным блоком (или IV)
		for j := 0; j < BlockSize; j++ {
			plaintext[i+j] = decrypted[j] ^ prevBlock[j]
		}
		
		printState(plaintext[i:i+BlockSize], fmt.Sprintf("Block %d after XOR", i/BlockSize))
		prevBlock = block
	}
	
	// 4. Удаление padding
	return unpadPKCS7(plaintext), nil
}

// Шифрование одного блока
func encryptBlock(block, expandedKey []byte) []byte {
	state := make([]byte, BlockSize)
	copy(state, block)
	
	printState(state, "Initial State")
	
	// Начальный раунд
	addRoundKey(state, expandedKey[0:BlockSize])
	printState(state, "After AddRoundKey (Round 0)")
	printRoundKey(expandedKey[0:BlockSize], 0, true)
	
	// Основные раунды
	for round := 1; round < Nr; round++ {
		subBytes(state)
		printState(state, fmt.Sprintf("After SubBytes (Round %d)", round))
		
		shiftRows(state)
		printState(state, fmt.Sprintf("After ShiftRows (Round %d)", round))
		
		mixColumns(state)
		printState(state, fmt.Sprintf("After MixColumns (Round %d)", round))
		
		addRoundKey(state, expandedKey[round*BlockSize:(round+1)*BlockSize])
		printState(state, fmt.Sprintf("After AddRoundKey (Round %d)", round))
		printRoundKey(expandedKey[round*BlockSize:(round+1)*BlockSize], round, true)
	}
	
	// Финальный раунд (без MixColumns)
	subBytes(state)
	printState(state, fmt.Sprintf("After SubBytes (Round %d)", Nr))
	
	shiftRows(state)
	printState(state, fmt.Sprintf("After ShiftRows (Round %d)", Nr))
	
	addRoundKey(state, expandedKey[Nr*BlockSize:(Nr+1)*BlockSize])
	printState(state, fmt.Sprintf("After AddRoundKey (Round %d)", Nr))
	printRoundKey(expandedKey[Nr*BlockSize:(Nr+1)*BlockSize], Nr, true)
	
	return state
}

// Дешифрование одного блока
func decryptBlock(block, expandedKey []byte) []byte {
	state := make([]byte, BlockSize)
	copy(state, block)
	
	printState(state, "Initial State (Encrypted)")
	
	// Начальный раунд
	addRoundKey(state, expandedKey[Nr*BlockSize:(Nr+1)*BlockSize])
	printState(state, fmt.Sprintf("After AddRoundKey (Round %d)", Nr))
	printRoundKey(expandedKey[Nr*BlockSize:(Nr+1)*BlockSize], Nr, false)
	
	invShiftRows(state)
	printState(state, fmt.Sprintf("After InvShiftRows (Round %d)", Nr))
	
	invSubBytes(state)
	printState(state, fmt.Sprintf("After InvSubBytes (Round %d)", Nr))
	
	// Основные раунды
	for round := Nr - 1; round >= 1; round-- {
		addRoundKey(state, expandedKey[round*BlockSize:(round+1)*BlockSize])
		printState(state, fmt.Sprintf("After AddRoundKey (Round %d)", round))
		printRoundKey(expandedKey[round*BlockSize:(round+1)*BlockSize], round, false)
		
		invMixColumns(state)
		printState(state, fmt.Sprintf("After InvMixColumns (Round %d)", round))
		
		invShiftRows(state)
		printState(state, fmt.Sprintf("After InvShiftRows (Round %d)", round))
		
		invSubBytes(state)
		printState(state, fmt.Sprintf("After InvSubBytes (Round %d)", round))
	}
	
	// Финальный раунд
	addRoundKey(state, expandedKey[0:BlockSize])
	printState(state, "After AddRoundKey (Round 0)")
	printRoundKey(expandedKey[0:BlockSize], 0, false)
	
	return state
}

// Key Expansion
func keyExpansion(key []byte) []byte {
	expandedKey := make([]byte, BlockSize*(Nr+1))
	copy(expandedKey, key)
	
	for i := Nk; i < Nb*(Nr+1); i++ {
		temp := make([]byte, 4)
		copy(temp, expandedKey[(i-1)*4:i*4])
		
		if i%Nk == 0 {
			// RotWord
			temp = append(temp[1:], temp[0])
			
			// SubWord
			for j := 0; j < 4; j++ {
				temp[j] = S_box[temp[j]]
			}
			
			// XOR с Rcon
			temp[0] ^= Rcon[i/Nk]
		}
		
		// XOR с предыдущим словом
		for j := 0; j < 4; j++ {
			expandedKey[i*4+j] = expandedKey[(i-Nk)*4+j] ^ temp[j]
		}
	}
	
	return expandedKey
}

// Добавление PKCS#7 padding
func padPKCS7(data []byte, blockSize int) []byte {
	padding := blockSize - len(data)%blockSize
	padText := bytes.Repeat([]byte{byte(padding)}, padding)
	return append(data, padText...)
}

// Удаление PKCS#7 padding
func unpadPKCS7(data []byte) []byte {
	if len(data) == 0 {
		return data
	}
	padding := int(data[len(data)-1])
	if padding > len(data) {
		return data
	}
	return data[:len(data)-padding]
}

// Преобразования AES
func subBytes(state []byte) {
	for i := 0; i < BlockSize; i++ {
		state[i] = S_box[state[i]]
	}
}

func invSubBytes(state []byte) {
	for i := 0; i < BlockSize; i++ {
		state[i] = Inv_S_box[state[i]]
	}
}

func shiftRows(state []byte) {
	// Строка 0 - без изменений
	
	// Строка 1 - циклический сдвиг на 1 байт влево
	temp := state[1]
	state[1] = state[5]
	state[5] = state[9]
	state[9] = state[13]
	state[13] = temp
	
	// Строка 2 - циклический сдвиг на 2 байта влево
	state[2], state[10] = state[10], state[2]
	state[6], state[14] = state[14], state[6]
	
	// Строка 3 - циклический сдвиг на 3 байта влево (или 1 вправо)
	temp = state[15]
	state[15] = state[11]
	state[11] = state[7]
	state[7] = state[3]
	state[3] = temp
}

func invShiftRows(state []byte) {
	// Строка 0 - без изменений
	
	// Строка 1 - циклический сдвиг на 1 байт вправо
	temp := state[13]
	state[13] = state[9]
	state[9] = state[5]
	state[5] = state[1]
	state[1] = temp
	
	// Строка 2 - циклический сдвиг на 2 байта влево (как в shiftRows)
	state[2], state[10] = state[10], state[2]
	state[6], state[14] = state[14], state[6]
	
	// Строка 3 - циклический сдвиг на 3 байта вправо (или 1 влево)
	temp = state[3]
	state[3] = state[7]
	state[7] = state[11]
	state[11] = state[15]
	state[15] = temp
}

func mixColumns(state []byte) {
	for i := 0; i < 4; i++ {
		s0 := state[i*4]
		s1 := state[i*4+1]
		s2 := state[i*4+2]
		s3 := state[i*4+3]
		
		state[i*4] = gmul(0x02, s0) ^ gmul(0x03, s1) ^ s2 ^ s3
		state[i*4+1] = s0 ^ gmul(0x02, s1) ^ gmul(0x03, s2) ^ s3
		state[i*4+2] = s0 ^ s1 ^ gmul(0x02, s2) ^ gmul(0x03, s3)
		state[i*4+3] = gmul(0x03, s0) ^ s1 ^ s2 ^ gmul(0x02, s3)
	}
}

func invMixColumns(state []byte) {
	for i := 0; i < 4; i++ {
		s0 := state[i*4]
		s1 := state[i*4+1]
		s2 := state[i*4+2]
		s3 := state[i*4+3]
		
		state[i*4] = gmul(0x0e, s0) ^ gmul(0x0b, s1) ^ gmul(0x0d, s2) ^ gmul(0x09, s3)
		state[i*4+1] = gmul(0x09, s0) ^ gmul(0x0e, s1) ^ gmul(0x0b, s2) ^ gmul(0x0d, s3)
		state[i*4+2] = gmul(0x0d, s0) ^ gmul(0x09, s1) ^ gmul(0x0e, s2) ^ gmul(0x0b, s3)
		state[i*4+3] = gmul(0x0b, s0) ^ gmul(0x0d, s1) ^ gmul(0x09, s2) ^ gmul(0x0e, s3)
	}
}

func addRoundKey(state, roundKey []byte) {
	for i := 0; i < BlockSize; i++ {
		state[i] ^= roundKey[i]
	}
}

// Умножение в поле GF(2^8)
func gmul(a, b byte) byte {
	var p byte = 0
	for i := 0; i < 8; i++ {
		if (b & 1) != 0 {
			p ^= a
		}
		
		hiBitSet := (a & 0x80) != 0
		a <<= 1
		if hiBitSet {
			a ^= 0x1b // x^8 + x^4 + x^3 + x + 1
		}
		b >>= 1
	}
	return p
}

// Вспомогательные функции для вывода
func printState(state []byte, title string) {
	fmt.Printf("%s:\n", title)
	for row := 0; row < 4; row++ {
		for col := 0; col < 4; col++ {
			fmt.Printf("%02x ", state[row+4*col])
		}
		fmt.Println()
	}
	fmt.Println()
}

func printRoundKey(roundKey []byte, round int, isEncrypt bool) {
	mode := "Encryption"
	if !isEncrypt {
		mode = "Decryption"
	}
	fmt.Printf("Round Key %d (%s):\n", round, mode)
	for i := 0; i < BlockSize; i++ {
		fmt.Printf("%02x ", roundKey[i])
		if (i+1)%4 == 0 {
			fmt.Println()
		}
	}
	fmt.Println()
}

func printKeySchedule(expandedKey []byte, title string) {
	fmt.Printf("%s:\n", title)
	for i := 0; i < len(expandedKey); i++ {
		fmt.Printf("%02x ", expandedKey[i])
		if (i+1)%16 == 0 {
			fmt.Println()
		}
	}
	fmt.Println()
}
