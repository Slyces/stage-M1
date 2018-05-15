#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ───────────────────────────────── imports ────────────────────────────────── #
import networkx as nx, random as rd, pytest
from threading import Lock
# ─────────────────────────── project code imports ─────────────────────────── #
from RoutingTable import RoutingTable, Row
from AdaptationFunction import AdaptationFunction, EC, DC, CV
from Messages import Message, ConfigurationMessage
from Nodes import Node
from Network import Network


# ────────────────────────── utilitaries for tests ─────────────────────────── #
def random_network(nodes):
    n = Network(nx.erdos_renyi_graph(nodes, 0.15))
    print("\n# ---------------------------- Network : ----------------------------- #")
    for line in nx.generate_adjlist(n.graph):
        print(" " * 4 + line.split()[0] + " → " + " - ".join(line.split()[1:]))
    return n

# ─────────────────── thread-safe counter to count things ──────────────────── #
def safe_increment(counter, inc):
    counter.lock.acquire()
    counter += inc
    counter.lock.release()

# ─────────────────────────── tests communication ──────────────────────────── #
def test_initialisation_messages():

    class TestNode(Node):
        received = 0
        lock = Lock()
        def receive(self, sender_id, item):
            if isinstance(item, ConfigurationMessage):
                print("\nRouter ({:1}) | {} from ({:1})".format(
                    self.id, item, sender_id), end='')
                TestNode.lock.acquire()
                TestNode.received += 1
                TestNode.lock.release()
        def send(self, receiver_id, message):
            super().send(receiver_id, message)
            print("\n{} sending {}".format(self.id, message), end='')

    network = random_network(10)
    network.set_nodes(dict([(node_id, TestNode(node_id, network))
        for node_id in network.graph.nodes]))

    expected_received = sum([len(node.neighbors_id) * len(node.In)
        for node in network.threads.values()])

    network.start(1)
    assert TestNode.received == expected_received

# ────────────────────────────── test messages ─────────────────────────────── #
protocols = "abcdefghi"

def random_stack(alphabet, min_size, max_size):
    return [rd.choice(alphabet) for x in range(rd.randint(min_size, max_size))]

def test_messages():
    for i in range(10):
        H = random_stack(protocols, 2, 10)
        with pytest.raises(Exception):
            Message(-1, 1, H, "~payload~", len(H) - 1)
        msg = Message(-1, 1, H, "~payload~", 10)

        print(msg)

# ──────────────────────── test adaptation functions ───────────────────────── #
conversion = lambda x, y: AdaptationFunction(x, y, CV)
encapsulation = lambda x, y: AdaptationFunction(x, x + y, EC)
decapsulation = lambda x, y: AdaptationFunction(x + y, x, DC)

def test_adapt_func_validity():
    # Correct adaptation functions
    for x in protocols:
        for y in protocols:
            AdaptationFunction(x, y, CV)  # should pass
            AdaptationFunction(x, x + y, EC)
            AdaptationFunction(x, x + x, EC)
            AdaptationFunction(x + y, x, DC)
            AdaptationFunction(y + y, y, DC)
    # Incorrect adaptation functions
    with pytest.raises(Exception):
        for x in protocols:
            for y in protocols:
                AdaptationFunction(x, y, EC)
                AdaptationFunction(x, y, DC)
                AdaptationFunction(x + y, y, DC)
                AdaptationFunction(x, y + x, EC)
                AdaptationFunction(x + y, y + x, CV)

def test_adapt_func_hash():
    for x in protocols:
        for y in protocols:
            cv1 = conversion(x, y)
            cv2 = conversion(x, y)
            assert cv1 is not cv2 and hash(cv1) == hash(cv2) and cv1 == cv2

            ec1 = AdaptationFunction(x, x + y, EC)
            ec2 = AdaptationFunction(x, x + y, EC)
            assert ec1 is not ec2 and hash(ec1) == hash(ec2) and ec1 == ec2

            dc1 = AdaptationFunction(x + y, x, DC)
            dc2 = AdaptationFunction(x + y, x, DC)
            assert dc1 is not dc2 and hash(dc1) == hash(dc2) and dc1 == dc2

def test_adapt_func_apply():
    n = 10
    for x in protocols:
        for y in protocols:
            H = random_stack(protocols, n, n)
            msg_x = Message("start", "end", H + [x],  2 * n)
            msg_y = Message("start", "end", H + [y],  2 * n)
            msg_xy = Message("start", "end", H + [x, y],  2 * n)
            msg_yx = Message("start", "end", H + [y, x],  2 * n)

            encap_x = encapsulation(x, y)
            decap_yx = decapsulation(x, y)
            conv_x = conversion(x, y)

            # Correct calls
            assert conv_x.apply(msg_x).stack == H + [y]
            assert conv_x.apply(msg_yx).stack == H + [y, y]

            assert encap_x.apply(msg_x).stack == H + [x, y]
            assert encap_x.apply(msg_yx).stack == H + [y, x, y]

            assert decap_yx.apply(msg_yx).stack == H + [y]

            with pytest.raises(Exception):
                conv_x.apply(msg_y)
                conv_x.apply(msg_xy)

                encap_x.apply(msg_y)
                encap_x.apply(msg_xy)

                decap_yx.apply(msg_x)
                decap_yx.apply(msg_y)
                decap_yx.apply(msg_xy)


def test_adapt_func_reverse():
    n = 4
    for x in protocols:
        for y in protocols:
            H = random_stack(protocols, n, n)
            msg_x = Message("start", "end", H + [x], 2 * n)
            msg_yx = Message("start", "end", H + [y, x], 2 * n)

            encap = encapsulation(x, y)
            decap = decapsulation(x, y)
            conv = conversion(x, y)

            assert msg_x.stack == conv.reverse.apply(conv.apply(msg_x)).stack
            assert msg_x.stack == encap.reverse.apply(encap.apply(msg_x)).stack
            assert msg_yx.stack == decap.reverse.apply(decap.apply(msg_x)).stack

# ──────────────────────────── test routing table ──────────────────────────── #
def test_routing_table_add_valid():
    table = RoutingTable()
    nodes = [i for i in range(10)]
    for node in nodes:
        H = random_stack(protocols, 20, 20)
        function = AdaptationFunction(H[-1], H[-1], CV)
        assert table.add_route(node, H, node, function, 1)
        row = table.get(node, H)
        assert row.cost == 1 and row.next_hop == node and row.func == function

def test_routing_table_add_invalid():
    table = RoutingTable()
    function = AdaptationFunction("a", "a", CV)
    assert table.add_route(0, ["a"], 1, function, 12)
    for i in range(30):
        assert not table.add_route(0, ["a"], 1, function, rd.randint(12, 20))
