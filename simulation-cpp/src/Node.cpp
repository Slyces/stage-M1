#include <vector>
#include <iostream>
#include <assert.h>
#include "Node.hpp"
#include "AdaptationFunction.hpp"
#include "ConfMessage.hpp"

using namespace std;

Node::Node() {
    /* stats */
    confReceived = 0;
    confSent = 0;
    messageReceived = 0;
    messageRouted = 0;
    messageDiscarded = 0;
}

Node::Node(Network * net, unsigned int id, vector<AdaptationFunction> const &adaptFunctions) {
    this->id = id;
    this->adaptFunctions = adaptFunctions;
    this->network = net;
    bool alreadyHere;

    /* stats */
    confReceived = 0;
    confSent = 0;
    messageReceived = 0;
    messageRouted = 0;
    messageDiscarded = 0;

    for (auto &func : adaptFunctions) {
        /* Buid the In list of stacks */
        if (func.type == DC && network->maxStack == 1)
            break;
        ProtocolStack * inStack = func.In(network->maxStack);

        alreadyHere = false;
        for (auto &stack : In)
            alreadyHere |= (*stack == *inStack);
        if (!alreadyHere) In.push_back(inStack);
        else delete inStack;

        /* Buid the Out list of stacks */
        ProtocolStack * outStack = func.Out(network->maxStack);

        alreadyHere = false;
        for (auto &stack : Out)
            alreadyHere |= (*stack == *outStack);
        if (!alreadyHere) Out.push_back(outStack);
        else delete outStack;
    }
}

Node::~Node() {
    for (auto &stack : In)
        delete stack;
    for (auto &stack : Out)
        delete stack;
}

string Node::toString() {
    string str;
    str += "Node (";
    str += std::to_string(id);
    str += ")\n----------------------------------------\n";
    str += "Functions :\n";
    for (auto &func : adaptFunctions) {
        str += "  ";
        str += func.toString();
        str += "\n";
    }
    str += "----------------------------------------\n";
    str += "In stacks :\n";
    for (auto * stack : In) {
        str += "  ";
        str += stack->toString();
        str += "\n";
    }
    str += "----------------------------------------\n";
    str += "Out stacks :\n";
    for (auto * stack : Out) {
        str += "  ";
        str += stack->toString();
        str += "\n";
    }
    return str;
}

void Node::StartNode(void * ptr) {
    auto node = (Node *) ptr;
    node->initialize();
    node->start();
}

void Node::initialize() {
    /* Send initialisation messages to each neighbor */
    for (auto &f : adaptFunctions)
        if (! (f.type == DC && network->maxStack == 1)) {
            auto * in = f.In(network->maxStack);
            table.addRoute(id, id, 0, f, in);
            delete in;
        }
    for (auto &inStack : In) {
        for (auto neighbors = boost::adjacent_vertices(id, network->graph);
                neighbors.first != neighbors.second; ++neighbors.first)  {
            send((int) * neighbors.first, new ConfMessage(id, inStack->clone(), 0));
        }
        /* For each stack accepted as input */
    }
}

void Node::start() { /* ------------------------------ */ stop = false;
    while (!stop) { // -----
        PhysicalMessage * receivedMessage = nullptr;
        auto receiveTimeout = chrono::milliseconds(timeout);
        network->queues[id].wait_dequeue_timed(receivedMessage, receiveTimeout);
        if (receivedMessage != nullptr)
            receive(receivedMessage);
    }
}

void Node::send(int to, ConfMessage * msg) {
    confSent++;
    send(PhysicalMessage::encode(id, to, msg));
}

void Node::send(int to, Message * msg) {
    send(PhysicalMessage::encode(id, to, msg));
}

void Node::send(PhysicalMessage * msg) {
    // ---
    assert(msg->sender == (int) id);
    network->queues[msg->receiver].enqueue(msg);
}

void Node::receive(PhysicalMessage * physMessage) {
    network->running = true;
    assert(physMessage->receiver == (int) id);
    if (physMessage->type == CONF) {
        receive(physMessage->sender, (ConfMessage *) physMessage->content);
    } else if (physMessage->type == MSG) {
        auto * message = (Message *) physMessage->content;
        if (message->dest == (int) id)
            receive(message);
        else
            route(message);
    } else if (physMessage->type == STOP) {
        stop = true;
    }
    delete physMessage;
}

void Node::receive(int from, ConfMessage * msg) {
    confReceived++;
    for (auto &f : adaptFunctions) {
        auto rev = f.makeReverse();
        assert(msg->stack->size == network->maxStack);
        if (rev.valid(*msg->stack)) {
            // This node is capable of creating this stack, given the right entry
            auto * required = msg->stack->clone();
            rev.apply(*required);
            /* the new route receives the <required> stack, applies f, then sends it
             * to the emitting node (<from>). */
            auto edge_link = boost::edge(id, (unsigned long) from, network->graph).first;
            int fee = network->graph[edge_link].getCost(f);

            bool added = table.addRoute(msg->dest, from, msg->cost + fee, f, required);
            if (added) {
                for (auto neighbors = boost::adjacent_vertices(id, network->graph); neighbors.first != neighbors.second; ++neighbors.first)  {
                    send((int) * neighbors.first, new ConfMessage(msg->dest, required->clone(), msg->cost + fee));
                }
            }
            delete required;
        }
        // else, it cannot create such a stack, no matter the entry
    }
    delete msg;
}

void Node::receive(Message * msg) {
    /* Do some things */
    messageReceived++;
    delete msg;

}

void Node::route(Message * msg) {
    messageRouted++;
}

