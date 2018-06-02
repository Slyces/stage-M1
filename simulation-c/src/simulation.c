#include "simulation.h"
#include <assert.h>

#define true 1
#define false 0

void * RandomGraph(int n) {
    return NULL;
}

node * RandomNode(int id, adaptFunction * allAdapt, size_t maxAdapt, double p) {
    int selected[maxAdapt];
    int count;
    size_t subsetLength = 0;
    // select a subset of adaptation functions from all adaptation functions
    for (int i = 0; i < maxAdapt; i++) {
        if ((rand() / RAND_MAX) < p) {
            selected[i] = true;
            subsetLength++; // count the number of functions used
        } else {
            selected[i] = false;
        }
    }
    adaptFunction * adaptSubset = malloc(subsetLength * sizeof(adaptFunction));
    count = 0;
    for (int i = 0; i < maxAdapt; i++) {
        if (selected[i]) {
            adaptSubset[count] = allAdapt[i];
            count++;
        }
    }
    assert(count == subsetLength);
    return NodeCreate(id, adaptSubset, subsetLength);
}

network * RandomNetwork(int n, size_t nbProtocols, double p, size_t maxStack) {
    // create a random graph
    void * graph = NULL;

    // create links for each edge in the graph

    /* ──────────── create an array of every adaptation function ──────────── */
    // First, create all protocols needed
    const char alphabet[26] = "abcdefghijklmnopqrstuvwxyz";
    protocol prots[26];
    for (int i = 0; i < 26; i++) {
        prots[i] = (protocol) alphabet[i];
    }

    size_t adaptMax = (nbProtocols * 3 + 3 * nbProtocols * (nbProtocols - 1));
    int count = 0;
    adaptFunction * allAdapt = malloc(adaptMax * sizeof(adaptFunction));

    for (int i = 0; i < nbProtocols; i++) {
        allAdapt[count++] = * AdaptCreate(prots[i], prots[i], CV);
        allAdapt[count++] = * AdaptCreate(prots[i], prots[i], EC);
        allAdapt[count++] = * AdaptCreate(prots[i], prots[i], DC);
        for (int j = i + 1; j < nbProtocols; j++) {
            // i → j
            allAdapt[count++] = * AdaptCreate(prots[i], prots[j], CV);
            allAdapt[count++] = * AdaptCreate(prots[i], prots[j], EC);
            allAdapt[count++] = * AdaptCreate(prots[i], prots[j], DC);
            // j → i
            allAdapt[count++] = * AdaptCreate(prots[j], prots[i], CV);
            allAdapt[count++] = * AdaptCreate(prots[j], prots[i], EC);
            allAdapt[count++] = * AdaptCreate(prots[j], prots[i], DC);
        }
    }
    assert(adaptMax == count);

    //    printf("/* ------------------------ adaptation functions ------------------------ */\n");
    /*for (int i = 0; i < adaptMax; i++) {*/
        /*char buff[300];*/
        /*AdaptPrint(buff, &allAdapt[i]);*/
        /*printf("%s ", buff);*/
    /*}*/
    /*printf("\n");*/
    
    
    /* ───── create an array of nodes, with random adaptation functions ───── */
    node ** nodesArray = malloc(n * sizeof(node *));
    for (int i = 0; i < n; i++) {
        nodesArray[i] = RandomNode(i, allAdapt, adaptMax, p);
    }

    /* ──────────────────────── free all structures ───────────────────────── */
    free(allAdapt);

   /* ─────────────────────────────────────────────────────────────────────── */
    return NetworkCreate(graph, nodesArray, n);
}

int main(int argc, const char *argv[]) {
    printf("/* ─────────────────────────── simulation started ─────────────────────────── */\n");
    network * net = RandomNetwork(10, 2, 0.2, 2); // 10 nodes, everything else disabled
    NetworkStart(net, 3.0);
    printf("/* ──────────────────────────── simulation ended ──────────────────────────── */\n");
}
