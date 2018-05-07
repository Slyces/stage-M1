#!/usr/bin/env python
__author__ = "Simon Lassourreuille"

# --------------------------------- imports ---------------------------------- #
from main import Simulation
import networkx as nx

# --------------------------------- globals ---------------------------------- #
N = 10  # default hyperparameter for stochastic testing

# ---------------------------------- tests ----------------------------------- #
def test_init():
    # generating a bunch of graphs with nx
    for i in range(N):
        er = nx.erdos_renyi_graph(100, 0.15)
        sim = Simulation(er)
