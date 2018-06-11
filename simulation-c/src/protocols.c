#include "protocols.h"
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

void pStackCreate(pStack * stack, size_t size) {
    stack->size = size;
    stack->top = -1;
    stack->protocols = malloc(size * sizeof(protocol));
}

void pStackDestroy(pStack * stack) {
    free(stack->protocols);
}

int pStackFull(pStack * stack) {
    return stack->top == stack->size - 1;
}

int pStackEmpty(pStack * stack) {
    return stack->top == -1;
}

void pStackPush(pStack * stack, protocol p) {
    stack->protocols[++stack->top] = p;
}

protocol pStackPop(pStack * stack) {
    return stack->protocols[stack->top--];
}

int pStackEquals(pStack stackA, pStack stackB) {
    if (stackA.size != stackB.size || stackA.top != stackB.top)
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

void pStackCopy(pStack * origin, pStack * copy) {
    pStackCreate(copy, origin->size);
    copy->top = origin->top;
    memcpy(copy->protocols, origin->protocols, origin->size * sizeof(protocol));
}

#include <stdio.h>
void pStackPrint(char str[], pStack * stack) {
    str[0] = '\'';
    for (int i = 0; i <= stack->top; i++) {
        str[1 + i] = stack->protocols[i];
    }
    str[2 + stack->top] = '\'';
    str[3 + stack->top] = '\0';
}
