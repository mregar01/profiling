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

void conditionalMove(Seq_T registers, uint32_t word);

void addition(Seq_T registers, uint32_t word);

void multiplication(Seq_T registers, uint32_t word);

void division(Seq_T registers, uint32_t word);

void BitwiseNAND(Seq_T registers, uint32_t word);

void loadValue(Seq_T registers, uint32_t word);

struct values unpackWord(uint32_t word);
