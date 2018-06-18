#include <AdaptationFunction.hpp>
#include <iostream>

using namespace std;

AdaptationFunction::AdaptationFunction(protocol in, protocol out, adaptType type) {
    this->in = in;
    this->out = out;
    this->type = type;
}

AdaptationFunction  AdaptationFunction::makeReverse() {
    adaptType newType = CV; // default type is [conversion]
    if (type == EC) { // [encapsulation]
        newType = DC;
    } else if (type == DC) { // [decapsulation]
        newType = EC;
    }
    return AdaptationFunction(out, in, newType);
}

bool AdaptationFunction::valid(ProtocolStack &stack) {
//    cout << "in stack : " << stack.toString() << " size : " << stack.size << " top : " << stack.topIndex << endl;
    if (stack.empty())
        return false;
    if (type == CV) { // [conversion] always possible
        return stack.top() == in;
    } else if (type == EC) { // [encapsulation] increases size
        return !stack.full() && stack.top() == in;
    } else if (type == DC) { // [decapsulation] decreases size
        return stack.topIndex >= 1 && stack.top(0) == in &&
                                 stack.top(1) == out;
    }
    return false;
}

bool AdaptationFunction::operator==(const AdaptationFunction &f) {
    return in == f.in && out == f.out && type == f.type;
}

unsigned long AdaptationFunction::hash() {
    unsigned long hash = 0;
    hash <<= in;
    hash <<= out;
    return hash << type;
}

void AdaptationFunction::apply(ProtocolStack & stack) {
    if (type == CV) {
        stack.pop();
        stack.push(out);
    } else if (type == EC) {
        stack.push(out);
    } else if (type == DC) {
        stack.pop();
    }
}

void AdaptationFunction::setAll(protocol in, protocol out, adaptType type) {
    this->in = in;
    this->out = out;
    this->type = type;
}

string AdaptationFunction::toString() {
    string str = "f('";
    if (type == DC) str += out;
    str += in;
    str += "' → '";
    if (type == EC) str += in;
    str += out;
    return str + "')";
}

ProtocolStack * AdaptationFunction::In()const {
    auto * stack = new ProtocolStack(2);
    if (type == DC) stack->push(out);
    stack->push(in);
    return stack;
}

ProtocolStack * AdaptationFunction::Out()const {
    auto * stack = new ProtocolStack(2);
    if (type == EC) stack->push(in);
    stack->push(out);
    return stack;
}