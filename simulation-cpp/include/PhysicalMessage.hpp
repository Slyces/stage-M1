#ifndef SIMULATION_CPP_PHYSICALMESSAGE_HPP
#define SIMULATION_CPP_PHYSICALMESSAGE_HPP


#include <string>
#include "ConfMessage.hpp"
#include "Message.hpp"

enum messageType {MSG, CONF, STOP};

class PhysicalMessage {
  public:
    int sender, receiver;
    messageType type;
    void * content;

  public:
    PhysicalMessage(int sender, int receiver, messageType, void *);
    ~PhysicalMessage() = default;

    static PhysicalMessage * encode(int sender, int receiver, ConfMessage * msg);
    static PhysicalMessage * encode(int sender, int receiver, Message * msg);

    std::string toString();
};

#endif //SIMULATION_CPP_PHYSICALMESSAGE_HPP
