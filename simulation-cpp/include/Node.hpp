#ifndef SIMULATION_CPP_NODE_HPP
#define SIMULATION_CPP_NODE_HPP

#include <vector>
#include "AdaptationFunction.hpp"
#include "Network.hpp"
#include "PhysicalMessage.hpp"

class Node {
  public:
    Network * network;
    unsigned int id;
    vector<AdaptationFunction> adaptFunctions;
    vector<ProtocolStack *> In;
    vector<ProtocolStack *> Out;

    bool stop = false;

    unsigned int confReceived, confSent,
            messageReceived, messageRouted, messageDiscarded;

  public:
    Node();
    Node(Network *, unsigned int id, vector<AdaptationFunction> const&);
    ~Node();

    static void *StartNode(void *);

    void start();

    void send(PhysicalMessage * msg);
    void send(int to, ConfMessage * msg);
    void send(int to, Message * msg);

    void receive(PhysicalMessage * physMessage);
    void receive(ConfMessage * msg);
    void receive(Message * msg);
    void route(Message * msg);

    string toString();
};

#endif //SIMULATION_CPP_NODE_HPP
