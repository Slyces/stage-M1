// test headers and define
#define CATCH_CONFIG_MAIN

#include "catch.hpp"

// includes
#include "ProtocolStack.hpp"
#include "AdaptationFunction.hpp"
#include "Node.hpp"
#include "Message.hpp"
#include "ConfMessage.hpp"
#include "PhysicalMessage.hpp"

// testing protocol stacks
SCENARIO("Protocols can be added and removed from a protocol stack", "[ProtocolStack]") {
    size_t n = 10;
    ProtocolStack stack(n);
    WHEN("The stack is created") {
        REQUIRE(stack.size == n);

        THEN ("it is empty, and has an empty repr") {
            REQUIRE(stack.empty());
            REQUIRE(stack.toString() == "<>");
        }
    }

    WHEN("Elements are pushed") {
        stack.push('a');
        stack.push('b');

        THEN("Top pointer moves accordingly") {
            REQUIRE(stack.topIndex == 1);
        }

        THEN("The stack is not empty anymore") {
            REQUIRE(!stack.empty());
        }

        THEN("Repr is not empty anymore") {
            REQUIRE(stack.toString() == "<a-b>");
        }
    }

    WHEN("Elements are popped") {
        stack.push('a');
        stack.push('b');
        protocol p = stack.pop();
        THEN("We get the last element") {
            REQUIRE(p == 'b');
        }
    }

    WHEN("Stack is emptied") {
        stack.push('a');
        stack.pop();
        THEN("It is empty") {
            REQUIRE(stack.empty());
        }
    }

    WHEN("Stack is filled") {
        for (int i = 0; i < n; ++i) {
            stack.push('a');
        }
        THEN("The stack is full") {
            REQUIRE(stack.full());
        }
    }

    WHEN("Stack is copied") {
        for (int i = 0; i < n; ++i) {
            stack.push('a');
        }

        ProtocolStack * copy = stack.clone();
        THEN("Copy modification does not alter the original") {
            copy->pop();
            REQUIRE(!copy->full());
            REQUIRE(stack.full());
        }
        delete copy;
    }
}

SCENARIO("Protocol stacks can be created from string", "[ProtocolStack]") {
    size_t n = 10;
    ProtocolStack stack(10);
    WHEN("The creation string is empty") {
        ProtocolStack fromString(n, "");
        bool b = stack == fromString;
        THEN("The stack is also empty") {
            REQUIRE(fromString.empty());
        }
        THEN("The stack is equal to a size-created stack") {
            bool equal = (stack == fromString);
            REQUIRE(equal);
        }
    }

    WHEN("The creation string is too large") {
        string str;
        for (int i = 0; i < 2 * n; i++) {
            str += (char) ('a' + i);
        }
        ProtocolStack fromString(n, str);
        THEN("The protocol stack is full, stopping at size elements") {
            REQUIRE(fromString.full());
            for (int i = 0; i < n; i++) {
                REQUIRE(fromString.protocols[i] == str.at(i));
            }
        }
    }
    WHEN("A standard stack and from string stack have the same elements") {
        stack.push('a');
        stack.push('b');
        stack.push('c');
        stack.push('d');
        ProtocolStack fromString(n, "abcd");
        THEN("They are equal") {
            REQUIRE((stack == fromString));
        }
    }
}

SCENARIO("Adaptation functions can not be applied to every stack", "[AdaptationFunction]") {
    // problèmes de valgrind
    ProtocolStack x (2, "x");
    ProtocolStack y (2, "y");
    ProtocolStack xy(2, "xy");
    ProtocolStack yx(2, "yx");
    AdaptationFunction conv_x('x', 'x', CV);
    AdaptationFunction conv_y('y', 'y', CV);
    AdaptationFunction decap_x('x', 'x', DC);
    AdaptationFunction decap_y('y', 'x', DC);
    AdaptationFunction encap_x('x', 'x', EC);
    AdaptationFunction encap_y('x', 'y', EC);
    WHEN("The topIndex stack does not correspond to the <in> of the function") {
        THEN("Functions with x as entry (<in>) do not accept stacks with y as topIndex") {
            REQUIRE(!conv_x.valid(y));
        }
        THEN("Functions with y as entry (<in>) do not accept stacks with x as topIndex") {
            REQUIRE(!decap_y.valid(yx));
        }
    }
    WHEN("The stack is full") {
        THEN("Encapsulations do not work, no matter the content") {
            REQUIRE(!encap_x.valid(xy));
            REQUIRE(!encap_y.valid(xy));
        }
    }
    WHEN("The stack is empty") {
        ProtocolStack empty(2);
        THEN("No function work") {
            REQUIRE(!conv_x.valid(empty));
            REQUIRE(!conv_y.valid(empty));
            REQUIRE(!encap_x.valid(empty));
            REQUIRE(!encap_y.valid(empty));
            REQUIRE(!decap_x.valid(empty));
            REQUIRE(!decap_y.valid(empty));
        }
    }
    WHEN("The stack is one element") {
        THEN("Decapsulations do not work") {
            REQUIRE(!decap_x.valid(x));
            REQUIRE(!decap_y.valid(y));
        }
    }
    WHEN("The stack is valid for the function") {
        THEN("The application is valid") {
            REQUIRE(conv_x.valid(x));
            REQUIRE(conv_y.valid(y));
            REQUIRE(conv_y.valid(xy));
            REQUIRE(encap_x.valid(x));
            REQUIRE(decap_y.valid(xy));
        }
    }
}

