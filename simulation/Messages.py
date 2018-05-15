#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ───────────────────────────────── Messages ───────────────────────────────── #
class ConfigurationMessage(object):
    def __init__(self, dest, stack, cost):
        "Configuration message exchanged between routers to build the routing table"
        self.dest = dest
        self.stack = stack
        self.cost = cost

    def __str__(self):
        return "→ {:1} : [cost: {}] {}".format(self.dest,
                self.cost, self.stack)

# ────────────────────────────── Routed message ────────────────────────────── #
class Message(object):
    def __init__(self, src, dst, stack, payload, max_height=100):
        "Regular message sent between routers to convey any content"
        self.dst = dst
        self.src = src
        self.__stack_height = 0
        self.stack = stack
        self.payload = payload
        self.max_height = max_height

        # use the setter to make sure the stack is not too long
        self.stack_height = len(stack)

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
        return "src: {s}, dst: {d}, stack: {st}, payload: «{p}»".format(
            s = self.src, d = self.dst, st = self.stack, p = self.payload
        )
