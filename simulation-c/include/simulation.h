/*
 * File: simulation.h
 * Author: Simon Lassourreuille <simon.lassourreuille@u-bordeaux.fr>
 * Topic: Main file for the network simulation. Creates and runs a random network.
 */
#ifndef _SIMULATION_H
#define _SIMULATION_H

#include <stddef.h>
#include <nodes.h>
#include <network.h>

/*
 * Function: RandomGraph
 * ---------------------
 * Creates a random symetrical graph with n nodes
 */
void * RandomGraph(int n);

/*
 * Function: RandomNode
 * --------------------
 * Creates a random subset of adaptation fuctions. Each functions has
 * a probability p to be included in the subset. Then creates a node
 * using those adaptation functions.
 */
node * RandomNode(int id, adaptFunction * allAdapt, size_t maxAdapt, double p);

/*
 * Function: RandomLink
 * --------------------
 * Create a random link, with random costs for each adaptation function.
 */
networkLink * RandomLink(void);

/*
 * Function: RandomNetwork
 * -----------------------
 * Creates a random network for a given number of nodes and number of
 * protocols.
 *
 * This network is created in several steps:
 * - create a random graph
 * - create links structures for each edge in the graph
 * - create an array with every valid adaptation function with this number of
 *   protocols
 * - create an array of nodes, with random adaptation functions
 */
network * RandomNetwork(int n, size_t nbProtocols, double p, size_t maxStack);

/*
 * Function: main
 * --------------
 *  Parses the console arguments to create a simulation, then gathers
 *  statistitcs about the run and prints those stats to stderr
 *
 *  Inputs:
 *  - n: the size of the graph
 *  - nbProtocols: number of protocols to be included
 *  - maxStack: maximum stackSize of a message
 *  - p: probability for a node to possess each adaptation function
 *  - nbIter: number of iterations
 */
int main(int argc, const char *argv[]);

#endif
