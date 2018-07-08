#ifndef SIMULATION_CPP_NETWORK_HPP
#define SIMULATION_CPP_NETWORK_HPP

#include <thread>
#include <boost/graph/adjacency_list.hpp>
#include "blockingconcurrentqueue.h"
#include "PhysicalMessage.hpp"
#include "Link.hpp"

class Node;

typedef moodycamel::BlockingConcurrentQueue<PhysicalMessage *> PhysicalQueue;

typedef boost::adjacency_list<boost::vecS, boost::vecS,
        boost::bidirectionalS, boost::no_property, Link> Graph;

class Network {
  public:
    Graph graph;
    Node ** nodes;
    std::thread ** threads;
    PhysicalQueue * queues;
    unsigned int maxStack;
    unsigned int n;
    bool running = true;
    int timeout = 20;

  public:
    Network(Graph &graph, Node ** nodes, unsigned int netSize, unsigned int maxStack);
    ~Network();

    long start();
    void stop();

};

#endif //SIMULATION_CPP_NETWORK_HPP
