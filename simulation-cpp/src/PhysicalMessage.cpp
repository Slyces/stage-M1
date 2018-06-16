#include <PhysicalMessage.hpp>
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
