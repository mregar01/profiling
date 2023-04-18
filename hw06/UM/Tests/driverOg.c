/*
 *     driver.c
 *     by Vir Bhatia (vbhati02) and Max Regardie (mregar01), 
 *     4/14/23
 *     
 *     UM
 *
 *     //perhaps will be changed
 *
 */

#include "driver.h"

#define NUMREGISTERS 8
#define NUMSEGMENTS 100
#define MAXLENGTH 4
#define NUMARGUMENTS 2
#define INITIALCOUNTER 0

#define WIDTH 3
#define ALSB 6
#define BLSB 3
#define CLSB 0

/*struct that is used for storing a single segment in the sequence of 
segments*/
struct segment {
        bool mapped;
        Seq_T words;
};

/*enum that is used for opcodes*/
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


int main(int argc, char *argv[]){
        assert(argc == NUMARGUMENTS);
        
        // Seq_T registers = initializeRegisters();
        uint32_t registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        Seq_T segments = initializeSegments(argv[1]);

        commandLoop(registers, segments);
        
        // printRegisters(registers);

        freeSegments(segments);
        freeRegisters(registers);
}


/**********initializeRegisters********
 * initializes a sequence of registers to hold all 0's
 * Inputs: 
 * Return: sequence of registers
 * Expects: 
 * Notes: 
 ************************/
Seq_T initializeRegisters()
{
        uint32_t *curr;
        Seq_T registers = Seq_new(NUMREGISTERS);
        for (int i = 0; i < NUMREGISTERS; i++){
                curr = malloc(sizeof(int32_t));
                *curr = 0;
                Seq_addhi(registers, curr);
        }
        return registers;

        
}

/**********printRegisters********
 * takes a sequence of registers and prints all values(used for testing)
 * Inputs: 
 *      - sequence of registers
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void printRegisters(Seq_T sequence) 
{
        printf("Printing registers...\n");
        for (int i = 0; i < NUMREGISTERS; i++) {
                uint32_t *curr = Seq_get(sequence, i);
                printf("$r[%d]: %d\n", i, *curr);
        }    
}


/**********freeRegisters********
 * frees all memory allocated in the sequence of registers
 * Inputs: 
 *      - sequence of registers
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void freeRegisters(Seq_T sequence)
{
        for (int i = 0; i < NUMREGISTERS; i++) {
                free(Seq_get(sequence, i));
        }
        Seq_free(&sequence);
}


/**********commandLoop********
 * main driver for the program. Continuosly processes input words from segment
 * zero and calls helper functions based on OPCODEs
 * Inputs: 
 *      - sequence of registers
 *      - sequence of segments
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void commandLoop(Seq_T registers, Seq_T segments)
{
       
        struct segment *zero = Seq_get(segments, 0);
        int size = Seq_length(zero->words);
        uint32_t *curr;
        uint32_t opcode;
        for (int i = 0; i < size; i++) {
                curr = Seq_get(zero->words, i);
                opcode = Bitpack_getu(*curr, 4, 28);
                // fprintf(stderr, "code: %x\n", *curr);
                if (opcode == CMOV) {
                        conditionalMove(registers, *curr);
                } else if (opcode == SLOAD) {
                        segmentedLoad(registers, segments, *curr);
                } else if (opcode == SSTORE) {
                        segmentedStore(registers, segments, *curr);
                } else if (opcode == ADD) {
                        addition(registers, *curr);
                } else if (opcode == MUL) {
                        multiplication(registers, *curr);
                } else if (opcode == DIV) {
                        division(registers, *curr);
                } else if (opcode == NAND) {
                        BitwiseNAND(registers, *curr);
                } else if (opcode == HALT) {
                        // fprintf(stderr, "halting\n");
                        break;
                } else if (opcode == ACTIVATE) {
                        mapSegment(registers, segments, *curr);
                } else if (opcode == INACTIVATE) {
                        unmapSegment(registers, segments, *curr);
                } else if (opcode == OUT) {
                        output(registers, *curr);
                                // printRegisters(registers);

                } else if (opcode == IN) {
                        input(registers, *curr);
                } else if (opcode == LOADP) {
                        // fprintf(stderr, "starting load\n");
                        i = loadProgram(registers, segments, *curr);
                        // fprintf(stderr, "i value: %d\n", i);
                        zero = Seq_get(segments, 0);
                        size = Seq_length(zero->words);
                        // fprintf(stderr, "ending load\n");
                        // commandLoop(registers, segments, newCounter);
                } else if (opcode == LV) {
                        loadValue(registers, *curr);
                }
        }
}

/*
 *     basicInstructions.c
 *     by Vir Bhatia (vbhati02) and Max Regardie (mregar01), 
 *     4/14/23
 *     
 *     UM
 *
 *     //perhaps will be changed
 *
 */

