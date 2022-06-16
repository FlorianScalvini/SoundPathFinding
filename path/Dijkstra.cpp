//
// Created by ubuntu on 16/06/22.
//

#include "Dijkstra.h"
#include <fstream>
#include <string>
#include <iostream>


/*
 *  Format fichier .txt : ID, List of links,
 */



void skipSpace(const char * txt, const char * lastTxt);
int extractValue(const char * txt, char separator, const char * lastTxt);
std::vector<int> extractListValue(const char * txt,  char separator, const char * lastTxt);




PathFinding::PathFinding(const char* path)
{
    std::ifstream infile(path);
    std::string line;

    std::string numbersChars = "";
    int index;
    std::vector<int> links;

    if (!infile.is_open()) {
        std::cerr << "Could not open the file - '" << path << "'" << std::endl;
        return;
    }
    while(std::getline(infile, line))
    {
        Node node;
        char * lastChr = &line[line.size() -1];
        char * ptrLine = &line[0];
        node.id = extractValue(ptrLine, ',', lastChr);
        node.specialAttribut = extractValue(ptrLine + 1, ',', lastChr);
        node.links = extractListValue(ptrLine + 1, ',', lastChr);
        graph.push_back(node);
    }
}



std::vector<int> extractListValue(const char * txt,  char separator, const char * lastTxt)
{
    std::vector<int> result;
    while(*txt != separator)
    {
        skipSpace(txt, lastTxt);
        result.push_back(extractValue(txt, separator, lastTxt));
    }
    return result;
}

void skipSpace(const char * txt, const char * lastTxt)
{
    while(txt != lastTxt && *txt == ' ')
        txt++;
}

int extractValue(const char * txt, char separator, const char * lastTxt) {
    std::string numberString;
    int number;
    while (txt != lastTxt && *txt != ' ' && *txt != separator)
    {
        numberString += *txt;
        txt++;
    }
    number = std::stoi(numberString);
    return number;
}