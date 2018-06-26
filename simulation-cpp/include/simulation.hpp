#ifndef SIMULATION_CPP_SIMULATION_HPP
#define SIMULATION_CPP_SIMULATION_HPP

#include "Network.hpp"

Graph BarabasiAlbert(int n, unsigned long m);

/* Function: main
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

#endif //SIMULATION_CPP_SIMULATION_HPP
