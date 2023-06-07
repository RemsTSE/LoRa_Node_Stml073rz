/*
 * routing_table.c
 *
 *  Created on: 7 juin 2023
 *      Author: remil
 *      Description : Function definitions for the routing table
 */


#include <stdlib.h>
#include <stdbool.h>
#include "routing_table.h"



SecondHop* create_second_hop_node(int id, int SF) {
    SecondHop* new_node = malloc(sizeof(SecondHop));
    new_node->id = id;
    new_node->SF = SF;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

Neighbor* create_neighbor_node(int id, int SF) {
    Neighbor* new_node = malloc(sizeof(Neighbor));
    new_node->id = id;
    new_node->SF = SF;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->second_hop = NULL;
    return new_node;
}

//Insert neighbour in the binary tree
Neighbor* insert_neighbor(Neighbor* root, int id, int SF) {
    if (root == NULL) {
        return create_neighbor_node(id, SF);
    }
    if (id < root->id) {
        root->left = insert_neighbor(root->left, id, SF);
    } else if (id > root->id) {
        root->right = insert_neighbor(root->right, id, SF);
    }
    return root;
}

//Insert 2nd-hop neighbor
SecondHop* insert_second_hop(SecondHop* root, int id, int SF) {
    if (root == NULL) {
        return create_second_hop_node(id, SF);
    }
    if (id < root->id) {
        root->left = insert_second_hop(root->left, id, SF);
    } else if (id > root->id) {
        root->right = insert_second_hop(root->right, id, SF);
    }
    return root;
}

Neighbor* search_neighbor(Neighbor* root, int id) {
    if (root == NULL) {
        return NULL;
    } else if (root->id == id) {
        return root;
    } else if (id < root->id) {
        return search_neighbor(root->left, id);
    } else {
        return search_neighbor(root->right, id);
    }
}

bool search_second_hop(SecondHop* root, int id) {
    if (root == NULL) {
        return false;
    } else if (root->id == id) {
        return true;
    } else if (id < root->id) {
        return search_second_hop(root->left, id);
    } else {
        return search_second_hop(root->right, id);
    }
}


Neighbor* minValueNode(Neighbor* node) {
    Neighbor* current = node;

    while (current && current->left != NULL)
        current = current->left;

    return current;
}

Neighbor* remove_neighbor(Neighbor* root, int id) {
    if (root == NULL) {
        return root;
    }

    if (id < root->id) {
        root->left = remove_neighbor(root->left, id);
    } else if (id > root->id) {
        root->right = remove_neighbor(root->right, id);
    } else {
        if (root->left == NULL) {
            Neighbor* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            Neighbor* temp = root->left;
            free(root);
            return temp;
        }

        Neighbor* temp = minValueNode(root->right);
        root->id = temp->id;
        root->SF = temp->SF;
        root->second_hop = temp->second_hop;

        root->right = remove_neighbor(root->right, temp->id);
    }

    return root;
}

SecondHop* minValueNodeSecondHop(SecondHop* node) {
    SecondHop* current = node;

    while (current && current->left != NULL)
        current = current->left;

    return current;
}

SecondHop* remove_second_hop(SecondHop* root, int id) {
    if (root == NULL) {
        return root;
    }

    if (id < root->id) {
        root->left = remove_second_hop(root->left, id);
    } else if (id > root->id) {
        root->right = remove_second_hop(root->right, id);
    } else {
        if (root->left == NULL) {
            SecondHop* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            SecondHop* temp = root->left;
            free(root);
            return temp;
        }

        SecondHop* temp = minValueNodeSecondHop(root->right);
        root->id = temp->id;
        root->SF = temp->SF;

        root->right = remove_second_hop(root->right, temp->id);
    }

    return root;
}


Neighbor* create_example_routing_table(Neighbor* routing_table) {


    // Insert 1st hop neighbors
    routing_table = insert_neighbor(routing_table, 1, 10);
    routing_table = insert_neighbor(routing_table, 2, 8);
    routing_table = insert_neighbor(routing_table, 3, 9);

    // Find each 1st hop neighbor and insert its 2nd hop neighbors
    Neighbor* first_hop_neighbor = search_neighbor(routing_table, 1);
    if (first_hop_neighbor != NULL) {
        first_hop_neighbor->second_hop = insert_second_hop(first_hop_neighbor->second_hop, 4, 7);
        first_hop_neighbor->second_hop = insert_second_hop(first_hop_neighbor->second_hop, 5, 6);
    }

    first_hop_neighbor = search_neighbor(routing_table, 2);
    if (first_hop_neighbor != NULL) {
        first_hop_neighbor->second_hop = insert_second_hop(first_hop_neighbor->second_hop, 6, 5);
    }

    first_hop_neighbor = search_neighbor(routing_table, 3);
    if (first_hop_neighbor != NULL) {
        first_hop_neighbor->second_hop = insert_second_hop(first_hop_neighbor->second_hop, 7, 8);
        first_hop_neighbor->second_hop = insert_second_hop(first_hop_neighbor->second_hop, 8, 9);
    }

    return routing_table;
}

