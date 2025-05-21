#include <iostream>
#include <vector>
#include <iomanip>
#include <random>
#include <fstream>
#include <algorithm>
#include <stdexcept>

using namespace std;

// Константы AES
const int BLOCK_SIZE = 16;    // Размер блока (State) в байтах (4x4)
const int KEY_SIZE = 16;      // Размер ключа (Nk = 4 для AES-128)
const int ROUNDS = 10;       // Количество раундов (Nr)
const int Nk = 4;            // Число 32-битных слов в ключе
const int Nb = 4;            // Число столбцов в State
const int Nr = 10;           // Количество раундов

// S-box (рисунок 2.2)
const unsigned char S_box[256] = {
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
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

// Обратный S-box (рисунок 2.3)
const unsigned char Inv_S_box[256] = {
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
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

// Константы для Key Expansion (Rcon)
const unsigned char Rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
};

// Вспомогательные функции
void printState(const vector<unsigned char>& state, const string& title) {
    cout << title << " (4x4 State):" << endl;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            cout << hex << setw(2) << setfill('0')
                 << (int)state[row + 4 * col] << " ";
        }
        cout << endl;
    }
    cout << dec << endl;
}

void printRoundKey(const vector<unsigned char>& key, int round, bool isEncrypt) {
    cout << "Round Key " << round << " ("
         << (isEncrypt ? "Encryption" : "Decryption") << "):" << endl;
    for (int i = 0; i < 16; ++i) {
        cout << hex << setw(2) << setfill('0') << (int)key[i] << " ";
        if ((i + 1) % 4 == 0) cout << endl;
    }
    cout << dec << endl;
}

void printKeySchedule(const vector<unsigned char>& RoundKeys, const string& title) {
    cout << title << ":" << endl;
    for (size_t i = 0; i < RoundKeys.size(); ++i) {
        cout << hex << setw(2) << setfill('0') << (int)RoundKeys[i] << " ";
        if ((i + 1) % 16 == 0) cout << endl;
    }
    cout << dec << endl;
}

vector<unsigned char> generateRandomBytes(size_t count) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);

    vector<unsigned char> result(count);
    for (size_t i = 0; i < count; ++i) {
        result[i] = static_cast<unsigned char>(dis(gen));
    }
    return result;
}

void writeKeyToFile(const vector<unsigned char>& key, const string& filename) {
    ofstream file(filename, ios::binary);
    if (!file) {
        throw runtime_error("Cannot open file for writing key");
    }
    file.write(reinterpret_cast<const char*>(key.data()), key.size());
}

vector<unsigned char> readKeyFromFile(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file) {
        throw runtime_error("Cannot open file for reading key");
    }

    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    vector<unsigned char> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw runtime_error("Failed to read key from file");
    }

    return buffer;
}

// Key Expansion (генерация Round Keys из Cipher Key)
vector<unsigned char> KeyExpansion(const vector<unsigned char>& CipherKey) {
    vector<unsigned char> RoundKeys(176); // 16 байт * 11 раундов

    // Первые 16 байт - это исходный Cipher Key
    copy(CipherKey.begin(), CipherKey.end(), RoundKeys.begin());

    // Генерация оставшихся Round Keys
    for (int i = Nk; i < Nb * (Nr + 1); ++i) {
        vector<unsigned char> temp(4);
        copy(RoundKeys.begin() + (i-1)*4, RoundKeys.begin() + i*4, temp.begin());

        if (i % Nk == 0) {
            // RotWord
            rotate(temp.begin(), temp.begin()+1, temp.end());

            // SubWord (применение S-box к каждому байту)
            for (auto& byte : temp) byte = S_box[byte];

            // XOR с Rcon[i/Nk]
            temp[0] ^= Rcon[i/Nk];
        }

        // XOR с RoundKeys[i-Nk]
        for (int j = 0; j < 4; ++j) {
            RoundKeys[i*4 + j] = RoundKeys[(i-Nk)*4 + j] ^ temp[j];
        }
    }

    return RoundKeys;
}

vector<unsigned char> GetRoundKey(const vector<unsigned char>& RoundKeys, int round) {
    vector<unsigned char> roundKey(16);
    copy(RoundKeys.begin() + round*16, RoundKeys.begin() + (round+1)*16, roundKey.begin());
    return roundKey;
}

// Основные преобразования AES
void SubBytes(vector<unsigned char>& state) {
    for (auto& byte : state) byte = S_box[byte];
}

void InvSubBytes(vector<unsigned char>& state) {
    for (auto& byte : state) byte = Inv_S_box[byte];
}

