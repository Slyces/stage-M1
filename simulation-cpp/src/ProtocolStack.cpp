#include <cstddef>
#include <ProtocolStack.hpp>
#include <iostream>

ProtocolStack::ProtocolStack(size_t size) : size(size) {
    protocols = static_cast<protocol *>(malloc(size * sizeof(protocol)));
    topIndex = -1;
    for (int i = 0; i < size; i++) {
        protocols[i] = '-';
    }
//    cout << "creation : " << toString() << " size : " << size << " top : " << topIndex << endl;
}

ProtocolStack::ProtocolStack(size_t size, string str) : size(size) {
    protocols = static_cast<protocol *>(malloc(size * sizeof(protocol)));
    topIndex = static_cast<int>(str.length() < size ? str.length() - 1 : size - 1);
    for (int i = 0; i < size; i++) {
        if (i < str.length())
            protocols[i] = str.at(static_cast<unsigned long>(i));
        else
            protocols[i] = 0;
    }
//    cout << "creation from string [" << str << "] : " << toString() << " size : " << size << " top : " << topIndex << endl;
}

ProtocolStack::~ProtocolStack() {
    free(protocols);
}

bool ProtocolStack::empty() {
    return topIndex == -1;
}

bool ProtocolStack::full() {
    return topIndex == size - 1;
}

void ProtocolStack::push(protocol p) {
    protocols[++topIndex] = p;
}

protocol ProtocolStack::pop() {
    return protocols[topIndex--];
}

bool ProtocolStack::operator==(const ProtocolStack &stack) {
    if (topIndex != stack.topIndex)
        return false;
    for (int i = 0; i <= topIndex; i++) {
        if (protocols[i] != stack.protocols[i])
            return false;
    }
    return true;
}

string ProtocolStack::toString() {
    string repr = "<";
    for (int i = 0; i < topIndex; i++) {
        repr += protocols[i];
        repr += '-';
    }
    if (topIndex >= 0) {
        repr += protocols[topIndex];
    }
    return repr + ">";
}

ProtocolStack ProtocolStack::clone() const {
    ProtocolStack stack(size);
    stack.topIndex = topIndex;
    for (int i = 0; i <= topIndex; i++) {
        stack.protocols[i] = protocols[i];
    }
    return stack;
}

protocol ProtocolStack::top(int index) {
    return protocols[topIndex - index];
}

protocol ProtocolStack::top() {
    return protocols[topIndex];
}





