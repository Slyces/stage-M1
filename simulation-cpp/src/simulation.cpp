#include <cstdio>
#include <iostream>
#include <random>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <Link.hpp>
#include "Node.hpp"
#include "AdaptationFunction.hpp"
#include "ProtocolStack.hpp"
#include "Network.hpp"

using namespace std;
using namespace boost;

typedef adjacency_list<vecS, vecS, bidirectionalS, no_property, Link> Graph;

int run(int n, int nbProtocols, int maxStack, double p) {
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

    Node * nodes[n];
    auto * network = new Network(nullptr, nodes, static_cast<unsigned int>(n));

    /* create an array of random nodes */
    for (int i = 0; i < n; i++) {
        /* create a random subset of adaptation functions */
        std::vector<AdaptationFunction> selected;
        selected.reserve(static_cast<unsigned long>(p * (nbProtocols * nbProtocols * 3)));
        for (auto function : allFunctions) {
            if (dist(e2) < p)
                selected.push_back(function);
        }
        cout << "Selected :" << endl;
        nodes[i] = new Node(network, (unsigned int) i, selected);
    }

    /* run the network */
//    network->timeout = 40;
    network->start();

    /* free the memory */
    for (auto &node : nodes)
        delete node;
    delete network;
    return 1;
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
    int nbiter = 1;
    for (int i = 0; i < nbiter; i++)
        run(3 /*n*/, 2 /*nbprotocols*/, 3 /*maxStacks*/, 1 /*p*/);
    cout << sizeof(size_t);
    return 0;
}