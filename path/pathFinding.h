//
// Created by ubuntu on 20/06/22.
//

#ifndef STAG_PATHFINDING_H
#define STAG_PATHFINDING_H

#include "graph.h"


class PathFinding {
public:
    explicit PathFinding(Graph* graph);
    int newPath(unsigned int src, unsigned int dst);
    Node* update();
    Node* getCurrentNode();
    bool changeToClosestNode(unsigned int label);

private:
    std::vector<unsigned int> markerList;
    unsigned int currPosition;
    void computeBFS(unsigned int src, unsigned int dst);
    Graph *graph;
};


#endif //STAG_PATHFINDING_H
