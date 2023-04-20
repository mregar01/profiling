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
void input(uint32_t* registers, uint32_t word) {
        
        char c = getchar();

        if (c == EOF) {
                c = ~0;
        } 

        // int cval = Bitpack_getu(word, CWIDTH, 0);

        // assert(CWIDTH <= 64);
        
        unsigned hi = 0 + CWIDTH; /* one beyond the most significant bit */
        assert(hi <= 64);
        /* different type of right shift */
        int cval = shrio(shlio(word, 64 - hi), 64 - CWIDTH);




        // uint32_t *curr = Seq_get(registers, cval);

        // *curr = c;
        // Seq_put(registers, cval, curr);
        registers[cval] = c;

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

void output(uint32_t* registers, uint32_t word) {
        
        // int cval = Bitpack_getu(word, CWIDTH, 0);


        unsigned hi = 0 + CWIDTH; /* one beyond the most significant bit */
        // assert(hi <= 64);
        /* different type of right shift */
        int cval = shrio(shlio(word, 64 - hi), 64 - CWIDTH);



        // uint32_t *value = Seq_get(registers, cval);
        // uint32_t *value = regis

        char c = registers[cval];
        // char c = *value;
        assert(c > MINVAL);
        putchar(c);
}

uint64_t shlio(uint64_t word, unsigned bits)
{
        // assert(bits <= 64);
        if (bits == 64)
                return 0;
        else
                return word << bits;
}

/*
 * shift R logical
 */
uint64_t shrio(uint64_t word, unsigned bits)
{
        // assert(bits <= 64);
        if (bits == 64)
                return 0;
        else
                return word >> bits;
}