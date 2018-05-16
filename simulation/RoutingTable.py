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

    def __iter__(self):
        for row_keys in self.table:
            yield self.get(*row_keys)

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

    def __str__(self):
        titles = ["dest", "stack", "next_hop", "adaptation function", "cost"]
        rows = [[str(dest), "".join(stack), str(row.next_hop), str(row.func),
                 str(row.cost)] for ((dest, stack), row) in self.table.items()]
        total = rows + [titles]
        max_len = list(map(max, [map(len, line) for line in zip(*total)]))
        max_len = [x + 2 for x in max_len]
        string = "|".join([titles[i].center(max_len[i]) for i in range(5)])
        string += '\n' + (1 * 4 + sum(max_len)) * "-" + '\n'
        for line in rows:
            string += " ".join([line[i].center(max_len[i]) for i in range(2)])
            string += "|"
            string += " ".join([line[i + 2].center(max_len[i + 2]) for i in range(3)])
            string += '\n'
        return string


# ──────────────────────────────── Rows class ──────────────────────────────── #
class Row(object):
    def __init__(self, next_hop, func, cost):
        self.next_hop = next_hop
        self.func = func
        self.cost = cost
