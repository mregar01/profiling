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
#include <sys/types.h>
#include <sys/stat.h>

#define NUMREGISTERS 8
#define NUMARGUMENTS 2

struct segment {
        bool mapped;
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
        FILE *fp = fopen(argv[1], "r");
        assert(fp != NULL);
        
        Seq_T segments = Seq_new(0);

        int c;
        int counter = 3;
        struct stat *statbuf = malloc(sizeof(struct stat));
        stat(argv[1], statbuf);
        int wordsize = statbuf->st_size;
        free(statbuf);
        uint32_t currVal = 0;
        uint32_t* initial = malloc(wordsize + sizeof(uint32_t));
        int index = 1;
        while((c = getc(fp)) != -1) {
                /*gets 8 bits at a time, then adds after 3 iterations*/
                currVal = Bitpack_newu(currVal, 8, counter * 8, (uint64_t) c);
                if (counter == 0) {
                        initial[index] = currVal;
                        counter = 3;
                        currVal = 0;
                        index++;
                } else {
                        counter --;
                }  
        }
        initial[0] = index - 1;
        
        struct segment *segmentZero = malloc(sizeof(struct segment));
        segmentZero->mapped = true;
        segmentZero->words = initial;

        Seq_addhi(segments, segmentZero);
        fclose(fp);


        Seq_T availability = Seq_new(0);

        struct segment *zero = Seq_get(segments, 0);
        int size = initial[0];
        uint64_t curr;
        uint32_t opcode;
        for (int i = 1; i < size + 1; i++) {
                curr = zero->words[i];
                
                opcode = (curr << 32) >> 60;
                if (opcode == 0) {
                        struct values data;    
                        data.a = (curr << 55) >> 61;
                        data.b = (curr << 58) >> 61;
                        data.c = (curr << 61) >> 61;

                        uint32_t cval = registers[data.c];
                        if (cval != 0) {
                                registers[data.a] = registers[data.b];
                        }
                } else if (opcode == 1) {
                        struct values data;
                        data.b = (curr << 58) >> 61;
                        data.c = (curr << 61) >> 61;
                        data.a = (curr << 55) >> 61;

                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];
                        struct segment *currSegment = Seq_get(segments, bval);
                        registers[data.a] = currSegment->words[cval + 1];
                } else if (opcode == 2) {
                        struct values data;
                        data.b = (curr << 58) >> 61;
                        data.c = (curr << 61) >> 61;
                        data.a = (curr << 55) >> 61;

                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];
                        uint32_t aval = registers[data.a];
                        struct segment *currSegment = Seq_get(segments, aval);
                        
                        currSegment->words[bval + 1] = cval;
                } else if (opcode == 3) {
                        struct values data;    
                        data.a = (curr << 55) >> 61;
                        data.b = (curr << 58) >> 61;
                        data.c = (curr << 61) >> 61;

                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];
                        uint32_t result = (bval + cval) % 4294967296;
                        registers[data.a] = result;
                } else if (opcode == 4) {
                        struct values data;    
                        data.a = (curr << 55) >> 61;
                        data.b = (curr << 58) >> 61;
                        data.c = (curr << 61) >> 61;

                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];
                        uint32_t result = (bval * cval) % 4294967296;
                        registers[data.a] = result;
                } else if (opcode == 5) {
                        struct values data;    
                        data.a = (curr << 55) >> 61;
                        data.b = (curr << 58) >> 61;
                        data.c = (curr << 61) >> 61;

                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];   

                        assert(cval != 0);   
                        uint32_t result = (bval / cval);
                        registers[data.a] = result;
                } else if (opcode == 6) {
                        struct values data;    
                        data.a = (curr << 55) >> 61;
                        data.b = (curr << 58) >> 61;
                        data.c = (curr << 61) >> 61;   

                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];               
                        uint32_t result = ~(bval & cval);
                        registers[data.a] = result;
                } else if (opcode == 7) {
                        break;
                } else if (opcode == 8) {
                        struct values data;
                        data.b = (curr << 58) >> 61;
                        data.c = (curr << 61) >> 61;

                        uint32_t cval = registers[data.c];
                        uint32_t *initial = calloc(cval + 1, sizeof(uint32_t));
                        initial[0] = cval;

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
                                Seq_addhi(segments, newSegment);
                                registers[data.b] = (Seq_length(segments) - 1);
                        }
                        
                } else if (opcode == 9) {
                        uint32_t clocation = (curr << 61) >> 61;

                        uint32_t cval = registers[clocation];
                        struct segment *currSegment = Seq_get(segments, cval);

                        free(currSegment->words);

                        currSegment->mapped = false;
                        currSegment->words = NULL;

                        uint32_t *slot = malloc(sizeof(uint32_t));
                        *slot = cval;
                        Seq_addhi(availability, slot);
                        
                } else if (opcode == 10) {
                        int cval = (curr << 61) >> 61;
                        char c = registers[cval];
                        putchar(c);

                } else if (opcode == 11) {

                        char c = getchar();

                        if (c == EOF) {
                                c = ~0;
                        } 
                        int cval = (curr << 61) >> 61;
                        registers[cval] = c;

                } else if (opcode == 12) {
                        struct values data;
                        data.b = (curr << 58) >> 61;
                        data.c = (curr << 61) >> 61;

                        uint32_t cval = registers[data.c];
                        uint32_t bval = registers[data.b];
                        
                        if (bval != 0) {
                                struct segment* currSegment = Seq_get(segments, bval);
                                int segmentlength = currSegment->words[0] + 1;

                                uint32_t *newSegmentZero = malloc((segmentlength) * sizeof(uint32_t));
                                
                                for (int i = 0; i < segmentlength; i++) {
                                        newSegmentZero[i] = currSegment->words[i];
                                }

                                struct segment* oldZero = Seq_get(segments, 0);
                                free(oldZero->words);

                                oldZero->words = newSegmentZero;
                        }
                        
                        i = cval;
                        zero = Seq_get(segments, 0);
                        size = zero->words[0];
                        
                } else if (opcode == 13) {
                        uint32_t aval = (curr << 36) >> 61;
                        uint32_t loadval = (curr << 39) >> 39;
                        registers[aval] = loadval;
                }
        }

        int numsegments = Seq_length(segments);
        
        for (int i = 0; i < numsegments; i++) {
                struct segment *curr = Seq_get(segments, i);
                if (curr->mapped) {
                        free(curr->words);
                }
                free(curr);

        }
        Seq_free(&segments);
        free(registers);
        int Currsize = Seq_length(availability);
        for (int i = 0; i < Currsize; i++) {
                free(Seq_get(availability, i));
        }
        Seq_free(&availability);
        exit(0);
}