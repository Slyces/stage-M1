#include "network.h"
#include <unistd.h>

network * NetworkCreate(void * graph, node ** nodesArray, int nodeNumber) {
    network * net = malloc(sizeof(network));
    net->n = nodeNumber;
    net->graph = graph;
    net->running = 0;
    net->threads = malloc(nodeNumber * sizeof(pthread_t));
    return net;
}

void NetworkDestroy(network * net) {
    // delete the graph
    
    // delete every node
    for (int i = 0; i < net->n; i++) {
        NodeDestroy(net->nodes[i]);
    }
    //delete the array of node pointers
    free(net->nodes);

    //delete the array of threads id
    free(net->threads);
}

void NetworkStart(network * net, double maxTime) {
    int n = net->n;
    net->running = 1; 

    /* ---------------------- creation of the threads ----------------------- */
    for (int i = 0; i < n; i++) {
        pthread_create(&net->threads[i], NULL, ThreadStart, (void *) net);
        printf("Created thread %d with pthread_id %lu\n", i, net->threads[i]);
    }

    /* -------------------- wait for end of the network --------------------- */
    NetworkCheckEnd(net, maxTime);

    /* -------------------------- stop the network -------------------------- */
    NetworkDestroy(net);
}

void NetworkCheckEnd(network * net, double maxTime) {
    sleep(maxTime);
}

void * ThreadStart(void * ptr) {
    network * net = (network *) ptr;
    node * router = NodeFromThread(net);
    NodeStart(net, router);
    return NULL;
}

int IdFromThread(network * net) {
    pthread_t threadId = pthread_self();
    for (int i = 0; i < net->n; i++) {
        if (net->threads[i] == threadId)
            return i;
    }
    return -1;
}

node * NodeFromThread(network * net) {
    int nodeId = IdFromThread(net);
    if (nodeId >= 0)
        return net->nodes[nodeId];
    return NULL;
}
