/*
 *     driverNew.h
 *     by Vir Bhatia (vbhati02) and Max Regardie (mregar01), 
 *     4/24/23
 *     
 *     Profile
 *
 *     This is the implementation file for the profiled UM.
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



int main(int argc, char *argv[]){

        if (argc != NUMARGUMENTS){
                fprintf(stderr, "Bad Format!\n");
                exit(1);
        }
        
        uint32_t *registers = calloc(NUMREGISTERS, sizeof(uint32_t));
        FILE *fp = fopen(argv[1], "r");
        assert(fp != NULL);
        
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

                unsigned hi = (counter * 8) + 8;
                
                currVal = (currVal >> hi) << hi                
                        | (currVal << (64 - (counter * 8))) >> (64 - (counter * 8))  
                        | ((uint64_t) c << (counter * 8));      

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

        struct segment *segments = malloc((initial[0] + 2) * sizeof(struct segment));
        
        uint32_t *segLength = malloc(sizeof(uint32_t));
        segLength[0] = 1;
        segments[0].mapped = true;
        segments[0].words = segLength;

        uint32_t *segCapacity = malloc(sizeof(uint32_t));
        segCapacity[0] = index - 1;
        segments[1].mapped = true;
        segments[1].words = segCapacity;
        
        segments[2].mapped = true;
        segments[2].words = initial;

        fclose(fp);


        Seq_T availability = Seq_new(0);

        int size = index - 1;
        uint64_t curr;
        uint32_t opcode;
        for (int i = 1; i < size + 1; i++) {
                curr = segments[2].words[i];
                
                opcode = (curr << 32) >> 60;
                switch(opcode) {
                case 0: ;
                        uint32_t dataa = (curr << 55) >> 61;
                        uint32_t datab = (curr << 58) >> 61;
                        uint32_t datac = (curr << 61) >> 61;

                        uint32_t cval = registers[datac];
                        if (cval != 0) {
                                registers[dataa] = registers[datab];
                        }
                        break;
                case 1: ;
                        uint32_t dataa1 = (curr << 55) >> 61;
                        uint32_t datab1 = (curr << 58) >> 61;
                        uint32_t datac1 = (curr << 61) >> 61;

                        uint32_t cval1 = registers[datac1];
                        uint32_t bval1 = registers[datab1];
                        registers[dataa1] = 
                                        segments[bval1 + 2].words[cval1 + 1];
                        break;
                case 2: ;
                        uint32_t dataa2 = (curr << 55) >> 61;
                        uint32_t datab2 = (curr << 58) >> 61;
                        uint32_t datac2 = (curr << 61) >> 61;

                        uint32_t cval2 = registers[datac2];
                        uint32_t bval2 = registers[datab2];
                        uint32_t aval2 = registers[dataa2];
                        
                        segments[aval2 + 2].words[bval2 + 1] = cval2;
                        break;
                
                case 3: ;
                        uint32_t dataa3 = (curr << 55) >> 61;
                        uint32_t datab3 = (curr << 58) >> 61;
                        uint32_t datac3 = (curr << 61) >> 61;

                        uint32_t cval3 = registers[datac3];
                        uint32_t bval3 = registers[datab3];
                        registers[dataa3] = (bval3 + cval3) % 4294967296;
                        break;
                case 4: ;
                        uint32_t dataa4 = (curr << 55) >> 61;
                        uint32_t datab4 = (curr << 58) >> 61;
                        uint32_t datac4 = (curr << 61) >> 61;

                        uint32_t cval4 = registers[datac4];
                        uint32_t bval4 = registers[datab4];
                        registers[dataa4] = (bval4 * cval4) % 4294967296;
                        break;
                case 5: ;
                        uint32_t dataa5 = (curr << 55) >> 61;
                        uint32_t datab5 = (curr << 58) >> 61;
                        uint32_t datac5 = (curr << 61) >> 61;

                        uint32_t cval5 = registers[datac5];
                        uint32_t bval5 = registers[datab5];   

                        assert(cval5 != 0);   
                        registers[dataa5] = (bval5 / cval5);
                        break;
                case 6: ;
                        uint32_t dataa6 = (curr << 55) >> 61;
                        uint32_t datab6 = (curr << 58) >> 61;
                        uint32_t datac6 = (curr << 61) >> 61;   

                        uint32_t cval6 = registers[datac6];
                        uint32_t bval6 = registers[datab6];               
                        registers[dataa6] = ~(bval6 & cval6);
                        break;
                case 7: ;
                        i = size + 1;
                        break;
                        
                case 8: ;
                        uint32_t datab8 = (curr << 58) >> 61;
                        uint32_t datac8 = (curr << 61) >> 61;

                        uint32_t cval8 = registers[datac8];
                        uint32_t *initial = calloc(cval8 + 1, sizeof(uint32_t));
                        initial[0] = cval8;

                        if (Seq_length(availability) != 0) {
                                int index = 
                                        *(uint32_t*)Seq_get(availability, 0);
                                free(Seq_remlo(availability));

                                segments[index + 2].mapped = true;
                                segments[index + 2].words = initial;
                                registers[datab8] = index;
                        }
                        else {
                                if (segments[0].words[0] >= 
                                                segments[1].words[0]) {
                                        segments = realloc(segments, 
                                        ((segments[1].words[0] + 2) * 
                                        sizeof(struct segment) * 2));
                                        segments[1].words[0] *= 2; 
                                } 
                                segments[segments[0].words[0] + 2].mapped = 
                                                                        true;
                                segments[segments[0].words[0] + 2].words = 
                                                                initial;

                                registers[datab8] = segments[0].words[0];
                                segments[0].words[0]++;
                        }
                        break;   
                case 9: ;
                        uint32_t clocation = (curr << 61) >> 61;

                        uint32_t cval9 = registers[clocation];

                        free(segments[cval9 + 2].words);

                        segments[cval9 + 2].mapped = false;
                        segments[cval9 + 2].words = NULL;

                        uint32_t *slot = malloc(sizeof(uint32_t));
                        *slot = cval9;
                        Seq_addhi(availability, slot);
                        break;    
                case 10: ;
                        int cval10 = (curr << 61) >> 61;
                        char c = registers[cval10];
                        putchar(c);
                        break;
                case 11: ;
                        char c1 = getchar();

                        if (c1 == EOF) {
                                c1 = ~0;
                        } 
                        int cval11 = (curr << 61) >> 61;
                        registers[cval11] = c1;
                        break;
                case 12: ;
                        uint32_t datab12 = (curr << 58) >> 61;
                        uint32_t datac12 = (curr << 61) >> 61;

                        uint32_t cval12 = registers[datac12];
                        uint32_t bval12 = registers[datab12];
                        
                        if (bval12 != 0) {
                                int segmentlength = 
                                        segments[bval12 + 2].words[0] + 1;

                                uint32_t *newSegmentZero = 
                                malloc((segmentlength) * sizeof(uint32_t));
                                
                                for (int i = 0; i < segmentlength; i++) {
                                        newSegmentZero[i] = 
                                                segments[bval12 + 2].words[i];
                                }

                                free(segments[2].words);

                                segments[2].words = newSegmentZero;
                        }
                        
                        i = cval12;
                        size = segments[2].words[0];
                        break;      
                case 13: ;
                        uint32_t aval13 = (curr << 36) >> 61;
                        uint32_t loadval13 = (curr << 39) >> 39;
                        registers[aval13] = loadval13;
                        break;
                }
        }

        int numsegments = segments[0].words[0];
        
        for (int i = 0; i < numsegments + 2; i++) {
                if (segments[i].mapped) {
                        free(segments[i].words);
                }
        }
        
        free(registers);
        free(segments);
        int Currsize = Seq_length(availability);
        for (int i = 0; i < Currsize; i++) {
                free(Seq_get(availability, i));
        }
        Seq_free(&availability);
        exit(0);
}