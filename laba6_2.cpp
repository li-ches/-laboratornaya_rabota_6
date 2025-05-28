#include <iostream>
#include <iomanip>
#include <random>
#include <vector>
#include <array>
#include <string>
#include <limits>

using namespace std;

using Byte = unsigned char;
using Block = array<array<Byte, 4>, 4>;

// Генерация случайного ключа заданной длины (по умолчанию 16 байт)
void generateRandomKey(vector<Byte>& key, size_t length = 16) {
    random_device rd; // источник энтропии
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255); // Один байт может хранить значения от 0 до 255

    key.resize(length);
    for (size_t i = 0; i < length; ++i) {
        key[i] = static_cast<Byte>(dis(gen)); // Заполнение случайными байтами
    }
}

// Генерация случайного вектора-блока инициализации (IV) (уникальность шифрования)
void generateRandomIV(Block& iv) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);
 // Заполнение блока IV 4x4 случайными байтами
    for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 4; ++col)
            iv[row][col] = static_cast<Byte>(dis(gen));
}


// Вывод ключа в шестнадцатеричном формате (hex)
void printKey(const vector<Byte>& key) {
    cout << "Ключ (16 байт): ";
    for (Byte b : key) { //перебор байтов
        cout << hex << setw(2) << setfill('0') //дополнение 0, чтобы было 2 символа
             << static_cast<int>(b) << " ";
    }
    cout << dec << "\n"; // 10-тичный формат (dec)
}

// Вывод блока данных в виде матрицы шестнадцатеричных значений
void printBlock(const Block& block, const string& title = "") {
    if (!title.empty()) cout << title << endl;
    for (const auto& row : block) {
        for (Byte b : row) {
            cout << hex << setw(2) << setfill('0')
                 << static_cast<int>(b) << " ";
        }
        cout << endl;
    }
    cout << dec;
}


// Преобразование текста в блоки по 16 байт с дополнением пробелов (чтобы были кратны 16)
vector<Block> textToBlocks(const string& text) {
    string padded = text;

    while (padded.size() % 16 != 0) {
        padded += ' ';
    }

// Результирующий вектор блоков (разбиение и заполнение)
    vector<Block> blocks;
    for (size_t i = 0; i < padded.size(); i += 16) {
        Block block{};
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                block[row][col] = static_cast<Byte>(padded[i + col * 4 + row]);
            }
        }
        blocks.push_back(block);//добавление блоков в вектор
    }

    return blocks;
}

// Преобразование блоков обратно в текст с удалением дополнения
string blocksToText(const vector<Block>& blocks) {
    string text;
   // Сборка строки из всех блоков
    for (const auto& block : blocks) {
        for (int col = 0; col < 4; ++col)
            for (int row = 0; row < 4; ++row)
                text += static_cast<char>(block[row][col]);
    }
// Удаление дополнения
    if (!text.empty()) {
        unsigned char pad = static_cast<unsigned char>(text.back());
        if (pad > 0 && pad <= 16 && text.size() >= pad) {    // Проверка, что последние байты - это дополнение
            bool valid = true;
            for (int i = 0; i < pad; ++i) {
                if (static_cast<unsigned char>(text[text.size() - 1 - i]) != pad) {
                    valid = false;
                    break;
                }
            }
            if (valid) text.erase(text.size() - pad);
        }
    }

    return text;
}

// =============================================
//          Таблицы для AES (S-box, Rcon)
// =============================================

// Таблица замены (S-box) для AES - главный источник нелинейности
// Каждый байт блока заменяется на соответствующий ему байт из S-box.
const Byte sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// Обратная таблица замены (Inverse S-box) для AES
const Byte invSBox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

// Константы для расширения ключа (добавляют вариативность в раундовые ключи)
const Byte Rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08,
    0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
};

