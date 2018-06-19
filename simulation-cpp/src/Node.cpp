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

Node::Node(Network * net, unsigned int id, vector<AdaptationFunction> const& adaptFunctions) {
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
        ProtocolStack * inStack = func.In();

        alreadyHere = false;
        for (auto &stack : In)
            alreadyHere |= (* stack == * inStack);
        if (!alreadyHere) In.push_back(inStack);
        else delete inStack;

        /* Buid the Out list of stacks */
        ProtocolStack * outStack = func.Out();

        alreadyHere = false;
        for (auto &stack : Out)
            alreadyHere |= (* stack == * outStack);
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

void Node::StartNode(void *ptr) {
    auto node = (Node *) ptr;
    node->initialize();
    node->start();
}

void Node::initialize(){
    printf("Thread %d started.\n", id);
    /* Send initialisation messages to each neighbor */
    for (int i = 0; i < network->n; i++) {
        auto neighbor = network->nodes[i];
        if (neighbor != this) {
            /* For each stack accepted as input */
            for (auto &inStack : In) {
                auto * initMessage = new ConfMessage(id, inStack->clone(), 0);
                send(neighbor->id, initMessage);
            }
        }
    }
}

void Node::start() {
    /* ------------------------------ */
    stop = false;
    while (!stop) {
        // -----
        PhysicalMessage * receivedMessage = nullptr;
        auto receiveTimeout = chrono::milliseconds(timeout);
        network->queues[id].wait_dequeue_timed(receivedMessage, receiveTimeout);
        if (receivedMessage != nullptr)
            receive(receivedMessage);
    }
}

void Node::send(int to, ConfMessage * msg) {
    confSent++;
//    printf("[%d] sent <%s> to %d\n", id, msg->toString().c_str(), to);
    send(PhysicalMessage::encode(id, to, msg));
}

void Node::send(int to, Message * msg) {
    send(PhysicalMessage::encode(id, to, msg));
}

void Node::send(PhysicalMessage * msg) {
    // ---
    assert(msg->sender == id);
    network->queues[msg->receiver].enqueue(msg);
}

void Node::receive(PhysicalMessage * physMessage) {
    network->running = true;
    assert(physMessage->receiver == id);
    if (physMessage->type == CONF) {
        receive((ConfMessage *) physMessage->content);
    } else if (physMessage->type == MSG) {
        auto * message = (Message *) physMessage->content;
        if (message->dest == id)
            receive(message);
        else
            route(message);
    } else if (physMessage->type == STOP) {
        printf("Node %d receiving STOP request from the network.\n", id);
        stop = true;
    }
    delete physMessage;
}

void Node::receive(ConfMessage * msg) {
    /* Do some things */
    confReceived++;
//    printf("[%d] received <%s>\n", id, msg->toString().c_str());
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
