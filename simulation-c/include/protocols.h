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

/* Type: protocol
 * --------------
 * We use a char (1 byte) to code a 
 * protocol, as the number of protocols
 * will never go above 2^8
 */
typedef char protocol;

/*
 * Type: pStack
 * -------------------
 * A stack of protocols requires fewer informations
 * than a classic stack of pointers. This types gives
 * an interface to the most basic stack manipulations.
 */
typedef struct {
    protocol * content;
    int maxSize;
    int top;
} pStack;

/*
 * Function: pStackinit
 * Usage: pStackinit(&stack, maxSize);
 * -----------------------------------
 * Allocates memory for the protocol stack. The initial
 * stack is empty.
 */
void pStackInit(pStack * stack, int maxSize);

/*
 * Function: pStackDestroy
 * Usage: pStackDestroy(&stack);
 * -----------------------------
 * Frees all memory associated to the stack. Be carefull
 * not to use the variable after destruction.
 */
void pStackDestroy(pStack * stack);

/*
 * Function: pStackPush, pStackPop
 * Usage: pStackpush(&stack, element);
 *        element = pStackPop(&stack);
 * -----------------------------------
 * Elementary operations to add and remove / retrieve
 * elements from the stack. be carefull not to remove
 * elements from an empty stack (see pStackEmpty) or
 * add elements to a full stack (see pStackFull)
 */
void pStackPush(pStack * stack, protocol p);
protocol pStackPop(pStack * stack);

/*
 * Function: pStackGet
 * Usage: element = pStackget(index);
 * -------------------------------
 * Non-conventional function to access the value of an
 * element of the stack. A positive index, starting from
 * 0. If a negative number is given, looks up from the
 * end of the stack. It is an error if the index does not
 * exist.
 */
protocol pStackget(int index);

#endif
