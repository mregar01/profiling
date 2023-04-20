/*
 *     basicInstructions.h
 *     by Vir Bhatia (vbhati02) and Max Regardie (mregar01), 
 *     4/13/23
 *     
 *     UM
 *
 *     This is the header file for the basicInstructions module. This 
 *     module reads and edits registers and handles the non-memory related 
 *     instructions.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>

/*struct that is used for storing a, b, c values of a code word*/
struct values {
        uint32_t a;
        uint32_t b;
        uint32_t c;
};

void conditionalMove(uint32_t* registers, uint32_t word);

void addition(uint32_t* registers, uint32_t word);

void multiplication(uint32_t* registers, uint32_t word);

void division(uint32_t* registers, uint32_t word);

void BitwiseNAND(uint32_t* registers, uint32_t word);

void loadValue(uint32_t* registers, uint32_t word);

struct values unpackWord(uint32_t word);

uint64_t shl(uint64_t word, unsigned bits);

uint64_t shr(uint64_t word, unsigned bits);
