/*
 * File: messages.h
 * Author: Simon Lassourreuille <simon.lassourreuille@u-bordeaux.fr>
 * Topic: Definition of messages for a network routing simulation
 */
#ifndef _MESSAGES_H
#define _MESSAGES_H

#include "protocols.h"

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
 * Function: WritePhysical
 * Usage: physicalMessage * physMessage;
 *        physMessage = WritePhysical(sender, receiver, MSG, &message);
 * --------------------------------------------------------------------
 * Converts a message to a physical message. Allocates the memorry for 
 * the new object.
 */

#endif
