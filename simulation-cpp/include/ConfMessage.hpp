#ifndef SIMULATION_CPP_CONFMESSAGE_HPP
#define SIMULATION_CPP_CONFMESSAGE_HPP

#include "ProtocolStack.hpp"

class ConfMessage {
  public:
    int dest, cost;
    const ProtocolStack stack;

  public:
    ConfMessage(int dest, ProtocolStack stack, int cost);
    ~ConfMessage();

    string toString();
};

#endif //SIMULATION_CPP_CONFMESSAGE_HPP
