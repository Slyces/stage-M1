#ifndef SIMULATION_CPP_NODE_HPP
#define SIMULATION_CPP_NODE_HPP

#include <vector>
#include "AdaptationFunction.hpp"
#include "Network.hpp"
#include "PhysicalMessage.hpp"
#include "RoutingTable.hpp"

class Node {
  public:
    Network * network;
    unsigned int id;
    RoutingTable table;
    std::vector<AdaptationFunction> adaptFunctions;
    std::vector<ProtocolStack *> In;
    std::vector<ProtocolStack *> Out;
    int timeout = 10;
    bool stop = false;

    unsigned int confReceived, confSent,
            messageReceived, messageRouted, messageDiscarded;

  public:
    Node();
    Node(Network *, unsigned int id, std::vector<AdaptationFunction> const&);
    ~Node();

    static void StartNode(void * ptr);

    void start();
    void initialize();

    void send(PhysicalMessage * msg);
    void send(int to, ConfMessage * msg);
    void send(int to, Message * msg);

    void receive(PhysicalMessage * physMessage);
    void receive(int from, ConfMessage * msg);
    void receive(Message * msg);
    void route(Message * msg);

    std::string toString();
};

#endif //SIMULATION_CPP_NODE_HPP
