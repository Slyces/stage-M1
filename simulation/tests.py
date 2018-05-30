#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ───────────────────────────────── imports ────────────────────────────────── #
from time import time, sleep

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


# ─────────────────────────── tests communication ──────────────────────────── #
@pytest.mark.focus
def test_initialisation_messages():
    print("=" * 80)
    print("TEST WITH A LOT OF PRINTS".center(80))

    # --------------------------- inner test class --------------------------- #
    class TestNode(Node):
        def receive(self, sender_id, message):
            if isinstance(message, ConfigurationMessage):
                self.conf_received += 1
                Node.last_conf_received = time()
            Node.last_received = time()

    network = random_network(50)
    nodes = dict([(node_id, TestNode(node_id, network))
                  for node_id in network.graph.nodes])
    network.set_nodes(nodes)

    expected_received = sum([len(node.neighbors_id) * len(node.In)
                             for node in network.threads.values()])
    network.start(0.02)
    received = sum([x.conf_received for x in nodes.values()])
    sent = sum([x.conf_sent for x in nodes.values()])
    # print("received : {}, sent : {}, expected : {}".format(received, sent, expected_received))
    assert received == sent == expected_received
    print('\n' + ' Converged in {:5f} -- Stopped in {:5f} '.center(60, '=').format(network.convergence_time(),
                                                                                   network.duration))
    print("=" * 80)


# ────────────────────────────── test messages ─────────────────────────────── #
protocols = "abcdefghi"


def random_stack(alphabet, min_size, max_size):
    return [rd.choice(alphabet) for x in range(rd.randint(min_size, max_size))]


def test_messages():
    for i in range(10):
        H = random_stack(protocols, 2, 10)
        msg = Message(-1, 1, H, "~payload~", len(H) - 1)
        assert not msg.valid
        msg = Message(-1, 1, H, "~payload~", 10)
        assert msg.valid
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
            msg_x = Message("start", "end", H + [x], 2 * n)
            msg_y = Message("start", "end", H + [y], 2 * n)
            msg_xy = Message("start", "end", H + [x, y], 2 * n)
            msg_yx = Message("start", "end", H + [y, x], 2 * n)

            encap_x = encapsulation(x, y)
            decap_xy = decapsulation(x, y)
            conv_x = conversion(x, y)

            # Correct calls
            assert conv_x.apply(msg_x.copy()).stack == H + [y]
            assert conv_x.apply(msg_yx.copy()).stack == H + [y, y]

            assert encap_x.apply(msg_x.copy()).stack == H + [x, y]
            assert encap_x.apply(msg_yx.copy()).stack == H + [y, x, y]

            assert decap_xy.apply(msg_xy.copy()).stack == H + [x]

            if x != y:
                with pytest.raises(Exception):
                    conv_x.apply(msg_y.copy())
                    conv_x.apply(msg_xy.copy())

                    encap_x.apply(msg_y.copy())
                    encap_x.apply(msg_xy.copy())
                    decap_xy.apply(msg_yx.copy())


def test_adapt_func_reverse():
    def chain_apply(msg, f, g):
        copy = msg.copy()
        f.apply(copy)
        return g.apply(copy)

    n = 4
    for x in protocols:
        for y in protocols:
            H = random_stack(protocols, n, n)
            msg_x = Message("start", "end", H + [x], 2 * n)
            msg_y = Message("start", "end", H + [y], 2 * n)
            msg_xy = Message("start", "end", H + [x, y], 2 * n)

            encap_x_xy = encapsulation(x, y)
            decap_xy_x = decapsulation(x, y)
            conv_x_y = conversion(x, y)

            assert msg_x.stack == chain_apply(msg_x.copy(), conv_x_y, conv_x_y.reverse).stack
            assert msg_y.stack == chain_apply(msg_y.copy(), conv_x_y.reverse, conv_x_y).stack

            assert msg_x.stack == chain_apply(msg_x.copy(), encap_x_xy, encap_x_xy.reverse).stack
            assert msg_xy.stack == chain_apply(msg_xy.copy(), encap_x_xy.reverse, encap_x_xy).stack

            assert msg_xy.stack == chain_apply(msg_xy.copy(), decap_xy_x, decap_xy_x.reverse).stack
            assert msg_x.stack == chain_apply(msg_x.copy(), decap_xy_x.reverse, decap_xy_x).stack


# ──────────────────────────── test routing table ──────────────────────────── #
def test_routing_table_add_valid():
    table = RoutingTable()
    nodes = [i for i in range(10)]
    for node in nodes:
        H = random_stack(protocols, 20, 20)
        function = AdaptationFunction(H[-1], H[-1], CV)
        assert table.add_route(node, H, node, function, 1)
        row = table.get(node, H)
        assert row.cost == 1 and row.next_hop == node and row.function == function


