//
// Created by slassourreui on 18/06/18.
//

#ifndef SIMULATION_CPP_ROUTINGTABLE_HPP
#define SIMULATION_CPP_ROUTINGTABLE_HPP

#include "ProtocolStack.hpp"
#include "AdaptationFunction.hpp"

typedef struct {
    int dest, next_hop, cost;
    ProtocolStack stack;
    AdaptationFunction function;
} row;

class RoutingTable {
  public:
    RoutingTable();
    ~RoutingTable();

    bool contains(int dest, ProtocolStack & stack);
    row get(int dest, ProtocolStack & stack);
    bool addRoute(int dest, int next_hop, int cost,
                  AdaptationFunction function, ProtocolStack stack);
    string toString();
};

#endif //SIMULATION_CPP_ROUTINGTABLE_HPP
