//
// Created by slyces on 22/06/18.
//

#ifndef SIMULATION_CPP_LINK_HPP
#define SIMULATION_CPP_LINK_HPP

#include <vector>
#include <unordered_map>
#include "AdaptationFunction.hpp"

class Link {
  public:
    std::unordered_map<AdaptationFunction, unsigned int> costs;
  public:
    Link();
    explicit Link(unsigned int default_cost);
    Link(unsigned int default_cost, const std::vector<AdaptationFunction> &);
    ~Link() = default;

    void addFunction(AdaptationFunction f);
    void addFunction(AdaptationFunction f, unsigned int cost);
    unsigned int getCost(AdaptationFunction);

    unsigned int default_cost;
};

//struct LinkProperty {
//    Link link;
//};

#endif //SIMULATION_CPP_LINK_HPP
