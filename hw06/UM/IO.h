/*
 *     IO.h
 *     by Vir Bhatia (vbhati02) and Max Regardie (mregar01), 
 *     4/14/23
 *     
 *     UM
 *
 *     This is the header file for the IO module. This module reads and
 *     edits registers and handles inputs and outputs from the client.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>

void input(Seq_T registers, uint32_t word);

void output(Seq_T registers, uint32_t word);