void ShiftRows(vector<unsigned char>& state) {
    // Строка 0 - нет сдвига

    // Строка 1 - циклический сдвиг на 1 влево
    unsigned char temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;

    // Строка 2 - циклический сдвиг на 2 влево
    swap(state[2], state[10]);
    swap(state[6], state[14]);

    // Строка 3 - циклический сдвиг на 3 влево (или 1 вправо)
    temp = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7] = state[3];
    state[3] = temp;
}

void InvShiftRows(vector<unsigned char>& state) {
    // Строка 0 - нет сдвига

    // Строка 1 - циклический сдвиг на 1 вправо
    unsigned char temp = state[13];
    state[13] = state[9];
    state[9] = state[5];
    state[5] = state[1];
    state[1] = temp;

    // Строка 2 - циклический сдвиг на 2 влево (как в ShiftRows)
    swap(state[2], state[10]);
    swap(state[6], state[14]);

    // Строка 3 - циклический сдвиг на 3 вправо (или 1 влево)
    temp = state[3];
    state[3] = state[7];
    state[7] = state[11];
    state[11] = state[15];
    state[15] = temp;
}

unsigned char gfMultiply(unsigned char a, unsigned char b) {
    unsigned char result = 0;
    unsigned char hi_bit_set;

    for (int i = 0; i < 8; i++) {
        if (b & 1) {
            result ^= a;
        }

        hi_bit_set = a & 0x80;
        a <<= 1;
        if (hi_bit_set) {
            a ^= 0x1b; // x^8 + x^4 + x^3 + x + 1
        }
        b >>= 1;
    }

    return result;
}

void MixColumns(vector<unsigned char>& state) {
    unsigned char temp[4];

    for (int i = 0; i < 4; ++i) {
        temp[0] = gfMultiply(0x02, state[4*i]) ^ gfMultiply(0x03, state[4*i+1]) ^ state[4*i+2] ^ state[4*i+3];
        temp[1] = state[4*i] ^ gfMultiply(0x02, state[4*i+1]) ^ gfMultiply(0x03, state[4*i+2]) ^ state[4*i+3];
        temp[2] = state[4*i] ^ state[4*i+1] ^ gfMultiply(0x02, state[4*i+2]) ^ gfMultiply(0x03, state[4*i+3]);
        temp[3] = gfMultiply(0x03, state[4*i]) ^ state[4*i+1] ^ state[4*i+2] ^ gfMultiply(0x02, state[4*i+3]);

        state[4*i] = temp[0];
        state[4*i+1] = temp[1];
        state[4*i+2] = temp[2];
        state[4*i+3] = temp[3];
    }
}

void InvMixColumns(vector<unsigned char>& state) {
    unsigned char temp[4];

    for (int i = 0; i < 4; ++i) {
        temp[0] = gfMultiply(0x0e, state[4*i]) ^ gfMultiply(0x0b, state[4*i+1]) ^ gfMultiply(0x0d, state[4*i+2]) ^ gfMultiply(0x09, state[4*i+3]);
        temp[1] = gfMultiply(0x09, state[4*i]) ^ gfMultiply(0x0e, state[4*i+1]) ^ gfMultiply(0x0b, state[4*i+2]) ^ gfMultiply(0x0d, state[4*i+3]);
        temp[2] = gfMultiply(0x0d, state[4*i]) ^ gfMultiply(0x09, state[4*i+1]) ^ gfMultiply(0x0e, state[4*i+2]) ^ gfMultiply(0x0b, state[4*i+3]);
        temp[3] = gfMultiply(0x0b, state[4*i]) ^ gfMultiply(0x0d, state[4*i+1]) ^ gfMultiply(0x09, state[4*i+2]) ^ gfMultiply(0x0e, state[4*i+3]);

        state[4*i] = temp[0];
        state[4*i+1] = temp[1];
        state[4*i+2] = temp[2];
        state[4*i+3] = temp[3];
    }
}

void AddRoundKey(vector<unsigned char>& state, const vector<unsigned char>& roundKey) {
    for (int i = 0; i < 16; ++i) {
        state[i] ^= roundKey[i];
    }
}

