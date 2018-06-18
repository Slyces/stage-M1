
#include "ProtocolStack.hpp"
#include "ConfMessage.hpp"

ConfMessage::ConfMessage(int dest, ProtocolStack * stack, int cost) : stack(stack) {
    this->dest = dest;
    this->cost = cost;
}

ConfMessage::~ConfMessage() {
    delete stack;
}

string ConfMessage::toString() {
    string str;
    str += "∙ → ";
    str += to_string(dest);
    str += " : [";
    str += to_string(cost);
    str += "] ";
    return str + stack->toString();
}



