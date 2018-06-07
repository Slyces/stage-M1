/*
 * File: nodes.h
 * Author: Simon Lassourreuille <simon.lassourreuille@u-bordeaux.fr>
 * Topic: Definition of nodes (routers) for a network routing simulation
 */
#ifndef _NETWORK_H
#define _NETWORK_H

/* ──────────────────────────── library includes ──────────────────────────── */
#include <pthread.h>
//#include <igraph.h>
#include <stdio.h>
/* ────────────────────────── project code include ────────────────────────── */
#include "pipe.h"
#define MAX_SIZE 1024

struct nodeStruct;
typedef struct nodeStruct node;

/*
 * Type: network
 * -------------
 * This structure represents a complete network. It is built according to:
 *   - nodesArray: an array of nodes
 *   - nodesNumber: self-explanatory
 *   - graph: a graph representing links between nodes
 * Supplementary informations are:
 *   - threads: an array of p_thread_t ids of the threads
 *   - running: an int true if the network is on, false when it's off
 */
typedef struct {
    int n;  // number of nodes 
    pthread_t * threads;
    node * nodes;
    int running;
    void * graph;
    clock_t started;
    pipe_producer_t ** producers;
    pipe_consumer_t ** consumers;
} network;

/*
 * Function: NetworkCreate
 * -----------------------
 * Creates a new network. This does not allocate the inputs, so make sure
 * to create a valid graph and valid nodes.
 * The graph must be oriented and symetric.
 */
void NetworkCreate(network *, void * graph, node * nodesArray, int nodeNumber);

/*
 * Function: NetworkStart
 * Usage: NetworkStart(&net, maxTime);
 * --------------------------------------
 * Starts the network for maxTime seconds at most. Creates the thread
 * team to process each node of the network.
 * Calls NetworkCheckEnd in a loop to see if end conditions are met.
 * At the end of the timer or when those conditions are met, calls
 * NetworkDestroy. This is a one-time use function.
 */
void NetworkStart(network * net, double maxTime);

/*
 * Function: NetworkCheckEnd
 * -------------------------
 * Checks if the conditions to close the network are met.
 * If not, ends it when the timer reaches maxTime seconds.
 */
void NetworkCheckEnd(network * net, double maxTime);

/*
 * Function: NetworkStop
 * ---------------------
 * Sends a message to each node to terminate it.
 */
void NetworkStop(network * net);

/*
 * Function: NetworkDestroy
 * ------------------------
 * Properly frees all memorry associated with the network. Also frees the
 * contained nodes, along with the graph and its links.
 */
void NetworkDestroy(network * net);

/*
 * Function: RunTime
 * -----------------
 * Return the number of ms elapsed since the network started
 */
double RunTime(network * net);

/*
 * Function: IdFromThread, NodeFromThread
 * --------------------------------------
 * Returns the id (resp. node pointer) of the node handled by the calling
 * thread. Return -1 (resp. NULL) if not found.
 */
int IdFromThread(network * net);
node * NodeFromThread(network * net);

/*
 * Function: ThreadStart
 * ---------------------
 * Function called as the main task for each thread. Starts the node
 * and waits for messages.
 */
void * ThreadStart(void * ptr);

#endif
