#!/usr/bin/env python
# -*- coding: utf-8 -*-

# --------------------------------- imports ---------------------------------- #
import networkx as nx, random #, matplotlib.pyplot as plt
from threading import Thread, Condition
from queue import Queue
from time import sleep

# --------------------------------- globals ---------------------------------- #
QUEUE_SIZE = 100

# --------------------------- utilitary functions ---------------------------- #
def make_directed_graph(graph):
    "transforms an undirected graph into a symmetric directed graph"
    new_graph = nx.DiGraph()
    for a, b in graph.edges_iter():
        new_graph.add_edge(a, b)
        new_graph.add_edge(b, a)
    return new_graph

# ---------------------------- the network class ----------------------------- #
class Network(object):
    """
    This class creates and holds the different nodes and let them communicate
    network[x] is the node x
    network[x, y]
    """
    def __init__(self, graph):
        """
        Creates the network according to a graph.
        """
        if isinstance(graph, nx.Graph):
            graph = make_directed_graph(graph)
        self.graph = graph

        # creating every router (node)
        self.threads = {}
        for node_id in graph.nodes_iter():
            self.threads[node_id] = Node(node_id, self)

        self.links = {}
        for edge in graph.edges_iter():
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

    def start(self):
        "starts every thread"
        print("# ---------------------------- simulation started ---------------" \
                "------------- #")
        for thread in self.threads.values():
            thread.start()

        x = self.threads.values()[0]
        y = x.neighbors_id[0]

        # self.send(x.id, y, Message(x.id, y, ["a"], 0))

        while True:
            sleep(1)
        # nx.draw(self.graph, arrows = True, with_labels = True,
                # nodecolor = 'b')
        # plt.show()

# ------------------------- the router (node) class -------------------------- #
class Node(Thread):
    """
    Each node represents a router in the network
    """
    def __init__(self, node_id, network, adapt_functions=None, **kwargs):
        """
        :param node_id: any unique identifier
        :param network: a data structure holding every node
        """
        Thread.__init__(self, **kwargs)

        #temp
        if adapt_functions is None:
            adapt_functions = [
                    AdaptationFunction(random.choice(("a", "b", "c")),
                        random.choice(("a", "b", "c")), CV) for i in range(2)
            ]
        # --------------- communication related initialisation --------------- #
        self.daemon = True
        self.id = node_id
        self.condition = Condition()  # a tool to receive a notification from 
                                      # other threads
        self.network = network
        self.adapt_functions = adapt_functions
        self.routing_table = RoutingTable()

    @property
    def neighbors_id(self):
        "returns the id of this node's neighbors"
        return self.network.graph.neighbors(self.id)

    # ------------------------- adaptation functions ------------------------- #
    @property
    def In(self):
        return list(set([x._from for x in self.adapt_functions]))

    @property
    def Out(self):
        return list(set([x._to for x in self.adapt_functions]))

    # ---------------------- send and receive messages ----------------------- #
    def send(self, receiver_id, item):
        self.network.links[self.id, receiver_id].put(item)
        self.network.notify(receiver_id)

    def receive(self, sender_id, item):
        "called upon reception of an item sent by anoother node"
        if type(item) is str:
            print("{:02d} ⟶  {:02d} : {}".format(sender_id, self.id, item))

            # ------------- temporarily send it to a random neighbor ------------- #
            next_hop = random.choice(self.neighbors_id)
            sleep(1)
            self.send(next_hop, item)
        elif isinstance(item, Message):
            print("\nreceived message >> {} from {}".format(item, sender_id))

    # ---------------------------- initialisation ---------------------------- #
    def init(self):
        "Sends messages to each neighbors to initialise the routing table"
        for x in self.In:
            for n_id in self.neighbors_id:
                message = Message(self.id, n_id, [x], 0)
                self.send(n_id, message)

    # ------------------------ wait for notifications ------------------------ #
    def wait_for_messages(self):
        self.init()
        while True:
            self.condition.wait()
            for sender in self.neighbors_id:
                link = self.network.links[sender, self.id]
                while not link.empty():
                    self.receive(sender, link.get_nowait())

    # ---------------------------- run the thread ---------------------------- #
    def run(self):
        self.condition.acquire()
        self.wait_for_messages()

class Message(object):
    def __init__(self, src, dst, stack, cost):
        self.src = src
        self.dst = dst
        self.stack = stack
        self.cost = cost

    def push_to_stack(self, x):
        self.stack.append(x)

    def pop_from_stack(self):
        return self.stack.pop()

    def __str__(self):
        return "{} ⟶ {} : [cost: {}] {}".format(self.src, self.dst,
                self.cost, ' > '.join(["'" + str(x) + "'" for x in self.stack]))


EC, DC, CV = 1, 2, 3
class AdaptationFunction(object):
    def __init__(self, _from, _to, _type):
        """
        :param type_: The type of adaptation. Must be either :
                      - EC : encapsulation
                      - DC : decapsulation
                      - CV : conversion
        """
        assert _type in (EC, DC, CV)
        self._from = _from
        self._to = _to
        self.type = _type

        def __str__(self):
            return "({} \longrightarrow {}, {})".format(self._from, self._to,
                    {EC: "encap", DC: "decap", CV: "conv"}[self._type])

class RoutingTable(object):
    def __init__(self):
        self.table = {}

    def add_row(self, dest, pile, next_hop, cost):
        self.table[dest, pile] = (next_hop, cost)

    @property
    def rows(self):
        return [list(key) + list(value) for key, value in self.table.items()]

if __name__ == '__main__':
    network = Network(nx.erdos_renyi_graph(20, 0.15))
    network.start()