SCENARIO("Adaptation functions are hashable and comparable", "[AdaptationFunction]") {
    ProtocolStack x (2, "x");
    ProtocolStack y (2, "y");
    ProtocolStack xy(2, "xy");
    ProtocolStack yx(2, "yx");
    AdaptationFunction conv_x('x', 'x', CV);
    AdaptationFunction conv_y('x', 'x', CV);
    AdaptationFunction decap_x('x', 'x', DC);
    AdaptationFunction decap_y('y', 'x', DC);
    AdaptationFunction encap_x('x', 'x', EC);
    AdaptationFunction encap_y('x', 'y', EC);
    WHEN("Two adaptation functions have different address but same parameters") {
        AdaptationFunction x_x('x', 'x', CV);
        THEN("They are equal") {
            bool equal = conv_x == x_x;
            bool diffAdress = &conv_x != &x_x;
            REQUIRE((diffAdress && equal));
        }
        THEN("They have the same hash") {
            bool hashEqual = conv_x.hash() == x_x.hash();
            bool diffAdress = &conv_x != &x_x;
            REQUIRE((hashEqual && diffAdress));
        }
    }
}

SCENARIO("Adaptation functions can be applied to stacks", "[AdaptationFunction]") {
    AdaptationFunction functions[3][4];
    adaptType types[] = {CV, EC, DC};
    for (auto type : types) {
        for (int x = 0; x < 2; x++) {
            for (int y = 0; y < 2; y++) {
                functions[type][x * 2 + y].setAll("xy"[x], "xy"[y], type);
            }
        }
    }
    int n = 3;
    ProtocolStack * stacks[n * n * n];
    for (int i = 0; i < n; i++) {
        auto x = static_cast<char>('x' + i);
        for (int j = 0; j < n; j++) {
            auto y = static_cast<char>('x' + j);
            for (int k = 0; k < n; k++) {
                auto z = static_cast<char>('x' + k);
                stacks[i * 9 + j * 3 + k] = new ProtocolStack(3, string({x, y, z, '\0'}));
            }
        }
    }
    WHEN("The stack is valid and we apply the function") {
        for (auto &stackPtr : stacks) {
            auto stackCopy = stackPtr->clone();
            for (auto &type : types) {
                for (auto &func : functions[type]) {
                    if (func.valid(* stackCopy)) {
                        THEN("The topIndex protocol corresponds to the <in> of the function") {
                            REQUIRE(stackCopy->top() == func.in);
                        }
                        THEN("After application, the topIndex protocol corresponds to the <out> of the function") {
                            func.apply(* stackCopy);
                            REQUIRE(stackCopy->top() == func.out);
                        }
                    }
                }
            }
            delete stackCopy;
        }
    }
    // delete
    for (auto &stackPtr : stacks) {
        delete stackPtr;
    }
}

SCENARIO("Adaptation functions can be reversed", "[AdaptationFunction]") {
    AdaptationFunction * functions[3][4];
    adaptType types[] = {CV, EC, DC};
    for (auto type : types) {
        for (int x = 0; x < 2; x++) {
            for (int y = 0; y < 2; y++) {
                functions[type][x * 2 + y] = new AdaptationFunction("xy"[x], "xy"[y], type);
            }
        }
    }
    int n = 3;
    ProtocolStack * stacks[n * n * n];
    for (int i = 0; i < n; i++) {
        auto x = static_cast<char>('x' + i);
        for (int j = 0; j < n; j++) {
            auto y = static_cast<char>('x' + j);
            for (int k = 0; k < n; k++) {
                auto z = static_cast<char>('x' + k);
                stacks[i * 9 + j * 3 + k] = new ProtocolStack(3, string({x, y, z, '\0'}));
            }
        }
    }
    WHEN("f is a conversion function") {
        for (auto &funcPtr : functions[CV]) {
            AdaptationFunction f = *funcPtr;
            AdaptationFunction reverse = f.makeReverse();
            for (auto &stackPtr : stacks) {
                ProtocolStack * copy = stackPtr->clone();
                THEN("f valid => (f o f⁻¹) (x) = x") {
                    if (f.valid(* copy)) {
                        f.apply(* copy);
                        reverse.apply(* copy);
                        bool unchanged = * copy == * stackPtr;
                        REQUIRE(unchanged);
                    }
                }
                THEN("f⁻¹ valid => (f⁻¹ o f) (x) = x") {
                    if (reverse.valid(* copy)) {
                        reverse.apply(* copy);
                        f.apply(* copy);
                        bool unchanged = (* copy == * stackPtr);
                        REQUIRE(unchanged);
                    }
                }
                delete copy;
            }
        }
    }
    for (auto &type : types) {
        for (auto &funcPtr : functions[type]) {
            delete funcPtr;
        }
    }
    for (auto &stackPtr : stacks) {
        delete stackPtr;
    }
}

