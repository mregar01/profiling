/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>




typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc) {
        Um_instruction final = 0;
        final = Bitpack_newu(final, 3, 0, rc);
        final = Bitpack_newu(final, 3, 3, rb);
        final = Bitpack_newu(final, 3, 6, ra);
        final = Bitpack_newu(final, 4, 28, op);
        return final;
}


Um_instruction loadval(unsigned ra, unsigned val) {
        Um_instruction final = 0;
        final = Bitpack_newu(final, 25, 0, val);
        final = Bitpack_newu(final, 3, 25, ra);
        final = Bitpack_newu(final, 4, 28, LV);
        return final;
}


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction cmove(Um_register a, Um_register b, Um_register c) 
{
        return three_register(CMOV, a, b, c);
}

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction multiply(Um_register a, Um_register b, 
                                                        Um_register c) 
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction divide(Um_register a, Um_register b, 
                                                        Um_register c) 
{
        return three_register(DIV, a, b, c);
}

static inline Um_instruction BNAND(Um_register a, Um_register b, 
                                                        Um_register c) 
{
        return three_register(NAND, a, b, c);
}

Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);
}

Um_instruction input(Um_register c)
{
        return three_register(IN, 0, 0, c);
}

Um_instruction map(Um_register b, Um_register c)
{
        return three_register(ACTIVATE, 0, b, c);
}

Um_instruction unmap(Um_register c)
{
        return three_register(INACTIVATE, 0, 0, c);
}

Um_instruction sload(Um_register a, Um_register b, Um_register c)
{
        return three_register(SLOAD, a, b, c);
}

Um_instruction sstore(Um_register a, Um_register b, Um_register c)
{
        return three_register(SSTORE, a, b, c);
}

Um_instruction loadp(Um_register b, Um_register c)
{
        return three_register(LOADP, 0, b, c);
}


/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
}

void build_add_test(Seq_T stream)
{
        append(stream, add(r1, r2, r3));
        append(stream, halt());
}

void build_print_digit_test(Seq_T stream)
{
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 6));
        append(stream, add(r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

void build_load_test(Seq_T stream)
{
        append(stream, loadval(r1, 51));;
        append(stream, output(r1));
        append(stream, halt());
}

void build_conditional_move_true(Seq_T stream)
{
        append(stream, loadval(r0, 0));
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 5));
        append(stream, cmove(r0, r1, r2));
        append(stream, output(r0));
        append(stream, halt());
}

void build_conditional_move_false(Seq_T stream)
{
        append(stream, loadval(r0, 52));
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 0));
        append(stream, cmove(r0, r1, r2));
        append(stream, output(r0));
        append(stream, halt());
}

void build_multiply_test(Seq_T stream)
{
        append(stream, loadval(r1, 5));
        append(stream, loadval(r2, 10));
        append(stream, multiply(r0, r1, r2));
        append(stream, output(r0));
        append(stream, halt());
}

void build_divide_test(Seq_T stream)
{
        append(stream, loadval(r1, 288));
        append(stream, loadval(r2, 6));
        append(stream, divide(r0, r1, r2));
        append(stream, output(r0));
        append(stream, halt());
}

void build_divide_0_test(Seq_T stream)
{
        append(stream, loadval(r1, 289));
        append(stream, loadval(r2, 0));
        append(stream, divide(r0, r1, r2));
        append(stream, output(r0));
        append(stream, halt());
}

void build_divide_floor_test(Seq_T stream)
{
        append(stream, loadval(r1, 289));
        append(stream, loadval(r2, 6));
        append(stream, divide(r0, r1, r2));
        append(stream, output(r0));
        append(stream, halt());
}


	
void build_NAND_test(Seq_T stream) 
{
        append(stream, loadval(r2, 256)); 
        append(stream, loadval(r3, 16777215)); 
        append(stream, multiply(r4, r2, r3));
        append(stream, loadval(r2, 255)); 
        append(stream, add(r1, r4, r2)); 
        append(stream, loadval(r2, 256)); 
        append(stream, loadval(r5, 16777215));
        append(stream, multiply(r6, r2, r5));
        append(stream, loadval(r2, 148)); 
        append(stream, add(r7, r6, r2));
        append(stream, BNAND(r0, r1, r7));
        append(stream, output(r0));
        append(stream, halt());
}

void build_input_test(Seq_T stream)
{
        append(stream, input(r0));
        append(stream, output(r0));
        append(stream, halt());
}

void build_input_add_test(Seq_T stream)
{
        append(stream, input(r0));
        append(stream, loadval(r1, 50));
        append(stream, add(r2, r0, r1));
        append(stream, output(r2)); 
        append(stream, halt());
}

void build_map_test(Seq_T stream)
{
        append(stream, loadval(r2, 50));
        append(stream, loadval(r3, 100));
        append(stream, map(r1, r2));
        append(stream, map(r4, r3));
        append(stream, halt());
}

void build_unmap_test(Seq_T stream)
{
        append(stream, loadval(r2, 50));
        append(stream, map(r1, r2));
        append(stream, unmap(r1));
        append(stream, halt());
}

void build_map_twice_test(Seq_T stream)
{
        append(stream, loadval(r2, 50));
        append(stream, loadval(r3, 100));
        append(stream, loadval(r0, 700));
        append(stream, map(r1, r2));
        append(stream, map(r4, r3));
        append(stream, unmap(r1));
        append(stream, map(r5, r0));
        append(stream, halt());
}

void build_segLoad_test(Seq_T stream)
{
        append(stream, loadval(r0, 51));
        append(stream, loadval(r7, 49));
        append(stream, loadval(r3, 26));
        append(stream, map(r1, r0));
        append(stream, sstore(r1, r3 ,r7));
        append(stream, sload(r5, r1, r3));
        append(stream, output(r5));
        append(stream, halt());
}

void build_segStore_test(Seq_T stream)
{
        append(stream, loadval(r0, 53));
        append(stream, loadval(r7, 50));
        append(stream, loadval(r3, 27));
        append(stream, map(r1, r0));
        append(stream, sstore(r1, r3 ,r7));
        append(stream, sload(r5, r1, r3));
        append(stream, output(r5));
        append(stream, halt());
}

void build_load_program_test(Seq_T stream)
{
        append(stream, input(r1));
        append(stream, BNAND(r2, r1, r1));
        append(stream, loadval(r3, 9));
        append(stream, loadval(r7, 6));
        append(stream, cmove(r3 ,r7 ,r2));
        append(stream, loadp(r0, r3));
        append(stream, output(r1));
        append(stream, loadval(r7, 0));
        append(stream, loadp(r0, r7));
        append(stream, halt());
}


