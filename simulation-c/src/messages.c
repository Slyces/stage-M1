#include "messages.h"
#include <stdlib.h>
#include <stdio.h>

/* ─────────────────────────── physical messages ──────────────────────────── */
physicalMessage * PhysicalCreate(int sender, int receiver,
       messageType type, void * content) {
    physicalMessage * physMsg = malloc(sizeof(physicalMessage));
    physMsg->sender = sender;
    physMsg->receiver = receiver;
    physMsg->type = type;
    physMsg->content = content;
    return physMsg;
}

void PhysicalDestroy(physicalMessage * msg) {
    free(msg);
}

void PhysicalPrint(char str[], physicalMessage * msg) {
    sprintf(str, "%d → %d, (content)", msg->sender, msg->receiver);
}

/* ───────────────────────── configuration messages ───────────────────────── */
confMessage * ConfCreate(int dest, pStack * protocolStack, int cost) {
    confMessage * msg = malloc(sizeof(confMessage));
    msg->dest = dest;
    msg->cost = cost;
    msg->protocolStack = protocolStack;
    return msg;
}

void ConfDestroy(confMessage * msg) {
    pStackDestroy(msg->protocolStack);
    /*free(msg);*/
}

void ConfPrint(char str[], confMessage * msg) {
    char strStack[msg->protocolStack->size + 20];
    pStackPrint(strStack, msg->protocolStack);
    sprintf(str, "∙ → %d : [%d] %s", msg->dest, msg->cost, strStack);
}

/* ───────────────────────── communication messages ───────────────────────── */
message * MessageCreate(int src, int dest, pStack * protocolStack,
        void * payload) {
    message * msg = malloc(sizeof(message));
    msg->src = src;
    msg->dest = dest;
    msg->protocolStack = protocolStack;
    msg->payload = payload;
    return msg;
}

void MessageDestroy(message * msg) {
    pStackDestroy(msg->protocolStack);
    /*free(msg);*/
}
