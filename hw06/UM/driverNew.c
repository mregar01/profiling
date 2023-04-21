/*
 *     driver.h
 *     by Vir Bhatia (vbhati02) and Max Regardie (mregar01), 
 *     4/13/23
 *     
 *     UM
 *
 *     This is the implementation file for the driver module. This module is 
 *     used to initialize a set of registers and segments and then use a 
 *     command loop to perform machine instructions
 *
 */

#include "driver.h"

#define NUMREGISTERS 8
#define NUMARGUMENTS 2

/*enum that is used for opcodes*/
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


int main(int argc, char *argv[]){
        assert(argc == NUMARGUMENTS);
        if(argc != NUMARGUMENTS){
                fprintf(stderr, "Bad Format!\n");
                exit(EXIT_FAILURE);
        }
        
        uint32_t *registers = calloc(NUMREGISTERS, sizeof(uint32_t));
        // Seq_T segments = initializeSegments(argv[1]);
        /*this sequence stores the slots in the segments sequence that
        have been unmapped and can now be mapped on*/

        FILE *fp = fopen(argv[1], "r");
        assert(fp != NULL);
        
        Seq_T segments = Seq_new(0);
        Seq_T initial = Seq_new(0);

        /*takes in words from a file*/


        // readWords(initial, fp);

        int c;
        int counter = 3;
        uint32_t *curr = malloc(sizeof(uint32_t));
        while((c = getc(fp)) != -1) {
                /*gets 8 bits at a time, then adds after 3 iterations*/
                *curr = Bitpack_newu(*curr, 8, counter * 8, (uint64_t) c);
                if (counter == 0) {
                        Seq_addhi(initial, curr);
                        counter = 3;
                        curr = malloc(sizeof(uint32_t));
                        *curr = 0;
                } else {
                        counter --;
                }  
        }
        /*allocated one extra uint32_t because filelength is unknown*/
        free(curr);
        
        ///////
        struct segment *segmentZero = malloc(sizeof(struct segment));
        segmentZero->mapped = true;
        segmentZero->words = initial;

        /*adds segment 0 to segments sequence*/
        Seq_addhi(segments, segmentZero);
        fclose(fp);


        Seq_T availability = Seq_new(0);

        commandLoop(registers, segments, availability);
        
        /*frees all allocated memory*/
        // freeSegments(segments);
        int numsegments = Seq_length(segments);
        
        for (int i = 0; i < numsegments; i++) {
                /*free overall segments*/
                struct segment *curr = Seq_get(segments, i);
                if (curr->mapped) {
                        /*free each individual segments*/
                        int segmentlength = Seq_length(curr->words);
                        for (int j = 0; j < segmentlength; j++) {
                                free(Seq_get(curr->words, j));
                        }
                
                        Seq_free(&curr->words);
                }
                free(curr);

        }
        Seq_free(&segments);
        //
        free(registers);
        // freeSequence(registers);
        int size = Seq_length(availability);
        for (int i = 0; i < size; i++) {
                free(Seq_get(availability, i));
        }
        Seq_free(&availability);
        // freeSequence(availability);
        exit(0);
}


/**********initializeRegisters********
 * initializes a sequence of registers to hold all 0's
 * Inputs: 
 * Return: sequence of registers
 * Expects: 
 * Notes: 
 ************************/
// Seq_T initializeRegisters()
// {
//         uint32_t *curr;
//         Seq_T registers = Seq_new(NUMREGISTERS);
//         for (int i = 0; i < NUMREGISTERS; i++){
//                 curr = malloc(sizeof(int32_t));
//                 *curr = 0;
//                 Seq_addhi(registers, curr);
//         }
//         return registers;
        
// }

/**********printRegisters********
 * takes a sequence of registers and prints all values(used for testing)
 * Inputs: 
 *      - sequence of registers
 * Return: none
 * Expects: 
 * Notes: THIS WAS USED FOR TESTING, COMMENTED OUT NOW
 ************************/
// void printRegisters(Seq_T sequence) 
// {
//         for (int i = 0; i < NUMREGISTERS; i++) {
//                 uint32_t *curr = Seq_get(sequence, i);
//                 printf("$r[%d]: %d\n", i, *curr);
//         }    
// }


