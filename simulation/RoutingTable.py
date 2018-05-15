#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ────────────────────────────── Routing tables ────────────────────────────── #
class RoutingTable(object):
    def __init__(self):
        self.table = {}

    def add_row(self, dest, stack, next_hop, cost):
        self.table[dest, stack] = (next_hop, cost)

    @property
    def rows(self):
        return [list(key) + list(value) for key, value in self.table.items()]
