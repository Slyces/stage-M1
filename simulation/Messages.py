#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ───────────────────────────────── Messages ───────────────────────────────── #
class ConfigurationMessage(object):
    def __init__(self, dest, stack, cost):
        "Configuration message exchanged between routers to build the routing table"
        self.dest = dest
        self.stack = stack
        self.cost = cost

    def copy(self):
        return ConfigurationMessage(self.dest, self.stack[:], self.cost)

    def __str__(self):
        return "• → {:1} : [cost: {}] {}".format(self.dest,
                self.cost, self.stack)

# ────────────────────────────── Routed message ────────────────────────────── #
class Message(object):
    def __init__(self, src, dest, stack, payload, max_height=100):
        "Regular message sent between routers to convey any content"
        self.dest = dest
        self.src = src
        self.__stack_height = 0
        self.stack = stack
        self.payload = payload
        self.max_height = max_height
        self.valid = True

        # use the setter to make sure the stack is not too long
        self.stack_height = len(stack)

    @property
    def stack_height(self):
        "getter for stack height"
        return self.__stack_height

    @stack_height.setter
    def stack_height(self, new_value):
        "setter for stack height, being carefull with max_height"
        if new_value > self.max_height:
            self.valid = False
        self.__stack_height = new_value

    def copy(self):
        return Message(self.src, self.dest, self.stack[:], self.payload, self.max_height)

    def __str__(self):
        return "src: {s}, dst: {d}, stack: {st}, payload: «{p}»".format(
            s = self.src, d = self.dest, st = self.stack, p = self.payload
        )
