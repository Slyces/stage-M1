
/*
 * File: simulation.h
 * Author: Simon Lassourreuille <simon.lassourreuille@u-bordeaux.fr>
 * Topic: Main file for the network simulation. Creates and runs a random network.
 */
#ifndef _LINKSK_H
#define _LINKS_H

/*
 * Type: networkLink
 * ----------
 * This type represents a link between two nodes. A link is provided with
 * a correspondance (adaptFunction → cost) for each adaptFunction of the
 * network. If the adaptFunction is not found, a default cost is used.
 */
typedef struct {
    int defaultCost;
} networkLink;

#endif
