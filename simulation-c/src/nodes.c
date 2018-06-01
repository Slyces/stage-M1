#include "nodes.h"
#include <stdlib.h>
#include <unistd.h>

#define true 1
#define false 0

node * NodeCreate(int id, adaptFunction * adaptArray, size_t adaptNumber) {
    node * newNode = malloc(sizeof(node));
    newNode->id = id;
    newNode->adaptNumber = adaptNumber;
    newNode->adaptArray = adaptArray;
    /* ------------------- creation of the routign table -------------------- */
    newNode->table = NULL;
    /* --------------------- construction of in and out --------------------- */
    size_t inCount = 0, outCount = 0;
    pStack inArray[adaptNumber], outArray[adaptNumber];
    int found;
    for (int i = 0; i < adaptNumber; i++) {
        /* --------------- adding new 'in' stacks to an array --------------- */
        found = false;
        pStack inStack = * AdaptIn(&adaptArray[i]);
        for (int j = 0; j < inCount; j++) {
            if (pStackEquals(inStack, inArray[j]))
                found = true;
        }
        if (!found) {
            inArray[inCount] = inStack;
        }
        /* --------------- adding new 'out' stacks to an array -------------- */
        found = false;
        pStack outStack = * AdaptOut(&adaptArray[i]);
        for (int j = 0; j < outCount; j++) {
            if (pStackEquals(outStack, outArray[j]))
                found = true;
        }
        if (!found) {
            outArray[inCount] = outStack;
        }
    }
    /* ------------------ allocate the in and out pointers ------------------ */
    newNode->in = malloc(inCount * sizeof(pStack));
    newNode->out = malloc(outCount * sizeof(pStack));
    for (int i = 0; i < inCount; i++) {
        newNode->in[i] = inArray[i];
    }
    for (int i = 0; i < outCount; i++) {
        newNode->out[i] = outArray[i];
    }
    newNode->inSize = inCount;
    newNode->outSize = outCount;
    /* ------------------------ initialise the stats ------------------------ */
    newNode->confReceived = 0;
    newNode->confSent = 0;
    newNode->messageReceived = 0;
    newNode->messageRouted = 0;
    newNode->messageDiscarded = 0;
    /* -------------------------- return the node --------------------------- */
    return newNode;
}

void NodeDestroy(node * someNode) {
    /* --------------------- destroy the routing table ---------------------- */

    /* ------------------- destroy the in and out arrays -------------------- */
    for (int i = 0; i < someNode->inSize; i++) {
        pStackDestroy(&someNode->in[i]);
    }
    free(someNode->in);
    for (int i = 0; i < someNode->outSize; i++) {
        pStackDestroy(&someNode->out[i]);
    }
    free(someNode->out);
    /* ------------------ destroy the adaptation functions ------------------ */
    for (int i = 0; i < someNode->adaptNumber; i++) {
        AdaptDestroy(&someNode->adaptArray[i]);
    }
    free(someNode->adaptArray);
    /* ------------------------ free the node itself ------------------------ */
    free(someNode);
}

void NodeStart(network * net, node * router) {
    /* -------------------- send initialisation messages -------------------- */

    /* ------------------------- wait for messages -------------------------- */
    NodeWaitMessages(net, router);
}

void NodeWaitMessages(network * net, node * router) {
    /* --------------------- check if messages arrived ---------------------- */
    while (net->running) {
        sleep(1);
        printf("I'm %d and I'm awake\n", IdFromThread(net));
    }
}
