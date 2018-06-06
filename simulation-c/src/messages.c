#include "messages.h"
#include <stdlib.h>
#include <stdio.h>

/* ─────────────────────────── physical messages ──────────────────────────── */
void PhysicalCreate(physicalMessage * physMsg, int sender,
        int receiver, messageType type, void * content) {
    physMsg->sender = sender;
    physMsg->receiver = receiver;
    physMsg->type = type;
    physMsg->content = content;
}

void PhysicalDestroy(physicalMessage * msg) {
    free(msg);
}

void PhysicalPrint(char str[], physicalMessage * msg) {
    sprintf(str, "%d → %d, (content)", msg->sender, msg->receiver);
}

/* ───────────────────────── configuration messages ───────────────────────── */
void ConfCreate(confMessage * msg, int dest, pStack * protocolStack,
        int cost) {
    msg->dest = dest;
    msg->cost = cost;
    msg->protocolStack = protocolStack;
}

void ConfDestroy(confMessage * msg) {
    pStackDestroy(msg->protocolStack);
    free(msg);
}

void ConfPrint(char str[], confMessage * msg) {
    char strStack[msg->protocolStack->size + 20];
    pStackPrint(strStack, msg->protocolStack);
    sprintf(str, "∙ → %d : [%d] %s", msg->dest, msg->cost, strStack);
}

/* ───────────────────────── communication messages ───────────────────────── */
void MessageCreate(message * msg, int src, int dest, pStack * protocolStack,
        void * payload) {
    msg->src = src;
    msg->dest = dest;
    msg->protocolStack = protocolStack;
    msg->payload = payload;
}

void MessageDestroy(message * msg) {
    pStackDestroy(msg->protocolStack);
    free(msg);
}
