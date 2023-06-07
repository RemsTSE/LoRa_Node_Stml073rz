/*
 * scheduler.h
 *
 *  Created on: 6 juin 2023
 *      Author: remil
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_TRANSMISSIONS 10
#define MAX_CHANNELS 10
#define MAX_SLOTS 100

// Transmission data structure
typedef struct {
    char source;
    char destination;
    int sf;
} transmission_t;

// Schedule data structure

typedef struct {
    transmission_t transmissions[MAX_TRANSMISSIONS];
    int slots[MAX_CHANNELS][MAX_SLOTS];
    int num_transmissions;
    int num_channels;
} schedule_t;

// Function prototypes
void add_transmission(schedule_t* schedule, char source, char destination, int sf);
void schedule_transmissions(schedule_t* schedule);
int compare_transmissions(const void* a, const void* b);
void sort_transmissions(schedule_t* schedule);







#endif // SCHEDULER_H
