/*
 * routing_table.h
 *
 *  Created on: 7 juin 2023
 *      Author: remil
 *      Description : Implementation of the routing table
 */
#include <stdbool.h>
#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H
#define MAX_ENTRIES 20

typedef struct {
    int dest_node_id;
    bool dominant_dest;
    int next_hop_id;
    int cost;
    int sf;
} routing_entry_t;

typedef struct {
	int current_node_id;
    int num_entries;
    routing_entry_t entries[MAX_ENTRIES];
} routing_table_t;



void add_entry(routing_table_t* table, int dest_node_id, int next_hop_id, int cost, int sf);
void remove_entry(routing_table_t* table, int dest_node_id);
routing_entry_t* get_entry(routing_table_t* table, int dest_node_id);
void list_dominant_nodes(routing_table_t* table);
void print_routing_table(routing_table_t* table);
int get_spreading_factor_for_node(routing_table_t *routing_table, int node_id);

#endif
