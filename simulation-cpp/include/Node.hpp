#ifndef SIMULATION_CPP_NODE_HPP
#define SIMULATION_CPP_NODE_HPP

#include <vector>
#include "AdaptationFunction.hpp"

class Node {
  public:
    unsigned int id;
    vector<AdaptationFunction> adaptFunctions;
    vector<ProtocolStack *> In;
    vector<ProtocolStack *> Out;

    unsigned int confReceived, confSent,
            messageReceived, messageRouted, messageDiscarded;

  public:
    Node() = default;
    Node(unsigned int id, vector<AdaptationFunction> const&);
    ~Node();

    string toString();

};

#endif //SIMULATION_CPP_NODE_HPP
