/*
 * payload.h
 *
 *  Created on: 10 juil. 2023
 *      Author: remil
 */

#ifndef INC_PAYLOAD_H_
#define INC_PAYLOAD_H_
#define MAX_PAYLOAD_SIZE 64
#include "scheduler.h"

typedef struct {
    char source;
    char destination;
    char spreading_factor;
    char channel_index;
    short time_slot;
} ScheduledTransmissionPayload;


typedef struct {
    unsigned char type;
    unsigned short sequence_number;
    unsigned char data[61];
} PayloadFragment;

uint8_t* create_efficiency_score_payload(double score);
void create_scheduled_transmission_payload(ScheduledTransmission* transmission, unsigned char* payload);
void create_scheduled_transmissions_payload(ScheduledTransmission* transmissions, int num_transmissions, unsigned char* payload);
void fragment_payload(unsigned char* payload, int payload_size, unsigned char payload_type, PayloadFragment** fragments, int* num_fragments);


#endif /* INC_PAYLOAD_H_ */
