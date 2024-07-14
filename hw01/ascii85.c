#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void encode_bytes(uint32_t bytes)
{
    uint64_t new_bytes = 0;
    for (int8_t i = 4; i >= 0; i--) {
        new_bytes <<= 8;
        new_bytes |= bytes % 85 + 33;
        bytes /= 85;
    }
    for (uint8_t i = 0; i <= 4; i++) {
        putchar(new_bytes >> 8 * i);
    }
}

int encode(void)
{
    int16_t input;
    uint8_t input_unsigned;
    uint8_t counter = 0;
    uint32_t bytes = 0;
    while ((input = getchar()) != EOF) {
        input_unsigned = (uint8_t) input;
        bytes <<= 8;
        bytes |= input_unsigned;
        counter++;
        if (counter == 4) {
            encode_bytes(bytes);
            counter = 0;
            bytes = 0;
        }
    }
    if (counter != 0) {
        for (uint8_t i = counter; i <= 3; i++) {
            bytes <<= 8;
        }
        encode_bytes(bytes);
    }
    putchar('\n');
    return 0;
}

void decode_bytes(uint32_t bytes)
{
    for (int8_t i = 3; i >= 0; i--) {
        putchar(bytes >> 8 * i);
    }
}

int decode(void)
{
    int16_t input;
    uint8_t input_unsigned;
    uint8_t counter = 0;
    uint32_t bytes = 0;
    while ((input = getchar()) != EOF) {
        if (isspace(input)) {
            continue;
        }
        if (input < '!' || input > 'u') {
            return 1;
        }
        input_unsigned = (uint8_t) input;
        input_unsigned -= 33;
        if (counter == 0) {
            bytes = input_unsigned * 85;
            counter++;
        } else if (counter < 4) {
            bytes += input_unsigned;
            bytes *= 85;
            counter++;
        } else {
            bytes += input_unsigned;
            decode_bytes(bytes);
            bytes = 0;
            counter = 0;
        }
    }
    return counter != 0;
}

// ================================
// DO NOT MODIFY THE FOLLOWING CODE
// ================================
int main(int argc, char *argv[])
{
    int retcode = 1;

    if (argc == 1 || (argc == 2 && strcmp(argv[1], "-e") == 0)) {
        retcode = encode();
    } else if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        retcode = decode();
    } else {
        fprintf(stderr, "usage: %s [-e|-d]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (retcode != 0) {
        fprintf(stderr, "an error occured\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
