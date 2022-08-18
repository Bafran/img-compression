#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte) \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

enum Table_t {
    LUM_DC_TABLE,
    LUM_AC_TABLE,
    COL_DC_TABLE,
    COL_AC_TABLE
};

typedef struct {
    unsigned char offsets[17]; // number of symbols at each bit len (e.g. if index 4==1, 1 symbol that has code len 4)
    unsigned char symbols[162]; // symbols ordered by len (e.g symbols[0] has code len of offsets[i] where i=min(i>0)
    uint16_t codes[162]; // codes of the symbols with the correct binary representation
    uint8_t EOB_code_index;
    uint8_t EOB_code_len;
    uint8_t ZRL_code_index;
    uint8_t ZRL_code_len;
    bool set; // whether the codes have been generated yet
} HuffmanTable;

void zig_zag_order(int *output, float **Matrix);
void generate_huff_codes(HuffmanTable *table, enum Table_t type);

/* parameters - buf: buffer to write to, nbit: buf bit to write to, fptr is the ptr to the output file
 *              code_to_write: uint16_t code to be written, len: len of huffman coded symbol in number of bits
 * fptr must be open in wb mode
 * if the buffer is full (nbit == 15), then write to fptr and reset nbit to 0 */
void write_buffer(uint16_t *buf, uint8_t *nbit, FILE *fptr, uint16_t code_to_write, uint8_t len);
void flush_buffer(uint16_t *buf, uint8_t *nbit, FILE *fptr);