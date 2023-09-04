#include "ResultWriter.hpp"
#include <fstream>
#include <limits>
#include <iostream>

void ResultWriter::write(std::string const &filename)
{
    std::ofstream fout(filename);
    int wirelength = 0;
    for (auto hardblock : input->hardblocks)
    {
        int rotate = hardblock->isRotated;
        for( auto pin : hardblock->pin)
        {
            if(rotate == 0)
            {
                pin->x = hardblock->x + pin->dx;
                pin->y = hardblock->y + pin->dy;
            }
            else if( rotate == 1) 
            {
                /* (pinx, piny) = (upper_left_x + dy, upper_left_y - dx) */
                pin->x = hardblock->x + pin->dy;
                pin->y = hardblock->y + hardblock->height - pin->dx;
            }
            else if( rotate == 2)
            {
                /* (pinx, piny) = (upper_right_x - dx, upper_right_y - dy)*/
                pin->x = hardblock->x + hardblock->width - pin->dx;
                pin->y = hardblock->y + hardblock->height - pin->dy;
                
            }
            else
            {
                /* (pinx, piny) = (lower_right_x - dy, lower_right_y + dx)*/
                pin->x = hardblock->x + hardblock->width - pin->dy;
                pin->y = hardblock->y + pin->dx;
            }
        }
    }
    
    for (auto const &net : input->nets)
        wirelength += net->HPWL();

    fout << "Wirelength " << wirelength << '\n'
         << "Blocks\n";
    for (auto const &hardblock : input->hardblocks)
        fout << hardblock->name << ' '
             << hardblock->x << ' '
             << hardblock->y << ' '
             << hardblock->isRotated << '\n';
}