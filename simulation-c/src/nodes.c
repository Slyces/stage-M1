#include "nodes.h"
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define true 1
#define false 0

node * NodeCreate(int id, adaptFunction * adaptArray, size_t adaptNumber) {
    printf("New node : id = %d, adapt n = %d\n", id ,(int) adaptNumber);
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
        /*char buff[25];*/
        /*pStackPrint(buff, &inStack);*/
        /*printf("stack: %s\n", buff);*/
        for (int j = 0; j < inCount; j++) {
            if (pStackEquals(inStack, inArray[j]))
                found = true;
        }
        if (!found) {
            inArray[inCount++] = inStack;
        }
        /* --------------- adding new 'out' stacks to an array -------------- */
        found = false;
        pStack outStack = * AdaptOut(&adaptArray[i]);
        for (int j = 0; j < outCount; j++) {
            if (pStackEquals(outStack, outArray[j]))
                found = true;
        }
        if (!found) {
            outArray[outCount++] = outStack;
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
    /*printf("In stacks size : %lu\n", someNode->inSize);*/
    for (int i = 0; i < someNode->inSize; i++) {
        /*printf("In stacks destroy\n");*/
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
    /*printf("Router %d initializing ... ", router->id);*/
    // For each neighbor
    for (int neighbor = 0; neighbor < net->n; neighbor++) {
        if (neighbor != router->id) {
            // for each accepted stack
            for (int j = 0; j < router->inSize; j++) {
                // copy the stack, or it will be deleted when the message  arrives
                pStack * stackPtr = pStackCopy(&router->in[j]);
                // create a conf message saying: "I can join myself for 0"
                confMessage * confMsg = ConfCreate(router->id, stackPtr, 0);
                // send it
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
        int pointer[sizeof(void *)];
        size_t bytesRead = pipe_pop(net->consumers[node_id], pointer, sizeof(void *));
        assert(bytesRead == sizeof(void *));
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
    /*printf("Alright there\n");*/
    /*printf("List of some pointers: content %lu", (size_t) physMsg->content);*/
    printf("hellow !!\n");
    ConfDestroy((confMessage *) physMsg->content);
    printf("hellow ??\n");
    PhysicalDestroy(physMsg);
}

void SendPhysical(network * net, physicalMessage * msg) {
    char strPhys[200];
    PhysicalPrint(strPhys, msg);
    /*printf("%d pushing [%s] to %d\n", msg->sender, strPhys, msg->receiver);*/
    pipe_push(net->producers[msg->receiver], (void *) msg, sizeof(void *));
}

/* ──────────────────────────── sending messages ──────────────────────────── */
void SendMessage(network * net, int sender, int receiver, message * msg) {
    SendPhysical(net, PhysicalCreate(sender, receiver, MSG, (void *) msg));
}

void SendConfMessage(network * net, int sender, int receiver, confMessage * msg) {
    /*char strMsg[200];*/
    /*ConfPrint(strMsg, msg);*/
    /*printf("Sending \n");*/
    SendPhysical(net, PhysicalCreate(sender, receiver, CONF, (void *) msg));
}

/* ──────────────────── reception / routing of messages ───────────────────── */
void ReceiveConf(network * net, node * router, confMessage * msg) {
    char strMsg[200];
    ConfPrint(strMsg, msg);
    printf("Router (%d) received : %s\n", router->id, strMsg);
    /*ConfDestroy(msg);*/
}

void ReceiveMessage(network * net, node * router, message * msg) {
    printf("message for me received\n");
    MessageDestroy(msg);
}

void RouteMessage(network * net, node * router, message * msg) {
    printf("msg to route received\n");
}
