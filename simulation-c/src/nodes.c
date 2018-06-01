#include "nodes.h"
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

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

/* ──────────────────── initialisation and passive loop ───────────────────── */
void NodeStart(network * net, node * router) {
    /* -------------------- send initialisation messages -------------------- */
    for (int i = 0; i < net->n; i++) {
        if (i != router->id) {
            for (int j = 0; j < router->inSize; j++) {
                pStack
                confMessage * confMsg = ConfCreate(i, &router->in[j], 0);
                physicalMessage * physMsg = PhysicalCreate(router->id, i,
                        CONF, (void *) congMsg);
            }
        }
    }

    /* ------------------------- wait for messages -------------------------- */
    NodeWaitMessages(net, router->id);
}

void NodeWaitMessages(network * net, int node_id) {
    /* --------------------- check if messages arrived ---------------------- */
    while (net->running) {
        // try to pop messages
        int pointer[sizeof(void *)];
        size_t bytesRead = pipe_pop(net->consumers[node_id], pointer, 1);
        assert(bytesRead == sizeof(physicalMessage *));
        ReceivePhysical(net, node_id, (physicalMessage *) pointer);
    }
}

/* ────────────── emission and reception of physical messages ─────────────── */
void ReceivePhysical(network * net, int receiver, physicalMessage * physMsg) {
    int node_id = IdFromThread(net);
    assert(node_id == receiver);
    node * currentNode = net->nodes[node_id];
    if (physMsg->type == MSG) {
        message * msg = (message *) physMsg->content;
        if (msg->dest == receiver)
            ReceiveMessage(net, currentNode, msg);
        else
            RouteMessage(net, currentNode, msg);
    } else if (physMsg->type == CONF) {
        ReceiveConf(net, currentNode, (confMessage *) physMsg->content);
    }
    PhysicalDestroy(physMsg);
}

void SendPhysical(network * net, physicalMessage * msg) {
    pipe_push(net->producers[msg->receiver], (void *) msg, 1);
}

/* ──────────────────────────── sending messages ──────────────────────────── */
void SendMessage(network * net, int sender, int receiver, message * msg) {
    SendPhysical(net, PhysicalCreate(sender, receiver, MSG, (void *) msg));
}

void SendConfMessage(network * net, int sender, int receiver, confMessage * msg) {
    SendPhysical(net, PhysicalCreate(sender, receiver, CONF, (void *) msg));
}

/* ──────────────────── reception / routing of messages ───────────────────── */
void ReceiveConf(network * net, node * router, confMessage * msg) {
    printf("conf received\n");
}

void ReceiveMessage(network * net, node * router, message * msg) {
    printf("message for me received\n");
}

void RouteMessage(network * net, node * router, message * msg) {
    printf("msg to route received\n");
}