// Шифрование одного блока
vector<unsigned char> AES_Encrypt_Block(vector<unsigned char> state,
                                      const vector<unsigned char>& RoundKeys) {
    printState(state, "Initial State");

    // Начальный раунд
    AddRoundKey(state, GetRoundKey(RoundKeys, 0));
    printState(state, "After AddRoundKey (Round 0)");
    printRoundKey(GetRoundKey(RoundKeys, 0), 0, true);

    // Основные раунды
    for (int round = 1; round < Nr; ++round) {
        SubBytes(state);
        printState(state, "After SubBytes (Round " + to_string(round) + ")");

        ShiftRows(state);
        printState(state, "After ShiftRows (Round " + to_string(round) + ")");

        MixColumns(state);
        printState(state, "After MixColumns (Round " + to_string(round) + ")");

        AddRoundKey(state, GetRoundKey(RoundKeys, round));
        printState(state, "After AddRoundKey (Round " + to_string(round) + ")");
        printRoundKey(GetRoundKey(RoundKeys, round), round, true);
    }

    // Финальный раунд (без MixColumns)
    SubBytes(state);
    printState(state, "After SubBytes (Round " + to_string(Nr) + ")");

    ShiftRows(state);
    printState(state, "After ShiftRows (Round " + to_string(Nr) + ")");

    AddRoundKey(state, GetRoundKey(RoundKeys, Nr));
    printState(state, "After AddRoundKey (Round " + to_string(Nr) + ")");
    printRoundKey(GetRoundKey(RoundKeys, Nr), Nr, true);

    return state;
}

// Дешифрование одного блока
vector<unsigned char> AES_Decrypt_Block(vector<unsigned char> state,
                                      const vector<unsigned char>& RoundKeys) {
    printState(state, "Initial State (Encrypted)");

    // Начальный раунд
    AddRoundKey(state, GetRoundKey(RoundKeys, Nr));
    printState(state, "After AddRoundKey (Round " + to_string(Nr) + ")");
    printRoundKey(GetRoundKey(RoundKeys, Nr), Nr, false);

    InvShiftRows(state);
    printState(state, "After InvShiftRows (Round " + to_string(Nr) + ")");

    InvSubBytes(state);
    printState(state, "After InvSubBytes (Round " + to_string(Nr) + ")");

    // Основные раунды
    for (int round = Nr-1; round >= 1; --round) {
        AddRoundKey(state, GetRoundKey(RoundKeys, round));
        printState(state, "After AddRoundKey (Round " + to_string(round) + ")");
        printRoundKey(GetRoundKey(RoundKeys, round), round, false);

        InvMixColumns(state);
        printState(state, "After InvMixColumns (Round " + to_string(round) + ")");

        InvShiftRows(state);
        printState(state, "After InvShiftRows (Round " + to_string(round) + ")");

        InvSubBytes(state);
        printState(state, "After InvSubBytes (Round " + to_string(round) + ")");
    }

    // Финальный раунд
    AddRoundKey(state, GetRoundKey(RoundKeys, 0));
    printState(state, "After AddRoundKey (Round 0)");
    printRoundKey(GetRoundKey(RoundKeys, 0), 0, false);

    return state;
}

vector<unsigned char> xorBlocks(const vector<unsigned char>& a, const vector<unsigned char>& b) {
    vector<unsigned char> result(BLOCK_SIZE);
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        result[i] = a[i] ^ b[i];
    }
    return result;
}

vector<unsigned char> padData(const vector<unsigned char>& data) {
    size_t padLen = BLOCK_SIZE - (data.size() % BLOCK_SIZE);
    vector<unsigned char> padded = data;
    padded.insert(padded.end(), padLen, static_cast<unsigned char>(padLen));
    return padded;
}

vector<unsigned char> unpadData(const vector<unsigned char>& data) {
    if (data.empty()) return data;

    unsigned char padLen = data.back();
    if (padLen > BLOCK_SIZE) return data; // Неверное заполнение

    for (size_t i = data.size() - padLen; i < data.size(); ++i) {
        if (data[i] != padLen) return data; // Неверное заполнение
    }

    vector<unsigned char> unpadded(data.begin(), data.end() - padLen);
    return unpadded;
}

// CBC режим - шифрование
vector<unsigned char> AES_CBC_Encrypt(const vector<unsigned char>& plaintext,
                                    const vector<unsigned char>& key,
                                    const vector<unsigned char>& iv) {
    // 1. Key Expansion
    vector<unsigned char> RoundKeys = KeyExpansion(key);
    printKeySchedule(RoundKeys, "Expanded Key Schedule");

    // 2. Добавление PKCS#7 padding
    vector<unsigned char> padded = padData(plaintext);

    // 3. Шифрование каждого блока
    vector<unsigned char> ciphertext;
    vector<unsigned char> prevBlock = iv;

    for (size_t i = 0; i < padded.size(); i += BLOCK_SIZE) {
        vector<unsigned char> block(padded.begin()+i, padded.begin()+i+BLOCK_SIZE);

        cout << "\nEncrypting Block " << (i / BLOCK_SIZE) + 1 << ":" << endl;
        cout << "-----------------" << endl;

        // XOR с предыдущим зашифрованным блоком (или IV для первого блока)
        block = xorBlocks(block, prevBlock);
        printState(block, "After XOR with previous block/IV");

        // Шифрование блока
        vector<unsigned char> encryptedBlock = AES_Encrypt_Block(block, RoundKeys);

        // Сохраняем зашифрованный блок как предыдущий для следующей итерации
        prevBlock = encryptedBlock;

        // Добавляем к результату
        ciphertext.insert(ciphertext.end(), encryptedBlock.begin(), encryptedBlock.end());
    }

    return ciphertext;
}