// Расширение ключа для AES (Key Schedule)
vector<Block> expandKey(const vector<Byte>& key) {
    vector<Byte> expandedKey(176); //16*11
    for (int i = 0; i < 16; ++i)
        expandedKey[i] = key[i];//Первые 16 байт — это исходный ключ, который мы копируем в начало массива

    int bytesGenerated = 16;//счетчик уже сгенерированных байтов
    int rconIndex = 1; //счетчик для константы Rcon, которая меняется на каждом этапе
    Byte temp[4];

    while (bytesGenerated < 176) {
        for (int i = 0; i < 4; ++i)
            temp[i] = expandedKey[bytesGenerated - 4 + i]; //генерации новых байтов

 // Каждые 16 байтов выполняем специальные преобразования
// 1) циклический сдвиг влево по кругу
        if (bytesGenerated % 16 == 0) {
            Byte t = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;
// 2) замена байтов через S-box
            for (int i = 0; i < 4; ++i)
                temp[i] = sbox[temp[i]];
// 3) Первый байт temp[0] XORится с текущей константой Rcon.
//  После этого увеличивается индекс Rcon для следующего раунда.

            temp[0] ^= Rcon[rconIndex++];
        }
// Генерация новых четырех байтов через XOR
        for (int i = 0; i < 4; ++i) {
            expandedKey[bytesGenerated] = expandedKey[bytesGenerated - 16] ^ temp[i];
            ++bytesGenerated;
        }
    }
// Делим весь массив из expandedKey на отдельные блоки по 16 байт. Блок = раундовый ключ
    vector<Block> roundKeys;
    for (int i = 0; i < 11; ++i) {
        Block block{};
        for (int j = 0; j < 16; ++j) {
            block[j % 4][j / 4] = expandedKey[i * 16 + j];
        }
        roundKeys.push_back(block);
    }

    return roundKeys;
}

// Принимает два блока (a, b) и возвращает их побитовый XOR. Для AddRoundKey и СВС
Block xorBlocks(const Block& a, const Block& b) {
    Block result{};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            result[i][j] = a[i][j] ^ b[i][j];
    return result;
}

// =============================================
//      Базовые операции AES (шифрование)
// =============================================

//умножение двух байтов в поле Галуа GF(2^8) для mixColumns
Byte gmul(Byte a, Byte b) {
    Byte p = 0;
    Byte hi_bit_set;
    for (int i = 0; i < 8; i++) {
        if (b & 1)
            p ^= a; // добавляем a к результату, если младший бит b равен 1
        hi_bit_set = a & 0x80; // старший бит a
        a <<= 1; // сдвиг a влево
        if (hi_bit_set)
            a ^= 0x1b; // редукция по полиному x^8 + x^4 + x^3 + x + 1
        b >>= 1; // сдвиг b вправо
    }
    return p;
}

// нелинейная замена байтов
void subBytes(Block& state) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            state[i][j] = sbox[state[i][j]];// Замена через обратный S-box
}

// циклический сдвиг строк
void shiftRows(Block& state) {
    Block temp = state;

    for (int i = 1; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            state[i][j] = temp[i][(j + i) % 4];
}

// перемешивание байтов столбцов
void mixColumns(Block& state) {
    for (int col = 0; col < 4; ++col) {
        Byte s0 = state[0][col];
        Byte s1 = state[1][col];
        Byte s2 = state[2][col];
        Byte s3 = state[3][col];
// Каждое новое значение вычисляется как сумма произведений исходных байтов и констант матрицы
        state[0][col] = gmul(s0, 2) ^ gmul(s1, 3) ^ s2 ^ s3;
        state[1][col] = s0 ^ gmul(s1, 2) ^ gmul(s2, 3) ^ s3;
        state[2][col] = s0 ^ s1 ^ gmul(s2, 2) ^ gmul(s3, 3);
        state[3][col] = gmul(s0, 3) ^ s1 ^ s2 ^ gmul(s3, 2);
    }
}
// AddRoundKey (добавление раундового ключа) реализована с помощью xorBlocks
Block encryptBlock(const Block& input, const vector<Block>& roundKeys) {
    cout << "\nНачало шифрования блока:\n";
    printBlock(input, "Исходный блок:");

    Block state = xorBlocks(input, roundKeys[0]);
    printBlock(state, "После AddRoundKey (раунд 0):");

    for (int round = 1; round < 10; ++round) {
        subBytes(state);
        printBlock(state, "После SubBytes (раунд " + to_string(round) + "):");

        shiftRows(state);
        printBlock(state, "После ShiftRows (раунд " + to_string(round) + "):");

        mixColumns(state);
        printBlock(state, "После MixColumns (раунд " + to_string(round) + "):");

        state = xorBlocks(state, roundKeys[round]);
        printBlock(state, "После AddRoundKey (раунд " + to_string(round) + "):");
    }

    subBytes(state);
    printBlock(state, "После SubBytes (раунд 10):");

    shiftRows(state);
    printBlock(state, "После ShiftRows (раунд 10):");

    state = xorBlocks(state, roundKeys[10]);
    printBlock(state, "После AddRoundKey (раунд 10):");

    cout << "Конец шифрования блока\n";
    return state;
}

// =============================================
//      Базовые операции AES (дешифрование)
// =============================================

void invSubBytes(Block& state) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            state[i][j] = invSBox[state[i][j]];
}

