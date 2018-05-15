#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ----------------------------- library imports ------------------------------ #
from threading import Thread, Condition
from queue import Queue
import random
# ---------------------------- custom code import ---------------------------- #
from AdaptationFunction import AdaptationFunction, EC, DC, CV
from RoutingTable import RoutingTable
from Messages import Message, ConfigurationMessage

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
    def send(self, receiver_id, message):
        "sends a specific message to a node"
        self.network.links[self.id, receiver_id].put(message)
        self.network.notify(receiver_id)

    def receive(self, sender_id, message):
        "called upon reception of an message sent by anoother node"
        if isinstance(message, Message):
            self.route(sender_id, message)

    # ------------------------ broadcast to neighbors ------------------------ #
    def send_neighbors(self, message):
        "sends the same item to each neighbor"
        for node_id in self.neighbors:
            self.send(node_id, message)

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
