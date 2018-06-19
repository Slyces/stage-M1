#include <Node.hpp>
#include <cassert>
#include <chrono>
#include <thread>

using namespace std;


Network::Network(void * graph, Node ** nodes, unsigned int netSize) : n(netSize) {
    this->graph = graph;
    this->nodes = nodes;
    threads = new thread*[n];
    queues = new PhysicalQueue[n];
}

Network::~Network() {
    for (int i = 0; i < n; i++)
        delete threads[i];
    delete[] threads;
    delete[] queues;
}


void Network::start() {
    /* timing of the network */
    auto start_time = chrono::high_resolution_clock::now();

    /* creation of the threads */
    printf("/* ------------------------ start of thread creation ------------------------ */\n");
    for (int i = 0; i < n; i++) {
        threads[i] = new thread(Node::StartNode, (void *) nodes[i]);
        printf("Created thread %d.\n", i);
    }
    printf("/* ------------------------- end of thread creation ------------------------- */\n");

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
    printf("/* ---------------------------- end of simulation --------------------------- */\n");
    printf("The network took %li milliseconds to stop.\n", duration.count());
}

void Network::stop() {
    int sent = 0, received = 0;
    for (int i = 0; i < n; i++) {
        sent += nodes[i]->confSent;
        received += nodes[i]->confReceived;
    }
    printf("/* ------------------------- sending stop requests -------------------------- */\n");
    for (int i = 0; i < n; i++) {
        nodes[i]->send(new PhysicalMessage(i, i, STOP, nullptr));
    }
    for (int i = 0; i < n; i++) {
        threads[i]->join();
    }
    printf("/* -------------------------------------------------------------------------- */\n");
    printf("sent: %d, received: %d\n", sent, received);
    assert(sent == received);
}

