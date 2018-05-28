/*
 * File: adaptationFunctions.h
 * Author: Simon Lassourreuille <simon.lassourreuille@u-bordeaux.fr>
 * Topic: Adaptation functions definition for routing similation
 */
#ifdef _ADAPTATION_FUNCTIONS_H
#define _ADAPTATION_FUNCTIONS_H

#include "protocols.h"

/*
 * Type: adaptFunction
 * -------------------
 * This types defines an adaptation function. An adaptation function
 * is a function that modifies stacks of protocols, in the context
 * of routing a message with multiple encapsulated protocols. Its 
 * primitives can be called on a stack.
 */
typedef struct {
    adaptType type;
    protocol * protocols;
} adaptFunction;

/*
 * Enum: adaptType
 * ---------------
 * There is 3 types of adaptation functions:
 *   - CV: conversion. Converts the top protocol to another protocol
 *
 *     ex: ('x', 'y', EC): `--x` → `--y`
 *
 *   - EC: encapsulation. Encapsulates the stack by adding a new top
 *     protocol. Returns an error if the stack becomes full.
 *
 *     ex: ('x', 'y', EC): `--x` → `--xy`
 *
 *   - DC: decapsulation. Removes the top protocol of the stack.
 *     Returns an error if the stack becomes empty.
 *
 *     ex: ('x', 'y', DC): `--xy` → `--x`
 *
 * In each example, the first argument is the top-protocol of the stack
 * before application of the function, and the second argument is the
 * top protocol after application.
 */
typedef enum {CV, EC, DC} adaptType;

/*
 * Function: AdaptCreate
 * Usage: adaptFunction * func = AdaptCreate(p1, p2, CV);
 * ------------------------------------------------------
 * from: top protocol of the stack before application
 * to: top protocol of the stack after application
 * type: type of functions, see adaptType
 *
 * returns: a pointer to a new adaptFunction structure
 * ------------------------------------------------------
 * Creates and allocates the appropriate structure for an adaptation
 * function.
 */
adaptFunction * AdaptCreate(protocol from, protocol to, adaptType type);

/*
 * Function: AdaptDestroy
 * Usage: AdaptDestroy(&function);
 * -------------------------------
 * Frees the memorry associated to this adaptation function.
 */
void AdaptDestroy(adaptFunction * function);

/*
 * Function: AdaptReverse
 * Usage: adaptFunction * reverse = AdaptReverse(&function);
 * ---------------------------------------------------------
 * function: the function to be reversed
 *
 * returns: a pointer to a new adaptFunction
 * ---------------------------------------------------------
 * Creates the reverse of an adaptation function, i.e., another
 * function such as applying the original then the reverse leaves
 * a stack unchanged.
 * Be carefull : this is a creation, so the resulting variable
 * needs to be destroyed later on.
 *
 * With f the initial function and r its reverse:
 *
 * AdaptApply(&f, &stack); AdaptApply(&r, &stack); 
 *      → stack didn't change
 *
 * Note: if `f` is valid for `stack`, `r` is valid for stack after
 * the application of `f`. `r` may be valid for `stack` right off
 * the bat, but there is no guaratee.
 */
adaptFunction * AdaptReverse(adaptFunction * function);

/*
 * Function: AdaptValid
 * Usage: AdaptValid(&stack, &function);
 * -------------------------------------
 * function: the function to apply
 * stack: the stack to apply the function to
 *
 * returns: true if this function is valid for this stack
 * -------------------------------------
 * Checks if a given function can be applied to a given stack.
 * Depending on its type and protocols, a function can not be
 * applied to every stack.
 */
int AdaptValid(pStack * stack, adaptFunction * function);

/*
 * Function: AdaptApply
 * Usage: AdaptApply(&stack, &function);
 * -------------------------------------
 *
 * -------------------------------------
 * Applys a given function to a given stack. The result of the
 * application depends on the type of the function. There is
 * an error if the stack is not valid for this specific function, 
 * see AdaptValid.
 */
void AdaptApply(pStack * stack, adaptFunction * function);

#endif
