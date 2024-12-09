#include<stdio.h>
#include <stdint.h>

#define ARRAY_SIZE 8   // 64비트 = 8바이트
#define BIT_SIZE 64    // 비트 순열 크기

// 64비트 입력 배열: "123456ABCD132536"
uint8_t input[8] = { 0x12, 0x34, 0x56, 0xAB, 0xCD, 0x13, 0x25, 0x36 };
// uint8_t input[8] = { 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
uint8_t output[ARRAY_SIZE] = { 0 }; // 출력 배열

uint64_t roundKeys[16]; // 16개의 48비트 라운드 키

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

// 초기 치환 테이블
int initialPermutationTable[BIT_SIZE] = {
    58, 50, 42, 34, 26, 18, 10, 2,  // 첫 줄
    60, 52, 44, 36, 28, 20, 12, 4,  // 두 번째 줄
    62, 54, 46, 38, 30, 22, 14, 6,  // 세 번째 줄
    64, 56, 48, 40, 32, 24, 16, 8,  // 네 번째 줄
    57, 49, 41, 33, 25, 17,  9, 1,  // 다섯 번째 줄
    59, 51, 43, 35, 27, 19, 11, 3,  // 여섯 번째 줄
    61, 53, 45, 37, 29, 21, 13, 5,  // 일곱 번째 줄
    63, 55, 47, 39, 31, 23, 15, 7   // 여덟 번째 줄
};
// 마지막 치환 테이블
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

// 확장 P 박스 테이블
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
// 축소 S 박스 테이블
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


// 키 생성 - 패리티 제거 테이블
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
// 키 생성 - 좌측 순환 이동 테이블
int shiftLeftTable[16] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };
// 키 생성 - 축소 치환 테이블
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
    Permute(input, output, initialPermutationTable, BIT_SIZE);

    /*
    // 결과 출력
    printf("Permuted Array:\n");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("0x%02X ", output[i]);
    }
    printf("\n");
    */
    // 32비트씩 나누기
    uint32_t leftPart = (output[0] << 24) | (output[1] << 16) | (output[2] << 8) | output[3];
    uint32_t rightPart = (output[4] << 24) | (output[5] << 16) | (output[6] << 8) | output[7];

    // 확장 P 박스 적용
    uint64_t expandedLeft = ExpandPBox(leftPart);
    uint64_t expandedRight = ExpandPBox(rightPart);

    // 결과 출력
    printf("Original Left Part (32-bit):  0x%08X\n", leftPart);
    printf("Expanded Left Part (48-bit):  0x%012lX\n", expandedLeft);
    printf("Original Right Part (32-bit): 0x%08X\n", rightPart);
    printf("Expanded Right Part (48-bit): 0x%012lX\n", expandedRight);

    return 0;
}

// 특정 비트를 추출하는 함수 (0 또는 1 반환)
int PermutationGetBit(uint8_t byte, int pos) {
    return (byte >> pos) & 1;
}

// 특정 비트를 설정하는 함수
// - byte: 비트를 설정할 대상 바이트의 포인터
// - pos: 설정할 비트의 위치 (0부터 시작, 0은 최하위 비트)
// - value: 설정할 값 (0 또는 1)
// - 기능: 지정된 위치의 비트를 1로 설정하거나 0으로 초기화합니다.
void PermutationSetBit(uint8_t* byte, int pos, int value) {
    if (value) {
        // value가 1이면 pos 위치의 비트를 1로 설정
        *byte |= (1 << pos);
    }
    else {
        // value가 0이면 pos 위치의 비트를 0으로 초기화
        *byte &= ~(1 << pos);
    }
}

// Permutation 함수 (64비트 기준)
void Permute(uint8_t input[], uint8_t output[], int permutationTable[], int size) {
    uint8_t fullBits[BIT_SIZE] = { 0 };  // 입력된 비트를 풀어놓는 공간

    // Step 1: 입력 배열을 비트 단위로 추출
    int bitIndex = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        for (int b = 7; b >= 0; b--) { // 각 바이트의 비트를 추출
            fullBits[bitIndex++] = PermutationGetBit(input[i], b);
        }
    }

    // Step 2: 순열 테이블을 적용
    uint8_t permutedBits[BIT_SIZE] = { 0 };
    for (int i = 0; i < size; i++) {
        permutedBits[i] = fullBits[permutationTable[i] - 1]; // 1-based index
    }

    // Step 3: 순열된 비트를 다시 바이트 배열로 변환
    for (int i = 0; i < ARRAY_SIZE; i++) {
        output[i] = 0;  // 초기화
        for (int b = 7; b >= 0; b--) {
            PermutationSetBit(&output[i], b, permutedBits[i * 8 + (7 - b)]);
        }
    }
}



// 특정 비트를 추출하는 함수 - P 박스에서 사용
int PBoxGetBit(uint32_t data, int pos) {
    return (data >> (31 - pos)) & 1; // 0-based 인덱스 사용
}

// 특정 비트를 설정하는 함수 - P 박스에서 사용
void PBoxSetBit(uint64_t* data, int pos, int value) {
    if (value) {
        *data |= (1ULL << (47 - pos)); // 0-based 인덱스 사용
    }
    else {
        *data &= ~(1ULL << (47 - pos));
    }
}





// 확장 P 박스 함수
// 입력: 32비트
// 출력: 48비트
uint64_t ExpandPBox(uint32_t input) {
    uint64_t output = 0; // 48비트 출력
    for (int i = 0; i < 48; i++) {
        int bit = PBoxGetBit(input, expansionPBox[i] - 1); // 1-based 인덱스 변환
        PBoxSetBit(&output, i, bit);
    }
    return output;
}







// 비트 추출 함수 - 키 생성에서 사용
int GenKeyGetBit(uint64_t key, int pos) {
    return (key >> (64 - pos)) & 1;
}

// 비트를 설정하는 함수 - 키 생성에서 사용
void GenKeySetBit(uint64_t* key, int pos, int value) {
    if (value) {
        *key |= (1ULL << (64 - pos));
    }
    else {
        *key &= ~(1ULL << (64 - pos));
    }
}

// 좌측 순환 이동 함수
uint32_t LeftCircularShift(uint32_t block, int shifts) {
    return ((block << shifts) | (block >> (28 - shifts))) & 0xFFFFFFF;
}

// DES 키 생성 함수
void GenerateKeys(uint64_t input, uint64_t roundKeys[16]) {
    uint64_t permutedKey = 0;
    uint32_t C = 0, D = 0;

    // Step 1: PC-1 적용
    for (int i = 0; i < 56; i++) {
        int bit = GenKeyGetBit(input, removeParityBitTable[i]);
        GenKeySetBit(&permutedKey, i + 1, bit);
    }

    // Step 2: C와 D로 분할
    C = (permutedKey >> 28) & 0xFFFFFFF;
    D = permutedKey & 0xFFFFFFF;

    // Step 3: 16 라운드 키 생성
    for (int round = 0; round < 16; round++) {
        // 좌측 순환 이동
        C = LeftCircularShift(C, shiftLeftTable[round]);
        D = LeftCircularShift(D, shiftLeftTable[round]);

        // 합치기
        uint64_t combinedKey = ((uint64_t)C << 28) | D;

        // PC-2 적용
        roundKeys[round] = 0;
        for (int i = 0; i < 48; i++) {
            int bit = GenKeyGetBit(combinedKey, compressionTable[i]);
            GenKeySetBit(&roundKeys[round], i + 1, bit);
        }
    }
}