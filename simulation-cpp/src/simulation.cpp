#include <cstdio>
#include <AdaptationFunction.hpp>
#include <iostream>
#include <random>
#include "ProtocolStack.hpp"

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

    /* create every protocol in use for this run */
    protocol protocols[nbProtocols];
    for (int i = 0; i < nbProtocols; i++) {
        protocols[i] = static_cast<protocol>('a' + i);
    }

    /* create an array with every valid adaptation function */
    int count = 0;
    adaptType types[3] = {CV, EC, DC};
    AdaptationFunction allFunctions[(nbProtocols * nbProtocols) * 3];
    for (auto &in : protocols)
        for (auto &out : protocols)
            for (auto &type : types)
                allFunctions[count++].setAll(in, out, type);


    /* preparation to select a random number in range [0, 1) */
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 1);


    /* create an array of random nodes */
    for (int i = 0; i < n; i++) {
        /* create a random subset of adaptation functions */
        vector<AdaptationFunction> selected;
        for (auto function : allFunctions) { // function is a copy
            if (dist(e2) < p)
                selected.push_back(function);
        }
    }

    /* instantiate the network */

    /* run the network */

    /* free the memory */
}

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
    run(2, 3, 5, 1.0, 1);
    return 0;
}