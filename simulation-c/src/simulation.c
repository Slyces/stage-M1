#include "simulation.h"
#include <assert.h>

#define true 1
#define false 0

void * RandomGraph(int n) {
    return NULL;
}

node * RandomNode(int id, adaptFunction * allAdapt, size_t maxAdapt, double p) {
    int selected[maxAdapt];
    int count;
    size_t subsetLength = 0;
    // select a subset of adaptation functions from all adaptation functions
    for (int i = 0; i < maxAdapt; i++) {
        if (true /* some random test, like random() < p */) {
            selected[i] = true;
            subsetLength++; // count the number of functions used
        } else {
            selected[i] = false;
        }
    }
    adaptFunction * adaptSubset = malloc(subsetLength * sizeof(adaptFunction));
    count = 0;
    for (int i = 0; i < maxAdapt; i++) {
        if (selected[i]) {
            adaptSubset[count] = allAdapt[i];
            count++;
        }
    }
    assert(count == subsetLength);
    return NodeCreate(id, adaptSubset, subsetLength);
}

network * RandomNetwork(int n, size_t nbProtocols, double p, size_t maxStack) {
    // create a random graph
    void * graph = NULL;

    // create links for each edge in the graph

    // create an array with every valid adaptation function
    adaptFunction * allAdapt = NULL;
    size_t adaptMax = 0;
    
    // create an array of nodes, with random adaptation functions
    node ** nodesArray = malloc(n * sizeof(node *));
    for (int i = 0; i < n; i++) {
        nodesArray[i] = RandomNode(i, allAdapt, adaptMax, p);
    }
    return NetworkCreate(graph, nodesArray, n);
}

int main(int argc, const char *argv[]) {
    printf("/* ─────────────────────────── simulation started ─────────────────────────── */\n");
    network * net = RandomNetwork(10, 0, 0, 0); // 10 nodes, everything else disabled
    NetworkStart(net, 2.0);
}
