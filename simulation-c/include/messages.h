/*
 * File: messages.h
 * Author: Simon Lassourreuille <simon.lassourreuille@u-bordeaux.fr>
 * Topic: Definition of messages for a network routing simulation
 */
#ifndef _MESSAGES_H
#define _MESSAGES_H

#include <stddef.h>
#include "protocols.h"

/* ─────────────────────────── physical messages ──────────────────────────── */

/*
 * Enum: messageType
 * -----------------
 * Enumrate the different types of messages that can be conveyed
 * in the physical layer. It is used to write and read the physical
 * layer.
 */
typedef enum {MSG, CONF} messageType;

/*
 * Type: physicalMessage
 * ---------------------
 * This type represents the physicial layer of a message,
 * used to send messages between neighbors. It can contain
 * different type of messages, depending on its type attribute.
 */
typedef struct {
    int sender;
    int receiver;
    messageType type; 
    void ** content;
} physicalMessage;

/*
 * Function: PhysicalCreate
 * Usage: physicalMessage * physMessage;
 *        physMessage = PhysicalCreate(sender, receiver, MSG, &message);
 * ---------------------------------------------------------------------
 * sender, receiver: id of the nodes resp. sending and receiving.
 *                   Those nodes are supposed to be directly connected
 * type: type of the message, i.e., an identifier to know how to cast
 *       the content to read the message
 * content: any type of message supported in the messageType enum
 *
 * returns: a pointer to a newly allocated physical message
 * --------------------------------------------------------------------
 * Converts a message to a physical message. Allocates the memorry for 
 * the new object. Do not forget to destroy it.
 */
physicalMessage * PhysicalCreate(int sender, int receiver,
       messageType type, void * content);

/*
 * Function: PhysicalDestroy
 * Usage: PhysicalDestroy(&physMessage);
 * -------------------------------------
 * Frees all memorry associated with a physical message. Do not use the
 * pointer after this function call. It does not free the content.
 */
void PhysicalDestroy(physicalMessage * msg);

/*
 * Function: PhysicalPrint
 * Usage: printf("My physical message: %s", PhysicalPrint(&physMsg));
 * ------------------------------------------------------------------
 * Creates a string representing a given physical message.
 * Does not print the content of the message, only its header.
 */
void PhysicalPrint(char str[], physicalMessage * msg);

/* ───────────────────────── configuration messages ───────────────────────── */

/*
 * Type: confMessage
 * -----------------
 * This type defines a configuration message. Such messages are sent
 * between nodes to initialise and configure their routing table. They
 * are supposed to be sent from one neighbor to another, on a direct link.
 */
typedef struct {
    int dest;
    pStack * protocolStack;
    int cost;
} confMessage;

/*
 * Function: ConfCreate
 * --------------------
 * Creates and allocates memory for a configuration message.
 * This creates and allocate the stack. Do not forget
 * to properly destroy the message after reception.
 */
confMessage * ConfCreate(int dest, pStack * protocolStack, int cost);

/*
 * Function: ConfDestroy
 * ---------------------
 * Frees all memorry associated with a configuration message.
 * This also frees the stack conveyed in the message.
 */
void ConfDestroy(confMessage * confMsg);

/*
 * Function: ConfPrint
 * Usage: printf("My configuration message: %s", ConfPrint(&confMsg));
 * -------------------------------------------------------------------
 * Creates a string representing a given configuration message.
 * Also prints the content of the stack.
 */
void ConfPrint(char str[], confMessage * confMsg);

/* ───────────────────────── communication messages ───────────────────────── */

/*
 * Type: message
 * -----------------
 * This type defines a regular message, used for communication across
 * the network. The content of this messages can be literally anything.
 * The last protocol of the stack (first to be added) determines the type
 * of the content.
 */
typedef struct {
    int src;
    int dest;
    pStack * protocolStack;
    void * payload;
} message;

/*
 * Function: MessageCreate
 * --------------------
 * Creates and allocates memory for a communication message.
 * This does not create and allocate the payload. Do not forget
 * to properly destroy the message after reception.
 */
message * MessageCreate(int src, int dest, pStack * protocolStack,
        void * payload);

/*
 * Function: MessageDestroy
 * ---------------------
 * Frees all memorry associated with a communication message.
 * This does not free the payload. Please detroy the memorry
 * associated with the payload separately.
 */
void MessageDestroy(message * msg);

/*
 * Function: MessagePrint
 * Usage: printf("My communication message: %s", MessagePrint(&confMsg));
 * -------------------------------------------------------------------
 * Creates a string representing a given configuration message.
 * This does not print the payload, only its header.
 */
void MessagePrint(char str[], message * msg);

#endif
