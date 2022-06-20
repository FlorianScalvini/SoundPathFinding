//
// Created by ubuntu on 20/06/22.
//

#include "pathFinding.h"
#include <queue>
#include <algorithm>

PathFinding::PathFinding(Graph* graph)
{
    this->graph = graph;
}


void PathFinding::computeBFS(unsigned int src, unsigned int dst)
{
    markerList.clear();
    std::set<unsigned int> reversePath;
    std::map<unsigned int, unsigned int> activateLink;
    std::queue<Node*> queue;
    bool foundDst = false;

    queue.push(graph->getNode(src));
    while(!queue.empty())
    {
        Node* curr = queue.front();
        queue.pop();
        if(curr->label == dst)
        {
            foundDst = true;
            break;
        }
        for(auto node : curr->links)
        {
            if(activateLink.find(node->label) == activateLink.end())
            {
                activateLink[node->label] = curr->label;
                queue.push(node);
            }
        }
    }
    if(foundDst) {
        unsigned int indicePath = dst;
        markerList.push_back(dst);
        while (indicePath != src) {
            indicePath = activateLink[indicePath];
            markerList.push_back(indicePath);
        }
        std::reverse(markerList.begin(), markerList.end());
        this->currPosition = 0;
    }
}

Node* PathFinding::getCurrentNode()
{
    if(this->currPosition >= 0)
        return  this->graph->getNode(markerList[this->currPosition]);
    else
        return nullptr;
}

int PathFinding::newPath(unsigned int src, unsigned int dst)
{
    computeBFS(src, dst);
    if(this->markerList.size() <= 0)
    {
        return this->markerList.size();
    }
    else
        return 0;
}


bool PathFinding::changeCurrentNodeTo(unsigned int label)
{
    bool result = false;
    unsigned int i = currPosition;
    while(i < markerList.size())
    {
        if(label == markerList[i])
        {
            result = true;
            currPosition = i;
            break;
        }
        i++;
    }

    return result;
}


Node* PathFinding::update()
{
    this->currPosition++;
    return this->getCurrentNode();
}

