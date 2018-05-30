#include "protocols.h"
#include <stdlib.h>

#define true 1
#define false 0

pStack * pStackCreate(size_t size) {
    pStack * stack = malloc(sizeof(pStack));
    stack->size = size;
    stack->top = -1;
    stack->protocols = malloc(size * sizeof(protocol));
    return stack;
}

void pStackDestroy(pStack * stack) {
    free(stack->protocols);
    free(stack);
}

int pStackFull(pStack * stack) {
    return stack->top == stack->size - 1;
}

int pStackEmpty(pStack * stack) {
    return stack->top == -1;
}

void pStackPush(pStack * stack, protocol p) {
    stack->protocols[stack->top++] = p;
}

protocol pStackPop(pStack * stack) {
    return stack->protocols[--stack->top];
}

int pStackEquals(pStack stackA, pStack stackB) {
    if (stackA.size != stackB.size || stackA.top != stackB . top)
        return false;
    for (int i = 0; i <= stackA.top; i++) {
        if (stackA.protocols[i] != stackB.protocols[i])
           return false;
    }
    return true;
}

int pStackIsTop(pStack container, pStack top) {
    if (top.top > container.top)
        return false;
    for (int i = 0; i <= top.top; i++) {
        if (container.protocols[container.top - i] == top.protocols[top.top - i])
           return false; 
    }
    return true;
}
