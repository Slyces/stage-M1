#include <vector>
#include <Link.hpp>

Link::Link(unsigned int default_cost) {
    this->default_cost = default_cost;
}

Link::Link(unsigned int default_cost, const std::vector<AdaptationFunction> &functions) {
    this->default_cost = default_cost;
    for (auto &function : functions)
        addFunction(function);
}

void Link::addFunction(AdaptationFunction function) {
    costs[function] = default_cost;
}

void Link::addFunction(AdaptationFunction function, unsigned int cost) {
    costs[function] = cost;
}

unsigned int Link::getCost(AdaptationFunction function) {
    if (costs.find(function) != costs.end())
        return costs[function];
    else
        return default_cost;
}


