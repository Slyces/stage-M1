#ifndef _NETWORK_H
#define _NETWORK_H

// includes
#include <pthread.h>
#include <igraph.h>
#include <stdio.h>

#include "stack.h"


#define MAX_SIZE 1024

#define MSG 'm'
#define CONF 'c'

/* ────────────────────────── adaptation functions ────────────────────────── */
typedef enum adapt_types {CV, EC, DC} adapt_type;

typedef struct adapt_function {
    adapt_type type;
    /* array of protocols, size and contents depends on the type of function:
     *   - conversion: [from, to]
     *   - encapsulation: [from, to] 
     *   - decapsulation: [from, to] */
    protocol * protocols;
} adapt_f;

/* ---------------- adaptation function creation and display ---------------- */
adapt_f create_adapt_function(protocol from, protocol to, adapt_type);

void store_repr(adapt_f func, char * str);

/* ──────────────────────────────── messages ──────────────────────────────── */

/* ------------------------ physical layer messages ------------------------- */
typedef struct physical_layer {
    int sender;
    int receiver;
    char prot;
    void * content;
} physical_message;


/* ------------------------- configuration messages ------------------------- */
typedef struct conf_msg {
    int sender;
    int dest;
    int cost;
    int stack_size;
    stackT * protocol_stack;
} conf_message;

/* ---------------------------- regular messages ---------------------------- */
typedef struct msg_struct {
    int src;
    int dest;
    int stack_size;
    int max_stack;
    void * payload;  // literally anything
} message;

physical_message * create_physical(int sender, int receiver, char type, void * content);
/* ──────────────────────────── nodes (routers) ───────────────────────────── */
typedef struct router {
    int id;
    int n_adapt;
    adapt_f * adapt_functions; // array of adaptation functions
    stackT * messages_stack;
    // stats
    int received;
    int sent;
} node;

/* ──────────────────────────────── network ───────────────────────────────── */
typedef struct ntw {
    int n;  // number of nodes 
    pthread_t * threads;
    node ** nodes;
    int running;
    void * graph;
} network;

/* -------------------------------------------------------------------------- */
//int static stackSize;

/* -------------------------------------------------------------------------- */

/* find the id (0 <= id < n) of the thread according to its pthread_t value
 *   - returns -1 if not found */
int get_id(network * net);

/* emulation of sending a message through the physical layer
 *   - puts a physical_layer message in the receiver's stack */
void send(int sender, int receiver, message * msg);

/* same as send, but for conf_message */
void send_conf(int sender, int receiver, conf_message * msg);

/* emulates the reception of a message through the physical layer
 *   - reads the physical message, converts the inner message to the
 *     correct type
 *   - calls the associated handler (receive_conf, receive_msg, route_msg) */
void receive(network * net, physical_message * phys);

/* reception of a config message. 
 *   - adds rows to the routign table if necessary
 *   - send a message to each neighbor for each new route created */
void receive_conf(network * net, int sender, conf_message * msg);

/* reception of a regular message, destined to this node */ 
void receive_msg(network * net, message * msg);

/* routing of a regular message */
void route_msg(network * net, message * msg);

/* initialises the node (router), then calls messages_wait */
void * start_node(void * ptr);

/* crates the routing table and sends messages to fill it */
void routing_table_init(void * table);

/* constant loop to receive physical messages, in a producer / consumer fashion */
void messages_wait(network * net);

/* initializes the network with the following parameters :
 *   - graph: the topology of the network
 *   - nodes_array: an array of node structures
 *   - stack_size: the maximum size of a stack 
 *   - max_time: the network will stop running after max_time seconds if max_time > 0 */
network * create_network(void * graph, node ** nodes_array, int stack_size);

/* runs the network during at most max_time seconds */
void start_network(network * net, float max_time);

/* initializes a node */
node * create_node(int id, adapt_f * functions, int n_functions);

/* generates a random network with:
 *   - n: number of nodes
 *   - nb_prot: number of protocols available
 *   - p: chance for each possible function to be known by a node
 *   - max_stack: maximum stack_size of a route */
network * random_network(int n, float p, int nb_prot, int max_stack);

/* main */
int main(int argc, const char *argv[]);

#endif
