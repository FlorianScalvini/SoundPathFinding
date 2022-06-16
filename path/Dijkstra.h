//
// Created by ubuntu on 16/06/22.
//

#ifndef PATH_DIJKSTRA_H
#define PATH_DIJKSTRA_H

#include <vector>
struct Node
{
    int id;
    std::vector<int> links;
};

class PathFinding {
public:
    PathFinding(const char* path);
    void calculatePath();
private:
    std::vector<int> path;
    std::vector<Node> graph;
};


#endif //PATH_DIJKSTRA_H
