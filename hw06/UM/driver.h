/*
 *     driver.h
 *     by Vir Bhatia (vbhati02) and Max Regardie (mregar01), 
 *     4/13/23
 *     
 *     UM
 *
 *     This is the header file for the driver module. This module is used to
 *     initialize a set of registers and segments and then use a command loop to
 *     perform machine instructions
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>
// #include "basicInstructions.h"
#include "IO.h"
#include "memory.h"

Seq_T initializeRegisters();

void printRegisters(Seq_T sequence);

void freeSequence(Seq_T sequence);

void commandLoop(Seq_T registers, Seq_T segments, Seq_T availability);