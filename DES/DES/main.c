#include<stdio.h>
#include <stdint.h>

#define ARRAY_SIZE 8   // 64��Ʈ = 8����Ʈ
#define BIT_SIZE 64    // ��Ʈ ���� ũ��

// 64��Ʈ �Է� �迭: "123456ABCD132536"
uint8_t input[8] = { 0x12, 0x34, 0x56, 0xAB, 0xCD, 0x13, 0x25, 0x36 };
uint8_t initialKey[8] = { 0xAA, 0xBB, 0x09, 0x18, 0x27, 0x36, 0xCC, 0xDD };
// uint8_t input[8] = { 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
uint8_t output[ARRAY_SIZE] = { 0 }; // ��� �迭

uint64_t roundKeys[16]; // 16���� 48��Ʈ ���� Ű

int PermutationGetBit(uint8_t byte, int pos);
int PBoxGetBit(uint32_t data, int pos);
void PermutationSetBit(uint8_t* byte, int pos, int value);
void PBoxSetBit(uint64_t* data, int pos, int value);
void Permute(uint8_t input[], uint8_t output[], int permutationTable[], int size);

uint64_t ExpandPBox(uint32_t input);


int GenKeyGetBit(uint64_t key, int pos);
void GenKeySetBit(uint64_t* key, int pos, int value);
uint32_t LeftCircularShift(uint32_t block, int shifts);
void GenerateKeys(uint64_t input, uint64_t roundKeys[16]);

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
// ��� S �ڽ� ���̺�
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

void main() {
    // Step 1: PlainText ���
    printf("PlainText:\n");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("0x%02X ", input[i]);
    }
    printf("\n\n");

    // Step 2: �ʱ� ġȯ
    Permute(input, output, initialPermutationTable, BIT_SIZE);

    // Step 3: 16���� ���� Ű ����
    uint64_t key = 0;
    printf("Initial Key Bytes (8 bytes): ");
    for (int i = 0; i < 8; i++) {
        printf("0x%02X ", initialKey[i]); // �� ����Ʈ �� ���
        key = (key << 8) | initialKey[i]; // 64��Ʈ Ű ����
    }
    printf("\nGenerated 64-bit Key: 0x%016lX\n", key);
    GenerateKeys(key, roundKeys);

    // Step 5: 32��Ʈ�� ������
    uint32_t leftPart = (output[0] << 24) | (output[1] << 16) | (output[2] << 8) | output[3];
    uint32_t rightPart = (output[4] << 24) | (output[5] << 16) | (output[6] << 8) | output[7];

    printf("Initial Left:  0x%08X\n", leftPart);
    printf("Initial Right: 0x%08X\n\n", rightPart);

    // Step 6: 16���� ��ȣȭ ����
    for (int round = 0; round < 16; round++) {
        // 1. Ȯ�� P �ڽ�
        uint64_t expandedRight = ExpandPBox(rightPart);

        // 2. XOR with Round Key
        uint64_t xored = expandedRight ^ roundKeys[round];

        // 3. S �ڽ� ����
        uint32_t substituted = 0;
        for (int i = 0; i < 8; i++) {
            int row = ((xored >> (47 - 6 * i)) & 0x20) >> 4 | ((xored >> (47 - 6 * i)) & 0x01);
            int col = (xored >> (47 - 6 * i + 1)) & 0x0F;
            substituted = (substituted << 4) | S_BOX[i][row][col];
        }

        // 4. P �ڽ� ����
        uint32_t permuted = 0;
        for (int i = 0; i < 32; i++) {
            permuted |= ((substituted >> (31 - i)) & 1) << (31 - i);
        }

        // 5. XOR with Left
        uint32_t temp = leftPart;
        leftPart = rightPart;
        rightPart = temp ^ permuted;

        // ����� ���
        printf("Round %2d - Left: 0x%08X, Right: 0x%08X, Round Key: 0x%012lX\n",
            round + 1, leftPart, rightPart, roundKeys[round]);
    }

    // Step 7: ������ ���忡�� �¿츦 ��ȯ
    uint32_t temp = leftPart;
    leftPart = rightPart;
    rightPart = temp;

    // Step 8: ���� ġȯ
    uint8_t preOutput[ARRAY_SIZE] = {
        (leftPart >> 24) & 0xFF, (leftPart >> 16) & 0xFF, (leftPart >> 8) & 0xFF, leftPart & 0xFF,
        (rightPart >> 24) & 0xFF, (rightPart >> 16) & 0xFF, (rightPart >> 8) & 0xFF, rightPart & 0xFF
    };
    Permute(preOutput, output, finalPermutationTable, BIT_SIZE);

    // Step 9: CipherText ���
    printf("\nCipherText:\n");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("0x%02X ", output[i]);
    }
    printf("\n");
}

