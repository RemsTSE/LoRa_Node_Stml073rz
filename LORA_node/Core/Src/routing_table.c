/*
 * routing_table.c
 *
 *  Created on: 7 juin 2023
 *      Author: remil
 *      Description : Function definitions for the routing table
 */


#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "routing_table.h"



// Function to add a new entry to the routing table.
void add_entry(routing_table_t* table, int dest_node_id, int next_hop_id, int cost, int sf) {
    if (table->num_entries >= MAX_ENTRIES) {
        printf("Routing table is full.\n");

        return;
    }

    routing_entry_t* new_entry = &table->entries[table->num_entries++];
    new_entry->dest_node_id = dest_node_id;
    new_entry->next_hop_id = next_hop_id;
    new_entry->cost = cost;
    new_entry->sf = sf;
}

// Function to remove an entry from the routing table.
void remove_entry(routing_table_t* table, int dest_node_id) {
    int i;
    for (i = 0; i < table->num_entries; i++) {
        if (table->entries[i].dest_node_id == dest_node_id) {
            break;
        }
    }

    if (i == table->num_entries) {
        printf("No such entry found.\n");
        return;
    }

    for (int j = i; j < table->num_entries - 1; j++) {
        table->entries[j] = table->entries[j + 1];
    }

    table->num_entries--;
}

// Function to get an entry from the routing table.
routing_entry_t* get_entry(routing_table_t* table, int dest_node_id) {
    for (int i = 0; i < table->num_entries; i++) {
        if (table->entries[i].dest_node_id == dest_node_id) {
            return &table->entries[i];
        }
    }

    return NULL;
}

// Function to print all entries in the routing table.
void print_routing_table(routing_table_t* table) {
    printf("Routing table:\n");
    for (int i = 0; i < table->num_entries; i++) {
        printf("Dest: %d, Next hop: %d, Cost: %d, SF: %d\n", table->entries[i].dest_node_id, table->entries[i].next_hop_id, table->entries[i].cost, table->entries[i].sf);
    }
}


void list_dominant_nodes(routing_table_t* table) {
    int dominant_nodes[MAX_ENTRIES];
    int count = 0;
    for(int i = 0; i < table->num_entries; i++) {
        if(table->entries[i].dominant_dest) {
            dominant_nodes[count] = table->entries[i].dest_node_id;
            count++;
        }
    }

    // print the IDs of the dominant nodes
    printf("Dominant nodes known by node %d: \n", table->current_node_id);
    for(int i = 0; i < count; i++) {
        printf("%d\n", dominant_nodes[i]);
    }
}
