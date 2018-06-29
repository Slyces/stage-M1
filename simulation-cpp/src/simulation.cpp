#include <cstdio>
#include <iostream>
#include <random>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <set>
#include "simulation.hpp"
#include "Link.hpp"
#include "Node.hpp"
#include "AdaptationFunction.hpp"

using namespace std;
using namespace boost;

Graph BarabasiAlbert(int n, int m) {
    /* Barabasi Albert algorithm
     * -------------------------
     * See https://en.wikipedia.org/wiki/Barab%C3%A1si%E2%80%93Albert_model
     * for more informations.
     * Generates a graph using preferential attachment: the more connexions
     * a node already has, the more likely he is to receive new edges.
     * Each node will have m connexions, following this principle.
     *
     * Algorithm
     * ---------
     * 1. Make a fully connected graph G of m nodes
     * 2. Add a new node to the graph
     * 3. Pick a node j ∈ G according to the probability
     *       p(j) = d(j) / (∑d(i), i ∈ G)
     *    and connect it to the new node
     * 4. Repeat 3. until the node has m connexion
     * 5. Repeat 2. until there is exactly n nodes in G
     */
    assert(m >= 1 && m <= n); // validity conditions of the algorithm

    Graph graph(static_cast<unsigned long>(m)); // graph related variable
    Link default_link(1);

    int degrees[n]; // degree of each nodes
    int sum = m * (m - 1); // sum of degrees
    double sum_p[n]; // sum of probabilities of getting each node

    /* -------------- preparation for random number generation -------------- */
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 1);

    /* ----------------- create a complete graph of size m ------------------ */
    for (unsigned long u = 0; u < (unsigned long) m; u++)
        for (unsigned long v = u + 1; v < (unsigned long) m; v++) {
            boost::add_edge(u, v, default_link, graph);
            boost::add_edge(v, u, default_link, graph);
    }

    /* ------------- keep track of the degrees of each vertices ------------- */
    for (int i = 0; i < m; i++)
        degrees[i] = m - 1;

    auto current_vertex = static_cast<unsigned long>(m); // next node index is m
    while (current_vertex < (unsigned long) n) {
        std::set<unsigned long> connected; // vector of already connected nodes
        assert(connected.empty());

        boost::add_vertex(graph);
        /* ------------------ compute sum of probabilities ------------------ */
        for (auto &val : sum_p) val = 0; // reset previous values
        for (int i = 0; i <  (int) current_vertex; i++)
            for (int j = i; j < (int) current_vertex; j++)
                sum_p[j] += degrees[i];
        for (auto &val : sum_p)
            val /= sum;

        do {
            /* --- pick a node at random following computed probabilities --- */
            double r = dist(e2);
            unsigned long i = 0;
            while (i < current_vertex - 1 && r >= sum_p[i])
                i++;

            /* ----------------- add the link to the graph ------------------ */
            if (connected.insert(i).second) { // those operations have no effect
                // if the link already exists
                boost::add_edge(current_vertex, i, default_link, graph).second;
                boost::add_edge(i, current_vertex, default_link, graph).second;
            }
        } while (connected.size() < (size_t) m);

        degrees[current_vertex] = m; // new vertices always have m connections
        for (auto &index : connected)
            degrees[index]++;
        sum += 2 * m;
        current_vertex++;
    }
    for (unsigned long i = 0; i < (unsigned long) n; i++) {
        assert(boost::degree(i, graph) == (unsigned int) 2 * degrees[i]);
//        cout << "( " << boost::degree(i, graph) << " - " << degrees[i] << " )" << endl;
    }
//    exit(0);

    return graph;
}

