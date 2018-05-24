#ifndef NETWORK_HEADER
#define NETWORK_HEADER

// includes
#include <pthread.h>
#include <igraph.h>
#include <stdio.h>

// variables

// structs
struct physical_layer {

};
typedef struct physical_layer physical_message;

struct conf_message {

};
typedef struct conf_message conf_message;

struct message {

};
typedef struct message message;

// functions

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

/* main */
int main(int argc, const char *argv[]);

#endif
