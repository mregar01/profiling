/*
 *     basicInstructions.c
 *     by Vir Bhatia (vbhati02) and Max Regardie (mregar01), 
 *     4/13/23
 *     
 *     UM
 *
 *     This is the implementation file for the basicInstructions module. This 
 *     module reads and edits registers and handles the non-memory related 
 *     instructions.
 */

#include "basicInstructions.h"

#define WIDTH 3
#define ALSB 6
#define BLSB 3
#define CLSB 0
#define MODVAL 4294967296 
#define LVWIDTH 25


/**********ConditionalMove********
 * if the value in $r[C] is not 0, moves the value in $r[B] into $r[A].
 * Inputs: 
 *	- 32 bit word instruction
 *	- Sequence of registers   
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void conditionalMove(uint32_t* registers, uint32_t word)
{
        struct values data;    

        unsigned hi = ALSB + WIDTH; /* one beyond the most significant bit */

        /* different type of right shift */
        data.a = shr(shl(*curr, 64 - hi), 64 - WIDTH);

        hi = BLSB + WIDTH; /* one beyond the most significant bit */

        /* different type of right shift */
        data.b = shr(shl(*curr, 64 - hi), 64 - WIDTH);

        hi = CLSB + WIDTH; /* one beyond the most significant bit */

        /* different type of right shift */
        data.c = shr(shl(*curr, 64 - hi), 64 - WIDTH);


        // uint32_t *cval = Seq_get(registers, data.c); 
        uint32_t cval = registers[data.c];

        /* Checks if value in r[c] is 0 before executing move*/
        if (cval != 0) {
                // *(uint32_t*) Seq_get(registers, data.a) = 
                //                 *(uint32_t*) Seq_get(registers, data.b);
                registers[data.a] = registers[data.b];
        }
}


/**********Addition********
 * Adds the values held in $r[B] and $r[C], performs mod 2^32 and stores the 
 *      result in $r[A].
 * Inputs: 
 *	- 32 bit word instruction
 *	- Sequence of registers   
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void addition(uint32_t* registers, uint32_t word) 
{
        // struct values data = unpackWord(word);    
        // uint32_t *bval = Seq_get(registers, data.b);
        // uint32_t *cval = Seq_get(registers, data.c); 
        
        struct values data;    
        unsigned hi = 6 + 3; /* one beyond the most significant bit */

        /* different type of right shift */
        data.a = shr(shl(*curr, 64 - hi), 64 - 3);

        hi = 3 + 3; /* one beyond the most significant bit */

        /* different type of right shift */
        data.b = shr(shl(*curr, 64 - hi), 64 - 3);

        hi = 0 + 3; /* one beyond the most significant bit */

        /* different type of right shift */
        data.c = shr(shl(*curr, 64 - hi), 64 - 3);
          
        uint32_t cval = registers[data.c];
        uint32_t bval = registers[data.b];

        /* Adds values in two registers */
        uint32_t result = (bval + cval) % MODVAL;

        // *(uint32_t*) Seq_get(registers, data.a) = result;
        registers[data.a] = result;
}


/**********Multiplication********
 * Multiplies the values held in $r[B] and $r[C], performs mod 2^32 and stores 
 *      the result in $r[A].
 * Inputs: 
 *	- 32 bit word instruction
 *	- Sequence of registers   
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void multiplication(uint32_t* registers, uint32_t word)
{
        // struct values data = unpackWord(word);    
        // uint32_t *bval = Seq_get(registers, data.b);
        // uint32_t *cval = Seq_get(registers, data.c);  

        struct values data;    
        unsigned hi = 6 + 3; /* one beyond the most significant bit */

        /* different type of right shift */
        data.a = shr(shl(*curr, 64 - hi), 64 - 3);

        hi = 3 + 3; /* one beyond the most significant bit */

        /* different type of right shift */
        data.b = shr(shl(*curr, 64 - hi), 64 - 3);

        hi = 0 + 3; /* one beyond the most significant bit */

        /* different type of right shift */
        data.c = shr(shl(*curr, 64 - hi), 64 - 3);

        uint32_t cval = registers[data.c];
        uint32_t bval = registers[data.b];

        /* Multiplies values in two registers */                      
        uint32_t result = (bval * cval) % MODVAL;

        // *(uint32_t*) Seq_get(registers, data.a) = result;
        registers[data.a] = result;
}


