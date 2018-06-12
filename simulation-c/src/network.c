#include "network.h"
#include <unistd.h>
#include <stdlib.h>
#include "nodes.h"
#include <assert.h>

void NetworkCreate(network * net, void * graph, node * nodesArray, int nodeNumber) {
    net->n = nodeNumber;
    net->graph = graph;
    net->running = 0;
    net->threads = malloc(nodeNumber * sizeof(pthread_t));
    net->nodes = nodesArray;
    net->started = (clock_t) 0;
    net->nodeActive = 1; // nodes are active at creation
    /* -------------- creating pipes, producers and consumers --------------- */
    pipe_t ** pipes = malloc(net->n * sizeof(pipe_t *));
    net->producers = malloc(net->n * sizeof(pipe_producer_t *));
    net->consumers = malloc(net->n * sizeof(pipe_consumer_t *));
    for (int i = 0; i < net->n; i++) {
        pipes[i] = pipe_new(sizeof(physicalMessage *), 0);
        net->producers[i] = pipe_producer_new(pipes[i]);
        net->consumers[i] = pipe_consumer_new(pipes[i]);
        pipe_free(pipes[i]);
    }
}

void NetworkDestroy(network * net) {
    // delete the graph
    
    // delete every node and pipes
    for (int i = 0; i < net->n; i++) {
        pipe_producer_free(net->producers[i]);
        pipe_consumer_free(net->consumers[i]);
        NodeDestroy(&net->nodes[i]);
    }
    //delete the array of node pointers and prod / cons
    free(net->producers);
    free(net->consumers);
    free(net->nodes);

    //delete the array of threads id
    free(net->threads);
}

void NetworkStart(network * net, double maxTime) {
    int n = net->n;
    net->running = 1; 
    net->started = clock();

    /* ---------------------- creation of the threads ----------------------- */
    for (int i = 0; i < n; i++) {
        pthread_create(&net->threads[i], NULL, ThreadStart, (void *) net);
        printf("Created thread %d with pthread_id %lu\n", i, net->threads[i]);
    }
    printf("/* ------------------------- end of thread creation ------------------------- */\n");


    /* -------------------- wait for end of the network --------------------- */
    NetworkCheckEnd(net, maxTime);


    /* -------------- send a message to each thread to stop it -------------- */
    NetworkStop(net);
    
    printf("/* ────────────────────────── network stopped ─────────────────────────── */\n");
    printf("Total time : %f ms. Max time : %f ms.\n", RunTime(net) * 1000, maxTime * 1000);

    /* -------------------------- stop the network -------------------------- */
    NetworkDestroy(net);
}

void NetworkStop(network * net) {
    net->running = 0;
    int received = 0, sent = 0;
    for (int i = 0; i < net->n; i++) {
        received += net->nodes[i].confReceived;
        sent += net->nodes[i].confSent;
    }
    printf("sent : %d, received : %d\n", sent, received);
    assert(sent == received);
    for (int i = 0; i < net->n; i++) {
        physicalMessage * physMsg = malloc(sizeof(physicalMessage));
        PhysicalCreate(physMsg, -1, -1, STOP, (message *) NULL);
        pipe_push(net->producers[i], (void **) &physMsg, 1);
    }
    for (int i = 0; i < net->n; i++) {
        pthread_join(net->threads[i], NULL);
    }
}

#include <unistd.h>

void NetworkCheckEnd(network * net, double maxTime) {
    while (RunTime(net) < maxTime && net->nodeActive) {
        // stop if no nodes are active or time is up
        usleep(100 * 1000); // sleep 100 ms
    }
}

void * ThreadStart(void * ptr) {
    network * net = (network *) ptr;
    node * router = NodeFromThread(net);
    NodeStart(net, router);
    return NULL;
}

double RunTime(network * net) {
    return (double)(clock() - net->started) / CLOCKS_PER_SEC;    
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
        return &net->nodes[nodeId];
    return NULL;
}
