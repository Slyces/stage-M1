#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ----------------------------- library imports ------------------------------ #
import networkx as nx, random, pytest, argparse
from networkx.generators.random_graphs import barabasi_albert_graph
from time import sleep, time
from statistics import mean, pvariance as var
# --------------------------- custom code imports ---------------------------- #
from Nodes import Node
from Network import Network
from AdaptationFunction import AdaptationFunction, CV, EC, DC

# ───────────────────────────────── globals ────────────────────────────────── #

# ─────────────────────────── utilitary functions ──────────────────────────── #
def functions_subset(functions, p):
    subset = []
    for x in functions:
        if random.random() < p:
            subset.append(x)
    return subset

# ─────────────────────────── running a simulation ─────────────────────────── #
def run(size, p, nb_protocols, max_stack):
    """
    :param size: the number of nodes of the generated network
    :param p: probability for each adaptation function to be in a router
    :param nb_protocols: number of protocols used in the simulation
    :praram max_stack: maximum size of the stack carried by each message
    """
    # --------------------------- create the graph --------------------------- #
    graph = barabasi_albert_graph(size, 5)
    # ---------------- compute the diameter and longest path ----------------- #
    diameter = 0
    paths = dict(nx.all_pairs_shortest_path(graph))
    seen = set()
    for x in paths:
        for y in paths[x]:
            if y not in seen:
                seen.add(y)
                if len(paths[x][y]) > diameter:
                    diameter = len(paths[x][y])
                    u, v = x, y

    # ---------------- create every valid adaptation function ---------------- #
    print(nb_protocols)
    protocols = "abcdefghijklmnopqrstuvwxyz"[:nb_protocols]
    functions = set()
    for i in range(len(protocols)):
        x = protocols[i]
        functions.add(AdaptationFunction(x, x, CV))
        functions.add(AdaptationFunction(x, x + x, EC))
        functions.add(AdaptationFunction(x + x, x, DC))
        for j in range(i + 1, len(protocols)):
            y = protocols[j]
            functions.add(AdaptationFunction(x, y, CV))
            functions.add(AdaptationFunction(y, x, CV))
            functions.add(AdaptationFunction(x, x + y, EC))
            functions.add(AdaptationFunction(y, y + x, EC))
            functions.add(AdaptationFunction(x + y, x, DC))
            functions.add(AdaptationFunction(y + x, y, DC))

    # ------------------- create the network and the nodes ------------------- #
    network = Network(graph, max_stack=max_stack)
    nodes = {}
    for x in graph.nodes:
        nodes[x] = Node(x, network, functions_subset(functions, p))
    network.set_nodes(nodes)

    # -------------------------- start the network --------------------------- #
    network.start()

    received = sum([node.conf_received for node in nodes.values()])
    sent = sum([node.conf_sent for node in nodes.values()])
    # ---------------------- end of network somulation ----------------------- #
    print(" | ".join(['{} : {: <5}'.format(x.id, str(x.wake_buffer.empty())) for x in nodes.values()]))
    rows = 0
    for node in nodes.values():
        for row in node.routing_table:
            rows += 1
    assert received >= rows

    assert sent == received
    return

    path_exists = False
    for dest, stack in nodes[u].routing_table.table.keys():
        if dest == v:
            print(dest, stack)
            print(nodes[u].Out)
        if dest == v and len(stack) == 1 and stack[0] in nodes[u].Out:
            print('Here !')
            path_exists = True
    print('-' * 40)
    for dest, stack in nodes[v].routing_table.table.keys():
        if dest == u:
            print(dest, stack)
        if dest == u and stack in nodes[u].Out:
            path_exists = True
    print('-' * 40)
    print([str(x) for x in nodes[u].adapt_functions])

    print(nodes[u].routing_table)
    print(nodes[v].routing_table)
    print('  {}  '.format(path_exists).center(80, '-'))

    # returns
    #  - convergence time
    #  - path exists
    #  - nb messages
    #  - diameter
    return network.convergence_time(), int(path_exists), sent, diameter

def simulation(size, p, nb_protocols, max_stack, n):
    results = []
    for i in range(n):
        results.append(run(size, p, nb_protocols, max_stack))
    convergences, paths, sent, diameters = zip(*results)
    # conv_mean conv_var paths_mean paths_var sent_mean sent_var diam_mean diam_var
    print('{:.8f} {:.8f} {:.8f} {:.8f} {:.8f} {:.8f} {:.8f} {:.8f}'.format(
        mean(convergences), var(convergences),
        mean(paths), var(paths),
        mean(sent), var(sent),
        mean(diameters), var(diameters)
    ))

def test_simulation():
    simulation(7, 1, 2, 4, 10)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('size', help='number of nodes in the network', type=int)
    parser.add_argument('p', help='probability of each adaptation function to be added to a node', type=float)
    parser.add_argument('nb_protocols', help='number of protocols used in the simulation', type=int)
    parser.add_argument('max_stack', help='maximum stack size of a given route', type=int)
    parser.add_argument('n', help="number of iterations", type=int)
    args = parser.parse_args()
    simulation(args.size, args.p, args.nb_protocols, args.max_stack, args.n)
