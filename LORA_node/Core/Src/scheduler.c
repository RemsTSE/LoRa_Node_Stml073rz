/*
 * scheduler.c
 *
 *  Created on: 6 juin 2023
 *      Author: remil
 */
#include "scheduler.h"
#include <stdio.h>
#include <string.h>


void add_transmission(Transmission **transmissions, int *num_transmissions, char source, char destination, int spreading_factor) {
    // Reallocate memory for the new transmission
    *transmissions = realloc(*transmissions, sizeof(Transmission) * (*num_transmissions + 1));
    if (*transmissions == NULL) {
        // Handle memory allocation error, for example by exiting the function or the program.
        perror("Memory allocation failed");
        return;
    }
    // Add the new transmission
    (*transmissions)[*num_transmissions].source = source;
    (*transmissions)[*num_transmissions].destination = destination;
    (*transmissions)[*num_transmissions].spreading_factor = spreading_factor;

    // Increment the number of transmissions
    (*num_transmissions)++;
}

Transmission* create_transmissions_from_routing_table(routing_table_t *table, int *num_transmissions) {
    // Initialize an array of transmissions
    Transmission *transmissions = NULL;
    *num_transmissions = 0;

    // Iterate through the routing table entries and create transmissions
    for (int i = 0; i < table->num_entries; i++) {
        char source = table->current_node_id + 'A'; // Convert node ID to character (assuming node ID starts from 0)
        char next_hop = table->entries[i].next_hop_id + 'A'; // Convert next hop node ID to character
        char destination = table->entries[i].dest_node_id + 'A'; // Convert destination node ID to character
        int spreading_factor = table->entries[i].sf;

        // If next_hop_id is the current node, this is a direct transmission to the destination
        if (table->entries[i].next_hop_id == table->current_node_id) {
            add_transmission(&transmissions, num_transmissions, source, destination, spreading_factor);
            add_transmission(&transmissions, num_transmissions, destination, source, spreading_factor);
        }
        // If next_hop_id is not the current node, this is a transmission to a second hop neighbor
        else {
            add_transmission(&transmissions, num_transmissions, source, next_hop, spreading_factor);
            add_transmission(&transmissions, num_transmissions, next_hop, destination, spreading_factor);
            add_transmission(&transmissions, num_transmissions, destination, next_hop, spreading_factor);
            add_transmission(&transmissions, num_transmissions, next_hop, source, spreading_factor);
        }
    }

    return transmissions;
}


void cleanup_transmissions(Transmission **transmissions, int *num_transmissions) {
    if (transmissions == NULL || *transmissions == NULL) {
        // Nothing to free
        return;
    }

    // Free the dynamically-allocated array
    free(*transmissions);

    // Reset the pointer and number of transmissions
    *transmissions = NULL;
    *num_transmissions = 0;
}


int find_minimal_spreading_factor(Transmission *transmissions, int num_transmissions) {
    // Initialize the minimal spreading factor to the largest possible integer
    int minimal_spreading_factor = 1000;

    // Iterate through the transmissions
    for (int i = 0; i < num_transmissions; i++) {
        // If the spreading factor of the current transmission is less than the minimal found so far
        if (transmissions[i].spreading_factor < minimal_spreading_factor) {
            // Update the minimal spreading factor
            minimal_spreading_factor = transmissions[i].spreading_factor;
        }
    }

    // Return the minimal spreading factor found
    return minimal_spreading_factor;
}



int compare_transmissions(const void *a, const void *b) {
    const Transmission *transmission1 = (const Transmission *)a;
    const Transmission *transmission2 = (const Transmission *)b;

    // Sort by spreading factor
    int sf_diff = transmission1->spreading_factor - transmission2->spreading_factor;

    // If spreading factors are equal, sort by source to keep pairs together
    if (sf_diff == 0) {
        return transmission1->source - transmission2->source;
    }

    return sf_diff;
}


void sort_transmissions_by_spreading_factor(Transmission *transmissions, int num_transmissions) {
    qsort(transmissions, num_transmissions, sizeof(Transmission), compare_transmissions);
}


int num_time_slots(int sf) {
    return 1 << (sf - 7);
}


