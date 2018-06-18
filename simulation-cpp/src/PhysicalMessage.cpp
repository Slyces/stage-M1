#include "PhysicalMessage.hpp"
using namespace std;

PhysicalMessage::PhysicalMessage(int sender, int receiver, messageType type,
                                 void * content) {
    this->sender = sender;
    this->receiver = receiver;
    this->type = type;
    this->content = content;
}

string PhysicalMessage::toString() {
    string str = "[phys: ";
    str += to_string(sender);
    str += " → ";
    str += to_string(receiver);
    str += " :";
    switch (type) {
        case STOP:
            str += "STOP";
            break;
        case MSG:
            str += "MSG";
            break;
        case CONF:
            str += "CONF";
            break;
    }
    return str + "]";
}

PhysicalMessage * PhysicalMessage::encode(int sender, int receiver, ConfMessage * msg) {
    return new PhysicalMessage(sender, receiver, CONF, msg);
}

PhysicalMessage * PhysicalMessage::encode(int sender, int receiver, Message * msg) {
    return new PhysicalMessage(sender, receiver, MSG, msg);
}
