/*
 * scheduler.c
 *
 *  Created on: 6 juin 2023
 *      Author: remil
 */
#include "scheduler.h"
#include <stdlib.h>


int compare_transmissions(const void* a, const void* b) {
    transmission_t* transmission_a = (transmission_t*)a;
    transmission_t* transmission_b = (transmission_t*)b;

    return transmission_b->sf - transmission_a->sf;
}

void sort_transmissions(schedule_t* schedule) {
    qsort(schedule->transmissions, schedule->num_transmissions, sizeof(transmission_t), compare_transmissions);
}




void add_transmission(schedule_t* schedule, char source, char destination, int sf) {
    transmission_t transmission = {source, destination, sf};
    schedule->transmissions[schedule->num_transmissions++] = transmission;
}

void schedule_transmissions(schedule_t* schedule) {
    sort_transmissions(schedule);

    for (int i = 0; i < schedule->num_transmissions; i++) {
        transmission_t transmission = schedule->transmissions[i];

        int channel = 0;
        int slot = 0;

        // Find the first channel with enough free slots
        while (channel < schedule->num_channels && slot + transmission.sf > MAX_SLOTS) {
            slot = 0;
            while (slot < MAX_SLOTS && schedule->slots[channel][slot] != 0) {
                slot++;
            }
            if (slot + transmission.sf > MAX_SLOTS) {
                channel++;
            }
        }

        // If no such channel was found, create a new one
        if (channel == schedule->num_channels) {
            schedule->num_channels++;
        }

        // Assign the transmission to the found slots
        for (int j = 0; j < transmission.sf; j++) {
            schedule->slots[channel][slot + j] = i + 1;
        }
    }
}


