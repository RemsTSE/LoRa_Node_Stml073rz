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

void create_scheduled_transmission(ScheduledTransmission* transmission, unsigned char* payload) {
    ScheduledTransmissionPayload payload_struct;
    memcpy(&payload_struct, payload, sizeof(ScheduledTransmissionPayload));

    transmission->transmission.source = payload_struct.source;
    transmission->transmission.destination = payload_struct.final_destination;
    transmission->transmission.spreading_factor = (uint8_t) payload_struct.spreading_factor;
    transmission->channel_index = (uint8_t) payload_struct.channel_index;
    transmission->time_slot = (uint16_t) payload_struct.time_slot;
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




uint8_t* create_efficiency_score_payload_with_header(int source_id, int destination_id, int final_destination_id, double score) {
    // Scale and round the score to an integer
    uint16_t rounded_score = (uint16_t)(score * 10000 + 0.5); // Basic rounding

    // Create the payload with additional space for the header
    uint8_t *payload = (uint8_t*)malloc((5 + HEADER_SIZE) * sizeof(uint8_t)); // Allocate memory for the payload

    if (payload == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    // Add the source, destination, and final destination ids as the header
    payload[0] = (uint8_t)source_id;
    payload[1] = (uint8_t)destination_id;
    payload[2] = (uint8_t)final_destination_id;

    payload[3] = EFFICIENCY_SCORE_PACKET_TYPE;

    // Pack the rounded score into the payload as a 2-byte integer
    payload[4] = (rounded_score >> 8) & 0xFF; // High byte
    payload[5] = rounded_score & 0xFF; // Low byte

    // Calculate a simple checksum (sum of payload bytes)
    uint8_t checksum = payload[3] + payload[4] + payload[5];
    payload[6] = checksum;

    return payload;
}

void create_scheduled_transmissions_payload_with_header(int source_id, int destination_id, int final_destination_id, ScheduledTransmission* transmissions, int num_transmissions, unsigned char* payload) {
    // Add the source, destination, and final destination ids as the header
    payload[0] = (uint8_t)source_id;
    payload[1] = (uint8_t)destination_id;
    payload[2] = (uint8_t)final_destination_id;

    for (int i = 0; i < num_transmissions; i++) {
        create_scheduled_transmission_payload(&transmissions[i], &payload[i * sizeof(ScheduledTransmissionPayload) + HEADER_SIZE + 1]);
    }
}


void parse_scheduled_transmissions_payload(uint8_t* payload, int num_transmissions, int* source_id, int* destination_id, int* final_destination_id, ScheduledTransmission* transmissions) {
    *source_id = payload[0];
    *destination_id = payload[1];
    *final_destination_id = payload[2];

    for (int i = 0; i < num_transmissions; i++) {
        create_scheduled_transmission(&transmissions[i], &payload[i * sizeof(ScheduledTransmissionPayload) + HEADER_SIZE + 1]);
    }
}






void insert_received_fragment(PayloadFragment* fragments, int max_fragments, PayloadFragment received_fragment) {
    // Ensure that the sequence number is valid
    if (received_fragment.sequence_number >= max_fragments) {
        printf("Received fragment with invalid sequence number: %d\n", received_fragment.sequence_number);
        return;
    }

    // Insert the fragment into the array
    fragments[received_fragment.sequence_number] = received_fragment;
}


void rebuild_payload_from_fragments(unsigned char* payload, PayloadFragment* fragments, int num_fragments) {
    for (int i = 0; i < num_fragments; i++) {
        // Ensure that the sequence number is correct
        if (fragments[i].sequence_number != i) {
            printf("Fragment with incorrect sequence number: %d (expected %d)\n", fragments[i].sequence_number, i);
            return;
        }

        // Copy the data from the fragment to the payload
        memcpy(&payload[i * sizeof(fragments[i].data)], fragments[i].data, sizeof(fragments[i].data));
    }
}


void multicast_efficiency_score_packet(LoRa* lora, routing_table_t* routing_table, double score) {
    // Iterate through the entries in the routing table
    for (int i = 0; i < routing_table->num_entries; i++) {
        // Check if this node is a dominant node
        if (routing_table->entries[i].dominant_dest) {
            // Create an efficiency score payload for this destination
            uint8_t* payload = create_efficiency_score_payload_with_header(
                routing_table->current_node_id,    // Source ID is the current node
                routing_table->entries[i].next_hop_id,    // Destination ID is the next hop
                routing_table->entries[i].dest_node_id, // Final destination ID is the dominant node
                score    // The efficiency score to send
            );

            // Fragment the payload
            PayloadFragment* fragments;
            int num_fragments;
            fragment_payload(payload, sizeof(ScheduledTransmissionPayload), EFFICIENCY_SCORE_PACKET_TYPE, &fragments, &num_fragments);

            // Transmit each fragment
            for (int j = 0; j < num_fragments; j++) {
                // Transmit the fragment using the module, and the sf of each link
            	lora->spredingFactor = routing_table->entries[i].sf;
                uint8_t transmit_status = transmit_payload(lora, &fragments[j]);
                if (transmit_status != LORA_OK) {
                    // Handle failed transmission
                    printf("Failed to transmit fragment %d\n", j);
                    continue;
                }

                // If it's a 2-hop neighbour
                if (routing_table->entries[i].next_hop_id != routing_table->entries[i].dest_node_id) {
                    // The next hop needs to retransmit the packet to the final destination

                    instruct_retransmission(lora, routing_table->entries[i].next_hop_id, fragments[j].sequence_number);
                }
            }

            // Free the fragments
            free_fragments(&fragments);

            // Free the payload
            free(payload);
        }
    }
}


uint8_t transmit_payload(LoRa* lora, PayloadFragment* fragment) {
    // Create a byte array to hold the payload fragment
    unsigned char data[sizeof(PayloadFragment)];

    // Convert the PayloadFragment structure to a byte array
    memcpy(data, fragment, sizeof(PayloadFragment));

    // Transmit the payload
    uint8_t transmit_status = LoRa_transmit(lora, data, sizeof(PayloadFragment), TRANSMIT_TIMEOUT);

    return transmit_status;
}

void instruct_retransmission(LoRa* lora, int destination_id, int sequence_number) {
    // Create a payload to request the retransmission
    uint8_t payload[5];

    // First two bytes are the source and destination ids
    payload[0] = (uint8_t)lora->current_mode;
    payload[1] = (uint8_t)destination_id;

    // Next two bytes are the sequence number
    payload[2] = (sequence_number >> 8) & 0xFF; // High byte
    payload[3] = sequence_number & 0xFF; // Low byte

    // The last byte is the payload type (RETRANSMISSION_REQUEST)
    payload[4] = RETRANSMISSION_REQUEST_PACKET_TYPE;

    // Transmit the retransmission request
    LoRa_transmit(lora, payload, 5, TRANSMIT_TIMEOUT);
}


void relay_efficiency_score_packets(LoRa* lora, routing_table_t* routing_table) {
    // We start by checking if we have received any packet
    unsigned char received_data[SIZE_OF_PAYLOAD_FRAGMENT];
    uint8_t receive_status = LoRa_receive(lora, received_data, SIZE_OF_PAYLOAD_FRAGMENT);

    // If we've successfully received data
    if(receive_status == LORA_OK) {
        // Parse the received data into a payload fragment
        PayloadFragment received_fragment;
        memcpy(&received_fragment, received_data, SIZE_OF_PAYLOAD_FRAGMENT);

        // Parse the payload to get the final destination ID
        int source_id, destination_id, final_destination_id;
        parse_payload_header(received_fragment.data, &source_id, &destination_id, &final_destination_id);

        // Check if the current node is the final destination
        if (final_destination_id != routing_table->current_node_id) {
            // The current node is not the final destination, so it must relay the packet
            // Find the routing entry for the final destination
            for (int i = 0; i < routing_table->num_entries; i++) {
                if (routing_table->entries[i].dest_node_id == final_destination_id) {
                    // We found the routing entry. Now, relay the packet to the next hop.
                	lora->spredingFactor = routing_table->entries[i].sf;
                    transmit_payload(lora, &received_fragment);
                    break;
                }
            }
        }
    }
}

void parse_payload_header(uint8_t* payload, int* source_id, int* destination_id, int* final_destination_id) {



    // | Source ID (4 bytes) | Destination ID (4 bytes) | Final Destination ID (4 bytes) |

    // Extract the source id
    *source_id = *(int*)(payload);

    // Extract the destination id
    *destination_id = *(int*)(payload + 4);

    // Extract the final destination id
    *final_destination_id = *(int*)(payload + 8);
}


int is_smallest_id(int current_id, int* ids, int num_ids) {
    for (int i = 0; i < num_ids; i++) {
        if (ids[i] < current_id) {
            return 0;  // False: there is a smaller id
        }
    }
    return 1;  // True: current_id is the smallest
}

int get_index_of_id(int id, int* ids, int num_ids) {
    for (int i = 0; i < num_ids; i++) {
        if (ids[i] == id) {
            return i;
        }
    }
    return -1;  // ID not found
}

int all_scores_received(int* scores_received, int num_scores) {
    for (int i = 0; i < num_scores; i++) {
        if (scores_received[i] == -1) {
            return 0;  // False: not all scores received
        }
    }
    return 1;  // True: all scores received
}


int is_efficiency_score_packet(uint8_t* packet) {
    return packet[3] == EFFICIENCY_SCORE_PACKET_TYPE;
}


void parse_efficiency_score_packet(uint8_t* packet, int* source_id, double* score) {


    // Parse source id stored as 4 bytes (int) in the packet
    memcpy(source_id, &packet[0], 4);

    // Parse score  stored as 8 bytes (double) in the packet
    memcpy(score, &packet[4], 8);
}


int receive_packet(LoRa* lora, uint8_t* packet) {
    // Start receiving
    LoRa_startReceiving(lora);

    // Check if a packet has been received
    uint8_t irqFlags = LoRa_read(lora, RegIrqFlags);
    if (irqFlags & 0x40) { // 0x40 = RX_DONE flag
        // Clear RX_DONE flag
        LoRa_write(lora, RegIrqFlags, irqFlags | 0x40);

        // Read received packet
        uint8_t length = LoRa_read(lora, RegRxNbBytes);
        if (length > 0) {
            uint8_t addr = LoRa_read(lora, RegFiFoRxCurrentAddr);
            LoRa_readReg(lora, &addr, 1, packet, length);
        }

        return length;
    }

    return 0; // No packet received
}


void multicast_and_receive_efficiency_scores(LoRa* lora, routing_table_t* routing_table, double my_score,int* known_dominants, int num_known_dominants,double scores[num_known_dominants]) {


    // Score information array
    int scores_received[num_known_dominants];


    // Initialize to -1 (indicates score not received yet)
    for (int i = 0; i < num_known_dominants; i++) {
        scores_received[i] = -1;
    }

    // If this node has the smallest id among dominant nodes, it starts the transmission
    if (is_smallest_id(routing_table->current_node_id, known_dominants, num_known_dominants)) {
        multicast_efficiency_score_packet(lora, routing_table, my_score);

        // Update own score received
        scores_received[get_index_of_id(routing_table->current_node_id, known_dominants, num_known_dominants)] = 1;
        scores[get_index_of_id(routing_table->current_node_id, known_dominants, num_known_dominants)] = my_score;
    }

    // Receiving loop
    while (!all_scores_received(scores_received, num_known_dominants)) {
        // Listen for efficiency score packets
        uint8_t packet[MAX_PACKET_SIZE];
        int packet_length = receive_packet(lora, packet);

        // If a packet is received and it's an efficiency score packet
        if (packet_length > 0 && is_efficiency_score_packet(packet)) {
            // Parse packet and get efficiency score and source ID
            int source_id;
            double score;
            parse_efficiency_score_packet(packet, &source_id, &score);

            // Update scores_received and scores arrays
            scores_received[get_index_of_id(source_id, known_dominants, num_known_dominants)] = 1;
            scores[get_index_of_id(source_id, known_dominants, num_known_dominants)] = score;

            // If the current node has the smallest id among the dominant nodes that have not transmitted yet
            if (is_smallest_id(routing_table->current_node_id, known_dominants, num_known_dominants)) {
                // Multicast its efficiency score
                multicast_efficiency_score_packet(lora,routing_table, my_score);

                // Update own score received
                scores_received[get_index_of_id(routing_table->current_node_id, known_dominants, num_known_dominants)] = 1;
                scores[get_index_of_id(routing_table->current_node_id, known_dominants, num_known_dominants)] = my_score;
            }
        }

        // Error handling here
    }

}



void free_fragments(PayloadFragment** fragments) {
    free(*fragments);
    *fragments = NULL;
}


void relay_retransmission_request_packets(LoRa* lora, routing_table_t* routing_table) {
    // Buffer for received data
    unsigned char received_data[SIZE_OF_PAYLOAD_FRAGMENT];
    uint8_t receive_status = LoRa_receive(lora, received_data, SIZE_OF_PAYLOAD_FRAGMENT);

    // If data is successfully received
    if(receive_status == LORA_OK) {
        // Parse the received data into a payload fragment
        PayloadFragment received_fragment;
        memcpy(&received_fragment, received_data, SIZE_OF_PAYLOAD_FRAGMENT);

        // Parse the payload to get the source ID, destination ID, and packet ID
        int source_id, destination_id, final_destination_id;
        parse_payload_header(received_fragment.data, &source_id, &destination_id, &final_destination_id);

        // Check if the current node is the next hop for retransmission request
        if (destination_id == routing_table->current_node_id) {
            // Find the routing entry for the final destination
            for (int i = 0; i < routing_table->num_entries; i++) {
                if (routing_table->entries[i].dest_node_id == final_destination_id) {
                    // We found the routing entry. Now, relay the packet to the next hop.
                    lora->spredingFactor = routing_table->entries[i].sf;
                    transmit_payload(lora, &received_fragment);
                    break;
                }
            }
        }
    }
}




void receive_and_rebuild_schedule(LoRa *myLoRa, ScheduledTransmission *schedule, int num_transmissions) {
    // Allocate an array for the fragments
    PayloadFragment *fragments = malloc(MAX_FRAGMENTS * sizeof(PayloadFragment));
    int num_fragments_received = 0;
    uint8_t data[64];  // adjust size as needed

    // Listen for fragments until the schedule is complete or a timeout occurs
    while (num_fragments_received < num_transmissions) {
        // Attempt to receive a packet
        if (LoRa_receive(myLoRa, data, sizeof(data))) {
            // Packet was received, treat it as a fragment
            PayloadFragment fragment;
            memcpy(&fragment, data, sizeof(PayloadFragment));  // assuming the packet data is a PayloadFragment

            // Check the packet type
            if (fragment.type == SCHEDULED_TRANSMISSIONS_PACKET_TYPE) {
                // This is a schedule fragment, so insert it into the array
                insert_received_fragment(fragments, MAX_FRAGMENTS, fragment);

                num_fragments_received++;
            }
        }

        // TODO: Add a timeout or other termination condition
    }

    // Rebuild the payload from the fragments
    unsigned char *payload = malloc(sizeof(ScheduledTransmissionPayload) * num_transmissions);
    rebuild_payload_from_fragments(payload, fragments, num_fragments_received);

    // Parse the payload into a schedule
    int source_id, destination_id, final_destination_id;
    parse_scheduled_transmissions_payload(payload, num_transmissions, &source_id, &destination_id, &final_destination_id, schedule);

    // Clean up
    free(payload);
    free(fragments);
}


void relay_scheduled_transmissions(LoRa* lora, routing_table_t* routing_table,
                                   ScheduledTransmission *current_schedule,
                                   int *num_current_scheduled_transmissions, int num_known_dominants) {

    unsigned char received_data[SIZE_OF_PAYLOAD_FRAGMENT];
    uint8_t receive_status = LoRa_receive(lora, received_data, SIZE_OF_PAYLOAD_FRAGMENT);

    static PayloadFragment fragments[MAX_FRAGMENTS];
    static int num_fragments_received = 0;

    static ScheduledTransmission received_schedules[MAX_NODES][MAX_SCHEDULES];  // Assuming a maximum number of schedules and nodes
    static int num_received_schedules_from_nodes[MAX_NODES] = {0};
    static int total_received_schedules = 0;

    if(receive_status == LORA_OK) {
        PayloadFragment received_fragment;
        memcpy(&received_fragment, received_data, SIZE_OF_PAYLOAD_FRAGMENT);

        int source_id, destination_id, final_destination_id;
        parse_payload_header(received_fragment.data, &source_id, &destination_id, &final_destination_id);

        if (final_destination_id != routing_table->current_node_id) {
            for (int i = 0; i < routing_table->num_entries; i++) {
                if (routing_table->entries[i].dest_node_id == final_destination_id) {
                    lora->spredingFactor = routing_table->entries[i].sf;
                    transmit_payload(lora, &received_fragment);
                    break;
                }
            }
        } else {
            if (num_fragments_received < MAX_FRAGMENTS) {
                memcpy(&fragments[num_fragments_received], &received_fragment, sizeof(PayloadFragment));
                num_fragments_received++;
            }

            if (num_fragments_received >= *num_current_scheduled_transmissions) {
                ScheduledTransmission received_schedule[*num_current_scheduled_transmissions];
                receive_and_rebuild_schedule(lora, received_schedule, *num_current_scheduled_transmissions);

                // Store the received schedule for later merging
                memcpy(received_schedules[source_id], received_schedule, *num_current_scheduled_transmissions * sizeof(ScheduledTransmission));
                num_received_schedules_from_nodes[source_id]++;
                total_received_schedules++;

                if (total_received_schedules == num_known_dominants) {
                    // Merging all received schedules with the current schedule
                    for (int i = 0; i < num_known_dominants; i++) {
                        merge_schedules(current_schedule, num_current_scheduled_transmissions, received_schedules[i], num_received_schedules_from_nodes[i]);
                    }

                    // Transmitting the merged schedule back to the sender using the function with the header
                    unsigned char merged_schedule_payload[HEADER_SIZE + *num_current_scheduled_transmissions * sizeof(ScheduledTransmissionPayload)];

                    create_scheduled_transmissions_payload_with_header(routing_table->current_node_id, -1, final_destination_id, current_schedule, *num_current_scheduled_transmissions, merged_schedule_payload);

                    for (int i = 0; i < num_known_dominants; i++) {
                        transmit_payload(lora, (PayloadFragment *)merged_schedule_payload);  // Assuming the broadcast functionality if not one by one to dominants
                    }

                    // Reset for next cycle
                    total_received_schedules = 0;
                    memset(num_received_schedules_from_nodes, 0, sizeof(num_received_schedules_from_nodes));
                }

                num_fragments_received = 0;  // Reset fragment counter for next schedule
            }
        }
    }
}





void send_schedule_to_most_efficient_node(LoRa* lora, routing_table_t* routing_table, ScheduledTransmission* schedule, int num_transmissions, int* known_dominants, double scores[], int num_known_dominants) {
    // Get the most efficient node based on the score
    int best_index = 0;
    for (int i = 1; i < num_known_dominants; i++) {
        if (scores[i] > scores[best_index]) {
            best_index = i;
        }
    }
    int best_node_id = known_dominants[best_index];

    // Create a payload for sending the schedule
    unsigned char *payload = NULL;
    create_scheduled_transmissions_payload_with_header(routing_table->current_node_id, best_node_id, best_node_id, schedule, num_transmissions, payload);

    // Fragment the payload
    PayloadFragment* fragments;
    int num_fragments;
    fragment_payload(payload, sizeof(ScheduledTransmissionPayload) * num_transmissions + HEADER_SIZE + 1, SCHEDULED_TRANSMISSIONS_PACKET_TYPE, &fragments, &num_fragments);

    // Transmit each fragment
    for (int j = 0; j < num_fragments; j++) {
        lora->spredingFactor = get_spreading_factor_for_node(routing_table, best_node_id);
        transmit_payload(lora, &fragments[j]);
    }

    // Clean up
    free(payload);
    free(fragments);
}


