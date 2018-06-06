#include "adaptationFunctions.h"
#include <stdlib.h>
#include <stdio.h>

#define true 1
#define false 0

void AdaptCreate(adaptFunction * function, protocol in, protocol out,
       adaptType type) {
    function->in = in;
    function->out = out;
    function->type = type;
}

void AdaptDestroy(adaptFunction * function) {
    free(function);
}

void AdaptReverse(adaptFunction * function, adaptFunction * reversed) {
    adaptType newType;
    switch(function->type) {
        case EC:
           newType = DC; break;
        case DC:
           newType = EC; break;
        default: //CV
           newType = CV;
    }
    AdaptCreate(reversed, function->out, function->in, newType);
}

int AdaptValid(pStack * stack, adaptFunction * function) {
    protocol in = function->in, out = function->out;
    int top = stack->top;
    if (pStackEmpty(stack))
        return false;
    if (function->type == CV) {
        return  stack->protocols[top] == in;
    } else if (function->type == EC) {
        return !pStackFull(stack) && stack->protocols[top] == in;
    } else if (function->type == DC) {
        return top >= 1 && stack->protocols[top] == in
            && stack->protocols[top - 1] == out;
    }
    return false;
}

void AdaptApply(pStack * stack, adaptFunction * function) {
    if (function->type == CV) {
        pStackPop(stack);
        pStackPush(stack, function->out);
    } else if (function->type == EC) {
        pStackPush(stack, function->out);
    } else if (function->type == DC) {
        pStackPop(stack);
    }
}

void AdaptIn(adaptFunction * function, pStack * stack) {
    pStackCreate(stack, 2);
    /*char buff[25];*/
    /*AdaptPrint(buff, function);*/
    /*printf("In for adapt %s ?\n", buff);*/
    if (function->type == DC)
        pStackPush(stack, function->out);
    pStackPush(stack, function->in);
}

void AdaptOut(adaptFunction * function, pStack * stack) {
    pStackCreate(stack, 2);
    if (function->type == EC)
        pStackPush(stack, function->in);
    pStackPush(stack, function->out);
}

void AdaptPrint(char str[], adaptFunction * function) {
    switch(function->type) {
        case CV:
            sprintf(str, "f(%c → %c)", function->in, function->out);
            break;
        case EC:
            sprintf(str, "f(%c → %c%c)", function->in, function->in, function->out);
            break;
        case DC:
            sprintf(str, "f(%c%c → %c)", function->out, function->in, function->out);
            break;
    }
}
