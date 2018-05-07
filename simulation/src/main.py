#!/usr/bin/env python
__author__ = "Simon Lassourreuille"

# --------------------------------- imports ---------------------------------- #
import networkx as nx


# --------------------------------- classes ---------------------------------- #
class Simulation(object):

    def __init__(self, di_graph):
        """
        Generates a data structure to implement routing on the given graph

        Args:
            di_graph: A symmetrical, directed graph from nx_graph (nx.DiGraph)
        """
        if not isinstance(di_graph, nx.DiGraph):
            raise TypeError("This class expects an instance of networkx.DiGraph")


