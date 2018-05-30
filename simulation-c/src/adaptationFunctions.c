#include "adaptationFunctions.h"
#include <stdlib.h>

#define true 1
#define false 0

adaptFunction * AdaptCreate(protocol in, protocol out, adaptType type) {
    adaptFunction * function = malloc(sizeof(adaptFunction));
    function->in = in;
    function->out = out;
    function->type = type;
    return function;
}

void AdaptDestroy(adaptFunction * function) {
    free(function);
}

adaptFunction * AdaptReverse(adaptFunction * function) {
    adaptType newType;
    switch(function->type) {
        case EC:
           newType = DC; break;
        case DC:
           newType = EC; break;
        default: //CV
           newType = CV;
    }
    return AdaptCreate(function->out, function->in, newType);
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

pStack * AdaptIn(adaptFunction * function) {
    pStack * stack = pStackCreate(2);
    if (function->type == DC)
        pStackPush(stack, function->out);
    pStackPush(stack, function->in);
    return stack;
}

pStack * AdaptOut(adaptFunction * function) {
    pStack * stack = pStackCreate(2);
    if (function->type == EC)
        pStackPush(stack, function->in);
    pStackPush(stack, function->out);
    return stack;
}
