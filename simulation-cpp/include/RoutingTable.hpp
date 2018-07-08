//
// Created by slassourreui on 18/06/18.
//

#ifndef SIMULATION_CPP_ROUTINGTABLE_HPP
#define SIMULATION_CPP_ROUTINGTABLE_HPP

#include <unordered_map>
#include <map>
#include "ProtocolStack.hpp"
#include "AdaptationFunction.hpp"

struct Row {
    int dest;
    ProtocolStack * stack;
    AdaptationFunction function;
    int next_hop;
    int cost;
};

struct Key {
    int dest;
    ProtocolStack * stack;
};
bool operator==(const Key &lhs, const Key &rhs);
bool operator<(const Key &lhs, const Key &rhs);


unsigned int MurmurHash2(const void *key, int len, unsigned int seed);

namespace std {
    template<> struct hash<Key> {
        size_t operator()(const Key &k) const noexcept {
            size_t stackHash = MurmurHash2(k.stack->protocols, sizeof(protocol) * (k.stack->topIndex + 1), 324157103);
            stackHash ^= MurmurHash2(&k.dest, sizeof(k.dest), 2314654);
            return stackHash;
        }
    };
}

class RoutingTable {
  public:
    std::unordered_map<Key, Row> table;

    RoutingTable();

    ~RoutingTable();

    bool contains(int dest, ProtocolStack * stack);

    bool contains(int dest);

    Row get(int dest, ProtocolStack * stack);

    bool addRoute(int dest, int next_hop, int cost,
                  AdaptationFunction function, ProtocolStack * stack);

    std::string toString();

};

#endif //SIMULATION_CPP_ROUTINGTABLE_HPP
