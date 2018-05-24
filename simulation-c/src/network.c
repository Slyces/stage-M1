#include "network.h"
#include <unistd.h>

struct arg_container {
    network * net;
    int n_adapt;
    adapt_f * adapt_functions;
};
/* ───────────────────────────── nodes behavior ───────────────────────────── */

int get_id(network * net) {
    pthread_t thread_id = pthread_self();
    for (int i = 0; i < net->n; i++) {
        if (net->threads[i] == thread_id)
            return i;
    }
    return -1;
}

/* ----------------------------- start the node ----------------------------- */
void * start_node(void * ptr) {
    /* ------------------------ retrieving arguments ------------------------ */
    struct arg_container * cont = (struct arg_container *) ptr;
    network * net = cont->net;




    /* ---------------------- little printf for debug ----------------------- */
    int my_id = get_id(net);
    printf("Hello world, I'm the thread %d\n", my_id);

    /* ---------------- creating the stack for communication ---------------- */
    stackT * stack = malloc(sizeof(stack));
    StackInit(stack, MAX_SIZE);

    /* ---------------------- creating the node object ---------------------- */
    node * me = malloc(sizeof(node));

    me->n_adapt = cont->n_adapt;
    me->adapt_functions = cont->adapt_functions;
    me->messages_stack = stack;

    net->nodes[my_id] = me;

    /* ------------------------ waiting for messages ------------------------ */
    messages_wait(net);
    pthread_exit(0);
}

/* --------------------------- wait for messages ---------------------------- */
void messages_wait(network * net) {
    int my_id = get_id(net);
    while (net->running) {
        sleep(1);
        printf("Woop ! (%d) here !\n", my_id);    
    }
}

/* ─────────────────── creation and start of the network ──────────────────── */
void create_network(void * graph, int stack_size, float max_time) {
    int n = 10;
    pthread_t * pthreads_array = malloc(sizeof(pthread_t) * n);
    
    /* ------------------- creation of the network struct ------------------- */
    network * net = malloc(sizeof(network));
    net->n = n;
    net->graph = graph;
    net->threads = pthreads_array;
    net->running = 1;
    net->nodes = malloc(sizeof(node *) * n);

    /* ---------------------- creation of the threads ----------------------- */
    for (int i = 0; i < n; i++) {
        struct arg_container cont = {net, 0, (adapt_f *) NULL};
        struct arg_container * p_cont = &cont;
        pthread_create(&pthreads_array[i], NULL, start_node, (void *) p_cont);
        printf("Created thread %d with id %lu\n", i, pthreads_array[i]);
    }

    sleep(max_time);

    net->running = 0;

    /* -------------------- end condition of the network -------------------- */
    for (int i = 0; i < net->n; i++) {
        pthread_join(net->threads[i], NULL);
    } 
    printf("# ---------- clearing memory --------- #\n");
    for (int i = 0; i < net->n; i++) {
        StackDestroy(net->nodes[i]->messages_stack);
        free(net->nodes[i]->messages_stack);
        free(net->nodes[i]);
    }
    free(pthreads_array);
    free(net->nodes);
    free(net);
    printf("# ==================================== #\n");
}

/* ────────────────────────────────── main ────────────────────────────────── */
int main(int argc, const char * argv[]) {
    printf("# ==================================== #\n");
    create_network(NULL, 10, 4);    
}
