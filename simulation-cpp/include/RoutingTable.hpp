//
// Created by slassourreui on 18/06/18.
//

#ifndef SIMULATION_CPP_ROUTINGTABLE_HPP
#define SIMULATION_CPP_ROUTINGTABLE_HPP

#include <unordered_map>
#include <map>
#include "ProtocolStack.hpp"
#include "AdaptationFunction.hpp"

struct Row;
struct Key;

namespace std { struct hash<Key>; };

class RoutingTable {
  public:
    std::map<Key, Row> table;

    RoutingTable();

    ~RoutingTable();

    bool contains(int dest, ProtocolStack * stack);

    Row get(int dest, ProtocolStack * stack);

    bool addRoute(int dest, int next_hop, int cost,
                  AdaptationFunction function, ProtocolStack * stack);

    std::string toString();
};

#endif //SIMULATION_CPP_ROUTINGTABLE_HPP
