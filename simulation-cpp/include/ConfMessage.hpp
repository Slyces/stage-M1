#ifndef SIMULATION_CPP_CONFMESSAGE_HPP
#define SIMULATION_CPP_CONFMESSAGE_HPP

#include "ProtocolStack.hpp"

class ConfMessage {
  public:
    int dest, cost;
    ProtocolStack * stack;

  public:
    ConfMessage(int dest, ProtocolStack * stack, int cost);
    ~ConfMessage();

    string toString();
};

#endif //SIMULATION_CPP_CONFMESSAGE_HPP
