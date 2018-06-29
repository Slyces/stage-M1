#include <cstdio>
#include <iostream>
#include <random>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_selectors.hpp>
#include "Link.hpp"
#include "Node.hpp"
#include "AdaptationFunction.hpp"

using namespace std;
using namespace boost;

typedef adjacency_list<vecS, vecS, bidirectionalS, no_property, Link> Graph;

Graph BarabasiAlbert(int n, int m) {
    assert(m >= 1 && m <= n);
    /* variables */
    int degrees[n];
    int sum = m * (m - 1);
    Graph graph(m);
    Link default_link(1);
    /* First create a complete graph of size m */
    for (unsigned long u = 0; u < (unsigned long) m; u++)
        for (unsigned long v = u + 1; v < (unsigned long) m; v++) {
            boost::add_edge(u, v, default_link, graph);
            boost::add_edge(v, u, default_link, graph);
    }
    /* then keep track of the degrees of each vertices */
    for (int i = 0; i < m; i++)
        degrees[i] = m - 1;
    double sum_p[n];

    /* After initialization, we have m nodes marked from 0 to m - 1.
     * The next node's index is m. */
    auto current_vertex = static_cast<unsigned long>(m);
    while (current_vertex < (unsigned long) n) {
        std::vector<unsigned long> connected;
        assert(connected.empty());
        int count = 0;
        for (auto &val : sum_p)
            val = 0;
        for (int i = 0; i < current_vertex; i++)
            for (int j = i; j < current_vertex; j++)
                sum_p[current_vertex - i] += degrees[j];
        for (auto &val : sum_p)
            val /= sum;
        cout << "new node : " << current_vertex << endl;
        do {
            double r = ((double) rand() / (RAND_MAX));
            int i = 0;
            while (i < current_vertex - 1 && r >= sum_p[i] && r >= sum_p[i + 1])
                i++;
            bool added = true;
            boost::add_edge(current_vertex, i, default_link, graph).second;
            boost::add_edge(i, current_vertex, default_link, graph).second;
            for (auto &vertex : connected) {
                added &= (vertex != i);
            }
            if (added) {
                cout << "linking to " << i << endl;
                connected.push_back(i);
                count++;
            }
        } while (count < m);
        boost::add_vertex(graph);
        degrees[current_vertex] = m; // new vertices always have m connections
        for (auto &index : connected)
            degrees[index]++;
        sum += m;
        current_vertex++;
    }
    return graph;
}

int run(int n, int nbProtocols, int maxStack, double p) {
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
     */


    /* random graph creation */
    Graph graph = BarabasiAlbert(n, 5);

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


    /* preparation to select a random number in range [0, 1) */
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 1);

    Node * nodes[n];
    auto * network = new Network(graph, nodes,
            static_cast<unsigned int>(n), static_cast<unsigned int>(maxStack));

    /* create an array of random nodes */
    for (int i = 0; i < n; i++) {
        /* create a random subset of adaptation functions */
        std::vector<AdaptationFunction> selected;
        selected.reserve(static_cast<unsigned long>(p * (nbProtocols * nbProtocols * 3)));
        for (auto function : allFunctions) {
            if (dist(e2) < p)
                selected.push_back(function);
        }
        nodes[i] = new Node(network, (unsigned int) i, selected);
    }

    /* run the network */
//    network->timeout = 40;
    network->start();

    /* free the memory */
    for (auto &node : nodes)
        delete node;
    delete network;
    return 1;
}

static void usage(string &progname, int val) {
    cout << "Usage: " << progname << " n prot stack p iter" << endl;
    printf("Positional arguments are :\n");
    printf("\tn: size of the graph\n");
    printf("\tprot: number of protocols\n");
    printf("\tstack: maximum stack size\n");
    printf("\tp: probability for a node to contain each function\n");
    printf("\titer: number of iterations\n");
    exit(val);
}

inline double convertToDouble(const std::string& s,
                              bool failIfLeftoverChars = true) {
    std::istringstream i(s);
    double x;
    char c;
    if (!(i >> x) || (failIfLeftoverChars && i.get(c)))
        throw runtime_error("convertToDouble(\"" + s + "\")");
    return x;
}

int main(int argc, const char * argv[]) {
    /*
     * Expected arguments :
     *  - n: the size of the graph
     *  - nbProtocols: number of different protocols
     *  - maxStack: maximum stack size for messages
     *  - p: probability for a node to contain each adaptation function
     *  - nbIter: number of iteration
     */
//    namespace po = boost::program_options;
    string progname = "simulation-cpp";
    if (argc != 6) {
        cout << "Wrong number of arguments." << endl;
        usage(progname, 1);
    }
    int n = stoi(argv[1]);
    int nbProtocols = stoi(argv[2]);
    int maxStack = stoi(argv[3]);
    double p = convertToDouble(argv[4]);
    int nbIter = stoi(argv[5]);
    for (int i = 0; i < nbIter; i++)
        run(n /*n*/, nbProtocols /*nbprotocols*/, maxStack /*maxStacks*/, p /*p*/);
    return 0;
}