void invShiftRows(Block& state) {
    Block temp = state;

    for (int i = 1; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            state[i][j] = temp[i][(j - i + 4) % 4];
}

void invMixColumns(Block& state) {
    for (int col = 0; col < 4; ++col) {
        Byte s0 = state[0][col];
        Byte s1 = state[1][col];
        Byte s2 = state[2][col];
        Byte s3 = state[3][col];

        state[0][col] = gmul(s0, 0x0e) ^ gmul(s1, 0x0b) ^ gmul(s2, 0x0d) ^ gmul(s3, 0x09);
        state[1][col] = gmul(s0, 0x09) ^ gmul(s1, 0x0e) ^ gmul(s2, 0x0b) ^ gmul(s3, 0x0d);
        state[2][col] = gmul(s0, 0x0d) ^ gmul(s1, 0x09) ^ gmul(s2, 0x0e) ^ gmul(s3, 0x0b);
        state[3][col] = gmul(s0, 0x0b) ^ gmul(s1, 0x0d) ^ gmul(s2, 0x09) ^ gmul(s3, 0x0e);
    }
}

Block decryptBlock(const Block& input, const vector<Block>& roundKeys) {
    cout << "\nНачало дешифрования блока:\n";
    printBlock(input, "Зашифрованный блок:");

    Block state = xorBlocks(input, roundKeys[10]);
    printBlock(state, "После AddRoundKey (раунд 10):");

    for (int round = 9; round >= 1; --round) {
        invShiftRows(state);
        printBlock(state, "После InvShiftRows (раунд " + to_string(round) + "):");

        invSubBytes(state);
        printBlock(state, "После InvSubBytes (раунд " + to_string(round) + "):");

        state = xorBlocks(state, roundKeys[round]);
        printBlock(state, "После AddRoundKey (раунд " + to_string(round) + "):");

        invMixColumns(state);
        printBlock(state, "После InvMixColumns (раунд " + to_string(round) + "):");
    }

    invShiftRows(state);
    printBlock(state, "После InvShiftRows (раунд 0):");

    invSubBytes(state);
    printBlock(state, "После InvSubBytes (раунд 0):");

    state = xorBlocks(state, roundKeys[0]);
    printBlock(state, "После AddRoundKey (раунд 0):");

    cout << "Конец дешифрования блока\n";
    return state;
}

// =============================================
//      Режим CBC (Cipher Block Chaining)
// =============================================

vector<Block> AES_CBC_encrypt(const vector<Block>& plaintextBlocks,
                             const vector<Block>& roundKeys,
                             const Block& iv) {
    vector<Block> ciphertextBlocks;
    Block previous = iv;

    for (size_t i = 0; i < plaintextBlocks.size(); ++i) {
        cout << "\nШифрование блока " << i + 1 << " из " << plaintextBlocks.size() << "\n";
        printBlock(previous, "Вектор инициализации (IV) для этого блока:");

        Block xored = xorBlocks(plaintextBlocks[i], previous);
        printBlock(xored, "После XOR с IV:");

        Block encrypted = encryptBlock(xored, roundKeys);
        ciphertextBlocks.push_back(encrypted);
        previous = encrypted;
    }

    return ciphertextBlocks;
}

vector<Block> AES_CBC_decrypt(const vector<Block>& ciphertextBlocks,
                             const vector<Block>& roundKeys,
                             const Block& iv) {
    vector<Block> decryptedBlocks;
    Block previous = iv;

    for (size_t i = 0; i < ciphertextBlocks.size(); ++i) {
        cout << "\nДешифрование блока " << i + 1 << " из " << ciphertextBlocks.size() << "\n";
        printBlock(previous, "Вектор инициализации (IV) для этого блока:");

        Block decrypted = decryptBlock(ciphertextBlocks[i], roundKeys);
        Block plain = xorBlocks(decrypted, previous);
        decryptedBlocks.push_back(plain);
        previous = ciphertextBlocks[i];

        printBlock(plain, "После XOR с IV:");
    }

    return decryptedBlocks;
}

// =============================================
//               Основная программа
// =============================================

int main() {
    // 1. Ввод данных
    string inputText;

    cout << "Введите текст для шифрования: \n";
    getline(cin, inputText);

    // 2. Генерация ключа и IV
    vector<Byte> masterKey;
    Block iv;
    generateRandomKey(masterKey);
    generateRandomIV(iv);

    cout << "\n=============================================\n";
    cout << "Генерация ключей и вектора инициализации\n";
    cout << "=============================================\n";
    printKey(masterKey);
    printBlock(iv, "\nВектор инициализации (IV):");

    // 3. Подготовка данных
    vector<Block> plaintextBlocks = textToBlocks(inputText);

    cout << "\n=============================================\n";
    cout << "Преобразование текста в блоки\n";
    cout << "=============================================\n";
    for (size_t i = 0; i < plaintextBlocks.size(); ++i) {
        printBlock(plaintextBlocks[i], "Блок текста " + to_string(i + 1) + ":");
    }

    // 4. Расширение ключа
    vector<Block> roundKeys = expandKey(masterKey);

    cout << "\n=============================================\n";
    cout << "Раундовые ключи\n";
    cout << "=============================================\n";
    for (size_t i = 0; i < roundKeys.size(); ++i) {
        printBlock(roundKeys[i], "Раундовый ключ " + to_string(i) + ":");
    }

    // 5. Шифрование
    cout << "\n=============================================\n";
    cout << "Процесс шифрования (AES-CBC)\n";
    cout << "=============================================\n";
    vector<Block> ciphertextBlocks = AES_CBC_encrypt(plaintextBlocks, roundKeys, iv);

    // Сохраняем зашифрованные данные в строку (вместо файла)
    string ciphertextStr;
    for (const Block& block : ciphertextBlocks) {
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                ciphertextStr += static_cast<char>(block[row][col]);
            }
        }
    }
    cout << "\nЗашифрованные данные сохранены в памяти\n";

    cout << "\n=============================================\n";
    cout << "Результаты шифрования\n";
    cout << "=============================================\n";
    for (size_t i = 0; i < ciphertextBlocks.size(); ++i) {
        printBlock(ciphertextBlocks[i], "Зашифрованный блок " + to_string(i + 1) + ":");
    }

    // 6. Дешифрование
    cout << "\n=============================================\n";
    cout << "Процесс дешифрования (AES-CBC)\n";
    cout << "=============================================\n";
    vector<Block> decryptedBlocks = AES_CBC_decrypt(ciphertextBlocks, roundKeys, iv);
    string decryptedText = blocksToText(decryptedBlocks);

    // Удаление дополнения
    decryptedText.erase(decryptedText.find_last_not_of(' ') + 1);

    cout << "\n=============================================\n";
    cout << "Результаты дешифрования\n";
    cout << "=============================================\n";
    for (size_t i = 0; i < decryptedBlocks.size(); ++i) {
        printBlock(decryptedBlocks[i], "Дешифрованный блок " + to_string(i + 1) + ":");
    }

    cout << "\nИсходный текст: \n" << inputText << "\n";
    cout << "\nРасшифрованный текст: \n" << decryptedText << "\n";

    // Вывод зашифрованного текста в hex
    cout << "Зашифрованный текст:\n";
    cout << hex << setfill('0');
    for (unsigned char c : ciphertextStr) {
        cout << setw(2) << static_cast<int>(c) << " ";
    }
    cout << dec << "\n";  // Возвращаем вывод в десятичный режим

    return 0;
}
