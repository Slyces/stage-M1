#ifndef SIMULATION_CPP_PHYSICALMESSAGE_HPP
#define SIMULATION_CPP_PHYSICALMESSAGE_HPP


#include <string>

enum messageType {MSG, CONF, STOP};

class PhysicalMessage {
  public:
    int sender, receiver;
    messageType type;
    void * content;

  public:
    PhysicalMessage(int sender, int receiver, messageType, void *);
    ~PhysicalMessage() = default;

    std::string toString();
};

#endif //SIMULATION_CPP_PHYSICALMESSAGE_HPP
