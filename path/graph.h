//
//  Graph.hpp
//  ddd
//
//  Created by Florian Scalvini on 19/06/2022.
//

#ifndef Graph_h
#define Graph_h

#include <stdio.h>
#include <map>
#include <set>
#include <vector>

struct Node
{
    unsigned int label;
    int classe;
    std::set<Node*> links;
    explicit Node(unsigned int label, int classe = 0)
    {
        this->label = label;
        this->classe = classe;
    }
};

class Graph {
public:
    explicit Graph(std::vector<int> listId);
    explicit Graph(const char * path);
    ~Graph();
    Node* getNode(unsigned int indice);
    void addLink(unsigned int src, int dst);
    void showGraph();

private:
    unsigned int numNode;
    std::map<unsigned int, Node*> nodes;
    static void skipSpace( char ** txt, const char * lastTxt);
    static int extractValue( char ** txt, const char * lastTxt);
    static std::vector<int> extractListValue( char ** txt,  char separator, const char * lastTxt);
};

#endif /* Graph_h */
