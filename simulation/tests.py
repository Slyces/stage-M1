#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ───────────────────────────────── imports ────────────────────────────────── #
import networkx as nx, random as rd, pytest
from simulation import Message, Network, AdaptationFunction, CV, EC, DC

# ────────────────────────── utilitaries for tests ─────────────────────────── #
def random_network():
    n = Network(nx.erdos_renyi_graph(6, 0.15))
    print("\n# ---------------------------- Network : ----------------------------- #")
    for line in nx.generate_adjlist(n.graph):
        print("\t" + line)
    return n

# ────────────────────────────────── tests ─────────────────────────────────── #
def test_receive():
    def receive_text(self, sender_id, item):
        if type(item) is str:
            print("{:02d} ⟶  {:02d} : {}".format(sender_id, self.id, item))
            # ----------- temporarily send it to a random neighbor ----------- #
            next_hop = random.choice(self.neighbors_id)
            sleep(1)
            self.send(next_hop, item)

    network = random_network()
    network.receive = receive_text
    network.start(5)

def test_initialisation_messages():
    def receive_init_message(self, sender_id, item):
        if isinstance(item, ConfigurationMessage):
            print("\nRouter ({:2}) | {} from ({:2})".format(self.id, item, sender_id), end='')

    network = random_network()
    network.receive = receive_init_message
    network.start(5)

def test_messages():
    for i in range(10):
        n = rd.randint(1, 5)
        H = []
        for k in range(n):
            H.append(rd.choice("abcde"))

        if len(H) > 3:
            with pytest.raises(Exception):
                msg = Message(-1, 1, H, "~payload~", 3)
        else:
            msg = Message(-1, 1, H, "~payload~", 3)
            print(msg)


def test_adaptation_functions():
    p1 = "abcde"
    p2 = "fghij"
    # Correct adaptation functions
    for x in p1 + p2:
        for y in p1 + p2:
            AdaptationFunction(x, y, CV)  # should pass
            AdaptationFunction(x, x + y, EC)
            AdaptationFunction(x, x + x, EC)
            AdaptationFunction(x + y, x, DC)
            AdaptationFunction(y + y, y, DC)
    # Incorrect adaptation functions
    with pytest.raises(Exception):
        for x in p1 + p2:
            for y in p1 + p2:
                AdaptationFunction(x, y, EC)
                AdaptationFunction(x, y, DC)
                AdaptationFunction(x + y, y, DC)
                AdaptationFunction(x, y + x, EC)
                AdaptationFunction(x + y, y + x, CV)
