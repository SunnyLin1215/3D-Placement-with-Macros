#include "Parser.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iostream>                /*sunny modified*/
#include <string>


std::unordered_map<std::string, Pin *> strToPin;

void Parser::readHardblock(std::string const &filename)
{
    std::ifstream fin(filename);
    std::string buff;
    std::string tmp;         /*sunny modified*/
    while (std::getline(fin, buff))
    {
        if (buff.empty())
            continue;

        std::stringstream buffStream(buff);
        std::string name;
        buffStream >> name;

        /*sunny modified*/
        if (name == "DieArea")
        {
            buffStream >> tmp;
            buffStream >> DieX;
            buffStream >> DieY;
            std::cout << "Die length: " << DieX << " " << DieY << std::endl;

        }
        /*End*/

        if (name == "NumHardRectilinearBlocks" ||
            name == "NumTerminals")
            continue;

        std::string identifier;
        buffStream >> identifier;
        if (identifier == "hardrectilinear")
        {
            std::string hardblockInfo;
            std::getline(buffStream, hardblockInfo);
            int x[4], y[4], pin_num, dx, dy, rotate;

            /*sunny modified add second last %d for rotate*/
            std::sscanf(hardblockInfo.c_str(), " 4 (%d, %d) (%d, %d) (%d, %d) (%d, %d) %d %d",
                        x, y, x + 1, y + 1, x + 2, y + 2, x + 3, y + 3, &rotate, &pin_num);
            auto width = *std::max_element(x, x + 4) - *std::min_element(x, x + 4);
            auto height = *std::max_element(y, y + 4) - *std::min_element(y, y + 4);
            
            if(rotate == 1 || rotate == 3) {
                int temp = width;
                width = height;
                height = temp;
            }
            Hardblock* h = new Hardblock(name, width, height, 0, 0, rotate);      /*add xy sunny modified*/

            for( int i = 0; i < pin_num; i++) 
            {
                std::string PinInfo, pin_name;
                std::getline(fin, PinInfo);
                std::stringstream pinStream(PinInfo);
                pinStream >> pin_name;
                pinStream >> dx;
                pinStream >> dy;

                h->pin.emplace_back(new Pin(name, 0, 0, dx, dy));
                strToPin.emplace(name+"P"+ std::to_string(i+1), h->pin.back());
            }
            hardblocks.emplace_back(h);   /*add xy sunny modified*/
        }
        /*sunny modified*/
        else if(identifier == "stdcell")
        {
            std::string hardblockInfo;
            int x[4], y[4], pin_num, dx, dy, rotate;

            std::getline(buffStream, hardblockInfo);

            /*sunny modified add second last %d for rotate*/
            std::sscanf(hardblockInfo.c_str(), " 4 (%d, %d) (%d, %d) (%d, %d) (%d, %d) %d %d",
                        x, y, x + 1, y + 1, x + 2, y + 2, x + 3, y + 3, &rotate, &pin_num);
            auto width = *std::max_element(x, x + 4) - *std::min_element(x, x + 4);
            auto height = *std::max_element(y, y + 4) - *std::min_element(y, y + 4);
            
            if(rotate == 1 || rotate == 3) {
                int temp = width;
                width = height;
                height = temp;
            }
            
            Hardblock* h = new Hardblock(name, width, height, x[0], y[0], rotate);      /*add xy sunny modified*/

            for( int i = 0; i < pin_num; i++) 
            {
                std::string PinInfo, pin_name;
                std::getline(fin, PinInfo);
                std::stringstream pinStream(PinInfo);

                pinStream >> pin_name;
                pinStream >> dx;
                pinStream >> dy;

                h->pin.emplace_back(new Pin(name, x[0] + dx, y[0] + dy, dx, dy));
                strToPin.emplace(name+"P"+ std::to_string(i+1), h->pin.back());
            }

            stdcells.emplace_back(h);   /*add xy sunny modified*/
        }
        /*End*/
    }
}

void Parser::readPl(std::string const &filename)
{
    std::ifstream fin(filename);
    std::string name;
    int x, y;
    while (fin >> name >> x >> y)
        strToPin.emplace(name, new Pin(name, x, y, 0, 0));
}

void Parser::readNet(std::string const &filename)
{
    std::ifstream fin(filename);
    std::string identifier;
    while (fin >> identifier)
    {
        if (identifier == "NetDegree")
        {
            nets.emplace_back(new Net());
            std::string temp;
            size_t pinNum;
            fin >> temp >> pinNum;
            for (size_t i = 0; i < pinNum; ++i)
            {
                std::string inst_name, pin_name;
                fin >> inst_name;
                if(inst_name[0] == 'T')
                {
                    nets.back()->pins.emplace_back(strToPin.at(inst_name));
                }
                else
                {
                    fin >> pin_name;
                    nets.back()->pins.emplace_back(strToPin.at(inst_name+pin_name));
                }
            }
        }
    }
}

SAInput *Parser::parse(char *argv[])
{
    readHardblock(argv[1]);
    std::cout << "finish read hardblocks" << std::endl; 
    readPl(argv[3]);
    std::cout << "finish read pl" << std::endl; 
    readNet(argv[2]);
    std::cout << "finish read nets" << std::endl; 

    /*sunny modified*/
    return new SAInput(hardblocks, stdcells, nets, std::stod(argv[5]), DieX, DieY);
}