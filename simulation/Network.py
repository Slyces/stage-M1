#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ----------------------------- library imports ------------------------------ #
import networkx as nx, random
from threading import Thread, Condition
from queue import Queue
from time import sleep, time
# --------------------------- custom code imports ---------------------------- #
from Nodes import Node

# ───────────────────────────────── globals ────────────────────────────────── #
QUEUE_SIZE = 100

# ─────────────────────────── utilitary functions ──────────────────────────── #
def make_directed_graph(graph):
    "transforms an undirected graph into a symmetric directed graph"
    new_graph = nx.DiGraph()
    for a, b in graph.edges:
        new_graph.add_edge(a, b)
        new_graph.add_edge(b, a)
    return new_graph

# ───────────────────────────────── Network ────────────────────────────────── #
class Network(object):
    """
    This class creates and holds the different nodes and let them communicate
    network[x] is the node x
    network[x, y]
    """
    def __init__(self, graph, nodes = None, max_stack = 100, default_cost = 1):
        """
        Creates the network according to a graph.
        """
        if isinstance(graph, nx.Graph):
            graph = make_directed_graph(graph)
        self.graph = graph

        # creating every router (node)
        self.threads = {}

        self.links = {}
        for edge in graph.edges:
            self.links[edge] = Link(default_cost, None, QUEUE_SIZE)

    def set_nodes(self, nodes):
        """
        :param nodes: A dict (node_id) → (Node instance).
        """
        assert set(nodes.keys()) == set(self.graph.nodes)
        self.threads = nodes

    def send(self, sender_id, receiver_id, item):
        "manually sends a message from sender to receiver"
        self.links[sender_id, receiver_id].put(item)
        self.threads[receiver_id].wake_buffer.put(sender_id)

    def on_loop(self, timer):
        return sleep(1)

    def start(self, duration=None):
        "starts every thread, and stops their execution after <duration> seconds"
        print("# ---------------------------- simulation started ---------------" \
                "------------- #")
        if not self.threads:
            for node_id in self.graph.nodes:
                self.threads[node_id] = Node(node_id, self)

        start_time = time()
        for thread in self.threads.values():
            thread.start()

        while duration is None or time() - start_time < duration:
            self.on_loop(time() - start_time < duration)

# ──────────────────── Links to communicate between nodes ──────────────────── #
class Link(Queue):
    def __init__(self, default_cost, functions_costs, *args, **kwargs):
        Queue.__init__(self, *args, **kwargs)
        self.functions_costs = {} if functions_costs is None else functions_costs
        self.default_cost = default_cost

    def cost(self, function):
        return self.functions_costs.get(function, self.default_cost)