/**********Division********
 * Places the value of ⌊$r[B] ÷ $r[C]⌋ into $r[A]
 * Inputs: 
 *	- 32 bit word instruction
 *	- Sequence of registers   
 * Return: none
 * Expects: The value in $r[C] to not be zero
 * Notes: 
 ************************/
void division(uint32_t* registers, uint32_t word) 
{
        // struct values data = unpackWord(word);    
        // uint32_t *bval = Seq_get(registers, data.b);
        // uint32_t *cval = Seq_get(registers, data.c); 

        struct values data;    


        
        unsigned hi = ALSB + WIDTH; /* one beyond the most significant bit */

        /* different type of right shift */
        data.a = shr(shl(word, 64 - hi), 64 - WIDTH);

        hi = BLSB + WIDTH; /* one beyond the most significant bit */

        /* different type of right shift */
        data.b = shr(shl(word, 64 - hi), 64 - WIDTH);

        hi = CLSB + WIDTH; /* one beyond the most significant bit */

        /* different type of right shift */
        data.c = shr(shl(word, 64 - hi), 64 - WIDTH);

        uint32_t cval = registers[data.c];
        uint32_t bval = registers[data.b];   

        /* Prevents division by zero */
        assert(cval != 0);   

        /* Divides values in two registers */                 
        uint32_t result = (bval / cval);

        // *(uint32_t*) Seq_get(registers, data.a) = result;
        registers[data.a] = result;

}


/**********Bitwise NAND********
 * Places the value of  ¬($r[B] ∧ $r[C]) into $r[A]
 * Inputs: 
 *	- 32 bit word instruction
 *	- Sequence of registers   
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void BitwiseNAND(uint32_t* registers, uint32_t word)
{
        // struct values data = unpackWord(word);    
        // uint32_t *bval = Seq_get(registers, data.b);
        // uint32_t *cval = Seq_get(registers, data.c);  

        struct values data;    


        
        unsigned hi = ALSB + WIDTH; /* one beyond the most significant bit */

        /* different type of right shift */
        data.a = shr(shl(word, 64 - hi), 64 - WIDTH);

        hi = BLSB + WIDTH; /* one beyond the most significant bit */

        /* different type of right shift */
        data.b = shr(shl(word, 64 - hi), 64 - WIDTH);

        hi = CLSB + WIDTH; /* one beyond the most significant bit */

        /* different type of right shift */
        data.c = shr(shl(word, 64 - hi), 64 - WIDTH);     
        uint32_t cval = registers[data.c];
        uint32_t bval = registers[data.b];               
        
        /* Performs bitwise NAND on values in two registers */
        uint32_t result = ~(bval & cval);

        // *(uint32_t*) Seq_get(registers, data.a) = result;
        registers[data.a] = result;

}


/**********LoadValue********
 * Place the value stored in the 25 least significant bits of the word 
 *      into $r[A]
 * Inputs: 
 *	- 32 bit word instruction
 *	- Sequence of registers   
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void loadValue(uint32_t* registers, uint32_t word)
{
        // uint32_t aval =  Bitpack_getu(word, WIDTH, 25);
        // uint32_t loadval = Bitpack_getu(word, LVWIDTH, 0);
        

        
        unsigned hi = 25 + WIDTH; /* one beyond the most significant bit */

        /* different type of right shift */
        uint32_t aval = shr(shl(word, 64 - hi), 64 - WIDTH);

        hi = 0 + LVWIDTH; /* one beyond the most significant bit */

        /* different type of right shift */
        uint32_t loadval = shr(shl(word, 64 - hi), 64 - LVWIDTH);

        
        // *(uint32_t*) Seq_get(registers, aval) = loadval;
        registers[aval] = loadval;

}


/**********unpackWord********
 * Retrieves the a, b, c values from a uint32 word
 * Inputs: 
 *	- 32 bit word instruction 
 * Return: struct with a, b, c values
 * Expects: 
 * Notes: 
 ************************/
struct values unpackWord(uint32_t word)
{  
        /* Unpacks a word's a, b, and c register codes into a struct */

        struct values final;
        final.a = Bitpack_getu(word, WIDTH, ALSB);
        final.b = Bitpack_getu(word, WIDTH, BLSB);
        final.c = Bitpack_getu(word, WIDTH, CLSB);
        return final;
}

uint64_t shl(uint64_t word, unsigned bits)
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
uint64_t shr(uint64_t word, unsigned bits)
{
        // assert(bits <= 64);
        if (bits == 64)
                return 0;
        else
                return word >> bits;
}