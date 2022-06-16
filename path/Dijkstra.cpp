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

int movePtrToNextNumericChara(const char * txt, const char * lastTxt)
{
    while(*txt == ' ' && lastTxt != txt)
        txt++;
    if lastTxt ==
}

int extractValue(const char * txt, const char * lastTxt, char separator)
{

}

int extractFirstInBeforeSepOrSpace(const char * txt, const char * lastTxt, char separator)
{
    std::string numberTxt;
    int number;
    while(*txt != separator && *txt != ' ')
    {
        numberTxt += *txt;
        if(txt == lastTxt)
            break;
    }
    number = std::stoi(numberTxt);
    return number;
}

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
        node.id = extractFirstInBeforeSep();

        node.id = extractFirstInBeforeSepOrSpace(line, lastChr, ',');
    }
}