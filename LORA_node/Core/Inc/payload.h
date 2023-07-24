/*
 * payload.h
 *
 *  Created on: 10 juil. 2023
 *      Author: remil
 */

#ifndef INC_PAYLOAD_H_
#define INC_PAYLOAD_H_
#define MAX_PAYLOAD_SIZE 64
#define MAX_PACKET_SIZE 64
#define EFFICIENCY_SCORE_PACKET_TYPE 0x01
#define SCHEDULED_TRANSMISSIONS_PACKET_TYPE 0x02
#define RETRANSMISSION_REQUEST_PACKET_TYPE 0x03
#define SIZE_OF_PAYLOAD_FRAGMENT sizeof(PayloadFragment)
#define HEADER_SIZE 3

#include "scheduler.h"
#include "main.h"
#include "LoRa.h"

typedef struct {
    char source;
    char destination;
    char spreading_factor;
    char channel_index;
    short time_slot;
    char final_destination;
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
void free_fragments(PayloadFragment** fragments);
uint8_t* create_efficiency_score_payload_with_header(int source_id, int destination_id, int final_destination_id, double score);
void create_scheduled_transmissions_payload_with_header(int source_id, int destination_id, int final_destination_id, ScheduledTransmission* transmissions, int num_transmissions, unsigned char* payload);
void parse_scheduled_transmissions_payload(uint8_t* payload, int num_transmissions, int* source_id, int* destination_id, int* final_destination_id, ScheduledTransmission* transmissions);
void create_scheduled_transmission(ScheduledTransmission* transmission, unsigned char* payload);
void insert_received_fragment(PayloadFragment* fragments, int max_fragments, PayloadFragment received_fragment);
void rebuild_payload_from_fragments(unsigned char* payload, PayloadFragment* fragments, int num_fragments);
void multicast_efficiency_score_packet(LoRa* lora, routing_table_t* routing_table, double score);
uint8_t transmit_payload(LoRa* lora, PayloadFragment* fragment);
void instruct_retransmission(LoRa* lora, int destination_id, int sequence_number);
void relay_efficiency_score_packets(LoRa* _LoRa, routing_table_t* routing_table);
void parse_payload_header(uint8_t* payload, int* source_id, int* destination_id, int* final_destination_id);
int is_smallest_id(int current_id, int* ids, int num_ids);
int get_index_of_id(int id, int* ids, int num_ids);
int all_scores_received(int* scores_received, int num_scores);
int is_efficiency_score_packet(uint8_t* packet);
void parse_efficiency_score_packet(uint8_t* packet, int* source_id, double* score);
int receive_packet(LoRa* lora, uint8_t* packet);
void multicast_and_receive_efficiency_scores(LoRa* lora, routing_table_t* routing_table, double my_score, int* known_dominants, int num_known_dominants,double scores[num_known_dominants]);
void relay_retransmission_request_packets(LoRa* lora, routing_table_t* routing_table);

#endif /* INC_PAYLOAD_H_ */