/**********freeSequence********
 * frees all memory allocated in a Sequence
 * Inputs: 
 *      - Seq_T
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
// void freeSequence(Seq_T sequence)
// {
//         int size = Seq_length(sequence);
//         for (int i = 0; i < size; i++) {
//                 free(Seq_get(sequence, i));
//         }
//         Seq_free(&sequence);
// }


/**********commandLoop********
 * main driver for the program. Continuosly processes input words from segment
 * zero and calls helper functions based on OPCODEs
 * Inputs: 
 *      - sequence of registers
 *      - sequence of segments
 *      - sequence of availability slots
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void commandLoop(uint32_t *registers, Seq_T segments, Seq_T availability)
{
        struct segment *zero = Seq_get(segments, 0);
        int size = Seq_length(zero->words);
        uint32_t *curr;
        uint32_t opcode;
        for (int i = 0; i < size; i++) {
                curr = Seq_get(zero->words, i);
                /*unpack current opcode*/
                // opcode = Bitpack_getu(*curr, 4, 28);
                unsigned hi = 28 + 4; /* one beyond the most significant bit */

                /* different type of right shift */
                opcode = shr(shl(*curr, 64 - hi), 64 - 4);
                if (opcode == CMOV) {
                        // conditionalMove(registers, *curr);
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
                        // uint32_t *cval = Seq_get(registers, data.c); 
                        uint32_t cval = registers[data.c];
                        /* Checks if value in r[c] is 0 before executing move*/
                        if (cval != 0) {
                                // *(uint32_t*) Seq_get(registers, data.a) = 
                                //                 *(uint32_t*) Seq_get(registers, data.b);
                                registers[data.a] = registers[data.b];
                        }
                } else if (opcode == SLOAD) {
                        // segmentedLoad(registers, segments, *curr);
                        struct values data;
                        unsigned hi = 3 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        data.b = shr(shl(*curr, 64 - hi), 64 - 3);
                        hi = 0 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        data.c = shr(shl(*curr, 64 - hi), 64 - 3);
                        hi = 6 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        data.a = shr(shl(*curr, 64 - hi), 64 - 3);
                        // uint32_t *bval = Seq_get(registers, data.b);
                        // uint32_t *cval = Seq_get(registers, data.c);
                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];
                        struct segment *currSegment = Seq_get(segments, bval);
                        registers[data.a] = *(uint32_t*) Seq_get(currSegment->words, cval);
                } else if (opcode == SSTORE) {
                        // segmentedStore(registers, segments, *curr);
                        struct values data;
                        unsigned hi = 3 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        data.b = shr(shl(*curr, 64 - hi), 64 - 3);
                        hi = 0 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        data.c = shr(shl(*curr, 64 - hi), 64 - 3);
                        hi = 6 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        data.a = shr(shl(*curr, 64 - hi), 64 - 3);
                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];
                        uint32_t aval = registers[data.a];
                        struct segment *currSegment = Seq_get(segments, aval);
                        /*assigns value in memory to $r[C]*/
                        *(uint32_t*) Seq_get(currSegment->words, bval) = cval;
                } else if (opcode == ADD) {
                        // addition(registers, *curr);
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
                        uint32_t result = (bval + cval) % 4294967296;
                        // *(uint32_t*) Seq_get(registers, data.a) = result;
                        registers[data.a] = result;
                } else if (opcode == MUL) {
                        // multiplication(registers, *curr);
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
                        uint32_t result = (bval * cval) % 4294967296;
                        // *(uint32_t*) Seq_get(registers, data.a) = result;
                        registers[data.a] = result;
                } else if (opcode == DIV) {
                        // division(registers, *curr);

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

                        /* Prevents division by zero */
                        assert(cval != 0);   

                        /* Divides values in two registers */                 
                        uint32_t result = (bval / cval);

                        registers[data.a] = result;
        
                } else if (opcode == NAND) {
                        // BitwiseNAND(registers, *curr);
    
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
                        
                        /* Performs bitwise NAND on values in two registers */
                        uint32_t result = ~(bval & cval);

                        // *(uint32_t*) Seq_get(registers, data.a) = result;
                        registers[data.a] = result;
                        
                } else if (opcode == HALT) {
                        break;
                } else if (opcode == ACTIVATE) {
                        // mapSegment(registers, segments, availability, *curr);

                        struct values data;
        
                        unsigned hi = 3 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        data.b = shr(shl(*curr, 64 - hi), 64 - 3);

                        hi = 0 + 3; /* one beyond the most significant bit */

                        /* different type of right shift */
                        data.c = shr(shl(*curr, 64 - hi), 64 - 3);


                        uint32_t cval = registers[data.c];

                        Seq_T initial = Seq_new(cval);
                        
                        /*makes a new empty segment*/
                        for (uint32_t i = 0; i < cval; i++) {
                                uint32_t *curr = malloc(sizeof(uint32_t));
                                *curr = 0;
                                Seq_addhi(initial, curr);
                        }

                        struct segment *newSegment = malloc(sizeof(struct segment));
                        newSegment->mapped = true;
                        newSegment->words = initial;


                        /*checks where to map the new segment to*/
                        if (Seq_length(availability) != 0) {
                                /*if there is an unmapped spot*/
                                int index = *(uint32_t*)Seq_get(availability, 0);
                                free(Seq_remlo(availability));
                                struct segment *oldval = Seq_put(segments, index, newSegment);
                                free(oldval);
                                // *(uint32_t*) Seq_get(registers, data.b) = index;
                                registers[data.b] = index;
                        }
                        else {
                                /*if there is no unmapped spot*/
                                Seq_addhi(segments, newSegment);
                                // *(uint32_t*) Seq_get(registers, data.b) = 
                                //                                 (Seq_length(segments) - 1);

                                registers[data.b] = (Seq_length(segments) - 1);
                        }
                        
                } else if (opcode == INACTIVATE) {
                        // unmapSegment(registers, segments, availability, *curr);

                        unsigned hi = 0 + 3; /* one beyond the most significant bit */

                        /* different type of right shift */
                        uint32_t clocation = shr(shl(*curr, 64 - hi), 64 - 3);

                        uint32_t cval = registers[clocation];
                        // uint32_t bval = *registers[data.b];
                        struct segment *currSegment = Seq_get(segments, cval);

                        int segmentlength = Seq_length(currSegment->words);

                        /*frees old segment*/
                        for (int i = 0; i < segmentlength; i++) {
                                free(Seq_get(currSegment->words, i));
                        }
                        Seq_free(&currSegment->words);

                        currSegment->mapped = false;
                        currSegment->words = NULL;

                        /*adds unmapped slot to availability sequence*/
                        uint32_t *slot = malloc(sizeof(uint32_t));
                        *slot = cval;
                        Seq_addhi(availability, slot);
                        
                } else if (opcode == OUT) {
                        // output(registers, *curr);

                        unsigned hi = 0 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        int cval = shr(shl(*curr, 64 - hi), 64 - 3);

                        char c = registers[cval];
                        assert(c > -1);
                        putchar(c);

                } else if (opcode == IN) {
                        // input(registers, *curr);

                        char c = getchar();

                        if (c == EOF) {
                                c = ~0;
                        } 

                        unsigned hi = 0 + 3; /* one beyond the most significant bit */
                        assert(hi <= 64);
                        /* different type of right shift */
                        int cval = shr(shl(*curr, 64 - hi), 64 - 3);

                        registers[cval] = c;

                } else if (opcode == LOADP) {
                        /*set program counter*/
                        // i = loadProgram(registers, segments, *curr);

                        struct values data;
        
                        unsigned hi = 3 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        data.b = shr(shl(*curr, 64 - hi), 64 - 3);

                        hi = 0 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        data.c = shr(shl(*curr, 64 - hi), 64 - 3);

                  
                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];
                        
                        /*checks if it is just jumping within segment 0*/
                        if (bval != 0) {

                                struct segment* currSegment = Seq_get(segments, bval);
                                int segmentlength = Seq_length(currSegment->words);
                                Seq_T newSegmentZero = Seq_new(segmentlength);
                                
                                /*duplicates a segment*/
                                for (int i = 0; i < segmentlength; i++) {
                                        uint32_t *currVal = malloc(sizeof(uint32_t));
                                        *currVal = *(uint32_t*)Seq_get(currSegment->words, i);
                                        Seq_addhi(newSegmentZero, currVal);
                                }

                                struct segment* oldZero = Seq_get(segments, 0);
                                int oldZeroLength = (Seq_length(oldZero->words));
                                /*frees old segment zero*/
                                for (int i = 0; i < oldZeroLength; i++) {
                                        free(Seq_get(oldZero->words, i));
                                }

                                oldZero->words = newSegmentZero;
                        }
                        
                        i = cval - 1;
                        
                        /*reset segment zero*/
                        zero = Seq_get(segments, 0);
                        /*reset size for loop*/
                        size = Seq_length(zero->words);
                        
                } else if (opcode == LV) {
                        // loadValue(registers, *curr);
                        unsigned hi = 25 + 3; /* one beyond the most significant bit */

                        /* different type of right shift */
                        uint32_t aval = shr(shl(*curr, 64 - hi), 64 - 3);

                        hi = 0 + 25; /* one beyond the most significant bit */

                        /* different type of right shift */
                        uint32_t loadval = shr(shl(*curr, 64 - hi), 64 - 25);

                        
                        // *(uint32_t*) Seq_get(registers, aval) = loadval;
                        registers[aval] = loadval;
                        }
        }
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