// Ư�� ��Ʈ�� �����ϴ� �Լ� (0 �Ǵ� 1 ��ȯ)
int PermutationGetBit(uint8_t byte, int pos) {
    return (byte >> pos) & 1;
}

// Ư�� ��Ʈ�� �����ϴ� �Լ�
// - byte: ��Ʈ�� ������ ��� ����Ʈ�� ������
// - pos: ������ ��Ʈ�� ��ġ (0���� ����, 0�� ������ ��Ʈ)
// - value: ������ �� (0 �Ǵ� 1)
// - ���: ������ ��ġ�� ��Ʈ�� 1�� �����ϰų� 0���� �ʱ�ȭ�մϴ�.
void PermutationSetBit(uint8_t* byte, int pos, int value) {
    if (value) {
        // value�� 1�̸� pos ��ġ�� ��Ʈ�� 1�� ����
        *byte |= (1 << pos);
    }
    else {
        // value�� 0�̸� pos ��ġ�� ��Ʈ�� 0���� �ʱ�ȭ
        *byte &= ~(1 << pos);
    }
}

// Permutation �Լ� (64��Ʈ ����)
void Permute(uint8_t input[], uint8_t output[], int permutationTable[], int size) {
    uint8_t fullBits[BIT_SIZE] = { 0 };  // �Էµ� ��Ʈ�� Ǯ����� ����

    // Step 1: �Է� �迭�� ��Ʈ ������ ����
    int bitIndex = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        for (int b = 7; b >= 0; b--) { // �� ����Ʈ�� ��Ʈ�� ����
            fullBits[bitIndex++] = PermutationGetBit(input[i], b);
        }
    }

    // Step 2: ���� ���̺��� ����
    uint8_t permutedBits[BIT_SIZE] = { 0 };
    for (int i = 0; i < size; i++) {
        permutedBits[i] = fullBits[permutationTable[i] - 1]; // 1-based index
    }

    // Step 3: ������ ��Ʈ�� �ٽ� ����Ʈ �迭�� ��ȯ
    for (int i = 0; i < ARRAY_SIZE; i++) {
        output[i] = 0;  // �ʱ�ȭ
        for (int b = 7; b >= 0; b--) {
            PermutationSetBit(&output[i], b, permutedBits[i * 8 + (7 - b)]);
        }
    }
}



// Ư�� ��Ʈ�� �����ϴ� �Լ� - P �ڽ����� ���
int PBoxGetBit(uint32_t data, int pos) {
    return (data >> (31 - pos)) & 1; // 0-based �ε��� ���
}

// Ư�� ��Ʈ�� �����ϴ� �Լ� - P �ڽ����� ���
void PBoxSetBit(uint64_t* data, int pos, int value) {
    if (value) {
        *data |= (1ULL << (47 - pos)); // 0-based �ε��� ���
    }
    else {
        *data &= ~(1ULL << (47 - pos));
    }
}





// Ȯ�� P �ڽ� �Լ�
// �Է�: 32��Ʈ
// ���: 48��Ʈ
uint64_t ExpandPBox(uint32_t input) {
    uint64_t output = 0; // 48��Ʈ ���
    for (int i = 0; i < 48; i++) {
        int bit = PBoxGetBit(input, expansionPBox[i] - 1); // 1-based �ε��� ��ȯ
        PBoxSetBit(&output, i, bit);
    }
    return output;
}







// ��Ʈ ���� �Լ� - Ű �������� ���
int GenKeyGetBit(uint64_t key, int pos) {
    return (key >> (64 - pos)) & 1;
}