def test_routing_table_add_invalid():
    table = RoutingTable()
    function = AdaptationFunction("a", "a", CV)
    assert table.add_route(0, ["a"], 1, function, 12)
    for i in range(30):
        assert not table.add_route(0, ["a"], 1, function, rd.randint(12, 20))


def random_routing_table(n):
    table = RoutingTable()
    nodes = [i for i in range(n)]
    rd_prot = lambda: rd.choice(protocols)
    functions = [AdaptationFunction(rd_prot(), rd_prot(), CV)
                 for i in range(n)]
    for i in range(3 * n):
        table.add_route(rd.choice(nodes), random_stack(protocols, 1, 10),
                        rd.choice(nodes), rd.choice(functions), rd.randrange(100))
    return table


def test_routing_table_valid_get():
    for i in range(10):
        table = random_routing_table(10)
        for row in table:  # iteration actually gets every row
            assert isinstance(row, Row)


def test_routing_table_invalid_get():
    table = random_routing_table(10)
    with pytest.raises(Exception):
        for i in range(8):
            table.get(0, [])


def test_routing_table_replace_route():
    table = RoutingTable()
    function = AdaptationFunction("a", "a", CV)
    assert table.add_route(0, ["a"], 1, function, 100)
    for i in range(1, 101):
        assert table.add_route(0, ["a"], 1, function, 100 - i)  # == True


def test_routing_table_print():
    print('\n' + "=" * 80)
    print("PRINTING TEST".center(80))
    table = random_routing_table(3)
    print(table)
    print("=" * 80)


class StatsNode(Node):
    def __init__(self, *args, **kwargs):
        Node.__init__(self, *args, **kwargs)
        self.msg_routed = 0
        self.conf_sent = 0
        self.msg_received = 0
        self.conf_received = 0

    def receive(self, sender_id, message):
        super().receive(sender_id, message)
        if isinstance(message, ConfigurationMessage):
            self.conf_received += 1

    def destination_reached(self, message):
        self.msg_received += 1

    def route(self, sender_id, message):
        super().route(sender_id, message)
        self.msg_routed += 1

    def send(self, receiver_id, message):
        super().send(receiver_id, message)
        if isinstance(message, ConfigurationMessage):
            self.conf_sent += 1


def stats(nodes):
    print(" ")
    nodes_id = list(nodes.keys())
    nodes_id.sort()
    rows = []
    titles = [" "] + nodes_id
    for key in "msg_routed conf_sent msg_received conf_received".split():
        rows.append([key] + [str(getattr(nodes[node], key)) for node in nodes_id])
    total = [titles] + rows
    max_len = list(map(max, [map(len, line) for line in zip(*total)]))
    max_len = [x + 2 for x in max_len]
    string = "|".join([titles[i].center(max_len[i]) for i in range(len(titles))])
    string += '\n' + (1 * len(rows) + sum(max_len)) * "-" + '\n'
    rows.sort()
    for line in rows:
        string += " ".join([line[i].center(max_len[i]) for i in range(1)])
        string += "|"
        string += " ".join([line[i + 1].center(max_len[i + 1]) for i in range(len(nodes_id))])
        string += '\n'
    print(string)


# ────────────────────────── test automatic routing ────────────────────────── #
def routing_tables(nodes):
    nodes_id = list(nodes.keys())
    nodes_id.sort()
    for node_id in nodes_id:
        print("Routing table : {}\n".format(node_id))
        print(nodes[node_id].routing_table)
    received = sum([x.conf_received for x in nodes.values()])
    sent = sum([x.conf_sent for x in nodes.values()])
    return received, sent


def test_routing_1():
    """
    A ← → B ← → C ← → D
    -------------------
    A, B, C, D: x → x
    """
    graph = nx.Graph()
    # adding edges
    graph.add_edges_from([("A", "B"), ("B", "C"), ("C", "D")])

    network = Network(graph)
    x_to_x = AdaptationFunction("x", "x", CV)
    nodes = dict([(char, StatsNode(char, network, [x_to_x])) for char in "ABCD"])
    network.set_nodes(nodes)
    for i in range(10):
        network.send_after_convergence("A", "B", Message("A", "D", ["x"], "------"))
    network.start(0.1)
    received, sent = routing_tables(nodes)
    stats(nodes)
    assert received == sent
    assert network.sent == sum([x.msg_received for x in nodes.values()])
    print('\n' + ' Converged in {:5f} -- Stopped in {:5f} '.center(60, '=').format(network.convergence_time(),
                                                                                   network.duration))


