/*
 * File: simulation.h
 * Author: Simon Lassourreuille <simon.lassourreuille@u-bordeaux.fr>
 * Topic: Main file for the network simulation. Creates and runs a random network.
 */
#ifndef _NODES_H
#define _NODES_H

/*
 * Function: RandomGraph
 * ---------------------
 */
void * Randomgraph(int n);

/*
 * Function: RandomAdapt
 * ---------------------
 */
void RandomAdapt(adaptFunction * allAdapt, int maxAdapt, double p);

/*
 * Function: RandomNodes
 * ---------------------
 */
node ** RandomNodes(int nbNodes, int nbProtocols, double p);

/*
 * Function: RandomNetwork
 * -----------------------
 */
network * random_network(int n, double p, int nbProtocols, int max_stack);

/*
 * Function: main
 * --------------
 *  Parses the console arguments to create a simulation, then gathers
 *  statistitcs about the run and prints those stats to stderr
 */
int main(int argc, const char *argv[]);

#endif
