#!/usr/bin/env python
__author__ = "Simon Lassourreuille"

# --------------------------------- imports ---------------------------------- #
import networkx as nx, threading, queue, random
from time import sleep

# --------------------------------- globals ---------------------------------- #
STACK_SIZE = None
QUEUE_SIZE = 100

# threading.stack_size(STACK_SIZE)

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
class Node(threading.Thread):

    def __init__(self, node, simulation, *args, **kwargs):
        threading.Thread.__init__(self, *args, **kwargs)
        self.node = node
        self.simulation = simulation

    def on_receive(self, sender, item):
        print("{} received load {} from {}".format(
                self.getName(),
                item,
                sender
            ))
        neighbors = [x for x in self.simulation.graph[self.node].keys()]
        next_hop = random.choice(neighbors)
        sleep(1)
        self.send(next_hop, item)

    def notify(self, receiver):
        self.simulation.conditions[receiver].acquire()
        self.simulation.conditions[receiver].notify()
        self.simulation.conditions[receiver].release()

    def send(self, receiver, item):
        self.simulation.queues[self.node, receiver].put(item)
        self.notify(receiver)

    def run(self):
        node = self.node
        self.simulation.conditions[node].acquire()
        while True:
            self.simulation.conditions[node].wait()
            for sender in self.simulation.graph[node]:
                if not self.simulation.queues[sender, node].empty():
                    self.on_receive(sender, self.simulation.queues[sender, node].get_nowait())

class Simulation(object):

    def __init__(self, di_graph):
        """
        Generates a data structure to implement routing on the given graph
        Creates a thread for each node of the graph. Two threads are able to
        communicate if and only if their relatives nodes are connected.

        Args:
            di_graph: A symmetrical, directed graph from nx_graph (nx.DiGraph)
        """
        assert isinstance(di_graph, nx.DiGraph)
        self.graph = di_graph

        self.threads = {}
        self.conditions = {}
        for node in self.graph.nodes_iter():
            new_thread = Node(node, self)

            new_thread.setName("Thread {}".format(node))
            self.threads[node] = new_thread
            self.conditions[node] = threading.Condition()

        self.queues = {}
        for a, b in self.graph.edges_iter():
            self.queues[a, b] = queue.Queue(QUEUE_SIZE)


    def start(self):
        for thread_num in self.threads:
            self.threads[thread_num].start()

        x = self.graph.nodes()[0]
        y = self.graph[x].keys()[0]

        self.queues[x, y].put("<message>")
        self.conditions[y].acquire()
        self.conditions[y].notify()
        self.conditions[y].release()

