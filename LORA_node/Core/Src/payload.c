/*
 * payload.c
 *
 *  Created on: 10 juil. 2023
 *      Author: remil
 */


#include "payload.h"
#include <stdio.h>
#include <string.h>

uint8_t* create_efficiency_score_payload(double score) {
    // Scale and round the score to an integer
    uint16_t rounded_score = (uint16_t)(score * 10000 + 0.5); // Basic rounding

    // Create the payload
    uint8_t *payload = (uint8_t*)malloc(4 * sizeof(uint8_t)); // Allocate memory for the payload

    if (payload == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    payload[0] = EFFICIENCY_SCORE_PACKET_TYPE;

    // Pack the rounded score into the payload as a 2-byte integer
    payload[1] = (rounded_score >> 8) & 0xFF; // High byte
    payload[2] = rounded_score & 0xFF; // Low byte

    // Calculate a simple checksum (sum of payload bytes)
    uint8_t checksum = payload[0] + payload[1] + payload[2];
    payload[3] = checksum;

    return payload;
}

void create_scheduled_transmission_payload(ScheduledTransmission* transmission, unsigned char* payload) {
    ScheduledTransmissionPayload payload_struct;

    payload_struct.source = transmission->transmission.source;
    payload_struct.destination = transmission->transmission.destination;
    payload_struct.spreading_factor = (char) transmission->transmission.spreading_factor;
    payload_struct.channel_index = (char) transmission->channel_index;
    payload_struct.time_slot = (short) transmission->time_slot;

    memcpy(payload, &payload_struct, sizeof(ScheduledTransmissionPayload));
}

void create_scheduled_transmissions_payload(ScheduledTransmission* transmissions, int num_transmissions, unsigned char* payload) {
    for (int i = 0; i < num_transmissions; i++) {
        create_scheduled_transmission_payload(&transmissions[i], &payload[i * sizeof(ScheduledTransmissionPayload)]);
    }
}



void fragment_payload(unsigned char* payload, int payload_size, unsigned char payload_type, PayloadFragment** fragments, int* num_fragments) {
    // Calculate the number of fragments
    *num_fragments = (payload_size + sizeof(PayloadFragment) - 1) / sizeof(PayloadFragment);

    // Allocate memory for the fragments
    *fragments = malloc(*num_fragments * sizeof(PayloadFragment));

    for (int i = 0; i < *num_fragments; i++) {
        // Set the payload type and sequence number
        (*fragments)[i].type = payload_type;
        (*fragments)[i].sequence_number = i;

        // Calculate the size of the data in this fragment
        int data_size = i < *num_fragments - 1 ? sizeof(PayloadFragment) : payload_size % sizeof(PayloadFragment);

        // Copy the data to the fragment
        memcpy((*fragments)[i].data, &payload[i * sizeof(PayloadFragment)], data_size);

        // If this is the last fragment and it's not full, pad it with zeros
        if (data_size < sizeof(PayloadFragment)) {
            memset(&(*fragments)[i].data[data_size], 0, sizeof(PayloadFragment) - data_size);
        }
    }
}
