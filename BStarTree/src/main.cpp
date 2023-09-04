#include "GlobalTimer/GlobalTimer.hpp"
#include "Parser/Parser.hpp"
#include "Solver/Solver.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cerr << "Usage: " << argv[0] << " <hardblock file> <net file> <pl file> <floorplan file> <deadspace ratio>\n";
        return 0;
    }
    srand(time(NULL));

 
    Parser parser;
    auto input = parser.parse(argv);

    /*sunny modified*/
    /*GlobalTimer* globalTimer;
    if(input->hardblocks.size() < 5)  globalTimer = new GlobalTimer(20 - 5);
    else if(input->hardblocks.size() < 10) globalTimer = new GlobalTimer(30 - 5);
    else globalTimer = new GlobalTimer(1 * 60 - 5);*/
    
    GlobalTimer globalTimer(10 * 60 - 5);    /*sunny modified*/
    globalTimer.startTimer("runtime");
    globalTimer.startTimer("parse input");       
    // input->initialSeed = std::stoi(argv[6]);
    std::cerr<<"initialSeed: "<<input->initialSeed<<"\n";
    globalTimer.stopTimer("parse input");
    globalTimer.startTimer("SA process");

    SA_Solver solver(input, globalTimer);
    auto result = solver.solve();

    globalTimer.stopTimer("SA process");
    globalTimer.startTimer("write output");

    result->write(argv[4]);

    globalTimer.stopTimer("write output");
    globalTimer.stopTimer("runtime");

    // globalTimer.printTime("parse input");
    // globalTimer.printTime("SA process");
    // globalTimer.printTime("write output");
    globalTimer.printTime("runtime");
    return 0;
}