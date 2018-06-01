#include "network.h"
#include <unistd.h>
#include "nodes.h"

network * NetworkCreate(void * graph, node ** nodesArray, int nodeNumber) {
    network * net = malloc(sizeof(network));
    net->n = nodeNumber;
    net->graph = graph;
    net->running = 0;
    net->threads = malloc(nodeNumber * sizeof(pthread_t));
    net->nodes = nodesArray;
    /* -------------- creating pipes, producers and consumers --------------- */
    pipe_t * pipes[net->n];
    net->producers = malloc(net->n * sizeof(pipe_producer_t *));
    net->consumers = malloc(net->n * sizeof(pipe_consumer_t *));
    for (int i = 0; i < net->n; i++) {
        pipes[i] = pipe_new(sizeof(physicalMessage *), 0);
        net->producers[i] = pipe_producer_new(pipes[i]);
        net->consumers[i] = pipe_consumer_new(pipes[i]);
        pipe_free(pipes[i]);
    }
    return net;
}

void NetworkDestroy(network * net) {
    // delete the graph
    
    // delete every node and pipes
    for (int i = 0; i < net->n; i++) {
        NodeDestroy(net->nodes[i]);
        pipe_producer_free(net->producers[i]);
        pipe_consumer_free(net->consumers[i]);
    }
    //delete the array of node pointers and prod / cons
    free(net->nodes);
    free(net->producers);
    free(net->consumers);

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
    printf("/* ------------------------- end of thread creation ------------------------- */\n");


    /* -------------------- wait for end of the network --------------------- */
    NetworkCheckEnd(net, maxTime);
    for (int i = 0; i < net->n; i++) {
        pthread_join(net->threads[i], NULL);
    } 

    /* -------------------------- stop the network -------------------------- */
    NetworkDestroy(net);
}

void NetworkCheckEnd(network * net, double maxTime) {
    sleep(maxTime);
    net->running = 0;
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
