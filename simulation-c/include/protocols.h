/*
 *   File: protocols.h
 * Author: Simon Lassourreuille <simonlassourreuille@u-bordeaux.fr>
 *  Topic: protocols definition for a routing simulation
 * ----------------------------------------------------------------------------
 * This is an interface to a type representing protocols in a simulation.
 * Protocols are coded over 1 byte (char).
 * This file also contains primitives for a protocol stack with maximum size
 * implemented as a chain of characters.
 */
#ifndef _PROTOCOLS_H
#define _PROTOCOLS_H

#include <stddef.h>

/* Type: protocol
 * --------------
 * We use a char (1 byte) to code a protocol, as the number of protocols
 * will never go above 2^8
 */
typedef char protocol;

/*
 * Type: pStack
 * -------------------
 * A stack of protocols requires fewer informations
 * than a classic stack of pointers.
 */
typedef struct {
    protocol * content;
    size_t currentSize;
    size_t maxSize;
} pStack;

/*
 * Function: pStackEquals
 * ----------------------
 * Equality between two stacks, returns true or false
 */
int pStackEquals(pStack stackA, pStack stackB);

/*
 * Function: pStackIsTop
 * Usage: int isTop = pStackIsTop(&stack, &otherStack);
 * ----------------------------------------------------
 * container: a stack to be tested
 * top: a smaller stack than container
 *
 * returns: true if the top of container equals `top`
 * ----------------------------------------------------
 * This functions tests if the top of one stack corresponds
 * to another stack
 */
int pStackIsTop(pStack container, pStack top);

/*
 * Function: protocolsPrint
 * Usage: printf("My protocol stack: %s", protocolsPrint(&pStack, stackSize));
 * ----------------------------------------------------------------
 * Creates a string representing a given protocol stack.
 */
char * protocolsPrint(protocol * pStack, size_t stackSize);
#endif
