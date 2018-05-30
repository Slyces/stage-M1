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
    protocol * protocols;
    int top;
    size_t size;
} pStack;

/*
 * Function: pStackCreate
 * ----------------------
 * Allocates all necessary structures for the stack
 */
pStack * pStackCreate(size_t size);

/*
 * Funciton: pStackDestroy
 * -----------------------
 * Frees all memorry associated to the stack
 */
void pStackDestroy(pStack * stack);

/*
 * Function: pStackPush, pStackPop
 * -------------------------------
 * Push adds a value at the top of the stack. Pop removes
 * the value at the top and returns it.
 */
void pStackPush(pStack * stack, protocol p);
protocol pStackPop(pStack * stack);

/*
 * Function: pStackFull, pStackEmpty
 * ---------------------------------
 * This functions test if the stack is full (resp. empty).
 * Be sure to call it before pushing or poping.
 */
int pStackFull(pStack * stack);
int pStackEmpty(pStack * stack);

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
char * protocolsPrint(pStack * stack);
#endif
