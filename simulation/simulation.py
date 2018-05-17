#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ----------------------------- library imports ------------------------------ #
import networkx as nx, random
from networkx.fenerators.random_graphs import barabasi_albert_graph
from time import sleep, time
# --------------------------- custom code imports ---------------------------- #
from Nodes import Node

# ───────────────────────────────── globals ────────────────────────────────── #

def run(size, p, nb_protocols, max_stack):
    """
    :param size: the number of nodes of the generated network
    :param p: probability for each adaptation function to be in a router
    :param nb_protocols: number of protocols used in the simulation
    :praram max_stack: maximum size of the stack carried by each message
    """
    graph = barabasi_albert()
