#ifndef _NETWORK_H
#define _NETWORK_H

// includes
#include <pthread.h>
#include <igraph.h>
#include <stdio.h>

#include "stack.h"


#define MAX_SIZE 1024

/* ─────────────────────────────── protocols ──────────────────────────────── */
#define MSG 'm'
#define CONF 'c'
typedef char protocol;
typedef pthread_t node_id;

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
    node_id sender;
    node_id receiver;
    char protocol;
    void * content;
} physical_message;


/* ------------------------- configuration messages ------------------------- */
typedef struct conf_msg {
    node_id sender;
    node_id dest;
    int cost;
    int stack_size;
    stackT * protocol_stack;
} conf_message;

/* ---------------------------- regular messages ---------------------------- */
typedef struct msg {
    node_id src;
    node_id dest;
    int stack_size;
    int max_stack;
    void * payload;  // literally anything
} message;

/* ──────────────────────────── nodes (routers) ───────────────────────────── */
typedef struct router {
    node_id id;
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
void send(pthread_t sender_id, pthread_t receiver_id, void * message);

/* emulates the reception of a message through the physical layer
 *   - reads the physical message, converts the inner message to the
 *     correct type
 *   - calls the associated handler (receive_conf, receive_msg, route_msg) */
void receive(physical_message phys);

/* reception of a config message. 
 *   - adds rows to the routign table if necessary
 *   - send a message to each neighbor for each new route created */
void receive_conf(conf_message msg);

/* reception of a regular message, destined to this node */ 
void receive_msg(message msg);

/* routing of a regular message */
void route_msg(message msg);

/* initialises the node (router), then calls messages_wait */
void * start_node(void * ptr);

/* crates the routing table and sends messages to fill it */
void routing_table_init(void * table);

/* constant loop to receive physical messages, in a producer / consumer fashion */
void messages_wait(network * net);

/* initializes the network with the following parameters :
 *   - graph: the topology of the network
 *   - stack_size: the maximum size of a stack 
 *   - max_time: the network will stop running after max_time seconds if max_time > 0 */
void create_network(void * graph, int stack_size, float max_time);

/* main */
int main(int argc, const char *argv[]);

#endif
