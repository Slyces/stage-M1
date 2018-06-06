#include "simulation.h"
#include <assert.h>
#include <stdlib.h>
#include <memory.h>

#define true 1
#define false 0

void * RandomGraph(int n) {
    return NULL;
}

void RandomNode(node * newNode, int id, adaptFunction * allAdapt, 
        size_t maxAdapt, double p) {
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
            /*adaptSubset[count] = allAdapt[i];*/
            memcpy(&adaptSubset[count], &allAdapt[i], sizeof(adaptFunction));
            count++;
        }
    }
    assert(count == subsetLength);
    NodeCreate(newNode, id, adaptSubset, subsetLength);
}

void RandomNetwork(network * net, int n, size_t nbProtocols, double p,
        size_t maxStack) {
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
        AdaptCreate(&allAdapt[count++], prots[i], prots[i], CV);
        AdaptCreate(&allAdapt[count++], prots[i], prots[i], EC);
        AdaptCreate(&allAdapt[count++], prots[i], prots[i], DC);
        for (int j = i + 1; j < nbProtocols; j++) {
            // i → j
            AdaptCreate(&allAdapt[count++], prots[i], prots[j], CV);
            AdaptCreate(&allAdapt[count++], prots[i], prots[j], EC);
            AdaptCreate(&allAdapt[count++], prots[i], prots[j], DC);
            // j → i
            AdaptCreate(&allAdapt[count++], prots[j], prots[i], CV);
            AdaptCreate(&allAdapt[count++], prots[j], prots[i], EC);
            AdaptCreate(&allAdapt[count++], prots[j], prots[i], DC);
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
         RandomNode(nodesArray[i], i, allAdapt, adaptMax, p);
    }

    /* ──────────────────────── free all structures ───────────────────────── */
    free(allAdapt);

   /* ─────────────────────────────────────────────────────────────────────── */
    NetworkCreate(net, graph, nodesArray, n);
}

int main(int argc, const char *argv[]) {
    printf("/* ─────────────────────────── simulation started ─────────────────────────── */\n");
    network * net = malloc(sizeof(network));
    RandomNetwork(net, 2, 2, 0.2, 2); // 10 nodes, everything else disabled
    NetworkStart(net, 0.2);
    printf("/* ──────────────────────────── simulation ended ──────────────────────────── */\n");
}
