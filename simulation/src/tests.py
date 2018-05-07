#!/usr/bin/env python
__author__ = "Simon Lassourreuille"

# --------------------------------- imports ---------------------------------- #
from main import Simulation, make_symmetric
import networkx as nx
import matplotlib.pyplot as plt

# --------------------------------- globals ---------------------------------- #
N = 10  # default hyperparameter for stochastic testing

# ---------------------------------- tests ----------------------------------- #
def test_init():
    # generating a bunch of graphs with nx
    for i in range(N):
        er = nx.erdos_renyi_graph(20, 0.15)
        digraph = make_symmetric(er)
        sim = Simulation(digraph)


# --------------------------- interactive testing ---------------------------- #
if __name__ == '__main__':
    erdos_graph = nx.erdos_renyi_graph(20, 0.15)
    digraph = make_symmetric(erdos_graph)
    sim = Simulation(digraph)
    sim.start()