SCENARIO("Node are created and destroyed without memory leak", "[Node]") {
    int nbProtocols = 10; // a lot of protocols

    /* create every protocol in use for this run */
    protocol protocols[nbProtocols];
    for (int i = 0; i < nbProtocols; i++) {
        protocols[i] = static_cast<protocol>('a' + i);
    }

    /* create an array with every valid adaptation function */
    int count = 0;
    adaptType types[3] = {CV, EC, DC};
    AdaptationFunction allFunctions[(nbProtocols * nbProtocols) * 3];
    for (auto &in : protocols)
        for (auto &out : protocols)
            for (auto &type : types)
                allFunctions[count++].setAll(in, out, type);

    /* create an array of random nodes */
    /* create a random subset of adaptation functions */
    std::vector<AdaptationFunction> selected;
    selected.reserve((unsigned long) nbProtocols * nbProtocols * 3);
    for (auto function : allFunctions) {
        selected.push_back(function);
    }
    WHEN("A node is created") {
        auto * node = new Node(nullptr, (unsigned int) 0, selected);
        THEN("The deletion does not leak memory") {
            delete node;
        }
    }
}

SCENARIO("Messages are correctly printed", "[Message]") {
    Message * msg = new Message(1, 3, new ProtocolStack(2, "xy"), (void *) "secret content");
    ConfMessage * cMsg = new ConfMessage(7, new ProtocolStack(3, "abc"), 5);
    PhysicalMessage * pMsg = PhysicalMessage::encode(1, 2, msg);
    PhysicalMessage * pcMsg = PhysicalMessage::encode(1, 7, cMsg);
    WHEN("A message / confmessage / physicalmessage is created") {
        THEN("It can be printed with toString") {
            REQUIRE(msg->toString() == "(1) → (3) : <x-y>");
            REQUIRE(cMsg->toString() == "∙ → 7 : [5] <a-b-c>");
            REQUIRE(pMsg->toString() == "[phys: 1 → 2 :MSG]");
            REQUIRE(pcMsg->toString() == "[phys: 1 → 7 :CONF]");
        }
    }
    delete msg;
    delete cMsg;
    delete pMsg;
    delete pcMsg;
}

SCENARIO("Single node network", "[Network]") {
    Node * node;
    Node ** pNode = &node;
    auto * network = new Network(nullptr, pNode, 1);
    std::vector<AdaptationFunction> selected;
    node = new Node(network, 0, selected);

    WHEN("Network starts") {
        THEN("It stops instantly with no memory issue") {
            network->start();
        }
    }
    delete node;
    delete network;
}

SCENARIO("Multi node network", "[Network]") {
    unsigned int n = 200;
    AdaptationFunction functions[] = {AdaptationFunction('a', 'b', CV)};
    Node * nodes[n];
    auto * network = new Network(nullptr, nodes, static_cast<unsigned int>(n));

    /* create an array of random nodes */
    for (int i = 0; i < n; i++) {
        /* create a random subset of adaptation functions */
        std::vector<AdaptationFunction> selected;
        for (auto &function : functions)
            selected.push_back(function);
        nodes[i] = new Node(network, (unsigned int) i, selected);
        nodes[i]->timeout = 100;
    }

    WHEN("Nodes have no common in/out between neighbors") {
        network->timeout = 5;
        network->start();
        THEN("The sum of messages is [sum of degrees] * [size in]") {
            // every node has degree (n - 1) [fully connected graph]
            int received = 0, sent = 0;
            for (auto &node : nodes) {
                sent += node->confSent;
                received += node->confReceived;
            }
            REQUIRE((sent == received));
            REQUIRE((sent ==  n * (n - 1)));
        }
    }

    /* free the memory */
    for (auto &node : nodes)
        delete node;
    delete network;
}