# LoRa_Node_Stml073rz

Problem Statement:
The problem at hand is to schedule transmissions in a wireless network in a way that minimizes interference and maximizes network throughput. The network is represented as a graph, where nodes represent devices and edges represent connections between devices. Each edge is associated with a spreading factor (SF), which determines the duration of a transmission along that edge. The challenge is to schedule the transmissions in such a way that no two transmissions involving the same node overlap in time, and no two transmissions that could interfere with each other are scheduled at the same time.

Proposed Method:
Our approach to solving this problem is a distributed scheduling method with a minimum overhead. The method is based on the concept of a dominating set in graph theory. A dominating set is a subset of nodes such that every node in the graph is either in the set or is a neighbor of a node in the set.

The steps of our method are as follows:

Compute a dominating set of the network. Each node in the dominating set will be responsible for scheduling its own transmissions and the transmissions of its neighbors.

Each node in the dominating set creates a list of all possible transmissions involving itself and its neighbors. The list is sorted by spreading factor in descending order, and paired transmissions are placed next to each other.

Each node in the dominating set then creates a schedule for its transmissions. The schedule is organized into channels, with the first channel for transmissions involving the node itself, the second channel for transmissions involving no dominant nodes, and the remaining channels each dedicated to transmissions involving a known dominant node.

The schedules created by each node in the dominating set are then merged to create a global schedule for the network. The merging process involves merging the channels corresponding to the same dominant node, deleting duplicate transmissions, and resolving conflicts.

The final schedule is then visualized using a graphical representation, where each channel is represented vertically, each time slot is represented horizontally, and transmissions are represented as colored rectangles.

This method ensures that all transmissions are scheduled in a way that minimizes interference and maximizes network throughput. The use of a dominating set reduces the complexity of the scheduling problem by dividing it into smaller, more manageable subproblems. The use of channels further organizes the schedule and makes it easier to manage and visualize.
