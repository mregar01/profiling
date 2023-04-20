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
// #include "basicInstructions.h"

void input(uint32_t* registers, uint32_t word);

void output(uint32_t* registers, uint32_t word);

uint64_t shlio(uint64_t word, unsigned bits);

uint64_t shrio(uint64_t word, unsigned bits);