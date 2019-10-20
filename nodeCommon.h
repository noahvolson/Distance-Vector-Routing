//
// Created by Noah Olson on 10/5/19.
//
#include "project3.h"

#ifndef PROJECT_3_NODECOMMON_H
#define PROJECT_3_NODECOMMON_H



struct distance_table {
    int costs[MAX_NODES][MAX_NODES];
};

struct NeighborCosts* initNode (int nodeID, struct NeighborCosts *neighbor, struct distance_table *dt);
void updateNode (int nodeID, struct NeighborCosts *neighbor, struct RoutePacket *rcvdpkt, struct distance_table *dt);
void toNeighbors (int nodeID, struct NeighborCosts *neighbor, struct distance_table *dt);
int min(const int *array, int size);
void bellUpdate(int source, int dest, struct distance_table *dt);
void printdt( int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr );


#endif //PROJECT_3_NODECOMMON_H

