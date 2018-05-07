#!/usr/bin/env python
__author__ = "Simon Lassourreuille"

# --------------------------------- imports ---------------------------------- #
import networkx as nx

# --------------------------- utilitary functions ---------------------------- #
def make_symmetric(graph):
    "Creates a directed symmetrical graph from a classic graph"
    assert isinstance(graph, nx.Graph) and not isinstance(graph, nx.DiGraph)
    new_graph = nx.DiGraph()
    for a, b in graph.edges_iter():
        new_graph.add_edge(a, b)
        new_graph.add_edge(b, a)
    return new_graph

# --------------------------------- classes ---------------------------------- #
class Simulation(object):

    def __init__(self, di_graph):
        """
        Generates a data structure to implement routing on the given graph

        Args:
            di_graph: A symmetrical, directed graph from nx_graph (nx.DiGraph)
        """
        assert isinstance(di_graph, nx.DiGraph)