runStats run(int n, int nbProtocols, int maxStack, double p) {
    /*
     * A run is made following several steps:
     *  - create a random graph
     *  - create links structures [functions <-> cost] for each edge
     *  - create an array with every valid adaptation function
     *  - create an array of random nodes
     *    ~ each node has a subset of every adaptation function
     *  - create the network
     *  - make it run until convergence
     *  - free memory
     *
     * A run returns several statistics: 
     *  - convergence time
     *  - number of configuration messages sent in the network
     *  - diameter of the graph
     *  - is there a path between two of the most distant nodes ?
     */
    runStats stats;

    /* --------- create a graph using the Barabasi Albert algorithm --------- */
    Graph graph = BarabasiAlbert(n, 5);
    //stats.diameter = 

    /* ------------- create every protocol in use for this run -------------- */
    protocol protocols[nbProtocols];
    for (int i = 0; i < nbProtocols; i++) {
        protocols[i] = static_cast<protocol>('a' + i);
    }

    /* -------- create an array with every valid adaptation function -------- */
    int count = 0;
    adaptType types[3] = {CV, EC, DC};
    AdaptationFunction allFunctions[(nbProtocols * nbProtocols) * 3];
    for (auto &in : protocols)
        for (auto &out : protocols)
            for (auto &type : types)
                allFunctions[count++].setAll(in, out, type);

    /* ------- preparation to select a random number in range [0, 1) -------- */
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 1);

    /* ---- initialise the array of node pointers and create the network ---- */
    Node * nodes[n];
    auto * network = new Network(graph, nodes,
            static_cast<unsigned int>(n), static_cast<unsigned int>(maxStack));

    /* -------------- fill the pointer array with random nodes -------------- */
    /* 
     * A random node is created with a random subset of adaptation functions.
     * The new node has a probability p to contain each function
     */
    for (int i = 0; i < n; i++) {
        /* -------- create the random subset of adaptation functions -------- */
        std::vector<AdaptationFunction> selected;
        selected.reserve( // reserve the mean number of functions
                static_cast<unsigned long>(p * (nbProtocols * nbProtocols * 3)));
        for (auto function : allFunctions) {
            double r = dist(e2); // the randomly generated number
            if (r < p)
                selected.push_back(function);
        }
        nodes[i] = new Node(network, (unsigned int) i, selected);
    }

    /* -------------------------- run the network --------------------------- */
    /*  optionally set the timeout used by the network to assume convergence  */
     network->timeout = 20; //
    
    /* This method also stops the network automatically when no thread has woke
     * up since network.timeout ms */
    network->start();

    /* free the memory */
    for (auto &node : nodes)
        delete node;
    delete network;
    return stats;
}

static void usage(string &progname, int val) {
    cout << "Usage: " << progname << " size p nb-protocols stack iter" << endl;
    printf("Positional arguments are :\n");
    printf("\tsize: size of the graph\n");
    printf("\tp: probability for a node to contain each function\n");
    printf("\tnb-protocols: number of protocols\n");
    printf("\tstack: maximum stack size\n");
    printf("\titer: number of iterations\n");
    exit(val);
}

int main(int argc, const char * argv[]) {
    /*
     * Expected arguments :
     *  - p: probability for a node to contain each adaptation function
     *  - size: the size of the graph
     *  - nbProtocols: number of different protocols
     *  - maxStack: maximum stack size for messages
     *  - nbIter: number of iteration
     */
    string progname = "simulation-cpp";
    if (argc != 6) {
        cout << "Wrong number of arguments." << endl;
        usage(progname, 1);
    }
    /* ---- really basic attribution of parameters, assuming good usage ----- */
    int n = stoi(argv[1]);
    string a = argv[2];
    double p = stod(argv[2]);
    int nbProtocols = stoi(argv[3]);
    int maxStack = stoi(argv[4]);
    int nbIter = stoi(argv[5]);

    /* ---------- array of stats objects to compute means and vars ---------- */
    runStats allRuns[nbIter];
    for (int i = 0; i < nbIter; i++)
        allRuns[i] = run(n /*n*/, nbProtocols /*nbprotocols*/, maxStack /*maxStacks*/, p /*p*/);

    return 0;
}