int* get_known_dominant_nodes(routing_table_t* table, int* num_known_dominant_nodes) {
    // Keep track of the dominant nodes
    int dominant_nodes[MAX_ENTRIES]; // Assuming MAX_ENTRIES is defined, as in the routing_table_t structure
    int count = 0;

    // Iterate through the routing table entries
    for (int i = 0; i < table->num_entries; i++) {
        // If the entry has a dominant destination, add it to the list
        if (table->entries[i].dominant_dest) {
            dominant_nodes[count++] = table->entries[i].dest_node_id;
        }
    }

    // Create a dynamically-allocated array to store the dominant nodes
    int *result = (int *) malloc(sizeof(int) * count);

    // Copy the dominant nodes to the dynamically-allocated array
    for (int i = 0; i < count; i++) {
        result[i] = dominant_nodes[i];
    }

    // Return the number of known dominant nodes
    *num_known_dominant_nodes = count;

    // Return the dynamically-allocated array
    return result;
}



void cleanup_known_dominant_nodes(int **known_dominant_nodes) {
    if (known_dominant_nodes == NULL || *known_dominant_nodes == NULL) {
        // Nothing to free
        return;
    }

    free(*known_dominant_nodes);
    *known_dominant_nodes = NULL;
}




bool is_id_in_known_dominants(int *known_dominants, int num_known_dominants, int id) {
    // Iterate through the known dominators
    for (int i = 0; i < num_known_dominants; i++) {
        // If the id is found in the list, return true
        if (known_dominants[i] == id) {
            return true;
        }
    }
    // If the id is not found in the list, return false
    return false;
}


int get_index_in_known_dominants(int known_dominants[], int num_known_dominants, int id) {
    // Iterate through the known dominators
    for (int i = 0; i < num_known_dominants; i++) {
        // If the id is found in the array, return the index
        if (known_dominants[i] == id) {
            return i;
        }
    }
    // If the id is not found in the array, return -1
    return -1;
}


/*
channels_known_dominants[index].size++;
channels_known_dominants[index].transmissions = realloc(channels_known_dominants[index].transmissions, sizeof(Transmission*) * channels_known_dominants[index].size);
channels_known_dominants[index].transmissions[channels_known_dominants[index].size - 1] = malloc(sizeof(Transmission));
memcpy(channels_known_dominants[index].transmissions[channels_known_dominants[index].size - 1], &transmission, sizeof(Transmission));
 *
 *
 */

