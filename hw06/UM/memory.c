/*
 *     memory.c
 *     by Vir Bhatia (vbhati02) and Max Regardie (mregar01), 
 *     4/13/23
 *     
 *     UM
 *
 *     This is the implementation file for the memory module. This module makes 
 *     memory segments and performs all operations involving loading, mapping,
 *     and creating memory segments
 *
 */

#include "memory.h"

#define NUMSEGMENTS 100
#define WIDTH 3
#define ALSB 6
#define BLSB 3
#define CLSB 0

/**********initializeSegments********
 * takes in a filename, opens a file, initializes a sequence of segment structs
 * and returns that sequence
 * Inputs:
 *      - char* with a filename
 * Return: Sequence with one entry which contains a segments struct that has
 *         all of the words from a file in it     
 * Expects: char* to store a valid filename in the directory
 * Notes: 
 ************************/
Seq_T initializeSegments(char *filename)
{
        FILE *fp = fopen(filename, "r");
        assert(fp != NULL);
        
        Seq_T segments = Seq_new(NUMSEGMENTS);
        Seq_T initial = Seq_new(0);

        /*takes in words from a file*/
        readWords(initial, fp);

        struct segment *segmentZero = malloc(sizeof(struct segment));
        segmentZero->mapped = true;
        segmentZero->words = initial;

        /*adds segment 0 to segments sequence*/
        Seq_addhi(segments, segmentZero);
        fclose(fp);
        

        return segments;
}


/**********readWords********
 * Reads all code words from a file and adds them to a sequence
 * Inputs: 
 *      - sequence that will hold code words
 *      - file pointer with code words
 * Return: none
 * Expects: 
 * Notes: alters sequence that was passed in
 ************************/
void readWords(Seq_T sequence, FILE *fp)
{
        int c;
        int counter = 3;
        uint32_t *curr = malloc(sizeof(uint32_t));
        while((c = getc(fp)) != -1) {
                /*gets 8 bits at a time, then adds after 3 iterations*/
                *curr = Bitpack_newu(*curr, 8, counter * 8, (uint64_t) c);
                if (counter == 0) {
                        Seq_addhi(sequence, curr);
                        counter = 3;
                        curr = malloc(sizeof(uint32_t));
                        *curr = 0;
                } else {
                        counter --;
                }  
        }
        /*allocated one extra uint32_t because filelength is unknown*/
        free(curr);
}


/**********freeSegments********
 * frees all memory allocated in the sequence of segments
 * Inputs: 
 *      - sequence of segments
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void freeSegments(Seq_T sequence)
{
        int numsegments = Seq_length(sequence);
        
        for (int i = 0; i < numsegments; i++) {
                /*free overall segments*/
                struct segment *curr = Seq_get(sequence, i);
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
        Seq_free(&sequence);
}

/**********SegmentedLoad********
 * Performs segmented load by assigning $r[A] the value in the location 
 * $m[$r[B]][$r[C]] 
 * Inputs: 
 *	- 32 bit word instruction
 * 	- Seq_T of segments
 *	- Seq_T of registers     
 * Return: none
 * Expects: segment location to be mapped and in bounds
 * Notes: 
 ************************/
void segmentedLoad(uint32_t *registers, Seq_T segments, uint32_t word)
{
        struct values data = unpackWord(word);
        // uint32_t *bval = Seq_get(registers, data.b);
        // uint32_t *cval = Seq_get(registers, data.c);
        uint32_t cval = registers[data.c];
        uint32_t bval = registers[data.b];

        struct segment *currSegment = Seq_get(segments, bval);
        
        /*assigns $r[A] value in memory*/
        // *(uint32_t*) Seq_get(registers, data.a) = 
        //                 *(uint32_t*) Seq_get(currSegment->words, *cval);
                        
        registers[data.a] = *(uint32_t*) Seq_get(currSegment->words, cval);
}

/**********SegmentedStore********
 * Performs segmented store by assigning the value in $r[C] to $m[$r[A]][$r[B]]
 * Inputs: 
 *	- 32 bit word instruction
 * 	- Seq_T of segments
 *	- Seq_T of registers     
 * Return: none
 * Expects: segment location to be mapped and in bounds
 * Notes: 
 ************************/
void segmentedStore(uint32_t *registers, Seq_T segments, uint32_t word)
{
        struct values data = unpackWord(word);
        // uint32_t *aval = Seq_get(registers, data.a);
        // uint32_t *bval = Seq_get(registers, data.b);
        // uint32_t *cval = Seq_get(registers, data.c);
        uint32_t cval = registers[data.c];
        uint32_t bval = registers[data.b];
        uint32_t aval = registers[data.a];

        struct segment *currSegment = Seq_get(segments, aval);
        /*assigns value in memory to $r[C]*/
        *(uint32_t*) Seq_get(currSegment->words, bval) = cval;


}


/**********MapSegment********
 * Creates a new segment of size $r[C], initializes each word to 0, choose 
 * bit pattern identifier and places identifier in $r[b], places new segment in 
 * $m[$r[b]]
 * Inputs: 
 *	- 32 bit word instruction
 * 	- Seq_T of segments
 *	- Seq_T of registers     
 * Return: none
 * Expects: 
 * Notes: 
 ************************/
void mapSegment(uint32_t *registers, Seq_T segments, Seq_T availability, 
                uint32_t word)
{
        struct values data = unpackWord(word);

        // struct values data;
        // data.b = Bitpack_getu(word, WIDTH, BLSB);
        // data.c = Bitpack_getu(word, WIDTH, CLSB); 
        // uint32_t *cval = Seq_get(registers, data.c);
        uint32_t cval = registers[data.c];
        // uint32_t bval = *registers[data.b];
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
}


/**********UnmapSegment********
 * The segment at $m[$r[C]] gets unmapped so new segments can be stored there
 * Inputs: 
 *	- 32 bit word instruction
 * 	- Seq_T of segments
 *	- Seq_T of registers      
 * Return: none
 * Expects: given segment to be mapped and not zero
 * Notes: 
 ************************/
void unmapSegment(uint32_t *registers, Seq_T segments, Seq_T availability,
                  uint32_t word)
{
        struct values data = unpackWord(word);
        // struct values data;
        // data.b = Bitpack_getu(word, WIDTH, BLSB);
        // data.c = Bitpack_getu(word, WIDTH, CLSB); 
        // uint32_t *cval = Seq_get(registers, data.c);
        uint32_t cval = registers[data.c];
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
}


/**********LoadProgram********
 * Duplicate segment $m[$r[B]] replaces $m[0] with it. The program counter 
 * is set to point to $m[0][$r[C]].
 * Inputs: 
 *	- 32 bit word instruction
 * 	- Seq_T of segments
 *	- Seq_T of registers          
 * Return: int value of where to set program counter
 * Expects: given segment to be mapped
 * Notes: 
 ************************/
int loadProgram(uint32_t *registers, Seq_T segments, uint32_t word)
{
        struct values data = unpackWord(word);
        // struct values data;
        // data.b = Bitpack_getu(word, WIDTH, BLSB);
        // data.c = Bitpack_getu(word, WIDTH, CLSB); 
        // uint32_t *bval = Seq_get(registers, data.b);
        // uint32_t *cval = Seq_get(registers, data.c);
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
        return cval - 1;
}