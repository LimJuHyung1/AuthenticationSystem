#include<stdio.h>
#include <stdint.h>

#define BIT_SIZE 64    // ��Ʈ ���� ũ��
#define ROUND_KEY_SIZE 48  // ���� Ű ũ��

// �ʱ� ġȯ ���̺�
int initialPermutationTable[BIT_SIZE] = {
    58, 50, 42, 34, 26, 18, 10, 2,  // ù ��
    60, 52, 44, 36, 28, 20, 12, 4,  // �� ��° ��
    62, 54, 46, 38, 30, 22, 14, 6,  // �� ��° ��
    64, 56, 48, 40, 32, 24, 16, 8,  // �� ��° ��
    57, 49, 41, 33, 25, 17,  9, 1,  // �ټ� ��° ��
    59, 51, 43, 35, 27, 19, 11, 3,  // ���� ��° ��
    61, 53, 45, 37, 29, 21, 13, 5,  // �ϰ� ��° ��
    63, 55, 47, 39, 31, 23, 15, 7   // ���� ��° ��
};
// ������ ġȯ ���̺�
int finalPermutationTable[BIT_SIZE] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
};

// Ȯ�� P �ڽ� ���̺�
// ������ ����� Ȯ���Ͽ� 32��Ʈ�� 48��Ʈ�� ��ȯ
int expansionPBox[48] = {
    32,  1,  2,  3,  4,  5,
     4,  5,  6,  7,  8,  9,
     8,  9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32,  1
};
// S �ڽ� ���̺�
// 6��Ʈ�� �Է¹޾� 4��Ʈ ������� ��ȯ
int S_BOX[8][4][16] = {
    // S1
    {
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
    },
    // S2
    {
        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
    },
    // S3
    {
        {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
        {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
        {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
        {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
    },
    // S4
    {
        {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
        {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
        {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
        {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
    },
    // S5
    {
        {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
        {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
        {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
        {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
    },
    // S6
    {
        {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
        {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
        {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
        {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
    },
    // S7
    {
        {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
        {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
        {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
        {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
    },
    // S8
    {
        {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
        {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
        {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
        {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
    }
};
// �ܼ� ġȯ ���̺�
// 32��Ʈ �����͸� ��迭
int simplePermutationTable[32] = {
    16, 7, 20, 21, 29, 12, 28, 17,
    1, 15, 23, 26, 5, 18, 31, 10,
    2, 8, 24, 14, 32, 27, 3, 9,
    19, 13, 30, 6, 22, 11, 4, 25
};


// Ű ���� - �и�Ƽ ���� ���̺�
int removeParityBitTable[56] = {
    57, 49, 41, 33, 25, 17,  9,
     1, 58, 50, 42, 34, 26, 18,
    10,  2, 59, 51, 43, 35, 27,
    19, 11,  3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
     7, 62, 54, 46, 38, 30, 22,
    14,  6, 61, 53, 45, 37, 29,
    21, 13,  5, 28, 20, 12,  4
};
// Ű ���� - ���� ��ȯ �̵� ���̺�
int shiftLeftTable[16] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };
// Ű ���� - ��� ġȯ ���̺�
int compressionTable[48] = {
    14, 17, 11, 24,  1,  5,
     3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8,
    16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};


void initialPermutation(uint8_t* input, uint8_t* output, int* table);
void expansionPermutation(uint32_t input, uint64_t* output, int* table);
uint32_t permutation(uint32_t input, int* table);
void generateRoundKeys(uint8_t* key, uint64_t* subKeys, int* removeParity, int* shiftTable, int* compression);
void initializeDES(uint8_t* input, uint32_t* left, uint32_t* right, int* initialPerm);
void performRound(uint32_t* left, uint32_t* right, uint64_t roundKey, int* expansionPBox, int (*sBox)[4][16], int* permutationTable, int round);
void finalizeDES(uint32_t left, uint32_t right, uint8_t* output, int* finalPerm);
void desEncrypt(uint8_t* input, uint8_t* output, uint64_t* subKeys, int* initialPerm, int* finalPerm, int* expansionPBox, int (*sBox)[4][16], int* permutationTable);


// �׽�Ʈ ����
int main() {
    unsigned char input[8] = { 0x12, 0x34, 0x56, 0xAB, 0xCD, 0x13, 0x25, 0x36 };
    unsigned char key[8] = { 0xAA, 0xBB, 0x09, 0x18, 0x27, 0x36, 0xCC, 0xDD };
    unsigned char output[8] = { 0 };
    uint64_t subKeys[16] = { 0 };

    printf("Encrypted Output: ");
    for (int i = 0; i < 8; i++) {
        printf("%02X ", input[i]);
    }
    printf("\n\n");

    generateRoundKeys(key, subKeys, removeParityBitTable, shiftLeftTable, compressionTable);
    desEncrypt(input, output, subKeys, initialPermutationTable, finalPermutationTable, expansionPBox, S_BOX, simplePermutationTable);

    printf("\nEncrypted Output: ");
    for (int i = 0; i < 8; i++) {
        printf("%02X ", output[i]);
    }
    printf("\n");

    return 0;
}

// �ʱ� ġȯ
// �Է� ������(input)�� ��Ʈ�� �ʱ� ġȯ ���̺��� ����Ͽ� ��迭
void initialPermutation(uint8_t* input, uint8_t* output, int* table) {
    for (int i = 0; i < BIT_SIZE; i++) {
        int pos = table[i] - 1;
        output[i / 8] |= ((input[pos / 8] >> (7 - (pos % 8))) & 1) << (7 - (i % 8));
    }
}

// Ȯ�� ġȯ
// ������ ����� Ȯ�� P �ڽ� ���̺� ���� 48��Ʈ�� Ȯ��
void expansionPermutation(uint32_t input, uint64_t* output, int* table) {
    *output = 0;
    for (int i = 0; i < ROUND_KEY_SIZE; i++) {
        int pos = table[i] - 1;
        *output |= ((uint64_t)((input >> (31 - pos)) & 1)) << (47 - i);
    }
}

// S �ڽ� ����
// Ȯ��� 48��Ʈ �����͸� S �ڽ��� ����� 32��Ʈ�� ����
uint32_t sBoxSubstitution(uint64_t input, int sBox[8][4][16]) {
    uint32_t output = 0;
    for (int i = 0; i < 8; i++) {
        uint8_t block = (input >> (42 - (i * 6))) & 0x3F;
        int row = ((block & 0x20) >> 4) | (block & 0x01);
        int col = (block >> 1) & 0x0F;
        output |= sBox[i][row][col] << (28 - (i * 4));
    }
    return output;
}

// �ܼ� ġȯ
uint32_t permutation(uint32_t input, int* table) {
    uint32_t output = 0;
    for (int i = 0; i < 32; i++) {
        int pos = table[i] - 1;
        output |= ((input >> (31 - pos)) & 1) << (31 - i);
    }
    return output;
}

// Ű ����
// �ʱ� Ű���� 16���� ����Ű�� ����
void generateRoundKeys(uint8_t* key, uint64_t* subKeys, int* removeParity, int* shiftTable, int* compression) {
    uint64_t permutedKey = 0;
    for (int i = 0; i < 56; i++) {
        int pos = removeParity[i] - 1;
        permutedKey |= ((uint64_t)((key[pos / 8] >> (7 - (pos % 8))) & 1)) << (55 - i);
    }

    uint32_t left = (permutedKey >> 28) & 0xFFFFFFF;
    uint32_t right = permutedKey & 0xFFFFFFF;

    for (int i = 0; i < 16; i++) {
        left = ((left << shiftTable[i]) | (left >> (28 - shiftTable[i]))) & 0xFFFFFFF;
        right = ((right << shiftTable[i]) | (right >> (28 - shiftTable[i]))) & 0xFFFFFFF;

        uint64_t combinedKey = ((uint64_t)left << 28) | right;
        subKeys[i] = 0;

        for (int j = 0; j < 48; j++) {
            int pos = compression[j] - 1;
            subKeys[i] |= ((combinedKey >> (55 - pos)) & 1) << (47 - j);
        }
        subKeys[i] |= (combinedKey >> 28) & 0xFFFFFFF000000000; // Ensure full 56-bit key
    }
}

// �ʱ� ġȯ �� ������ �и�
void initializeDES(uint8_t* input, uint32_t* left, uint32_t* right, int* initialPerm) {
    uint8_t permutedInput[8] = { 0 };
    initialPermutation(input, permutedInput, initialPerm);

    *left = ((uint32_t)permutedInput[0] << 24) | ((uint32_t)permutedInput[1] << 16) |
        ((uint32_t)permutedInput[2] << 8) | (uint32_t)permutedInput[3];
    *right = ((uint32_t)permutedInput[4] << 24) | ((uint32_t)permutedInput[5] << 16) |
        ((uint32_t)permutedInput[6] << 8) | (uint32_t)permutedInput[7];

    printf("Initial L: %08X, R: %08X\n\n", *left, *right);
}

// ���� ���� ����
void performRound(uint32_t* left, uint32_t* right, uint64_t roundKey, int* expansionPBox, int (*sBox)[4][16], int* permutationTable, int round) {
    uint64_t expandedRight;
    expansionPermutation(*right, &expandedRight, expansionPBox);

    expandedRight ^= roundKey;
    uint32_t substituted = sBoxSubstitution(expandedRight, sBox);
    uint32_t permuted = permutation(substituted, permutationTable);

    uint32_t temp = *right;
    *right = *left ^ permuted;
    *left = temp;

    printf("Round %d:------------------------------------------------------\n", round + 1);
    printf("\t| L: %08X | R: %08X | Round Key: ", *left, *right);
    for (int bit = 44; bit >= 0; bit -= 4) {
        printf("%X", (int)((roundKey >> bit) & 0xF));
    }
    printf(" |\n");
}

// DES ��ȣȭ/��ȣȭ
// ���� ġȯ �� ��� ����
void finalizeDES(uint32_t left, uint32_t right, uint8_t* output, int* finalPerm) {
    uint64_t combined = ((uint64_t)right << 32) | left;

    for (int i = 0; i < 64; i++) {
        int pos = finalPerm[i] - 1;
        output[i / 8] |= ((combined >> (63 - pos)) & 1) << (7 - (i % 8));
    }
}

// DES ��ȣȭ
void desEncrypt(uint8_t* input, uint8_t* output, uint64_t* subKeys, int* initialPerm, int* finalPerm, int* expansionPBox, int (*sBox)[4][16], int* permutationTable) {
    uint32_t left, right;

    // �ʱ� ġȯ �� ��� �и�
    initializeDES(input, &left, &right, initialPerm);

    // 16 ���� ����
    for (int i = 0; i < 16; i++) {
        performRound(&left, &right, subKeys[i], expansionPBox, sBox, permutationTable, i);
    }

    // ���� ġȯ �� ��� ����
    finalizeDES(left, right, output, finalPerm);
}
