#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ----------------------------- library imports ------------------------------ #
from threading import Thread, Condition
from queue import Queue
from time import time
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
    last_received = 0
    def __init__(self, node_id, network, adapt_functions=None, **kwargs):
        """
        :param node_id: any unique identifier
        :param network: a data structure holding every node
        """
        Thread.__init__(self, **kwargs)

        # temp
        if adapt_functions is None:
            adapt_functions = [
                AdaptationFunction(random.choice(("a", "b", "c")),
                                   random.choice(("a", "b", "c")), CV) for i in range(2)
                ]
        # --------------- communication related initialisation --------------- #
        self.daemon = True
        self.id = node_id
        self.network = network
        self.wake_buffer = Queue()
        self.adapt_functions = adapt_functions
        self.routing_table = RoutingTable()
        for function in self.adapt_functions:
            self.routing_table.add_route(self.id, list(function._from),
                    self.id, function, 0)

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
        self.network.threads[receiver_id].wake_buffer.put(self.id)
        # if isinstance(message, ConfigurationMessage):
        #     if self.id == 'B' and message.dest == 'D':
        #         print("({}) SENDING to {} : {} → {} → {} - {}".format(self.id, receiver_id, self.id, message.stack, message.dest, message.cost))

    def receive(self, sender_id, message):
        "called upon reception of an message sent by anoother node"
        # ----------------------------- routing ------------------------------ #
        if isinstance(message, Message):
            if message.dest == self.id:
                self.destination_reached(message)
            else:
                self.route(sender_id, message)
        # ------------------ configuring the routing table ------------------- #
        if isinstance(message, ConfigurationMessage):
            # if self.id == 'A' and sender_id == 'B':
            #     print("({}) new {} → {} → {} - {}".format(self.id, sender_id, message.stack, message.dest, message.cost))
            for function in self.adapt_functions:
                # if self.id == 'B' and message.dest == 'D':
                #     top_protocols = tuple(message.stack[-len(function._from):])
                #     print(function, message.stack, top_protocols, tuple(function._from))
                msg_copy = message.copy()
                if function.reverse.appliable(msg_copy):
                    in_stack = function.reverse.apply_to_stack(msg_copy.stack)
                    cost = self.network.links[self.id, sender_id].cost(function) + msg_copy.cost
                    added = self.routing_table.add_route(
                        msg_copy.dest, in_stack, sender_id, function, cost)
                    if added:
                        for n_id in self.neighbors_id:
                            self.send(n_id, ConfigurationMessage(msg_copy.dest, in_stack, cost))
            Node.last_received = time()

    # ---------------------------- route messages ---------------------------- #
    def route(self, sender_id, message):
        "uses the routing table to route any received message"
        stack, dest = message.stack, message.dest
        if (dest, stack) in self.routing_table:
            row = self.routing_table.get(dest, stack)
            row.function.apply(message)
            if message.valid:
                self.send(row.next_hop, message)

    def destination_reached(self, message):
        """Any message passed through this function was destined to this router"""
        print("The message reached its destination:\n\t- {}\n" \
              "\tArrived to {}".format(message, self.id))

    # ---------------------------- initialisation ---------------------------- #
    def init(self):
        """
        Sends messages to each neighbors to initialise the routing table
        """
        for x in self.In:
            for n_id in self.neighbors_id:
                self.send(n_id, ConfigurationMessage(self.id, list(x), 0))

    # ------------------------ wait for notifications ------------------------ #
    def wait_for_messages(self):
        self.init()
        # print(">> {} Finished sending".format(self.id))
        while True:
            sender = self.wake_buffer.get(block=True)  # blocks
            link = self.network.links[sender, self.id]
            while not link.empty():
                self.receive(sender, link.get_nowait())

    # ---------------------------- run the thread ---------------------------- #
    def run(self):
        # self.condition.acquire()
        self.wait_for_messages()