// CBC режим - дешифрование
vector<unsigned char> AES_CBC_Decrypt(const vector<unsigned char>& ciphertext,
                                    const vector<unsigned char>& key,
                                    const vector<unsigned char>& iv) {
    // 1. Key Expansion
    vector<unsigned char> RoundKeys = KeyExpansion(key);
    printKeySchedule(RoundKeys, "Expanded Key Schedule");

    // 2. Дешифрование каждого блока
    vector<unsigned char> plaintext;
    vector<unsigned char> prevBlock = iv;

    for (size_t i = 0; i < ciphertext.size(); i += BLOCK_SIZE) {
        vector<unsigned char> block(ciphertext.begin()+i, ciphertext.begin()+i+BLOCK_SIZE);

        cout << "\nDecrypting Block " << (i / BLOCK_SIZE) + 1 << ":" << endl;
        cout << "-----------------" << endl;

        // Сохраняем зашифрованный блок для XOR после дешифрования
        vector<unsigned char> tempBlock = block;

        // Дешифрование блока
        vector<unsigned char> decryptedBlock = AES_Decrypt_Block(block, RoundKeys);

        // XOR с предыдущим зашифрованным блоком (или IV для первого блока)
        decryptedBlock = xorBlocks(decryptedBlock, prevBlock);
        printState(decryptedBlock, "After XOR with previous block/IV");

        // Обновляем предыдущий блок для следующей итерации
        prevBlock = tempBlock;

        // Добавляем к результату
        plaintext.insert(plaintext.end(), decryptedBlock.begin(), decryptedBlock.end());
    }

    // 3. Удаление заполнения
    return unpadData(plaintext);
}

int main() {
    try {
        // 1. Генерация случайного ключа и IV
        vector<unsigned char> key = generateRandomBytes(KEY_SIZE);
        vector<unsigned char> iv = generateRandomBytes(BLOCK_SIZE);

        // 2. Сохранение ключа в файл
        writeKeyToFile(key, "aes_key.bin");

        // 3. Пример текста для шифрования (может быть любым, включая русский)
        string plaintext = "Hello, AES! Привет, AES! 1234";
        vector<unsigned char> plaintextBytes(plaintext.begin(), plaintext.end());

        cout << "=== AES-128 CBC Mode Implementation ===" << endl;
        cout << "Original Text: " << plaintext << endl;
        cout << "Original Bytes: ";
        for (auto b : plaintextBytes) cout << hex << setw(2) << setfill('0') << (int)b << " ";
        cout << dec << endl;

        cout << "\nKey: ";
        for (auto b : key) cout << hex << setw(2) << setfill('0') << (int)b << " ";
        cout << dec << endl;

        cout << "IV:  ";
        for (auto b : iv) cout << hex << setw(2) << setfill('0') << (int)b << " ";
        cout << dec << endl << endl;

        // 4. Шифрование
        cout << "\n=== ENCRYPTION PROCESS ===" << endl;
        vector<unsigned char> ciphertext = AES_CBC_Encrypt(plaintextBytes, key, iv);

        cout << "\nCiphertext Bytes: ";
        for (auto b : ciphertext) cout << hex << setw(2) << setfill('0') << (int)b << " ";
        cout << dec << endl << endl;

        // 5. Дешифрование
        cout << "\n=== DECRYPTION PROCESS ===" << endl;
        vector<unsigned char> decryptedBytes = AES_CBC_Decrypt(ciphertext, key, iv);

        string decryptedText(decryptedBytes.begin(), decryptedBytes.end());
        cout << "\nDecrypted Text: " << decryptedText << endl;
        cout << "Decrypted Bytes: ";
        for (auto b : decryptedBytes) cout << hex << setw(2) << setfill('0') << (int)b << " ";
        cout << dec << endl;

        // 6. Проверка
        if (plaintextBytes == decryptedBytes) {
            cout << "\nSuccess! Decrypted text matches original." << endl;
        } else {
            cout << "\nError! Decrypted text does not match original." << endl;
        }

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
