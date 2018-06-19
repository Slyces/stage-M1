#ifndef SIMULATION_CPP_MESSAGE_HPP
#define SIMULATION_CPP_MESSAGE_HPP

#include "ProtocolStack.hpp"

class Message {
  public:
    int source, dest;
    ProtocolStack * stack;
    void * payload;

  public:
    Message(int sender, int receiver, ProtocolStack * stack, void * payload);
    ~Message();

    std::string toString();
};

#endif //SIMULATION_CPP_MESSAGE_HPP