/*
void create_channels_list(Transmission *transmissions, int num_transmissions,
		int current_node_id, int *known_dominants, int num_known_dominants,
		Channel **channels_list, int *num_channels) {
	// Sort the transmissions by SF in descending order
	sort_transmissions_by_spreading_factor(transmissions, num_transmissions);

	// Initialize the list of channels for the current node
	*channels_list = (Channel*) malloc(
			sizeof(Channel) * (num_known_dominants + 2));
	*num_channels = num_known_dominants + 2;

	// Initialize the channel for transmissions involving the current node
	Channel channel_self;
	channel_self.transmissions = NULL;
	channel_self.size = 0;

	// Initialize the channel for transmissions that don't involve any dominator nodes
	Channel channel_no_dominant;
	channel_no_dominant.transmissions = NULL;
	channel_no_dominant.size = 0;

	// Initialize the channels for transmissions involving known dominator nodes
	Channel *channels_known_dominants = (Channel*) malloc(
			sizeof(Channel) * num_known_dominants);
	for (int i = 0; i < num_known_dominants; i++) {
		channels_known_dominants[i].transmissions = NULL;
		channels_known_dominants[i].size = 0;
	}

	// For each transmission
	for (int i = 0; i < num_transmissions; i++) {
		Transmission transmission = transmissions[i];

		//If 2 dominant neighbors nodes other than itself are involved
		if (is_id_in_known_dominants(known_dominants, num_known_dominants,
				transmission.destination)
				&& is_id_in_known_dominants(known_dominants,
						num_known_dominants, transmission.source)) {

			if (transmission.source < transmission.destination) {
				int index = get_index_in_known_dominants(known_dominants,
						num_known_dominants, transmission.source);

				channels_known_dominants[index].size++;
				channels_known_dominants[index].transmissions = realloc(
						channels_known_dominants[index].transmissions,
						sizeof(Transmission*)
								* channels_known_dominants[index].size);
				channels_known_dominants[index].transmissions[channels_known_dominants[index].size
						- 1] = malloc(sizeof(Transmission));
				memcpy(
						channels_known_dominants[index].transmissions[channels_known_dominants[index].size
								- 1], &transmission, sizeof(Transmission));
			} else {
				int index = get_index_in_known_dominants(known_dominants,
						num_known_dominants, transmission.destination);
				channels_known_dominants[index].size++;
				channels_known_dominants[index].transmissions = realloc(
						channels_known_dominants[index].transmissions,
						sizeof(Transmission*)
								* channels_known_dominants[index].size);
				channels_known_dominants[index].transmissions[channels_known_dominants[index].size
						- 1] = malloc(sizeof(Transmission));
				memcpy(
						channels_known_dominants[index].transmissions[channels_known_dominants[index].size
								- 1], &transmission, sizeof(Transmission));
			}
		}

		//If the transmission involves the current and another node, add to channel_self
		else if (transmission.source == current_node_id
				|| transmission.destination == current_node_id) {

			channel_self.size++;
			channel_self.transmissions = realloc(channel_self.transmissions,
					sizeof(Transmission*) * channel_self.size);
			channel_self.transmissions[channel_self.size - 1] = malloc(
					sizeof(Transmission));
			memcpy(channel_self.transmissions[channel_self.size - 1],
					&transmission, sizeof(Transmission));
		}

		//If the transmission involves a dominant node and a classic node
		else if (is_id_in_known_dominants(known_dominants, num_known_dominants,
				transmission.source)
				&& !is_id_in_known_dominants(known_dominants,
						num_known_dominants, transmission.destination)) {
			int index = get_index_in_known_dominants(known_dominants,
					num_known_dominants, transmission.source);
			channels_known_dominants[index].size++;
			channels_known_dominants[index].transmissions = realloc(
					channels_known_dominants[index].transmissions,
					sizeof(Transmission*)
							* channels_known_dominants[index].size);
			channels_known_dominants[index].transmissions[channels_known_dominants[index].size
					- 1] = malloc(sizeof(Transmission));
			memcpy(
					channels_known_dominants[index].transmissions[channels_known_dominants[index].size
							- 1], &transmission, sizeof(Transmission));
		} else if (is_id_in_known_dominants(known_dominants,
				num_known_dominants, transmission.destination)
				&& !is_id_in_known_dominants(known_dominants,
						num_known_dominants, transmission.source)) {
			int index = get_index_in_known_dominants(known_dominants,
					num_known_dominants, transmission.destination);
			channels_known_dominants[index].size++;
			channels_known_dominants[index].transmissions = realloc(
					channels_known_dominants[index].transmissions,
					sizeof(Transmission*)
							* channels_known_dominants[index].size);
			channels_known_dominants[index].transmissions[channels_known_dominants[index].size
					- 1] = malloc(sizeof(Transmission));
			memcpy(
					channels_known_dominants[index].transmissions[channels_known_dominants[index].size
							- 1], &transmission, sizeof(Transmission));
		}

		//If the transmission doesn't involve any dominant nodes
		else if (!is_id_in_known_dominants(known_dominants, num_known_dominants,
				transmission.destination)
				&& !is_id_in_known_dominants(known_dominants,
						num_known_dominants, transmission.source)) {
			channel_no_dominant.size++;
			channel_no_dominant.transmissions = realloc(
					channel_no_dominant.transmissions,
					sizeof(Transmission*) * channel_no_dominant.size);
			channel_no_dominant.transmissions[channel_no_dominant.size - 1] =
					malloc(sizeof(Transmission));
			memcpy(
					channel_no_dominant.transmissions[channel_no_dominant.size
							- 1], &transmission, sizeof(Transmission));

		}

	}

	// Add the channels to the list of channels
	(*channels_list)[0] = channel_self;
	(*channels_list)[1] = channel_no_dominant;
	for (int i = 0; i < num_known_dominants; i++) {
		(*channels_list)[i + 2] = channels_known_dominants[i];
	}
}
*/

