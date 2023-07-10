# LoRa_Node_Stml073rz Scheduling

Python simulation tool : Scheduling_test.ipynb

C implementation : mostly in scheduler.h scheduler.c


# Problem Statement:

The problem at hand is to schedule transmissions in a wireless network in a way that minimizes interference and maximizes network throughput. The network is represented as a graph, where nodes represent devices and edges represent connections between devices. Each edge is associated with a spreading factor (SF), which determines the duration of a transmission along that edge. The challenge is to schedule the transmissions in such a way that no two transmissions involving the same node overlap in time, and no two transmissions that could interfere with each other are scheduled at the same time.

Proposed Method:

Our approach to solving this problem is a distributed scheduling method with a minimum overhead. The method is based on the concept of a dominating set in graph theory. A dominating set is a subset of nodes such that every node in the graph is either in the set or is a neighbor of a node in the set.

The steps of our method are as follows:

Compute a dominating set of the network. Each node in the dominating set will be responsible for scheduling its own transmissions and the transmissions of its neighbors.

Each node in the dominating set creates a list of all possible transmissions involving itself and its neighbors. The list is sorted by spreading factor in descending order, and paired transmissions are placed next to each other.

Each node in the dominating set then creates a schedule for its transmissions. The schedule is organized into channels, with the first channel for transmissions involving the node itself, the second channel for transmissions involving no dominant nodes, and the remaining channels each dedicated to transmissions involving a known dominant node. It then calculates an efficiency score, based on the utilization and throughput of its channels and timeslots.

The dominant nodes share their efficiency score to other dominant nodes to compare, and the node with the higher score will be responsible for merging the schedules.
The schedules created by each node in the dominating set are then merged to create a global schedule for the network. The merging process involves merging the channels corresponding to the same dominant node, deleting duplicate transmissions, and resolving conflicts.

After the global schedule is computed, the dominant nodes propagate specific schedules to neighboring nodes : only the scheduling information concerning each node.

This method ensures that all transmissions are scheduled in a way that minimizes interference and maximizes network throughput. The use of a dominating set reduces the complexity of the scheduling problem by dividing it into smaller, more manageable subproblems.

We are using the STM32CubeIDE framework, aswell as the Stm32L073RZ board equipped with the LoRa RA-01 module for this project.

# Project status

Currently the scheduling functions are implemented according to the method described above and simulated in the .ipynb file. They have been debugged but yet to be tested.

The implementation of an optimization function for the merged schedules is in progress.

Currently the focus is on the payload/packet formatting for the exchange of efficiency scores and schedules, a beginning of implementation can be found in "payload.c", the problematic is to optimize the packet size, and the exchange protocol.
