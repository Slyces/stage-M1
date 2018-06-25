#ifndef SIMULATION_CPP_NETWORK_HPP
#define SIMULATION_CPP_NETWORK_HPP

#include <thread>
#include <boost/graph/adjacency_list.hpp>
#include "blockingconcurrentqueue.h"
#include "PhysicalMessage.hpp"
#include "Link.hpp"

class Node;

typedef moodycamel::BlockingConcurrentQueue<PhysicalMessage *> PhysicalQueue;
typedef boost::property<boost::edge_weight_t, Link> LinkProperty;
typedef boost::adjacency_list<boost::vecS, boost::vecS,
        boost::bidirectionalS, boost::no_property, LinkProperty> Graph;

class Network {
  public:
    Graph * graph;
    Node ** nodes;
    boost::property_map<Graph, LinkProperty>::type linkMap;
    std::thread ** threads;
    PhysicalQueue * queues;
    unsigned int n;
    bool running = true;
    int timeout = 5;

  public:
    Network(Graph * graph, Node ** nodes, unsigned int netSize);
    ~Network();

    void start();
    void stop();

};

#endif //SIMULATION_CPP_NETWORK_HPP
