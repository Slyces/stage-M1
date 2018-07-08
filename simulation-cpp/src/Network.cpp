#include <cassert>
#include <chrono>
#include <thread>
#include <iostream>
#include "Node.hpp"
#include "Network.hpp"

using namespace std;

Network::Network(Graph &graph, Node ** nodes, unsigned int netSize, unsigned int maxStack) : n(netSize) {
    this->graph = graph;
    this->nodes = nodes;
    this->maxStack = maxStack;
    threads = new thread*[n];
    queues = new PhysicalQueue[n];
}

Network::~Network() {
    for (unsigned int i = 0; i < n; i++)
        delete threads[i];
    delete[] threads;
    delete[] queues;
}

long Network::start() {
    /* timing of the network */
    auto start_time = chrono::high_resolution_clock::now();

    /* creation of the threads */
    for (unsigned int i = 0; i < n; i++) {
        threads[i] = new thread(Node::StartNode, (void *) nodes[i]);
    }

    // end of the simulation conditions
    do {
        running = false;
        std::chrono::milliseconds timespan(timeout);
        std::this_thread::sleep_for(timespan);
    } while (running);

    // on-stop actions
    stop();

    auto stop_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop_time - start_time);

    cout << duration.count() << " ms" << endl;
    return duration.count();
}

void Network::stop() {
    int sent = 0, received = 0;
    for (unsigned int i = 0; i < n; i++) {
        sent += nodes[i]->confSent;
        received += nodes[i]->confReceived;
    }
    for (unsigned int i = 0; i < n; i++) {
        nodes[i]->send(new PhysicalMessage(i, i, STOP, nullptr));
    }
    for (unsigned int i = 0; i < n; i++) {
        threads[i]->join();
    }
    assert(sent == received);
}


