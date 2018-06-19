#include "ProtocolStack.hpp"
#include "Message.hpp"

using namespace std;

Message::Message(int sender, int receiver, ProtocolStack * stack, void * payload)
        : stack(stack), payload(payload) {
    this->source = sender;
    this->dest = receiver;
}

Message::~Message() {
    delete stack;
}

string Message::toString() {
    string str;
    str += "(";
    str += to_string(source);
    str += ") → (";
    str += to_string(dest);
    str += ") : ";
    return str + stack->toString();
}
