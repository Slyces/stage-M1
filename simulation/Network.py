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
QUEUE_SIZE = 1


# ─────────────────────────── utilitary functions ──────────────────────────── #
def make_directed_graph(graph):
    """transforms an undirected graph into a symmetric directed graph"""
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

    def __init__(self, graph, nodes=None, max_stack=100, default_cost=1):
        """
        Creates the network according to a graph.
        """
        if isinstance(graph, nx.Graph):
            graph = make_directed_graph(graph)
        self.graph = graph

        self.start_time = 0
        self.duration = 0
        self.to_send = []
        self.converged = False
        self.max_stack = max_stack
        self.sent = 0

        # creating every router (node)
        self.threads = {}

        self.links = {}
        for edge in graph.edges:
            self.links[edge] = Link(default_cost, None, QUEUE_SIZE)

    def send_after_convergence(self, first_node, next_hop, message):
        """
        Adds a message to be sent once the network converges
        """
        self.to_send.append((first_node, next_hop, message))

    def convergence_time(self):
        return Node.last_conf_received - self.start_time

    def set_nodes(self, nodes):
        """
        :param nodes: A dict (node_id) → (Node instance).
        """
        assert set(nodes.keys()) == set(self.graph.nodes)
        self.threads = nodes

    def send(self, sender_id, receiver_id, item):
        """manually sends a message from sender to receiver"""
        self.links[sender_id, receiver_id].put_nowait(item)
        self.threads[receiver_id].wake_buffer.put_nowait(sender_id)

    def on_loop(self):
        # print('{:5f}'.format((time() - Node.last_received) * 100))
        if time() - Node.last_received > 1e-3:
            self.duration = time() - self.start_time
            # for node in self.threads.values():
            # node.wake_buffer.join()

        sleep(1e-5)
        if time() - Node.last_conf_received > 1e-2:
            while self.to_send:
                self.send(*self.to_send.pop())
                self.sent += 1
                # Stopping the network

    def start(self, duration=None):
        """starts every thread, and stops their execution after <duration> seconds"""
        # print("# ---------------------------- simulation started ---------------" \
        # "------------- #")
        if not self.threads:
            for node_id in self.graph.nodes:
                self.threads[node_id] = Node(node_id, self)

        self.duration = duration
        self.start_time = time()
        for thread in self.threads.values():
            thread.start()

            # while self.duration is None or time() - self.start_time < self.duration:
            # # self.on_loop(time() - self.start_time)
            buffers = [node.wake_buffer for node in self.threads.values()]
        while not all([b.empty() for b in buffers]):
            # for b in buffers:
            # b.join()
            sleep(0.1)
        self.duration = time() - self.start_time


# ──────────────────── Links to communicate between nodes ──────────────────── #
class Link(Queue):
    def __init__(self, default_cost, functions_costs, *args, **kwargs):
        Queue.__init__(self, *args, **kwargs)
        self.functions_costs = {} if functions_costs is None else functions_costs
        self.default_cost = default_cost

    def cost(self, function):
        return self.functions_costs.get(function, self.default_cost)
