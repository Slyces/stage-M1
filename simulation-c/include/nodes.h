/*
 * File: nodes.h
 * Author: Simon Lassourreuille <simon.lassourreuille@u-bordeaux.fr>
 * Topic: Definition of nodes (routers) for a network routing simulation
 */
#ifndef _NODES_H
#define _NODES_H

#include "adaptationFunctions.h"
#include "links.h"
#include "routingTable.h"
#include "messages.h"

struct networkStruct;
typedef struct networkStruct network;

/*
 * Type: Node
 * ----------
 * This type defines all informations associated with a node.
 * A node is represented by the following informations:
 *   - id: an int, unique to each node
 *   - table: the routign table of this node.
 *   - functionsArray: an array of adaptation functions, representing
 *                     the protocols this node is able to handle
 *   - functionsNumber: size of this array.
 *   - in, out: arrays of protocols accepted as input (resp. output).
 *   - inSize, outSize: size of the corresponding arrays;
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   # Some statistics for the simulation
 *   - confReceived, confSent: counts the configuration messages sent
 *                             and received while building the
 *                             routing table.
 *   - messageReceived: counts the messages received directly by this
 *                      node.
 *   - messageRouted: counts the messages routed by this node.
 *   - messageDiscarded: counts the invalid messages thrown by this
 *                       node.
 */
typedef struct {
    int id;
    routingTable * table;
    adaptFunction * adaptArray;
    int adaptNumber;
    protocol * in, out;
    size_t inSize, outSize;
    /* statistics */
    int confReceived, confSent;
    int messageReceived, messageRouted, messageDiscarded;
} node;

/*
 * Function: NodeCreate
 * Usage: node * newNode = NodeCreate(id, &adaptArray, adaptNumber);
 * -------------------------------------------------------------------------
 * id: id of the new node
 * functionsArray: a pointer to an array of adaptation functions
 * functionsNumber: the length of that array
 * -------------------------------------------------------------------------
 * Creates a node and allocates memorry to different structures. This creates
 * the in and out arrays, along with the routign table. Do not forget to use
 * NodeDestroy to free those structures.
 */
node * NodeCreate(int id, adaptFunction * adaptArray, size_t adaptNumber);

/*
 * Function: NodeDestroy
 * ---------------------
 * Frees all memorry associated with this structure, including the in and out
 * arrays and the routing table.
 */
void NodeDestroy(node * router);

/*
 * Function: ValidInput, ValidOutput
 * Usage: int valid = ValidInput(&node, protocol);
 * -----------------------------------------------
 * Checks if a given protocol is accepted as an input (resp. output)
 * by a node.
 */
int ValidInput(node * router, protocol p);
int ValidOutput(node * router, protocol p);

/*
 * Function: NodeStart, NodeWaitMessages
 * -------------------------------------
 * NodeStart sends initialisation messages to the node's neighbors,
 * and NodeWaitMessages is a never-ending loop waiting for messages
 * and handling them.
 */
void NodeStart(network * net, node * router);
void NodeWaitMessages(network * net, node * router);


/*
 * Function: SendMessage, SendConf
 * Usage: SendMessage(A, B, &msg);
 *        SendConfMessage(A, B, &confMsg);
 * ---------------------------------------
 * Sends a message from one node to another, using the int id of
 * the nodes. Actually puts a physical messages on the node's
 * thread-safe stack, using the corresponding type.
 */
void SendMessage(network * net, int sender, int receiver, message * msg);
void SendConfMessage(network * net, int sender, int receiver, confMessage * msg);

/*
 * Function: ReceivePhysical
 * Usage: ReceivePhysical(&net, &router, &physMsg);
 * ------------------------------------------------
 * Function called upon reception of a physical message. It opens the message,
 * casts its content to the appropriate type and calls the appropriate reception
 * function.
 */
void ReceivePhysical(network * net, node * router, physicalMessage * msg);

/*
 * Function: ReceiveConf
 * ---------------------
 * This function is the core of the construction of routign tables.
 * When a router receives a new configuration message, it does several things:
 *   - is the top protocol a valid output ? No → discard the message
 *   - for each valid function, see what stack I should receive to output this
 *     stack
 *   - look up the duo (stack, dest) in my table
 *   - add / replace it if the cost is cheaper
 *   - for each added route, send a message to each neighbor
 *   - delete the message
 */
void ReceiveConf(network * net, node * router, confMessage * msg);

/*
 * Function: ReceiveMessage, RouteMessage
 * --------------------------------------
 * Basic functions called upon reception of a communication message.
 * ReceiveMessage corresponds to a message destined to this node, whereas
 * RouteMessage corresponds to a message still traveling to its destination.
 *
 * Received messages can be deleted.
 */
void ReceiveMessage(node * router, message * msg);
void RouteMessage(network * net, node * router, message * msg);

#endif
