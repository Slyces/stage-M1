#include <Node.hpp>
#include <cassert>
#include <chrono>
#include <thread>

Network::Network(void * graph, Node ** nodes, unsigned int netSize) {
    this->graph = graph;
    this->nodes = nodes;
    this->n = netSize;
    threads_id = new pthread_t[n];
}

Network::~Network() {
    delete[] threads_id;
}


void Network::start() {
    /* creation of the threads */
    printf("/* ------------------------ start of thread creation ------------------------ */\n");
    for (int i = 0; i < n; i++) {
        pthread_create(&threads_id[i], nullptr, Node::StartNode, (void *) nodes[i]);
        printf("Created thread %d with pthread_id %lu\n", i, threads_id[i]);
    }
    printf("/* ------------------------- end of thread creation ------------------------- */\n");

    std::chrono::milliseconds timespan(500); // or whatever

    std::this_thread::sleep_for(timespan);

    stop();
}

void Network::stop() {
    int sent = 0, received = 0;
    for (int i = 0; i < n; i++) {
        sent += nodes[i]->confSent;
        received += nodes[i]->confReceived;
    }
    printf("sent: %d, received: %d", sent, received);
    assert(sent == received);
    for (int i = 0; i < n; i++) {
        nodes[i]->send(new PhysicalMessage(-1, nodes[i]->id, STOP, nullptr));
    }
    for (int i = 0; i < n; i++) {
//        pthread_join()
    }
}


