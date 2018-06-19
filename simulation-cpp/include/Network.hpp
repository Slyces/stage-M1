#ifndef SIMULATION_CPP_NETWORK_HPP
#define SIMULATION_CPP_NETWORK_HPP

#include <thread>
#include "Node.hpp"
#include "blockingconcurrentqueue.h"
#include "PhysicalMessage.hpp"

class Node;

typedef moodycamel::BlockingConcurrentQueue<PhysicalMessage *> PhysicalQueue;

class Network {
  public:
    void * graph;
    Node ** nodes;
    std::thread ** threads;
    PhysicalQueue * queues;
    unsigned int n;
    bool running = true;
    int timeout = 5;

  public:
    Network(void * graph, Node ** nodes, unsigned int netSize);
    ~Network();

    void start();
    void stop();

};

#endif //SIMULATION_CPP_NETWORK_HPP
