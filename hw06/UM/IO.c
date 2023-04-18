/*
 *     IO.c
 *     by Vir Bhatia (vbhati02) and Max Regardie (mregar01), 
 *     4/14/23
 *     
 *     UM
 *
 *     This is the implementation file for the IO module. This module reads and
 *     edits registers and handles inputs and outputs from the client.
 *
 */

#include "IO.h"

#define MINVAL -1
#define CWIDTH 3

/**********Input********
 * uses stdin to read in a value that must be between 0-255 and stores it in 
 * $r[C]
 * Inputs: 
 *	- 32 bit word instruction
 *	- Sequence of registers   
 * Return: none
 * Expects: 
 * Notes: If I/O device is at the end of input $r[C] is loaded with a 32 bit 
 * word of all ones
 ************************/
void input(Seq_T registers, uint32_t word) {
        
        char c = getchar();

        if (c == EOF) {
                c = ~0;
        } 

        int cval = Bitpack_getu(word, CWIDTH, 0);

        uint32_t *curr = Seq_get(registers, cval);
        *curr = c;
        Seq_put(registers, cval, curr);

}



/**********Output********
 * Writes the value in $r[C] to stdout
 * Inputs: 
 *	- 32 bit word instruction
 *	- Sequence of registers   
 * Return: none
 * Expects: 
 * Notes: Function only outputs values from 0-255
 ************************/

void output(Seq_T registers, uint32_t word) {
        
        int cval = Bitpack_getu(word, CWIDTH, 0);
        uint32_t *value = Seq_get(registers, cval);

        char c = *value;
        assert(c > MINVAL);
        putchar(c);
}