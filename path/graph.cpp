//
//  Graph.cpp
//  ddd
//
//  Created by Florian Scalvini on 19/06/2022.
//

#include "graph.h"
#include <iostream>
#include <fstream>
#include <queue>

Graph::Graph(std::vector<int> listId)
{
    this->numNode = listId.size();
    for(int i = 0; i < numNode; i++)
    {
        this->nodes.insert(std::pair<int, Node*>(listId[0], new Node(listId[0])));
    }
}

Graph::Graph(const char * path)
{
    std::ifstream infile(path);
    std::string line;
    std::vector<std::pair<unsigned int, std::vector<int>>> link;

    std::vector<Node> graph;
    if (!infile.is_open()) {
        std::cerr << "Could not open the file - '" << path << "'" << std::endl;
        return;
    }
    int id;

    while(std::getline(infile, line))
    {
        char * lastChr = &line[line.size() -1];
        char * ptrLine = &line[0];
        id = extractValue(&ptrLine, lastChr);
        this->nodes.insert(std::pair<int, Node*>(id, new Node(id)));
        ptrLine++;
        link.emplace_back(id, extractListValue(&ptrLine, ',', lastChr));
        ptrLine++;
        this->nodes[id]->classe = extractValue(&ptrLine, lastChr);
    }
    this->numNode = link.size();
    for(unsigned int i = 0; i < numNode; i++)
    {
        for(unsigned int j=0; j < link[i].second.size(); j++)
        {
            addLink(link[i].first, link[i].second[j]);
        }
    }
}

Graph::~Graph()
{
}

Node* Graph::getNode(unsigned int indice)
{
    if(this->nodes.find(indice) != this->nodes.end())
        return nodes[indice];
    else
        return nullptr;
}

void Graph::addLink(unsigned int src, int dst)
{
    Node* srcNode = this->getNode(src);
    Node* dstNode = this->getNode(dst);
    if(srcNode != nullptr && dstNode != nullptr )
    {
        this->nodes[src]->links.insert(dstNode);
    }
    else
    {
        std::cerr<<"Impossible to build the connexion between the Node "<< src << " and" << dst << ".";
    }
}

void Graph::showGraph()
{
    for(auto node : this->nodes)
    {
        std::cout<<node.first << ":";
        for (auto neighbors : node.second->links)
        {
            std::cout<<" "<<neighbors->label;
        }
        std::cout<<"."<<std::endl;
    }
}


std::vector<int> Graph::extractListValue( char ** txt,  char separator, const char * lastTxt)
{
    std::vector<int> result;
    while(**txt != separator)
    {
        result.push_back(extractValue(txt, lastTxt));
    }
    return result;
}

void Graph::skipSpace( char ** txt, const char * lastTxt)
{
    while(txt != &lastTxt && **txt == ' ')
        *txt +=1;
}

int Graph::extractValue(char ** txt, const char * lastTxt) {
    std::string numberString;
    int number;
    skipSpace(txt, lastTxt);
    while (txt != &lastTxt && **txt >= 48 && **txt <= 57)
    {
        numberString += **txt;
        *txt += 1;
    }
    skipSpace(txt, lastTxt);
    number = std::stoi(numberString);
    return number;
}