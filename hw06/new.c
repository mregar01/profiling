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

// #include "driver.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <seq.h>
#include "bitpack.h"
#include <unistd.h>

#define NUMREGISTERS 8
#define NUMARGUMENTS 2

struct segment {
        bool mapped;
        // Seq_T words;
        uint32_t *words;
};

struct values {
        uint32_t a;
        uint32_t b;
        uint32_t c;
};

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
        struct stat *statbuf;
        stat(argv[1], statbuf);
        // uint32_t *currVal = malloc(sizeof(uint32_t));
        uint32_t currval;
        int index = 0;
        while((c = getc(fp)) != -1) {
                /*gets 8 bits at a time, then adds after 3 iterations*/
                currVal = Bitpack_newu(currVal, 8, counter * 8, (uint64_t) c);
                if (counter == 0) {
                        // Seq_addhi(initial, currVal);

                        counter = 3;
                        currVal = malloc(sizeof(uint32_t));
                        *currVal = 0;
                } else {
                        counter --;
                }  
        }
        /*allocated one extra uint32_t because filelength is unknown*/
        free(currVal);
        
        ///////
        struct segment *segmentZero = malloc(sizeof(struct segment));
        segmentZero->mapped = true;
        segmentZero->words = initial;

        /*adds segment 0 to segments sequence*/
        Seq_addhi(segments, segmentZero);
        fclose(fp);


        Seq_T availability = Seq_new(0);

        // commandLoop(registers, segments, availability);


        struct segment *zero = Seq_get(segments, 0);
        int size = Seq_length(zero->words);
        uint64_t *curr;
        uint32_t opcode;
        for (int i = 0; i < size; i++) {
                curr = Seq_get(zero->words, i);
                /*unpack current opcode*/
                // opcode = Bitpack_getu(*curr, 4, 28);
                // unsigned hi = 32; /* one beyond the most significant bit */

                /* different type of right shift */
                // opcode = shr(shl(*curr, 32), 60);
                opcode = (*curr << 32) >> 60;
                if (opcode == 0) {
                        struct values data;    
                        /* different type of right shift */
                        // data.a = shr(shl(*curr, 55), 61);
                        data.a = (*curr << 55) >> 61;
                        
                        /* different type of right shift */
                        // data.b = shr(shl(*curr, 58), 61);
                        data.b = (*curr << 58) >> 61;
                        
                        /* different type of right shift */
                        // data.c = shr(shl(*curr, 61), 61);
                        data.c = (*curr << 61) >> 61;


                        uint32_t cval = registers[data.c];
                        if (cval != 0) {
                                registers[data.a] = registers[data.b];
                        }
                } else if (opcode == 1) {
                        // segmentedLoad(registers, segments, *curr);
                        struct values data;
                        // unsigned hi = 6; /* one beyond the most significant bit */
                        /* different type of right shift */
                        // data.b = shr(shl(*curr, 58), 61);
                        data.b = (*curr << 58) >> 61;
                        // hi = 0 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        // data.c = shr(shl(*curr, 61), 61);
                        data.c = (*curr << 61) >> 61;
                        // hi = 6 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        // data.a = shr(shl(*curr, 55), 61);
                        data.a = (*curr << 55) >> 61;
                        // uint32_t *bval = Seq_get(registers, data.b);
                        // uint32_t *cval = Seq_get(registers, data.c);
                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];
                        struct segment *currSegment = Seq_get(segments, bval);
                        registers[data.a] = *(uint32_t*) Seq_get(currSegment->words, cval);
                } else if (opcode == 2) {
                        // segmentedStore(registers, segments, *curr);
                        struct values data;
                        // unsigned hi = 3 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        // data.b = shr(shl(*curr, 58), 61);
                        data.b = (*curr << 58) >> 61;
                        // hi = 0 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        // data.c = shr(shl(*curr, 61), 61);
                        data.c = (*curr << 61) >> 61;
                        // hi = 6 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        // data.a = shr(shl(*curr, 55), 61);
                        data.a = (*curr << 55) >> 61;

                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];
                        uint32_t aval = registers[data.a];
                        struct segment *currSegment = Seq_get(segments, aval);
                        /*assigns value in memory to $r[C]*/
                        *(uint32_t*) Seq_get(currSegment->words, bval) = cval;
                } else if (opcode == 3) {
                        // addition(registers, *curr);
                        struct values data;    
                        // unsigned hi = 6 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        // data.a = shr(shl(*curr, 55), 61);
                        data.a = (*curr << 55) >> 61;
                        // hi = 3 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        // data.b = shr(shl(*curr, 58), 61);
                        data.b = (*curr << 58) >> 61;
                        // hi = 0 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        // data.c = shr(shl(*curr, 61), 61);
                        data.c = (*curr << 61) >> 61;
                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];
                        /* Adds values in two registers */
                        uint32_t result = (bval + cval) % 4294967296;
                        // *(uint32_t*) Seq_get(registers, data.a) = result;
                        registers[data.a] = result;
                } else if (opcode == 4) {
                        // multiplication(registers, *curr);
                        struct values data;    
                        // unsigned hi = 6 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        // data.a = shr(shl(*curr, 55), 61);
                        data.a = (*curr << 55) >> 61;
                        // hi = 3 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        // data.b = shr(shl(*curr, 58), 61);
                        data.b = (*curr << 58) >> 61;
                        // hi = 0 + 3; /* one beyond the most significant bit */
                        /* different type of right shift */
                        // data.c = shr(shl(*curr, 61), 61);
                        data.c = (*curr << 61) >> 61;
                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];
                        /* Multiplies values in two registers */                      
                        uint32_t result = (bval * cval) % 4294967296;
                        // *(uint32_t*) Seq_get(registers, data.a) = result;
                        registers[data.a] = result;
                } else if (opcode == 5) {
                        // division(registers, *curr);

                        struct values data;    

                        // unsigned hi = 6 + 3; /* one beyond the most significant bit */

                        /* different type of right shift */
                        // data.a = shr(shl(*curr, 55), 61);
                        data.a = (*curr << 55) >> 61;

                        // hi = 3 + 3; /* one beyond the most significant bit */

                        /* different type of right shift */
                        // data.b = shr(shl(*curr, 58), 61);
                        data.b = (*curr << 58) >> 61;

                        // hi = 0 + 3; /* one beyond the most significant bit */

                        /* different type of right shift */
                        // data.c = shr(shl(*curr, 61), 61);
                        data.c = (*curr << 61) >> 61;

                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];   

                        /* Prevents division by zero */
                        assert(cval != 0);   

                        /* Divides values in two registers */                 
                        uint32_t result = (bval / cval);

                        registers[data.a] = result;
        
                } else if (opcode == 6) {
                        // BitwiseNAND(registers, *curr);
    
                        struct values data;    

                        // unsigned hi = 6 + 3; /* one beyond the most significant bit */

                        /* different type of right shift */
                        // data.a = shr(shl(*curr, 55), 61);
                        data.a = (*curr << 55) >> 61;

                        // hi = 3 + 3; /* one beyond the most significant bit */

                        /* different type of right shift */
                        // data.b = shr(shl(*curr, 58), 61);
                        data.b = (*curr << 58) >> 61;

                        // hi = 0 + 3; /* one beyond the most significant bit */

                        /* different type of right shift */
                        // data.c = shr(shl(*curr, 61), 61);  
                        data.c = (*curr << 61) >> 61;   
                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];               
                        
                        /* Performs bitwise NAND on values in two registers */
                        uint32_t result = ~(bval & cval);

                        // *(uint32_t*) Seq_get(registers, data.a) = result;
                        registers[data.a] = result;
                        
                } else if (opcode == 7) {
                        break;
                } else if (opcode == 8) {
                        // mapSegment(registers, segments, availability, *curr);

                        struct values data;
        
                        /* different type of right shift */
                        // data.b = shr(shl(*curr, 58), 61);
                        data.b = (*curr << 58) >> 61;


                        /* different type of right shift */
                        // data.c = shr(shl(*curr, 61), 61);
                        data.c = (*curr << 61) >> 61;

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
                                registers[data.b] = index;
                        }
                        else {
                                /*if there is no unmapped spot*/
                                Seq_addhi(segments, newSegment);

                                registers[data.b] = (Seq_length(segments) - 1);
                        }
                        
                } else if (opcode == 9) {

                        /* different type of right shift */
                        uint32_t clocation = (*curr << 61) >> 61;

                        uint32_t cval = registers[clocation];
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
                        
                } else if (opcode == 10) {

                        /* different type of right shift */
                        // int cval = shr(shl(*curr, 61), 61);
                        int cval = (*curr << 61) >> 61;

                        char c = registers[cval];
                        putchar(c);

                } else if (opcode == 11) {

                        char c = getchar();

                        if (c == EOF) {
                                c = ~0;
                        } 

                        /* different type of right shift */
                        // int cval = shr(shl(*curr, 61), 61);
                        int cval = (*curr << 61) >> 61;


                        registers[cval] = c;

                } else if (opcode == 12) {
                        /*set program counter*/

                        struct values data;
        
                        /* different type of right shift */
                        // data.b = shr(shl(*curr, 58), 61);
                        data.b = (*curr << 58) >> 61;

                        /* different type of right shift */
                        // data.c = shr(shl(*curr, 61), 61);
                        data.c = (*curr << 61) >> 61;


                  
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
                        
                } else if (opcode == 13) {

                        /* different type of right shift */
                        // uint32_t aval = shr(shl(*curr, 36), 61);
                        uint32_t aval = (*curr << 36) >> 61;


                        /* different type of right shift */
                        // uint32_t loadval = shr(shl(*curr, 39), 39);
                        
                        uint32_t loadval = (*curr << 39) >> 39;
                        
                        
                        
                        registers[aval] = loadval;
                }
        }

        
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
        int Currsize = Seq_length(availability);
        for (int i = 0; i < Currsize; i++) {
                free(Seq_get(availability, i));
        }
        Seq_free(&availability);
        // freeSequence(availability);
        exit(0);
}