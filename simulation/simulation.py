#!/usr/bin/env python3
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
    def __init__(self, graph):
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

    def start(self):
        "starts every thread"
        print("# ---------------------------- simulation started ---------------" \
                "------------- #")
        for thread in self.threads.values():
            thread.start()

        x = random.choice(list(self.threads.values()))
        y = x.neighbors_id[0]

        # self.send(x.id, y, ConfigurationMessage(x.id, y, ["a"], 0))

        while True:
            sleep(1)
        # nx.draw(self.graph, arrows = True, with_labels = True,
                # nodecolor = 'b')
        # plt.show()

# ───────────────────────────────── Routers ────────────────────────────────── #
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
        return list(self.network.graph.neighbors(self.id))

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
        elif isinstance(item, ConfigurationMessage):
            print("\nRouter ({:2}) | {} from ({:2})".format(self.id, item, sender_id), end='')
        elif isinstance(item, Message):
            self.route(sender_id, message)

    # ---------------------------- route messages ---------------------------- #
    def route(self, sender_id, message):
        "uses the routing table to route any received message"
        if message.dst == self.id:
            print("The message reached its destination:\n\t- {}\n\tArrived to {} from hop {}".format(message, self.id, sender_id))

    # ---------------------------- initialisation ---------------------------- #
    def init(self):
        "Sends messages to each neighbors to initialise the routing table"
        for x in self.In:
            for n_id in self.neighbors_id:
                message = ConfigurationMessage(self.id, n_id, [x], 0)
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

# ───────────────────────────────── Messages ───────────────────────────────── #
class ConfigurationMessage(object):
    def __init__(self, src, dst, stack, cost):
        "Configuration message exchanged between routers to build the routing table"
        self.src = src
        self.dst = dst
        self.stack = stack
        self.cost = cost

    def push_to_stack(self, x):
        self.stack.append(x)

    def pop_from_stack(self):
        return self.stack.pop()

    def __str__(self):
        return "{:2} ⟶ {:2} : [cost: {}] {}".format(self.src, self.dst,
                self.cost, ' > '.join(["'" + str(x) + "'" for x in self.stack]))

# ────────────────────────────── Routed message ────────────────────────────── #
class Message(object):
    def __init__(self, src, dst, stack, payload, max_height=100):
        "Regular message sent between routers to convey any content"
        self.dst = dst
        self.src = src
        self.__stack_height = len(stack)
        self.stack = stack
        self.payload = payload

    @property
    def stack_height(self):
        "getter for stack height"
        return self.__stack_height

    @stack_height.setter
    def stack_height(self, new_value):
        "setter for stack height, being carefull with max_height"
        assert new_value <= self.max_height, "Max stack height exceeded"
        self.__stack_height = new_value

    def __str__(self):
        "src: {s}, dst: {d}, stack: {}, payload: «{}»".format(
            s = self.src, d = self.dst, stack = self.stack, payload = self.payload
        )

# ─────────────────────────── Adaptation functions ─────────────────────────── #
EC, DC, CV = 1, 2, 3
class AdaptationFunction(object):
    def __init__(self, _from, _to, _type):
        """
        :param _type: The type of adaptation. Must be either :
                      - EC : encapsulation
                      - DC : decapsulation
                      - CV : conversion
        :param _from: The input protocol of the function
                      - string or tuple of protocols, last element is the top of the stack
        :param _to:   The output protocol of the function
                      - string or tuple of protocols, last element is the top of the stack
        """
        assert _type in (EC, DC, CV)

        # convert strings to tuples
        self._from = tuple(_from)) if _from is str else _from
        self._to = tuple(_to)) if _to is str else _to
        self._type = _type

        # protocol length in conversion
        if self._type == CV:
            assert len(self._from) == len(self._to) == 1, "can only convert one protocol"

        # protocol length in encapsulation
        if self._type == EC:
            assert len(self._from) == 1 and len(self._to) == 2, \
                    "encapsulations takes 1 protocol in and 2 protocols out"
            assert self._from[0] == self._to[0], \
                    "encapsulation can not change the initial protocol"

        # protocol length in decapsulation
        if self._type == DC:
            assert len(self._from) == 2 and len(self._to) == 1, \
                    "decapsulation takes 2 protocol in and 1 protocol out"
            assert self._from[0] == self._to[0], \
                    "decapsulation can not change the inner protocol"

    def apply(self, message):
        "Applies an adaptation function to a message"
        assert message.stack_height > 0, "not supposed to receive a message with empty stack"

        # Checking if the function may apply to this stack
        top_protocols = tuple(message.stack[len(self._from)])  # see the first or 2 firsts protocols
        assert top_procol == self._from, \
                "the function {} can't apply to top protocol {}".format(self, top_protocol)

        # apply conversion
        if self._type == CV:
            message.stack.pop()
            message.stack.push(*self._to)  # _to is only one protocol, safe to unpack with *

        # apply encapsulation
        if self._type == EC:
            message.stack.push(self._to[-1])  # push the new top protocol
            message.stack_height += 1

        # apply decapsulation
        if self._type == DC:
            message.stack.pop()  # remove the old top protocol
            message.stack_height -= 1

    def reverse(self):
        "returns the reverse operation corresponding to this adaptation function"
        # conversion
        if self._type == CV:
            return AdaptationFunction(self._to, self._from, self._type)

        # encapsulation / decapsulation : just invert the type and from / to
        return AdaptationFunction(self._to, self._from, EC if self._type == DC else DC)

    def __str__(self):
        return "({} \longrightarrow {}, {})".format(self._from, self._to,
                    {EC: "encap", DC: "decap", CV: "conv"}[self._type])

# ────────────────────────────── Routing tables ────────────────────────────── #
class RoutingTable(object):
    def __init__(self):
        self.table = {}

    def add_row(self, dest, stack, next_hop, cost):
        self.table[dest, stack] = (next_hop, cost)

    @property
    def rows(self):
        return [list(key) + list(value) for key, value in self.table.items()]

if __name__ == '__main__':
    network = Network(nx.erdos_renyi_graph(20, 0.15))
    network.start()
