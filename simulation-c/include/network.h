#ifndef NETWORK_HEADER
#define NETWORK_HEADER

// includes
#include <pthread.h>
#include <igraph.h>
#include <stdio.h>

/* ─────────────────────────────── protocols ──────────────────────────────── */
#define MSG 'm'
#define CONF 'c'
typedef char protocol;
typedef pthread_t node_id;

/* ────────────────────────── adaptation functions ────────────────────────── */
enum adapt_types {CV, EC, DC};
typedef enum adapt_types adapt_type;

struct adapt_function {
    adapt_type type;
    /* array of protocols, size and contents depends on the type of function:
     *   - conversion: [from, to]
     *   - encapsulation: [from, to] 
     *   - decapsulation: [from, to] */
    protocol * protocols;
};
typedef struct adapt_function adapt_f;

/* ---------------- adaptation function creation and display ---------------- */
adapt_function create_adapt_function(protocol from, protocol to, adapt_type);

void store_repr(adapt_f func, char * str);

/* ──────────────────────────────── messages ──────────────────────────────── */

/* ------------------------ physical layer messages ------------------------- */
struct physical_layer {
    node_id sender;
    node_id receiver;
    char protocol;
    void * content;
};
typedef struct physical_layer physical_message;


/* ------------------------- configuration messages ------------------------- */
struct conf_msg {
    node_id sender;
    node_id dest;
    int cost;
    int stack_size;
    protocol * protocol_stack;
};
typedef struct conf_msg conf_message;

/* ---------------------------- regular messages ---------------------------- */
struct msg {
    node_id src;
    node_id dest;
    int stack_size;
    int max_stack;
    void * payload;  // literally anything
};
typedef struct msg message;

/* ──────────────────────────── nodes (routers) ───────────────────────────── */
struct node {
    node_id id;
    adapt_f * adapt_functions; // array of adaptation functions
    // stats
    int received;
    int sent;
};
typedef struct node router;

/* -------------------------------------------------------------------------- */
int static stack_size;
int static running = 0;

/* -------------------------------------------------------------------------- */

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

/* constant loop to receive physical messages, in a producer / consumer fashion */
void messages_wait(void);

/* initializes the network with the following parameters :
 *   - graph: the topology of the network
 *   - stack_size: the maximum size of a stack */
void create_network(void * graph, int stack_size);

/* starts the network
 *   - max_time: time in seconds after which the network is stopped */
void start_network(float max_time);

/* main */
int main(int argc, const char *argv[]);

#endif
