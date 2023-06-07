/*
 * node.h
 *
 *  Created on: 7 juin 2023
 *      Author: remil
 */
#include "scheduler.h"
#include "routing_table.h"
#include <stdbool.h>
#ifndef INC_NODE_H_
#define INC_NODE_H_

typedef struct {
    int id;                     // Unique identifier for this node
    routing_table_t* routing_table;  // The node's routing table
    schedule_t* schedule;        // The node's schedule
    bool state;
    //TO DO : add parameters such as time, position, energy level maybe idk
} node_t;


#endif /* INC_NODE_H_ */
