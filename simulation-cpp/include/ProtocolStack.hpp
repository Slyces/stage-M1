#ifndef SIMULATION_CPP_PROTOCOLS_HPP
#define SIMULATION_CPP_PROTOCOLS_HPP

#include <cstdio>
#include <string>

using namespace std;

typedef char protocol;

class ProtocolStack {
  public:
    const size_t size;
    protocol * protocols;
    int topIndex;

    explicit ProtocolStack(size_t size);
    ProtocolStack(size_t size, string);
    ~ProtocolStack();

  public:
    bool empty();
    bool full();
    void push(protocol p);
    protocol top();
    protocol top(int);
    protocol pop();
    bool operator==(const ProtocolStack &stack);
    string toString()const;
    ProtocolStack * clone() const;
};

#endif //SIMULATION_CPP_PROTOCOLS_HPP
