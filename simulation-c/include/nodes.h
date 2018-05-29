/*
 * File: nodes.h
 * Author: Simon Lassourreuille <simon.lassourreuille@u-bordeaux.fr>
 * Topic: Definition of nodes (routers) for a network routing simulation
 */
#ifndef _MESSAGES_H
#define _MESSAGES_H

/*
 * Type: Node
 * ----------
 * This type defines all informations associated with a node.
 * A node is represented by the following informations:
 *   - id: an int, unique to each node
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
    adaptFunction * functionsArray;
    int functionsNumber;
    protocol * in, out;
    int inSize, outSize;
    /* statistics */
    int confReceived, confSent;
    int messageReceived, messageRouted, messageDiscarded;
} Node;

/*
 * Function: ValidInput, ValidOutput
 * Usage: int valid = ValidInput(&node, protocol);
 * -----------------------------------------------
 * Checks if a given protocol is accepted as an input (resp. output)
 * by a node.
 */
int ValidInput(Node * node, protocol p);
int ValidOutput(Node * node, protocol p);



#endif
