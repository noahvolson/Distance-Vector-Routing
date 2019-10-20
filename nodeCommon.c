//
// Created by Noah Olson on 10/5/19.
//

#include "nodeCommon.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "project3.h"

//init: initialize neighborcosts using getneighborcosts function
//      initialize number of nodes (assuming you have)
//      initialize distance table (loop through, set all to 9999)
//      send updates to neighbors for first time initialization
struct NeighborCosts* initNode (int nodeID, struct NeighborCosts *neighbor, struct distance_table *dt){
    printf("At time: %4.3f rtinit%d called \n", clocktime, nodeID);

    //initialize the costs of all neighbors
    neighbor = getNeighborCosts(nodeID);

    //initialize node row to neighbor costs, all other rows to infinity
    int r;
    int c;

    for (r = 0; r < neighbor->NodesInNetwork; r++) {
        for (c = 0; c < neighbor->NodesInNetwork; c++){

            if(r == nodeID) {
                dt->costs[r][c] = neighbor->NodeCosts[c];
            }
            else{
                dt->costs[r][c] = INFINITY;
            }
        }
    }

    //send initial configuration of this node to others
    toNeighbors(nodeID, neighbor, dt);

    return neighbor;
}


//updt: accepts a route packet, (src id, dest id, array) ***ONLY SEND UPDATE IF SOMETHING IS CHANGED***
//      compare row to internal distance table (check row of source ID)
//      now we have fixed that row
//      run bellman ford on all other entries
void updateNode (int nodeID, struct NeighborCosts *neighbor, struct RoutePacket *rcvdpkt, struct distance_table *dt) {
    printf("At time: %4.3f rtupdate%d called \n", clocktime, nodeID);
    printf("rtupdate%d received packet from node %d \n", nodeID, rcvdpkt->sourceid);

    //initial configuration to fill out table, then use bellman to sort out internally
    int i;
    for (i = 0; i < MAX_NODES; i++){
        if (rcvdpkt->mincost[i] < dt->costs[rcvdpkt->sourceid][i]){
            dt->costs[rcvdpkt->sourceid][i] = rcvdpkt->mincost[i];
        }
    }


    //update all nodes using bellman ford algorithm
    int r;
    int c;
    int x; // temp variable used to detect changes in nodeID row
    bool changed = false;


    for (r = 0; r < MAX_NODES; r++){
        for (c = 0; c < MAX_NODES; c++){

            x = dt->costs[r][c];    //initial value in table

            bellUpdate(r, c, dt);   //try to update it with the bellman ford algorithm

            if (x != dt->costs[r][c]){   //if this has changed, and we are in row of current node, update changed boolean
                changed = true;
            }
        }
    }

    //TODO update changed?
    for (r = 0; r < MAX_NODES; r++){
        for(c = 0; c < MAX_NODES; c++){


            if (dt->costs[r][c] < dt->costs[c][r]){     //because bi-directional consider diagonal reflection
                dt->costs[c][r] = dt->costs[r][c];
                changed = true;
            }
            if (dt->costs[c][r] < dt->costs[r][c]){    //now reverse
                dt->costs[r][c] = dt->costs[c][r];
                changed = true;
            }
            if (r == c) {                   //if we send to self, cost is 0
                if (dt->costs[r][c] != 0){
                    changed = true;
                }
                dt->costs[r][c] = 0;
            }

        }
    }


    //only need to send to neighbors if we have updated shortest path
    if (changed) {
        toNeighbors(nodeID, neighbor, dt);
    }

}


//send to neighbor function
//called by both updt and init
// makes packet, loop through neighbors set dest id = specific neighbor, send on layer 2
void toNeighbors (int nodeID, struct NeighborCosts *neighbor, struct distance_table *dt) {

    //create a new packet
    struct RoutePacket *routePacket;
    routePacket = (struct RoutePacket*) malloc(sizeof(struct RoutePacket));

    //source ID = node you send from = this node ID
    routePacket->sourceid = nodeID;

    int i;

    //initialize data to row of this node ID
    for(i = 0; i < MAX_NODES; i ++) {
        routePacket->mincost[i] = dt->costs[nodeID][i];
    }


    //send state to reachable neighbors
    for (i = 0; i < neighbor->NodesInNetwork; i++) {
        if (i != nodeID && neighbor->NodeCosts[i] < INFINITY) {    //don't send to self or unreachable nodes
            routePacket->destid = i;
            toLayer2(*routePacket);
            printf("Packet Sent at: %4.3f from: %d to: %d with contents: %d %d %d %d \n", clocktime, nodeID, i,
                    routePacket->mincost[0], routePacket->mincost[1], routePacket->mincost[2], routePacket->mincost[3]);
        }
    }

}

//Find the minimum of an array
int min (const int *array, int size){

    int min = array[0];

    int i;
    for(i = 1; i < size; i++){
        if (array[i] < min){
            min = array[i];
        }
    }

    return min;
}


//Bellman ford algorithm to update each element in distance table
void bellUpdate(int source, int dest, struct distance_table *dt){
    int costs[MAX_NODES];

    int i;
    for (i = 0; i < MAX_NODES; i++){
        costs[i] = dt->costs[source][i] + dt->costs[i][dest];
    }
    int calcMin = min(costs, MAX_NODES);

    if (calcMin < dt->costs[source][dest]){
        dt->costs[source][dest] = calcMin;
    }
}

/////////////////////////////////////////////////////////////////////
//  printdt
//  This routine is being supplied to you.  It is the same code in
//  each node and is tailored based on the input arguments.
//  Required arguments:
//  MyNodeNumber:  This routine assumes that you know your node
//                 number and supply it when making this call.
//  struct NeighborCosts *neighbor:  A pointer to the structure
//                 that's supplied via a call to getNeighborCosts().
//                 It tells this print routine the configuration
//                 of nodes surrounding the node we're working on.
//  struct distance_table *dtptr: This is the running record of the
//                 current costs as seen by this node.  It is
//                 constantly updated as the node gets new
//                 messages from other nodes.
/////////////////////////////////////////////////////////////////////
void printdt( int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr ) {
    int       i, j;
    int       TotalNodes = neighbor->NodesInNetwork;     // Total nodes in network
    int       NumberOfNeighbors = 0;                     // How many neighbors
    int       Neighbors[MAX_NODES];                      // Who are the neighbors

    // Determine our neighbors
    for ( i = 0; i < TotalNodes; i++ )  {
        if (( neighbor->NodeCosts[i] != INFINITY ) && i != MyNodeNumber )  {
            Neighbors[NumberOfNeighbors] = i;
            NumberOfNeighbors++;
        }
    }
    // Print the header
    //printf("                via     \n");
    printf("   D%d |", MyNodeNumber );
    for ( i = 0; i < MAX_NODES; i++ )
        printf(" dst %d", i);
    printf("\n");
    printf("  ----|-------------------------------\n");

    // For each node, print the cost by travelling thru each of our neighbors
    for ( i = 0; i < MAX_NODES; i++ )   {
        //if ( i != MyNodeNumber )  {
        printf("src %d |", i );
        for ( j = 0; j < MAX_NODES; j++ )  {
            printf( "  %4d", dtptr->costs[i][j] );
        }
        printf("\n");
        //}
    }
    printf("\n");
}


