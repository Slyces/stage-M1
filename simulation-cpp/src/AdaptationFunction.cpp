#include <AdaptationFunction.hpp>

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

size_t AdaptationFunction::hash() const {
    int offset = sizeof(protocol) * 8;
    auto hash = static_cast<size_t>(in);
    hash <<= offset;
    hash += out;
    hash <<= offset;
    hash += (protocol) type;
    return hash;
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

ProtocolStack * AdaptationFunction::In() const { return In(2);}

ProtocolStack * AdaptationFunction::Out() const { return Out(2);}

ProtocolStack * AdaptationFunction::In(size_t n) const {
    auto * stack = new ProtocolStack(n);
    if (type == DC) stack->push(out);
    stack->push(in);
    return stack;
}

ProtocolStack * AdaptationFunction::Out(size_t n) const {
    auto * stack = new ProtocolStack(n);
    if (type == EC) stack->push(in);
    stack->push(out);
    return stack;
}

bool operator==(const AdaptationFunction &lhs, const AdaptationFunction &rhs) {
    return lhs.in == rhs.in && lhs.out == rhs.out && lhs.type == rhs.type;
}
