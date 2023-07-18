/*
 * scheduler.h
 *
 *  Created on: 6 juin 2023
 *      Author: remil
 */


#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "node.h"
#include "routing_table.h"
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#define MAX_TRANSMISSIONS 10
#define MAX_CHANNELS 5
#define MAX_SLOTS 20
#define MAX_NODES 100
#define EFFICIENCY_SCORE_PACKET_TYPE 0x01 // arbitrary packet type for efficiency score

// Transmission data structure
typedef struct {
    char source;
    char destination;
    int spreading_factor;
} Transmission;


typedef struct {
    int channel_index;
    int time_slot;
    Transmission transmission;
} ScheduledTransmission;





typedef struct {
    Transmission ** transmissions;
    int size;
} Channel;


// Function prototypes
void add_transmission(Transmission **transmissions, int *num_transmissions, char source, char destination, int spreading_factor);
int num_time_slots(int sf);
Transmission* create_transmissions_from_routing_table(routing_table_t *table, int *num_transmissions);
int compare_transmissions(const void *a, const void *b);
int find_minimal_spreading_factor(Transmission *transmissions, int num_transmissions);
void sort_transmissions_by_spreading_factor(Transmission *transmissions, int num_transmissions);
void create_channels_list(Transmission *transmissions, int num_transmissions, int current_node_id, int *known_dominants, int num_known_dominants, Channel **channels_list, int *num_channels);
bool is_id_in_known_dominants(int *known_dominants, int num_known_dominants, int id);
int get_index_in_known_dominants(int known_dominants[], int num_known_dominants, int id);
void cleanup_transmissions(Transmission **transmissions, int *num_transmissions);
void cleanup_channels(Channel **channels_list, int num_channels);
int* get_known_dominant_nodes(routing_table_t* table, int* num_known_dominant_nodes);
void cleanup_known_dominant_nodes(int **known_dominant_nodes);
void schedule_transmissions(Channel *channels_list, int num_channels, ScheduledTransmission **scheduled_transmissions, int *num_scheduled_transmissions);
//void cleanup_schedule(ScheduleEntry **schedule, int num_channels);
bool is_duplicate(ScheduledTransmission *primary_scheduled_transmissions, int num_primary_scheduled_transmissions, ScheduledTransmission transmission);
void merge_schedules(ScheduledTransmission *primary_scheduled_transmissions, int *num_primary_scheduled_transmissions,
                     ScheduledTransmission *secondary_scheduled_transmissions, int num_secondary_scheduled_transmissions);


double calculate_efficiency_score(ScheduledTransmission *scheduled_transmissions, int num_scheduled_transmissions);



#endif // SCHEDULER_H