void add_transmission_to_channel(Channel* channel, Transmission* transmission) {
    channel->size++;
    channel->transmissions = realloc(channel->transmissions, sizeof(Transmission*) * channel->size);

    if (!channel->transmissions) {
        // Handle memory allocation error
        exit(1);  // or return an error code, based on your application's needs
    }

    channel->transmissions[channel->size - 1] = malloc(sizeof(Transmission));
    memcpy(channel->transmissions[channel->size - 1], transmission, sizeof(Transmission));
}

void create_channels_list(Transmission *transmissions, int num_transmissions,
		int current_node_id, int *known_dominants, int num_known_dominants,
		Channel **channels_list, int *num_channels) {
	// Sort the transmissions by SF in descending order
	sort_transmissions_by_spreading_factor(transmissions, num_transmissions);

	// Initialize the list of channels for the current node
	*channels_list = (Channel*) malloc(sizeof(Channel) * (num_known_dominants + 2));
	*num_channels = num_known_dominants + 2;

	Channel channel_self = {NULL, 0};
	Channel channel_no_dominant = {NULL, 0};

	// Initialize the channels for transmissions involving known dominator nodes
	Channel *channels_known_dominants = (Channel*) malloc(sizeof(Channel) * num_known_dominants);
	for (int i = 0; i < num_known_dominants; i++) {
		channels_known_dominants[i].transmissions = NULL;
		channels_known_dominants[i].size = 0;
	}

	// For each transmission
	for (int i = 0; i < num_transmissions; i++) {
		Transmission* transmission = &transmissions[i];

		int source_in_known = is_id_in_known_dominants(known_dominants, num_known_dominants, transmission->source);
		int dest_in_known = is_id_in_known_dominants(known_dominants, num_known_dominants, transmission->destination);

		// If 2 dominant neighbor nodes other than itself are involved
		if (source_in_known && dest_in_known) {
			int index = (transmission->source < transmission->destination)
			            ? get_index_in_known_dominants(known_dominants, num_known_dominants, transmission->source)
			            : get_index_in_known_dominants(known_dominants, num_known_dominants, transmission->destination);
			add_transmission_to_channel(&channels_known_dominants[index], transmission);
		}
		// If the transmission involves the current and another node
		else if (transmission->source == current_node_id || transmission->destination == current_node_id) {
			add_transmission_to_channel(&channel_self, transmission);
		}
		// If the transmission involves a dominant node and a classic node
		else if ((source_in_known && !dest_in_known) || (!source_in_known && dest_in_known)) {
			int index = source_in_known
			            ? get_index_in_known_dominants(known_dominants, num_known_dominants, transmission->source)
			            : get_index_in_known_dominants(known_dominants, num_known_dominants, transmission->destination);
			add_transmission_to_channel(&channels_known_dominants[index], transmission);
		}
		// If the transmission doesn't involve any dominant nodes
		else {
			add_transmission_to_channel(&channel_no_dominant, transmission);
		}
	}

	// Add the channels to the list of channels
	(*channels_list)[0] = channel_self;
	(*channels_list)[1] = channel_no_dominant;
	memcpy(*channels_list + 2, channels_known_dominants, sizeof(Channel) * num_known_dominants);

	// Free memory for channels_known_dominants as its data has been transferred
	free(channels_known_dominants);
}



void cleanup_channels(Channel **channels_list, int num_channels) {
    if (channels_list == NULL || *channels_list == NULL) {
        // Nothing to free
        return;
    }

    for (int i = 0; i < num_channels; i++) {
        Channel *channel = &((*channels_list)[i]);
        for (int j = 0; j < channel->size; j++) {
            free(channel->transmissions[j]);
        }
        free(channel->transmissions);
    }
    free(*channels_list);
    *channels_list = NULL;
}




bool check_collision(ScheduledTransmission *scheduled_transmissions, int num_scheduled_transmissions, int time_slot, int duration, char source, char destination, int exclude_channel) {
    for (int i = 0; i < num_scheduled_transmissions; i++) {
        ScheduledTransmission scheduled_transmission = scheduled_transmissions[i];
        int channel_index = scheduled_transmission.channel_index;

        if (exclude_channel != -1 && channel_index == exclude_channel) {
            continue;
        }

        int ts = scheduled_transmission.time_slot;
        int tr_duration = (int)pow(2, (scheduled_transmission.transmission.spreading_factor - 7));

        char scheduled_source = scheduled_transmission.transmission.source;
        char scheduled_destination = scheduled_transmission.transmission.destination;

        if ((ts < time_slot + duration && ts + tr_duration > time_slot) &&
            (scheduled_source == source || scheduled_source == destination ||
             scheduled_destination == source || scheduled_destination == destination)) {
            return true;
        }
    }
    return false;
}








