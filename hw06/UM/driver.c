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
        
        /*initialize all registers, segments, and availabilty sequences*/
        Seq_T registers = initializeRegisters();
        Seq_T segments = initializeSegments(argv[1]);
        /*this sequence stores the slots in the segments sequence that
        have been unmapped and can now be mapped on*/
        Seq_T availability = Seq_new(0);

        commandLoop(registers, segments, availability);
        
        /*frees all allocated memory*/
        freeSegments(segments);
        freeSequence(registers);
        freeSequence(availability);
        exit(0);
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
void freeSequence(Seq_T sequence)
{
        int size = Seq_length(sequence);
        for (int i = 0; i < size; i++) {
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
 *      - sequence of availability slots
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void commandLoop(Seq_T registers, Seq_T segments, Seq_T availability)
{
        struct segment *zero = Seq_get(segments, 0);
        int size = Seq_length(zero->words);
        uint32_t *curr;
        uint32_t opcode;
        for (int i = 0; i < size; i++) {
                curr = Seq_get(zero->words, i);
                /*unpack current opcode*/
                opcode = Bitpack_getu(*curr, 4, 28);
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
                        break;
                } else if (opcode == ACTIVATE) {
                        mapSegment(registers, segments, availability, *curr);
                } else if (opcode == INACTIVATE) {
                        unmapSegment(registers, segments, availability, *curr);
                } else if (opcode == OUT) {
                        output(registers, *curr);
                } else if (opcode == IN) {
                        input(registers, *curr);
                } else if (opcode == LOADP) {
                        /*set program counter*/
                        i = loadProgram(registers, segments, *curr);
                        /*reset segment zero*/
                        zero = Seq_get(segments, 0);
                        /*reset size for loop*/
                        size = Seq_length(zero->words);
                } else if (opcode == LV) {
                        loadValue(registers, *curr);
                }
        }
}