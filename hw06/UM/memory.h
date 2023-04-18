/*
 *     memory.h
 *     by Vir Bhatia (vbhati02) and Max Regardie (mregar01), 
 *     4/13/23
 *     
 *     UM
 *
 *     This is the header file for the memory module. This module creates 
 *     memory segments and performs all operations involving loading, mapping,
 *     and creating memory segments
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>
#include "basicInstructions.h"

/*struct that is used for storing a single segment in the sequence of 
segments*/
struct segment {
        bool mapped;
        Seq_T words;
};

Seq_T initializeSegments(char *filename);

void readWords(Seq_T sequence, FILE *fp);

void freeSegments(Seq_T sequence);

void segmentedLoad(Seq_T registers, Seq_T segments, uint32_t word);

void segmentedStore(Seq_T registers, Seq_T segments, uint32_t word);

void mapSegment(Seq_T registers, Seq_T segments, Seq_T availability, 
                uint32_t word);

void unmapSegment(Seq_T registers, Seq_T segments, Seq_T availability, 
                uint32_t word);

int loadProgram(Seq_T registers, Seq_T segments, uint32_t word);