def test_routing_2():
    """
    A ← → B ← → C ← → D
    -------------------
    A, D: x → x
    B, C: xy → x, x → xy
    """
    graph = nx.Graph()
    # adding edges
    graph.add_edges_from([("A", "B"), ("B", "C"), ("C", "D")])

    network = Network(graph)
    x_to_x = AdaptationFunction("x", "x", CV)
    x_to_xy = AdaptationFunction("x", "xy", EC)
    xy_to_x = AdaptationFunction("xy", "x", DC)
    nodes = dict(
        [(char, StatsNode(char, network, [x_to_x])) for char in "AD"] +
        [(char, StatsNode(char, network, [x_to_xy, xy_to_x])) for char in "BC"]
    )
    network.set_nodes(nodes)
    for i in range(10):
        network.send_after_convergence("A", "B", Message("A", "D", ["x"], "------"))
    network.start(0.1)
    received, sent = routing_tables(nodes)
    stats(nodes)
    assert received == sent
    assert network.sent == sum([x.msg_received for x in nodes.values()])
    print('\n' + ' Converged in {:5f} -- Stopped in {:5f} '.center(60, '=').format(network.convergence_time(),
                                                                                   network.duration))


def test_routing_3():
    """
         D   ← →   C
           ↖↘   ↙↗
    A ← →     B     ← → E ← → F ← → G
    ---------------------------------
    A, G: x → x
    B: x → x, y → y, z → z
    C: x → xh, y → yh
    D: h → y, h → z
    E: yz → y
    F: xy → x
    ---------------------------------
    Desired route :
    A → B → C → D  → B  → C  → D   → B   → E   → F  → G
    _ → x → x → xh → xy → xy → xyh → xyz → xyz → xy → x
    """
    graph = nx.Graph()
    # adding edges
    graph.add_edges_from([("A", "B"), ("B", "C"), ("C", "D"), ("D", "B"), ("B", "E"), ("E", "F"), ("F", "G")])

    network = Network(graph)
    x__x = AdaptationFunction('x', 'x', CV)
    y__y = AdaptationFunction('y', 'y', CV)
    z__z = AdaptationFunction('z', 'z', CV)
    x__xh = AdaptationFunction('x', 'xh', EC)
    y__yh = AdaptationFunction('y', 'yh', EC)
    h__y = AdaptationFunction('h', 'y', CV)
    h__z = AdaptationFunction('h', 'z', CV)
    yz__y = AdaptationFunction('yz', 'y', DC)
    xy__x = AdaptationFunction('xy', 'x', DC)

    nodes = {
        'A': StatsNode('A', network, [x__x]),
        'B': StatsNode('B', network, [x__x, y__y, z__z]),
        'C': StatsNode('C', network, [x__xh, y__yh]),
        'D': StatsNode('D', network, [h__y, h__z]),
        'E': StatsNode('E', network, [yz__y]),
        'F': StatsNode('F', network, [xy__x]),
        'G': StatsNode('G', network, [x__x])
    }
    network.set_nodes(nodes)
    for i in range(10):
        network.send_after_convergence("A", "B", Message("A", "G", ["x"], "------"))
    network.start(0.1)
    received, sent = routing_tables(nodes)
    stats(nodes)
    assert received == sent
    assert network.sent == sum([x.msg_received for x in nodes.values()])
    print('\n' + ' Converged in {:5f} -- Stopped in {:5f} '.center(60, '=').format(network.convergence_time(),
                                                                                   network.duration))


def test_routing_4():
    """
       B ← →  C ← → D
     ↙↗              ↖↘
    A    ← →  E  ← →   F
    ---------------------------------
    A, B, C: x → x
    D: x → y
    E: x → xy
    F: y → y
    ---------------------------------
    """
    graph = nx.Graph()
    # adding edges
    graph.add_edges_from([("A", "B"), ("B", "C"), ("C", "D"), ("D", "F"), ("A", "E"), ("E", "F")])

    network = Network(graph)
    x__x = AdaptationFunction('x', 'x', CV)
    y__y = AdaptationFunction('y', 'y', CV)
    x__y = AdaptationFunction('x', 'y', CV)
    x__xy = AdaptationFunction('x', 'xy', EC)

    nodes = {
        'A': StatsNode('A', network, [x__x]),
        'B': StatsNode('B', network, [x__x]),
        'C': StatsNode('C', network, [x__x]),
        'D': StatsNode('D', network, [x__y]),
        'E': StatsNode('E', network, [x__xy]),
        'F': StatsNode('F', network, [y__y])
    }
    network.set_nodes(nodes)
    for i in range(10):
        network.send_after_convergence("B", "A", Message("A", "F", ["x"], "------"))
    network.start()
    received, sent = routing_tables(nodes)
    stats(nodes)
    assert received == sent
    assert network.sent == sum([x.msg_received for x in nodes.values()])
    print('\n' + ' Converged in {:5f} -- Stopped in {:5f} '.center(60, '=').format(network.convergence_time(),
                                                                                   network.duration))
