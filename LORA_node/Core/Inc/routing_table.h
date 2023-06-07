/*
 * routing_table.h
 *
 *  Created on: 7 juin 2023
 *      Author: remil
 *      Description : Implementation of the routing table of a node under the form of a binary search tree, containing
 *      the neighbours and 2nd-hop neighbours, with key (SF, id)
 */
#include <stdbool.h>
#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

// Define the struct for the 2nd hop neighbors
typedef struct SecondHop {
    int id;
    int SF;
    struct SecondHop* left;
    struct SecondHop* right;
} SecondHop;

// Define the struct for the first hop neighbors
typedef struct Neighbor {
    int id;
    int SF;
    struct Neighbor* left;
    struct Neighbor* right;
    SecondHop* second_hop;
} Neighbor;

// Function prototypes
Neighbor* create_neighbor_node(int id, int SF);
SecondHop* create_second_hop_node(int id, int SF);
Neighbor* insert_neighbor(Neighbor* root, int id, int SF);
SecondHop* insert_second_hop(SecondHop* root, int id, int SF);
Neighbor* search_neighbor(Neighbor* root, int id);
bool search_second_hop(SecondHop* root, int id);
Neighbor* remove_neighbor(Neighbor* root, int id);
SecondHop* remove_second_hop(SecondHop* root, int id);
Neighbor* create_example_routing_table(Neighbor* routing_table);


#endif
