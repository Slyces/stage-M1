#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ────────────────────────────── Routing tables ────────────────────────────── #
class RoutingTable(object):
    def __init__(self):
        self.table = {}

    def __contains__(self, keys):
        dest, stack = keys
        return (dest, tuple(stack)) in self.table

    def get(self, dest, stack):
        stack = tuple(stack)
        if (dest, stack) in self:
            return self.table[dest, stack]
        else:
            raise KeyError

    def add_route(self, dest, stack, next_hop, func, cost):
        """
        :return added: boolean,
                        - True: the route is new or better than its predecessor
                        - False: this route costs more than the existing one, not added
        """
        stack = tuple(stack)
        if (dest, stack) not in self or self.table[dest, stack].cost > cost:
            self.table[dest, stack] = Row(next_hop, func, cost)
            return True
        else:
            return False

# ──────────────────────────────── Rows class ──────────────────────────────── #
class Row(object):
    def __init__(self, next_hop, func, cost):
        self.next_hop = next_hop
        self.func = func
        self.cost = cost
