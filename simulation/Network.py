
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# --------------------------------- imports ---------------------------------- #
import networkx as nx, random #, matplotlib.pyplot as plt
from threading import Thread, Condition
from queue import Queue
from time import sleep, time



# ───────────────────────────────── Network ────────────────────────────────── #
class Network(object):
    """
    This class creates and holds the different nodes and let them communicate
    network[x] is the node x
    network[x, y]
    """
    def __init__(self, graph, max_stack = 100, default_cost = 1):
        """
        Creates the network according to a graph.
        """
        if isinstance(graph, nx.Graph):
            graph = make_directed_graph(graph)
        self.graph = graph

        # creating every router (node)
        self.threads = {}
        for node_id in graph.nodes:
            self.threads[node_id] = Node(node_id, self)

        self.links = {}
        for edge in graph.edges:
            self.links[edge] = Queue(QUEUE_SIZE)

    def notify(self, node_id):
        "wakes up a thread to let him look up received messages"
        node = self.threads[node_id]

        node.condition.acquire()
        node.condition.notify()
        node.condition.release()

    def send(self, sender_id, receiver_id, item):
        "manually sends a message from sender to receiver"
        self.links[sender_id, receiver_id].put(item)
        self.notify(receiver_id)

    def start(self, duration=None):
        "starts every thread, and stops their execution after <duration> seconds"
        print("# ---------------------------- simulation started ---------------" \
                "------------- #")
        start_time = time()
        for thread in self.threads.values():
            thread.start()

        x = random.choice(list(self.threads.values()))
        y = x.neighbors_id[0]

        # self.send(x.id, y, ConfigurationMessage(x.id, y, ["a"], 0))

        while duration is None or time() - start_time < duration:
            sleep(1)

# ──────────────────── Links to communicate between nodes ──────────────────── #
class Link(Queue):
    def __init__(self, default_cost, functions_costs = None, *args, **kwargs):
        Queue.__init__(self, *args, **kwargs)
        self.functions_consts = {} if functions_costs is None else functions_costs
        self.default_cost = default_cost

    def cost(self, function):
        return self.functions_costs.get(function, self.default_cost)