#include "basicInstructions.h"

#define WIDTH 3
#define ALSB 6
#define BLSB 3
#define CLSB 0
#define MODVAL 4294967296 
#define LVWIDTH 25

/*struct that is used for storing a, b, c values of a code word*/
struct values {
        uint32_t a;
        uint32_t b;
        uint32_t c;
};


/**********ConditionalMove********
 * if the value in $r[C] is not 0, moves the value in $r[B] into $r[A].
 * Inputs: 
 *	- 32 bit word instruction
 *	- C Array of registers   
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void conditionalMove(Seq_T registers, uint32_t word)
{
        struct values data = unpackWord(word);    
        uint32_t *cval = Seq_get(registers, data.c); 
        if (*cval != 0) {
                *(uint32_t*) Seq_get(registers, data.a) = *(uint32_t*) Seq_get(registers, data.b);
        }
}


/**********Addition********
 * Adds the values held in $r[B] and $r[C], performs mod 232 and stores the 
 *      result in $r[A].
 * Inputs: 
 *	- 32 bit word instruction
 *	- C Array of registers   
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void addition(Seq_T registers, uint32_t word) 
{
        struct values data = unpackWord(word);    
        uint32_t *bval = Seq_get(registers, data.b);
        uint32_t *cval = Seq_get(registers, data.c);                        
        uint32_t result = (*bval + *cval) % MODVAL;

        *(uint32_t*) Seq_get(registers, data.a) = result;
}


/**********Multiplication********
 * Multiplies the values held in $r[B] and $r[C], performs mod 2^32 and stores 
 *      the result in $r[A].
 * Inputs: 
 *	- 32 bit word instruction
 *	- C Array of registers   
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void multiplication(Seq_T registers, uint32_t word)
{
        struct values data = unpackWord(word);    
        uint32_t *bval = Seq_get(registers, data.b);
        uint32_t *cval = Seq_get(registers, data.c);                        
        uint32_t result = (*bval * *cval) % MODVAL;

        *(uint32_t*) Seq_get(registers, data.a) = result;
}


/**********Division********
 * Places the value of ⌊$r[B] ÷ $r[C]⌋ into $r[A]
 * Inputs: 
 *	- 32 bit word instruction
 *	- C Array of registers   
 * Return: none
 * Expects: The value in $r[C] to not be zero
 * Notes: 
 ************************/
void division(Seq_T registers, uint32_t word) 
{
        struct values data = unpackWord(word);    
        uint32_t *bval = Seq_get(registers, data.b);
        uint32_t *cval = Seq_get(registers, data.c);    

        assert(*cval != 0);                    
        uint32_t result = (*bval / *cval);

        *(uint32_t*) Seq_get(registers, data.a) = result;
}


/**********Bitwise NAND********
 * Places the value of  ¬($r[B] ∧ $r[C]) into $r[A]
 * Inputs: 
 *	- 32 bit word instruction
 *	- C Array of registers   
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void BitwiseNAND(Seq_T registers, uint32_t word)
{
        struct values data = unpackWord(word);    
        uint32_t *bval = Seq_get(registers, data.b);
        uint32_t *cval = Seq_get(registers, data.c);                      
        uint32_t result = ~(*bval & *cval);

        *(uint32_t*) Seq_get(registers, data.a) = result;
}


/**********LoadValue********
 * Place the value stored in the 25 least significant bits of the word 
 *      into $r[A]
 * Inputs: 
 *	- 32 bit word instruction
 *	- C Array of registers   
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void loadValue(Seq_T registers, uint32_t word)
{
        uint32_t aval =  Bitpack_getu(word, WIDTH, 25);
        uint32_t loadval = Bitpack_getu(word, LVWIDTH, 0);
        
        *(uint32_t*) Seq_get(registers, aval) = loadval;

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
        struct values final;
        final.a = Bitpack_getu(word, WIDTH, ALSB);
        final.b = Bitpack_getu(word, WIDTH, BLSB);
        final.c = Bitpack_getu(word, WIDTH, CLSB);
        return final;
}