// ��Ʈ�� �����ϴ� �Լ� - Ű �������� ���
void GenKeySetBit(uint64_t* key, int pos, int value) {
    if (value) {
        *key |= (1ULL << (64 - pos));
    }
    else {
        *key &= ~(1ULL << (64 - pos));
    }
}

// ���� ��ȯ �̵� �Լ�
uint32_t LeftCircularShift(uint32_t block, int shifts) {
    return ((block << shifts) | (block >> (28 - shifts))) & 0xFFFFFFF;
}

// Function to remove parity bits using the table
void removeParityBits(uint64_t initialKey, uint8_t compressedKey[7]) {
    for (int i = 0; i < 7; i++) {
        compressedKey[i] = 0;
    }

    for (int i = 0; i < 56; i++) {
        int bitPosition = removeParityBitTable[i] - 1; // 1-based index
        int compressedByteIndex = i / 8;
        int compressedBitIndex = 7 - (i % 8);

        if (initialKey & (1ULL << (63 - bitPosition))) { // Access bit in uint64_t
            compressedKey[compressedByteIndex] |= (1 << compressedBitIndex);
        }
    }
}

// Function to split the compressed key into two 28-bit halves
void splitKeyIntoHalves(uint8_t compressedKey[7], uint32_t* left, uint32_t* right) {
    *left = 0;
    *right = 0;

    // Extract the first 28 bits for the left half
    for (int i = 0; i < 4; i++) {
        *left |= (uint32_t)(compressedKey[i] << (24 - i * 8));
    }
    *left |= (compressedKey[3] & 0xF0) >> 4;

    // Extract the next 28 bits for the right half
    for (int i = 3; i < 6; i++) {
        *right |= (uint32_t)((compressedKey[i] & 0x0F) << (28 - (i - 3) * 8));
        *right |= (uint32_t)(compressedKey[i + 1] << (24 - (i - 3) * 8));
    }
    *right |= (compressedKey[6] & 0x0F);
}

// Function to perform left circular shift
uint32_t leftCircularShift(uint32_t value, int shifts) {
    return ((value << shifts) | (value >> (28 - shifts))) & 0x0FFFFFFF;
}

// Function to apply compression table and reduce 56-bit to 48-bit
uint64_t applyCompressionTable(uint64_t combinedKey) {
    uint64_t compressedKey = 0;
    for (int i = 0; i < 48; i++) {
        int bitPosition = compressionTable[i] - 1;
        if (combinedKey & (1ULL << (55 - bitPosition))) {
            compressedKey |= (1ULL << (47 - i));
        }
    }
    return compressedKey;
}

void uint64ToUint8Array(uint64_t key, uint8_t keyArray[8]) {
    for (int i = 0; i < 8; i++) {
        keyArray[7 - i] = (key >> (i * 8)) & 0xFF; // MSB���� ����
    }
}

// DES Ű ���� �Լ�
void GenerateKeys(uint64_t input, uint64_t roundKeys[16]) {
    uint8_t compressedKey[7];
    uint32_t leftHalf, rightHalf;   

    // Step 1: PC-1 ���� (�и�Ƽ ��Ʈ ����)
    removeParityBits(input, compressedKey);

    // PC-1 ��� ���
    printf("Permuted Key after PC-1 (56-bit): 0x%014lX\n", compressedKey);

    // Step 2: C�� D�� ����
    printf("Compressed Key (Parity Removed):\n");
    for (int i = 0; i < 7; i++) {
        printf("0x%02X ", compressedKey[i]);
    }
    printf("\n");

    splitKeyIntoHalves(compressedKey, &leftHalf, &rightHalf);


    // Step 3: 16 ���� Ű ����
    for (int i = 0; i < 16; i++) {
        leftHalf = leftCircularShift(leftHalf, shiftLeftTable[i]);
        rightHalf = leftCircularShift(rightHalf, shiftLeftTable[i]);

        // Combine left and right halves into a 56-bit key
        uint64_t combinedKey = ((uint64_t)leftHalf << 28) | (uint64_t)rightHalf;

        // Apply PC-2 table to map 56-bit key to 48-bit
        roundKeys[i] = applyCompressionTable(combinedKey);
    }
}
