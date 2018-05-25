#include "network.h"
#include <unistd.h>

#define N 5

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
    network * net = (network *) ptr;

    /* ---------------------- little printf for debug ----------------------- */
    int my_id = get_id(net);
    printf("Hello world, I'm the thread %d\n", my_id);

    /* ------------------------ waiting for messages ------------------------ */
    messages_wait(net);
    pthread_exit(0);
}

/* --------------------------- wait for messages ---------------------------- */
void messages_wait(network * net) {
    int my_id = get_id(net);
    node * me = net->nodes[my_id];
    while (net->running) {
        sleep(0.25);
        while (!StackIsEmpty(me->messages_stack)) {
            receive(net, (physical_message *) StackPop(me->messages_stack));
        }
        /*for (int i )*/
        printf("Woop ! (%d) here !\n", my_id);    
    }
}

/* ──────────────────────────── receive messages ──────────────────────────── */
void receive(network * net, physical_message * phys) {
    if (phys->prot == CONF) {
        receive_conf(net, phys->sender, (conf_message *) phys->content);
    } else if (phys->prot == MSG) {
        message * msg = (message *) phys->content;
        if (msg->dest == get_id(net)) {
            receive_msg(net, msg);
        } else {
            route_msg(net, msg);
        }
    }
    free(phys);
}

void receive_msg(network * net, message * msg) {
    // -----------------------------------------
    free(msg);
}

void receive_conf(network * net, int sender, conf_message * msg) {
    // -----------------------------------------------------------
    free(msg);
}

/* ───────────────────────────── route messages ───────────────────────────── */
void route_msg(network * net, message * msg) {
    // ---------------------------------------
}

/* ───────────────────────────── send messages ────────────────────────────── */
/*void send()*/

/* ─────────────────────────── physical messages ──────────────────────────── */
/* ----------------------- create a physical message ------------------------ */
physical_message * create_physical(int sender, int receiver, char prot, void * content) {
    physical_message * p_msg = malloc(sizeof(physical_message));
    p_msg->sender = sender;
    p_msg->receiver = receiver;
    p_msg->prot = prot;
    p_msg->content = content;
    return p_msg;
}

/* ─────────────────────────── creation of nodes ──────────────────────────── */
node * create_node(int id, adapt_f * functions, int n_functions) {
    node * new_node = malloc(sizeof(node));
    new_node->id = id;

    /* ---------------------- adaptation functions ---------------------- */
    new_node->n_adapt = n_functions;
    new_node->adapt_functions = functions;

    /* ------------------------------- stack -------------------------------- */
    stackT * new_stack = malloc(sizeof(new_stack));
    StackInit(new_stack, MAX_SIZE);
    new_node->messages_stack = new_stack;
    return new_node;
}

/* ─────────────────── creation and start of the network ──────────────────── */
network * create_network(void * graph, node ** nodes, int stack_size) {
    int n = N;
    pthread_t * pthreads_array = malloc(sizeof(pthread_t) * n);
    
    /* ------------------- creation of the network struct ------------------- */
    network * net = malloc(sizeof(network));
    net->n = n;
    net->graph = graph;
    net->threads = pthreads_array;
    net->nodes = nodes;

    return net;
}

void start_network(network * net, float max_time) {
    /* --------------------- set the network to running --------------------- */
    net->running = 1;
    int n = net->n;
    /* ---------------------- creation of the threads ----------------------- */
    for (int i = 0; i < n; i++) {
        /* ------------ passing arguments to the thread creation ------------ */
        pthread_create(&net->threads[i], NULL, start_node, (void *) net);
        printf("Created thread %d with id %lu\n", i, net->threads[i]);
    }

    /* --------------------- end of network conditions ---------------------- */
    sleep(max_time);

    /* ------------------------ stopping the threads ------------------------ */
    net->running = 0;
    for (int i = 0; i < net->n; i++) {
        pthread_join(net->threads[i], NULL);
    } 

    /* --------------------------- memory freeing --------------------------- */
    printf("# ---------- clearing memory --------- #\n");
    for (int i = 0; i < net->n; i++) {
        StackDestroy(net->nodes[i]->messages_stack);
        free(net->nodes[i]->messages_stack);
        free(net->nodes[i]);
    }
    free(net->threads);
    free(net->nodes);
    free(net);
    printf("# ==================================== #\n");
}

network * random_network(int n, float p, int nb_prot, int max_stack){
    /* ------------------------- creating the graph ------------------------- */
    void * graph = NULL;

    /* ----------------- creating the adaptation functions ------------------ */
    
    /* ------------------------- creating the nodes ------------------------- */
    node ** nodes = malloc(sizeof(node *) * n);
    for (int i = 0; i < n; i++) {
        /* ----------------- choosing adaptation functions ------------------ */

        // --
        nodes[i] = create_node(i, NULL, 0);
    }
    return create_network(graph, nodes, max_stack);    
}

/* ────────────────────────────────── main ────────────────────────────────── */
int main(int argc, const char * argv[]) {
    printf("# ==================================== #\n");
    int n = N;
    /* ----------------- creation and start of the network ------------------ */
    network * net = random_network(n, 0, 4, 1);    
    start_network(net, 2);
}