void schedule_transmissions(Channel *channel_list, int num_channels, ScheduledTransmission *scheduled_transmissions, int *num_scheduled_transmissions) {
    // Initialize the schedule
	int max_schedule_size = 40;
    //*scheduled_transmissions = (ScheduledTransmission *) malloc(max_schedule_size * sizeof(ScheduledTransmission));
    //*scheduled_transmissions = (ScheduledTransmission *) calloc(max_schedule_size, sizeof(ScheduledTransmission));
    *num_scheduled_transmissions = 0;

    // Initialize the next available time slot for each channel
    int next_available_slot[num_channels];
    for (int i = 0; i < num_channels; i++) {
        next_available_slot[i] = 0;
    }

    // For each channel
    for (int channel_index = 0; channel_index < num_channels; channel_index++) {
        Channel channel = channel_list[channel_index];

        // For each transmission in the channel
        for (int j = 0; j < channel.size; j++) {
        	Transmission *transmission = channel.transmissions[j];

            // The number of time slots needed is 2^(SF-7)
            int num_time_slots = (int)pow(2, (transmission->spreading_factor - 7));
            char source = transmission->source;
            char destination = transmission->destination;

            // Check if the transmission can be added to the schedule
            while (check_collision(scheduled_transmissions, *num_scheduled_transmissions, next_available_slot[channel_index], num_time_slots, source, destination, -1)) {
                next_available_slot[channel_index] += 1;
            }

            // Add the transmission to the schedule
            ScheduledTransmission new_scheduled_transmission;
            new_scheduled_transmission.channel_index = channel_index;
            new_scheduled_transmission.time_slot = next_available_slot[channel_index];
            new_scheduled_transmission.transmission = *transmission;

            // Reallocate memory if needed

            if (*num_scheduled_transmissions >= max_schedule_size) {
                max_schedule_size *= 2;
                scheduled_transmissions = realloc(scheduled_transmissions, sizeof(ScheduledTransmission) * max_schedule_size);
            }

            // Add the new scheduled transmission to the array
            /*
            (*scheduled_transmissions)[*num_scheduled_transmissions].channel_index = new_scheduled_transmission.channel_index;
            (*scheduled_transmissions)[*num_scheduled_transmissions].time_slot = new_scheduled_transmission.time_slot;
            (*scheduled_transmissions)[*num_scheduled_transmissions].transmission.source = new_scheduled_transmission.transmission.source;
            (*scheduled_transmissions)[*num_scheduled_transmissions].transmission.destination = new_scheduled_transmission.transmission.destination;
            (*scheduled_transmissions)[*num_scheduled_transmissions].transmission.spreading_factor = new_scheduled_transmission.transmission.spreading_factor;
            */
            (scheduled_transmissions)[*num_scheduled_transmissions] = new_scheduled_transmission;

            (*num_scheduled_transmissions)++;

            // Update the next available time slot for the channel
            next_available_slot[channel_index] += num_time_slots;
        }
    }
}





bool is_duplicate(ScheduledTransmission *primary_scheduled_transmissions, int num_primary_scheduled_transmissions, ScheduledTransmission transmission) {
    for (int i = 0; i < num_primary_scheduled_transmissions; i++) {
        if (primary_scheduled_transmissions[i].transmission.source == transmission.transmission.source &&
            primary_scheduled_transmissions[i].transmission.destination == transmission.transmission.destination &&
            primary_scheduled_transmissions[i].transmission.spreading_factor == transmission.transmission.spreading_factor) {
            return true;
        }
    }
    return false;
}

