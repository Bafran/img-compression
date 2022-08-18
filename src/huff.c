#include "huff.h"

#define bitset(byte, nbit)   ((byte) |=  (1<<(nbit)))
#define bitclear(byte, nbit) ((byte) &= ~(1<<(nbit)))

#define BLOCK_SIZE 8

/* This module uses standard huffman tables for both luminosity (Y) and colour components (Cb, Cr) of the image */

/* Zig zag orders a matrix into an array, could probably be done with just a list of indexes as a speed improvement */
void zig_zag_order(int *output, float **Matrix){
    int row = 0, col = 0, d = 0;
    int dirs[2][2] = {{-1, 1}, {1, -1}};

    for(int i = 0; i < BLOCK_SIZE * BLOCK_SIZE; i++){
        output[i] = (int)Matrix[row][col];

        row += dirs[d][0];
        col += dirs[d][1];

        if (row >= BLOCK_SIZE) { row = BLOCK_SIZE - 1; col += 2; d = 1 - d;}
        if (col >= BLOCK_SIZE) { col = BLOCK_SIZE - 1; row += 2; d = 1 - d;}
        if (row < 0)           { row = 0; d = 1 - d;}
        if (col < 0)           { col = 0; d = 1 - d;}
    }
}

/* This function generates the huffman codes based on the above data showing the lengths of each code and the
 * symbol that's being encoded. Definitely slower than it needs to be, but it's only happening once. Also, the
 * 0x00 and 0xF0 codes are being saved so that they don't have to be fetched in the compression.c file.
*/
void generate_huff_codes(HuffmanTable *table, enum Table_t type){
    /* Calculate the bit rep of all DC or AC symbols */
    // Iterate through the code lengths
    uint16_t bit_rep = 0;
    uint8_t code_num = 0;
    // For every code length
    for (int i = 1; i < 17; i++) {
        // Iterate the number of codes there are of that length
        for (int j = 0; j < table->offsets[i]; j++) {
            // Store the code candidate
            table->codes[code_num] = bit_rep;
            // Add one
            bit_rep++;
            code_num++;
        }
        // Add a zero to the right of the BINARY of bit_rep
        bit_rep = bit_rep << 1;
    }
    table->set = true;

    if(type == LUM_AC_TABLE || type == COL_AC_TABLE){
        /* Fetch an AC bit representation given the int or hex code */ // (O(n) time :( sadly)
        uint8_t fetch_bit = 0xf0;
        uint8_t index = 0;
        for(int i = 0; i < 162; ++i) {
            if(table->symbols[i] == fetch_bit){
                index = i;
                break;
            }
        }
        bit_rep = table->codes[index];
        /* Fetch the length of the huffman symbol */
        uint8_t sum_index = 0;
        uint8_t fetch_index = 0;
        // fetch_bit replaced with is the index of the bit (see DC fetching) (add one to index to counter zero indexing)
        while(sum_index < index+1){
            sum_index += table->offsets[fetch_index];
            fetch_index++;
        }
        fetch_index--;
        table->ZRL_code_index = index; table->ZRL_code_len = fetch_index;

        /* Fetch an AC bit representation given the int or hex code */ // (O(n) time :( sadly)
        fetch_bit = 0x00;
        index = 0;
        for(int i = 0; i < 162; ++i) {
            if(table->symbols[i] == fetch_bit){
                index = i;
                break;
            }
        }
        bit_rep = table->codes[index];
        /* Fetch the length of the huffman symbol */
        sum_index = 0;
        fetch_index = 0;
        // fetch_bit replaced with is the index of the bit (see DC fetching) (add one to index to counter zero indexing)
        while(sum_index < index+1){
            sum_index += table->offsets[fetch_index];
            fetch_index++;
        }
        fetch_index--;
        table->EOB_code_index = index; table->EOB_code_len = fetch_index;
    }
}

void write_buffer(uint16_t *buf, uint8_t *nbit, FILE *fptr, uint16_t code_to_write, uint8_t len){
    code_to_write = code_to_write << (sizeof(code_to_write)*8-len);
    for(int i = 0; i < len; ++i) {
        ((code_to_write & (1 << (16 - 1))) >> (16 - 1)) ? bitset(*buf, 15-*nbit) : bitclear(*buf, 15-*nbit);
        code_to_write = code_to_write << 1;

        // If the last bit written to is 15 (buffer now full, cannot iterate nbit) (nbit is 0 indexed)
        if(*nbit == 15){
            // write buf to file (swap bytes because of endian issues :/ should be changed when serial communicating these values)
            uint16_t tmp = (*buf << 8);
            tmp = tmp | (*buf >> 8);
            fwrite(&tmp, sizeof(tmp), 1, fptr);
//            printf("Buffer wrote : "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n",
//                   BYTE_TO_BINARY(*buf>>8), BYTE_TO_BINARY(*buf));
            *nbit = 0;
        } else {
            (*nbit)++;
        }
    }
}

/* Writes whatever is in the buffer even if it is not full
 * Note that the values that are in the "empty" portion of the buffer will be garbage values */
void flush_buffer(uint16_t *buf, uint8_t *nbit, FILE *fptr){
    // return if the buffer is already empty
    if (*nbit == 0) {
        return;
    }

    while (*nbit <= 15) {
        bitclear(*buf, 15-*nbit);
        (*nbit)++;
    }

    uint16_t tmp = (*buf << 8);
    tmp = tmp | (*buf >> 8);
    fwrite(&tmp, sizeof(tmp), 1, fptr);
    printf("Buffer wrote : "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n",
           BYTE_TO_BINARY(*buf>>8), BYTE_TO_BINARY(*buf));
    *nbit = 0;
}