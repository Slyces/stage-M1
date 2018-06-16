#include <cstdio>
#include <AdaptationFunction.hpp>
#include <iostream>
#include "ProtocolStack.hpp"

int main(int argc, const char * argv[]) {
    printf("Hello, world !\n");
    /*
     * Expected arguments :
     *  - n: the size of the graph
     *  - nbProtocols: number of different protocols
     *  - maxStack: maximum stack size for messages
     *  - p: probability for a node to contain each adaptation function
     *  - nbIter: number of iteration
     */
}

int run(int n, int nbProtocols, int maxStack, double p, int nbIter) {
    /*
     * A run is made following several steps:
     *  - create a random graph
     *  - create links structures [functions <-> cost] for each edge
     *  - create an array with every valid adaptation function
     *  - create an array of random nodes
     *    ~ each node has a subset of every adaptation function
     *  - create the network
     *  - make it run until convergence
     *  - free memory
     */

    /* random graph creation */

    /* create links structures */

    /* create an array with every valid adaptation function */

    /* create an array of random nodes */

    /* create a random subset of adaptation functions */

    /* instantiate the network */

    /* run the network */

    /* free the memory */
}