void merge_schedules(ScheduledTransmission *primary_scheduled_transmissions, int *num_primary_scheduled_transmissions,
                     ScheduledTransmission *secondary_scheduled_transmissions, int num_secondary_scheduled_transmissions) {

    int max_schedule_size = *num_primary_scheduled_transmissions + num_secondary_scheduled_transmissions;
    primary_scheduled_transmissions = realloc(primary_scheduled_transmissions, sizeof(ScheduledTransmission) * max_schedule_size);

    int next_available_slot[MAX_CHANNELS];
    for (int i = 0; i < MAX_CHANNELS; i++) {
        next_available_slot[i] = 0;
    }

    // Find the next available time slots
    for (int i = 0; i < *num_primary_scheduled_transmissions; i++) {
        int channel_index = primary_scheduled_transmissions[i].channel_index;
        int time_slot = primary_scheduled_transmissions[i].time_slot;
        int spreading_factor = primary_scheduled_transmissions[i].transmission.spreading_factor;
        int duration = (int)pow(2, (spreading_factor - 7));
        if (time_slot + duration > next_available_slot[channel_index]) {
            next_available_slot[channel_index] = time_slot + duration;
        }
    }

    // Iterate through the secondary schedule and merge
    for (int i = 0; i < num_secondary_scheduled_transmissions; i++) {
        ScheduledTransmission transmission = secondary_scheduled_transmissions[i];

        if (!is_duplicate(primary_scheduled_transmissions, *num_primary_scheduled_transmissions, transmission)) {
            int channel_index = transmission.channel_index;
            int spreading_factor = transmission.transmission.spreading_factor;
            int duration = (int)pow(2, (spreading_factor - 7));
            char source = transmission.transmission.source;
            char destination = transmission.transmission.destination;

            // Find a suitable time slot in the primary schedule for this transmission
            while (check_collision(primary_scheduled_transmissions, *num_primary_scheduled_transmissions, next_available_slot[channel_index], duration, source, destination, -1)) {
                next_available_slot[channel_index]++;
            }

            // Add the transmission to the primary schedule
            transmission.time_slot = next_available_slot[channel_index];
            primary_scheduled_transmissions[(*num_primary_scheduled_transmissions)++] = transmission;

            // Update the next available time slot for the channel
            next_available_slot[channel_index] += duration;
        }
    }
}


double calculate_efficiency_score(ScheduledTransmission *scheduled_transmissions, int num_scheduled_transmissions) {
    // Count the total number of time slots used
    int total_time_slots_used = 0;

    // Count the total data transmitted
    int total_data_transmitted = 0;

    // Determine the maximum time slot used for calculating utilization
    int max_time_slot = 0;

    // Keep track of total channels used
    int total_channels_used = 0;
    //int channel_counter[MAX_CHANNELS] = {0};
    int channel_counter[MAX_CHANNELS];
    for (int i = 0; i < MAX_CHANNELS; i++) {
        channel_counter[i] = 0;
    }


    // Iterate through each scheduled transmission
    for (int i = 0; i < num_scheduled_transmissions; i++) {
        ScheduledTransmission transmission = scheduled_transmissions[i];

        // Count the data transmitted in this transmission
        int spreading_factor = transmission.transmission.spreading_factor;
        total_data_transmitted += (int)pow(2, (spreading_factor - 7));

        // Count the time slots used for this transmission
        int start_time_slot = transmission.time_slot;
        int duration = (int)pow(2, (spreading_factor - 7));
        total_time_slots_used += duration;

        // Update the maximum time slot used
        if (start_time_slot + duration > max_time_slot) {
            max_time_slot = start_time_slot + duration;
        }

        // Count channels
        int channel_index = transmission.channel_index;
        if (channel_counter[channel_index] == 0) {
            total_channels_used++;
            channel_counter[channel_index] = 1;
        }
    }

    // Calculate the total number of timeslots available
    int total_time_slots_available = max_time_slot * total_channels_used;

    // Calculate utilization (percentage of timeslots used)
    double utilization = total_time_slots_available > 0 ? (double)total_time_slots_used / total_time_slots_available : 0;

    // Calculate throughput (total data transmitted per time slot)
    double throughput = total_time_slots_used > 0 ? (double)total_data_transmitted / total_time_slots_used : 0;

    // Calculate the efficiency score based on utilization and throughput
    double efficiency_score = 0.5 * utilization + 0.5 * throughput;

    return efficiency_score;
}




