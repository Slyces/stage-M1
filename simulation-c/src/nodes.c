    
    
#include "nodes.h"
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#define true 1
#define false 0

void NodeCreate(node * newNode, int id, adaptFunction * adaptArray,
        size_t adaptNumber) {
    printf("New node : id = %d, adapt n = %d\n", id ,(int) adaptNumber);
    newNode->id = id;
    newNode->adaptNumber = adaptNumber;
    newNode->adaptArray = adaptArray;
    /* ------------------- creation of the routign table -------------------- */
    newNode->table = NULL;
    /* --------------------- construction of in and out --------------------- */
    size_t inCount = 0, outCount = 0;
    pStack inArray[adaptNumber];
    pStack outArray[adaptNumber];
    int found;
    for (int i = 0; i < adaptNumber; i++) {
        /* --------------- adding new 'in' stacks to an array --------------- */
        found = false;
        pStack inStack;
        AdaptIn(&adaptArray[i], &inStack);

        /*char buff[25];*/
        /*pStackPrint(buff, &inStack);*/
        /*printf("stack: %s\n", buff);*/
        for (int j = 0; j < inCount; j++) {
            if (pStackEquals(inStack, inArray[j]))
                found = true;
        }
        if (!found)
            inArray[inCount++] = inStack;
        /* --------------- adding new 'out' stacks to an array -------------- */
        found = false;
        pStack outStack;
        AdaptOut(&adaptArray[i], &outStack);
        for (int j = 0; j < outCount; j++) {
            if (pStackEquals(outStack, outArray[j]))
                found = true;
        }
        if (!found)
            outArray[outCount++] = outStack;
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

    // printf("/* ------------------------------ in array ------------------------------ */\n");
    // for (int i = 0; i < inCount; i++) {
    //     char buff[200];
    //     pStackPrint(buff, &(newNode->in[i]));
    // }
    /* ------------------------ initialise the stats ------------------------ */
    newNode->confReceived = 0;
    newNode->confSent = 0;
    newNode->messageReceived = 0;
    newNode->messageRouted = 0;
    newNode->messageDiscarded = 0;
    /* -------------------------- return the node --------------------------- */
}

void NodeDestroy(node * someNode) {
    /* --------------------- destroy the routing table ---------------------- */

    /* ------------------- destroy the in and out arrays -------------------- */
    for (int i = 0; i < someNode->inSize; i++) {
        pStackDestroy(&someNode->in[i]); //!\\ problem here
    }
    free(someNode->in);
    for (int i = 0; i < someNode->outSize; i++) {
        pStackDestroy(&someNode->out[i]); //!\\ problem here
    }
    free(someNode->out);
    /* ------------------ destroy the adaptation functions ------------------ */
    free(someNode->adaptArray);
}

/* ──────────────────── initialisation and passive loop ───────────────────── */
void NodeStart(network * net, node * router) {
    /* -------------------- send initialisation messages -------------------- */
    /*printf("Router %d initializing ... ", router->id);*/
    // For each neighbor
    for (int neighbor = 0; neighbor < net->n; neighbor++) {
        if (neighbor != router->id) {
            // for each accepted stack
            for (int j = 0; j < router->inSize; j++) {
                /* -------------- copy of the in-stack to send -------------- */
                char buffer[200];
                pStackPrint(buffer, &router->in[j]);
                // ------------
                pStack * stackPtr = malloc(sizeof(pStack));
                pStackCopy(&router->in[j], stackPtr);
                // ------------
                char bufferNew[200];
                pStackPrint(bufferNew, stackPtr);
                /*  create a conf message saying: "I can join myself for 0" - */
                confMessage * confMsg = malloc(sizeof(confMessage));
                ConfCreate(confMsg, router->id, stackPtr, 0);
                /*printf("trying to send\n");*/
                SendConfMessage(net, router->id, neighbor, confMsg);
            }
        }
    }
    /*printf("Router %d done.\n", router->id);*/
    /* ------------------------- wait for messages -------------------------- */
    NodeWaitMessages(net, router->id);
}

void NodeWaitMessages(network * net, int node_id) {
    /* --------------------- check if messages arrived ---------------------- */
    while (net->running) {
        // try to pop messages
        char pointer[sizeof(void *)];
        size_t bytesRead = pipe_pop(net->consumers[node_id], pointer, 1);
        net->nodes[node_id].last_message = RunTime(net);
        assert(bytesRead == 1);
        physicalMessage * msg = (physicalMessage *) pointer;
        if (msg->type == STOP) {
            PhysicalDestroy(msg);
            break;
        }
        else {
           ReceivePhysical(net, node_id, msg);
        }
    }
}

/* ────────────── emission and reception of physical messages ─────────────── */
void ReceivePhysical(network * net, int receiver, physicalMessage * physMsg) {
    int node_id = IdFromThread(net);
    assert(node_id == receiver);
    node * currentNode = &net->nodes[node_id];
    if (physMsg->type == MSG) {
        message * msg = (message *) physMsg->content;
        if (msg->dest == receiver)
            ReceiveMessage(net, currentNode, msg);
        else
            RouteMessage(net, currentNode, msg);
    } else if (physMsg->type == CONF) {
        ReceiveConf(net, currentNode, (confMessage *) physMsg->content);
    }
    /*PhysicalDestroy(physMsg);*/
}

void SendPhysical(network * net, physicalMessage * msg) {
    char strPhys[200];
    PhysicalPrint(strPhys, msg);
    /*printf("%d pushing [%s] to %d\n", msg->sender, strPhys, msg->receiver);*/
    pipe_push(net->producers[msg->receiver], (void *) msg, 1);
}

/* ──────────────────────────── sending messages ──────────────────────────── */
void SendMessage(network * net, int sender, int receiver, message * msg) {
    physicalMessage * physMsg = malloc(sizeof(physicalMessage));
    PhysicalCreate(physMsg, sender, receiver, MSG, (void *) msg);
    SendPhysical(net, physMsg);
}

void SendConfMessage(network * net, int sender, int receiver, confMessage * msg) {
    char strMsg[200];
    ConfPrint(strMsg, msg);
    printf("%d sending to %d : %s\n", sender, receiver, strMsg);
    net->nodes[sender].confSent++;
    physicalMessage * physMsg = malloc(sizeof(physicalMessage));
    PhysicalCreate(physMsg, sender, receiver, CONF, (void *) msg),
    SendPhysical(net, physMsg);
}

/* ──────────────────── reception / routing of messages ───────────────────── */
void ReceiveConf(network * net, node * router, confMessage * msg) {
    char strMsg[200];
    ConfPrint(strMsg, msg);
    printf("Router (%d) received : %s\n", router->id, strMsg);
    router->confReceived++;
    ConfDestroy(msg);
}

void ReceiveMessage(network * net, node * router, message * msg) {
    router->messageReceived++;
    printf("message for me received\n");
    MessageDestroy(msg);
}

void RouteMessage(network * net, node * router, message * msg) {
    router->messageRouted++;
    printf("msg to route received\n");
}
