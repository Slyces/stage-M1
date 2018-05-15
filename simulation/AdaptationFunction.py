#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ─────────────────────────── Adaptation functions ─────────────────────────── #
EC, DC, CV = 1, 2, 3
class AdaptationFunction(object):
    def __init__(self, _from, _to, _type):
        """
        :param _type: The type of adaptation. Must be either :
                      - EC : encapsulation
                      - DC : decapsulation
                      - CV : conversion
        :param _from: The input protocol of the function
                      - string or tuple of protocols, last element is the top of the stack
        :param _to:   The output protocol of the function
                      - string or tuple of protocols, last element is the top of the stack
        """
        assert _type in (EC, DC, CV)

        # convert strings to tuples
        self._from = tuple(_from) if _from is str else _from
        self._to = tuple(_to) if _to is str else _to
        self._type = _type

        # protocol length in conversion
        if self._type == CV:
            assert len(self._from) == len(self._to) == 1, "can only convert one protocol"

        # protocol length in encapsulation
        if self._type == EC:
            assert len(self._from) == 1 and len(self._to) == 2, \
                    "encapsulations takes 1 protocol in and 2 protocols out"
            assert self._from[0] == self._to[0], \
                    "encapsulation can not change the initial protocol"

        # protocol length in decapsulation
        if self._type == DC:
            assert len(self._from) == 2 and len(self._to) == 1, \
                    "decapsulation takes 2 protocol in and 1 protocol out"
            assert self._from[0] == self._to[0], \
                    "decapsulation can not change the inner protocol"

    def apply(self, message):
        "Applies an adaptation function to a message"
        assert message.stack_height > 0, "not supposed to receive a message with empty stack"

        # Checking if the function may apply to this stack
        top_protocols = tuple(message.stack[len(self._from)])  # see the first or 2 firsts protocols
        assert top_procol == self._from, \
                "the function {} can't apply to top protocol {}".format(self, top_protocol)

        # apply conversion
        if self._type == CV:
            message.stack.pop()
            message.stack.push(*self._to)  # _to is only one protocol, safe to unpack with *

        # apply encapsulation
        if self._type == EC:
            message.stack.push(self._to[-1])  # push the new top protocol
            message.stack_height += 1

        # apply decapsulation
        if self._type == DC:
            message.stack.pop()  # remove the old top protocol
            message.stack_height -= 1

    @property
    def reverse(self):
        "returns the reverse operation corresponding to this adaptation function"
        # conversion
        if self._type == CV:
            return AdaptationFunction(self._to, self._from, self._type)

        # encapsulation / decapsulation : just invert the type and from / to
        return AdaptationFunction(self._to, self._from, EC if self._type == DC else DC)

    def __str__(self):
        return "({} ⟶  {}, {})".format(self._from, self._to,
                    {EC: "encap", DC: "decap", CV: "conv"}[self._type])

    def __hash__(self):
        return hash(str(self))

    def __eq__(self, peer):
        return hash(self) == hash(peer)

    def __ne__(self, peer):
        return not self == peer
