/*
 * File: messages.h
 * Author: Simon Lassourreuille <simon.lassourreuille@u-bordeaux.fr>
 * Topic: Definition of messages for a network routing simulation
 */
#ifndef _MESSAGES_H
#define _MESSAGES_H

/* ────────────────────── protocols and protocol stack ────────────────────── */

/* Type: protocol
 * --------------
 * We use a char (1 byte) to code a protocol, as the number of protocols
 * will never go above 2^8
 */
typedef char protocol;

/*
 * Function: protocolsPrint
 * Usage: printf("My protocol stack: %s", protocolsPrint(&pStack));
 * ----------------------------------------------------------------
 * Creates a string representing a given protocol stack.
 */
char * protocolsPrint(protocol * pStack);

/* ─────────────────────────── physical messages ──────────────────────────── */

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
    char type; 
    void * content;
} physicalMessage;

/*
 * Enum: messageType
 * -----------------
 * Enumrate the different types of messages that can be conveyed
 * in the physical layer. It is used to write and read the physical
 * layer.
 */
typedef enum {MSG, CONF} messageType;

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
 * pointer after this function call.
 */
void PhysicalDestroy(physicalMessage * msg);

/*
 * Function: PhysicalPrint
 * Usage: printf("My physical message: %s", PhysicalPrint(&physMsg));
 * ------------------------------------------------------------------
 * Creates a string representing a given physical message.
 * Does not print the content of the message, only its header.
 */
char * PhysicalPrint(physicalMessage * msg);

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
    protocol * pStack;
    size_t stackSize;
    size_t maxStack;
    int cost;
} confMessage;

/*
 * Function: ConfCreate
 * --------------------
 * Creates and allocates memory for a configuration message.
 * This creates and allocate the stack. Do not forget
 * to properly destroy the message after reception.
 */
confMessage * ConfCreate(int dest, protocol * pStack,
        size_t stackSize, size_t maxStack, int cost);

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
char * ConfPrint(confMessage * confMsg);

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
    protocol * pStack;
    size_t stackSize;
    size_t maxStack;
    void * payload;
} message;

/*
 * Function: MessageCreate
 * --------------------
 * Creates and allocates memory for a communication message.
 * This does not create and allocate the payload. Do not forget
 * to properly destroy the message after reception.
 */
confMessage * MessageCreate(int src, int dest, protocol * pStack,
        size_t stackSize, size_t maxStack, void * payload);

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
char * MessagePrint(message * msg);

#endif
