#ifndef SIMULATION_CPP_NETWORK_HPP
#define SIMULATION_CPP_NETWORK_HPP

#include "Node.hpp"

class Node;

class Network {
  public:
    void * graph;
    Node ** nodes;
    pthread_t * threads_id;
    unsigned int n;

  public:
    Network(void * graph, Node ** nodes, unsigned int netSize);
    ~Network();

    void start();
    void stop();
};

#endif //SIMULATION_CPP_NETWORK_HPP
