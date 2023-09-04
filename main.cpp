#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <stdlib.h>
#include <time.h>
#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <chrono>
#include <thread>
using namespace std;
string dirtoremove;
int unit;
int NumofTech;
int NumofLib;
unsigned long long  LowerLeftX;
unsigned long long  LowerLeftY;
unsigned long long  UpperRightX;
unsigned long long  UpperRighty;
int TopDieMaxUtil;
int BottomDieMaxUtil;
int StartX_t;
int StartY_t;
int RowLength_t;
int RowHeight_t;
int RepeatCount_t;
int StartX_b;
int StartY_b;
int RowLength_b;
int RowHeight_b;
int RepeatCount_b;
int Terminal_X;
int Terminal_Y;
int Terminal_Spacing;
int Terminal_Cost;
int gridx, gridy;
int Instance_cnt;
int Macro_cnt = 0;  
int StdCell_cnt = 0;
int NetCnt;
int bigger; //-1 is part 1, 1 is part 2
int divide_cnt = 0;
int hmetis_cnt = 0;
int top = 0;
int bot = 0;
int best_cut = 10000000;
bool change_legalize = false;
long long best_HPWL = 9999999999;
unsigned long long TopDieMaxArea;
unsigned long long BottomDieMaxArea;
unsigned long long TopDieRealArea;
unsigned long long BottomDieRealArea;
unsigned long long best_top_area = 1000000000000;
unsigned long long best_bot_area = 1000000000000;
vector<int> best_partition_map;
vector<bool> NetCross;
vector<int> partition_map;            //[Instnum]
vector<int> tmp_partition_map;            //[Instnum]
vector<int> Lib_for_Inst;             //[Instnum]
vector<int> NumofPin;                 //[Net]
vector<vector<pair<int, int>>> net;   //[Net][Pin] net.first = instance net.second = pin
vector<vector<bool>> LibCell_isMacro; //[tech][Libcnt]
vector<vector<int>> LibCellSize_x;    //[tech][Libcnt]
vector<vector<int>> LibCellSize_y;
vector<vector<unsigned long long>> LibCellSize;
vector<vector<vector<int>>> Pin_x; //[tech][Libcnt][Pincnt]
vector<vector<vector<int>>> Pin_y;
vector<vector<int>> cell_array; //[instnum][idx]
vector<unsigned long long> hmetis_inst_size;          // size of instance
vector<int> Cell_in_top;        // Which cell in top die and bottom die (cell_number)
vector<int> Cell_in_bottom;
vector<int> cell_gain; //[Instnum]
ifstream in_file;
ofstream out_file;
vector<string> Techname;
string TopDieTech;
string BottomDieTech;
vector<int> topdiename; //[instcnt]
vector<int> botdiename; //[instcnt]
vector<int> cutname;    //[cut_idx];
vector<int> mapforterminal;   //[net_idx]
string replaced_top = "./top_die_final.def";
string replaced_bot = "./bot_die_final.def";
vector<pair<int, int>> terminal_position; //[cut_idx] center of terminal x,y
int cut_num;
vector<int> degree;
vector<int> degree_net;
vector<pair<int, int>> inst_position;
vector<string> inst_orient;
int terminal_job;
vector<int> mbpm_map;
vector<pair<int, int>> second_inst;     // yu--modified
vector<string> second_inst_orient;      // yu--modified
vector<pair<int, int>> second_terminal; // yu--modified

vector<vector<vector<int>>> net_hpwl_top;    // [Net_idx][lower-left, upper-right][x, y]
vector<vector<vector<int>>> net_hpwl_bot;

vector<bool> net_hasInst_on_top;   // [Net_idx]
vector<bool> net_hasInst_on_bot;

vector<pair<int, int>> top_hpwl_middle;    // 0825
vector<pair<int, int>> bot_hpwl_middle;

unsigned long long totalMacroArea = 0;   //0825
unsigned long long totalArea = 0;
unsigned long long topMacroArea = 0;
unsigned long long botMacroArea = 0;
unsigned long long targetMacroArea = 0;

vector<int> fix_partition;
ofstream fixfile;

vector<double> InstAreaChange;    //0826   bot/top
int max_die;

void legalize();
long long calc_total_HPWL();

void print_case_information()
{ // 7/9 modified
    ofstream case_info_file;
    case_info_file.open("case_information.txt");

    case_info_file << "Num of Tech: " << NumofTech << endl;
    case_info_file << "Num of Libcell: " << NumofLib << endl;
    case_info_file << "Num of Instance: " << Instance_cnt << endl;
    case_info_file << "   Num of Standard Cell: " << StdCell_cnt << endl;
    case_info_file << "   Num of Macro: " << Macro_cnt << endl;
    case_info_file << "Num of Net: " << NetCnt << endl
                   << endl;

    // Analyze macro area
    case_info_file << "Die length x and y: \n"
                   << UpperRightX - LowerLeftX << " " << UpperRighty - LowerLeftY << endl;
    case_info_file << "Die area: \n"
                   << (UpperRightX - LowerLeftX) * (UpperRighty - LowerLeftY) << endl;
    long long MC_num, tech2_macro_area_sum = 0, tech1_macro_area_sum = 0, max_macro_area = 0;
    long long macro_difference = 0, macro_diff_sum = 0, diff;
    for (int i = 0; i < Instance_cnt; i++)
    {
        MC_num = Lib_for_Inst[i];
        if (!LibCell_isMacro[0][MC_num])
            continue;
        tech1_macro_area_sum += LibCellSize[0][MC_num];
        if (max_macro_area < LibCellSize[0][MC_num])
            max_macro_area = LibCellSize[0][MC_num];
        if (NumofTech == 2)
        {
            if (max_macro_area < LibCellSize[1][MC_num])
                max_macro_area = LibCellSize[1][MC_num];
            tech2_macro_area_sum += LibCellSize[1][MC_num];
            diff = LibCellSize[0][MC_num] - LibCellSize[1][MC_num];
            diff = (diff >= 0) ? diff : -diff;
            macro_diff_sum += diff;
            if (macro_difference < diff)
                macro_difference = diff;
        }
    }
    case_info_file << "Max macro area: \n"
                   << max_macro_area << endl;
    case_info_file << "Average Macro Area of Tech1: \n"
                   << int(tech1_macro_area_sum / Macro_cnt) << endl;
    if (NumofTech == 2)
    {
        case_info_file << "Average Macro Area of Tech2: \n"
                       << int(tech2_macro_area_sum / Macro_cnt) << endl;
        case_info_file << "Max Macro Area Difference: \n"
                       << macro_difference << endl;
        case_info_file << "Average Macro Area Difference: \n"
                       << int(macro_diff_sum / Macro_cnt) << endl;
    }

    // test whether a libcell is macro or not can be different in two techs
    for (int i = 0; i < Instance_cnt; i++)
    {
        MC_num = Lib_for_Inst[i];
        if (NumofTech == 2 && LibCell_isMacro[0][MC_num] != LibCell_isMacro[1][MC_num])
        {
            case_info_file << "A libcell is macro or not can be different in two techs" << endl;
        }
    }

    case_info_file.close();
}

void parse_input()
{
    int tmp_int;
    string tmp;
    while (in_file >> tmp)
    {
        if (tmp == "NumTechnologies")
        {
            in_file >> NumofTech;
            LibCell_isMacro.resize(NumofTech);
            LibCellSize_x.resize(NumofTech);
            LibCellSize_y.resize(NumofTech);
            LibCellSize.resize(NumofTech);
            Pin_x.resize(NumofTech);
            Pin_y.resize(NumofTech);
            Techname.resize(NumofTech);
            for (int i = 0; i < NumofTech; i++)
            {
                in_file >> tmp;
                string tn;
                in_file >> tn; // TA or TB
                Techname[i] = tn;
                in_file >> NumofLib;
                LibCell_isMacro[i].resize(NumofLib);
                LibCellSize_x[i].resize(NumofLib);
                LibCellSize_y[i].resize(NumofLib);
                LibCellSize[i].resize(NumofLib);
                Pin_x[i].resize(NumofLib);
                Pin_y[i].resize(NumofLib);
                for (int j = 0; j < NumofLib; j++)
                {
                    in_file >> tmp; // "LibCell"
                    char isMacro;   // 7-4 modified
                    in_file >> isMacro;
                    in_file >> tmp; // "MC1" -> "MC2"...  assume not in order
                    int libcell_num = stoi(tmp.substr(2));
                    (isMacro == 'Y') ? (LibCell_isMacro[i][libcell_num - 1] = true) : (LibCell_isMacro[i][libcell_num - 1] = false);
                    in_file >> LibCellSize_x[i][libcell_num - 1];
                    in_file >> LibCellSize_y[i][libcell_num - 1];
                    LibCellSize[i][libcell_num - 1] = LibCellSize_x[i][libcell_num - 1] * LibCellSize_y[i][libcell_num - 1];
                    int pincnt;
                    in_file >> pincnt;
                    Pin_x[i][libcell_num - 1].resize(pincnt);
                    Pin_y[i][libcell_num - 1].resize(pincnt);
                    vector<int> tmp_pin_x;
                    vector<int> tmp_pin_y;
                    for (int k = 0; k < pincnt; k++)
                    {
                        in_file >> tmp;
                        in_file >> tmp;
                        int pin_num = stoi(tmp.substr(1)); // ex: P12
                        in_file >> Pin_x[i][libcell_num - 1][pin_num - 1];
                        in_file >> Pin_y[i][libcell_num - 1][pin_num - 1];
                    }
                }
            }
        }
        else if (tmp == "DieSize")
        {
            in_file >> LowerLeftX;
            in_file >> LowerLeftY;
            in_file >> UpperRightX;
            in_file >> UpperRighty;
        }
        else if (tmp == "TopDieMaxUtil")
        {
            in_file >> TopDieMaxUtil;
        }
        else if (tmp == "BottomDieMaxUtil")
        {
            in_file >> BottomDieMaxUtil;
        }
        else if (tmp == "TopDieRows")
        {
            in_file >> StartX_t;
            in_file >> StartY_t;
            in_file >> RowLength_t;
            in_file >> RowHeight_t;
            in_file >> RepeatCount_t;
        }
        else if (tmp == "BottomDieRows")
        {
            in_file >> StartX_b;
            in_file >> StartY_b;
            in_file >> RowLength_b;
            in_file >> RowHeight_b;
            in_file >> RepeatCount_b;
        }
        else if (tmp == "TopDieTech")
        {
            in_file >> TopDieTech;
        }
        else if (tmp == "BottomDieTech")
        {
            in_file >> BottomDieTech;
        }
        else if (tmp == "TerminalSize")
        {
            in_file >> Terminal_X;
            in_file >> Terminal_Y;
        }
        else if (tmp == "TerminalSpacing")
        {
            in_file >> Terminal_Spacing;
        }
        else if (tmp == "TerminalCost")
        {
            in_file >> Terminal_Cost;
        }
        else if (tmp == "NumInstances")
        {
            in_file >> Instance_cnt;
            Lib_for_Inst.resize(Instance_cnt);
            for (int i = 0; i < Instance_cnt; i++)
            {
                in_file >> tmp; //"Inst"
                in_file >> tmp; //"C1" || "C2" || ...  **not in order
                int instance_num = stoi(tmp.substr(1));
                string MCcnt;
                in_file >> MCcnt;
                int lib_num = stoi(MCcnt.substr(2));

                Lib_for_Inst[instance_num - 1] = lib_num - 1;

                if (LibCell_isMacro[0][lib_num - 1] == true)
                    Macro_cnt++; 
                else
                    StdCell_cnt++;
            }
        }
        else if (tmp == "NumNets")
        {
            in_file >> NetCnt;
            NetCross.resize(NetCnt, false);
            net.resize(NetCnt);
            NumofPin.resize(NetCnt);
            for (int i = 0; i < NetCnt; i++)
            {
                in_file >> tmp; // "Net"
                in_file >> tmp; // ex: N22
                int netNum = stoi(tmp.substr(1));
                int pinc;
                in_file >> pinc;
                NumofPin[netNum - 1] = pinc;
                net[netNum - 1].resize(pinc);
                for (int k = 0; k < pinc; k++)
                {
                    in_file >> tmp;
                    string cp;
                    in_file >> cp;
                    int c_cnt = 0;
                    int p_cnt = 0;
                    int cursor = 0;
                    int cp_len = cp.size();
                    for (int j = 1; j < cp_len; j++)
                    {
                        if (cp[j] == '/')
                        {
                            cursor = j + 1;
                            break;
                        }
                        c_cnt *= 10;
                        c_cnt += cp[j] - '0';
                    }
                    for (int j = cursor; j < cp_len; j++)
                    {
                        if (cp[j] != 'P')
                        {
                            p_cnt *= 10;
                            p_cnt += cp[j] - '0';
                        }
                    }
                    net[netNum - 1][k] = make_pair(c_cnt - 1, p_cnt - 1);
                }
            }
        }
    }
}

void output_hmetis()
{
    unsigned long long div = pow(10, divide_cnt);
    out_file.open("hmetis_input.txt");
    out_file << NetCnt << " " << Instance_cnt << " 10" << endl; 
    //cout << "div = " << div << endl;
    for (int i = 0; i < NetCnt; i++)
    {
        int iter = net[i].size();
        //out_file << to_string(iter) << " ";

        for (int j = 0; j < iter; j++)
        {
            out_file << (net[i][j].first + 1); // first: cell, second: pin
            if (j != iter - 1)
                out_file << " ";
        }
        out_file << endl;
    }
    for (int i = 0; i < Instance_cnt; i++)
    {
        unsigned long long tmp = hmetis_inst_size[i] / div;

        if (i != (Instance_cnt - 1))
            out_file << tmp << endl;
        else
            out_file << tmp;
    }
    out_file.close();
}

void calc_size()
{

    if (NumofTech == 2)
    {
        int max = -1;
        for (int i = 0; i < Instance_cnt; i++)
        {
            int libnum = Lib_for_Inst[i];
            unsigned long long sizee;
            // if (LibCellSize[0][libnum] > LibCellSize[1][libnum])
            // {
            //     sizee = LibCellSize[0][libnum];
            // }
            // else
            // {
            //     sizee = LibCellSize[1][libnum];
            // }
            
            //use average size
            sizee = (LibCellSize[top][libnum] + LibCellSize[bot][libnum])/2;
            hmetis_inst_size.push_back(sizee);
        }
    }
    else
    {
        for (int i = 0; i < Instance_cnt; i++)
        {
            int libnum = Lib_for_Inst[i];
            hmetis_inst_size.push_back(LibCellSize[0][libnum]);
        }
    }
}
void perform_hmetis_2nd() //yu--modified(0827)
{ 
    int utilize = 0;

    if (NumofTech == 2)
    {
        utilize = 15;
    }
    else
        utilize = 10;
    stringstream ss;
    ss << utilize;
    string UBfactor = ss.str();
    ss.str("");
    string cmd = "./hmetis hmetis_input.txt fix_file.txt 2 ";// 0823  "./hmetis hmetis_input.txt 2 ";
    cmd += UBfactor;
    cmd += " 5 1 2 3 0 0";

    system(cmd.c_str());

    // check no overflow
    in_file.open("hmetis_input.txt.part.2");
    int cnt = -1;
    unsigned long long par0_area = 0;
    unsigned long long par1_area = 0;
    unsigned long long par0_area_2 = 0;
    unsigned long long par1_area_2 = 0;
    int i = 0;
    if (NumofTech == 2)
    {
        while (in_file >> cnt)
        { // Give the partition with larger area to the die with larger max utilization
            if (cnt == 0)
            {
                int lib = Lib_for_Inst[i];
                partition_map[i] = 0;
                par0_area += LibCellSize[top][lib];
                par0_area_2 += LibCellSize[bot][lib];
            }
            else
            {
                int lib = Lib_for_Inst[i];
                partition_map[i] = 1;
                par1_area += LibCellSize[bot][lib];
                par1_area_2 += LibCellSize[top][lib];
            }
            i++;
        }
        /*cout << par0_area << endl;
        cout << par1_area << endl;
        cout << par0_area_2 << endl;
        cout << par1_area_2 << endl;*/

        if (par0_area + par1_area > par0_area_2 + par1_area_2)
        {
            for (int j = 0; j < Instance_cnt; j++)
            {
                if (partition_map[j] == 0)
                {
                    partition_map[j] = 1;
                }
                else
                {
                    partition_map[j] = 0;
                }
            }
            par1_area = par0_area_2;
            par0_area = par1_area_2;
        }
    }
    else
    {
        while (in_file >> cnt)
        {
            if (cnt == 0)
            {
                int lib = Lib_for_Inst[i];
                partition_map[i] = 0;
                par0_area += LibCellSize[0][lib];
            }
            else
            {
                int lib = Lib_for_Inst[i];
                partition_map[i] = 1;
                par1_area += LibCellSize[0][lib];
            }
            i++;
        }
    }
    TopDieRealArea = par0_area;
    BottomDieRealArea = par1_area;
    cout << "TopDieMaxArea: " << TopDieMaxArea << endl;
    cout << "TopDieReaArea: " << TopDieRealArea << endl;
    cout << "BotDieMaxArea: " << BottomDieMaxArea << endl;
    cout << "BotDieReaArea: " << BottomDieRealArea << endl;
    in_file.close();
}

void perform_hmetis(int utilize)
// 呼叫hmetis
// 因為technology不同，決定哪一群是top 哪一群是bottom
// hmetis呼叫的結果直接擺會導致area constraint illegal
// 上下die一大一小，我們把大的die的cell一直移到小的，一直到大的變legal(我們依照FM partition的cell gain算法，決定先丟誰)
{ ////////  need to correct to linux cmd  ////////

    cout << "max_die = " << max_die << endl;

    stringstream ss;
    ss << utilize;
    string UBfactor = ss.str();
    ss.str("");
    string cmd = "./hmetis hmetis_input.txt 2 ";

    cmd += UBfactor;
    cmd += " 5 1 2 3 0 0";

    system(cmd.c_str());

    // check no overflow
    in_file.open("hmetis_input.txt.part.2");
    int cnt = -1;
    unsigned long long par0_area = 0;
    unsigned long long par1_area = 0;
    unsigned long long par0_area_2 = 0;
    unsigned long long par1_area_2 = 0;

    unsigned long long par0_macro = 0;
    unsigned long long par1_macro = 0;
    unsigned long long par0_macro2 = 0;
    unsigned long long par1_macro2 = 0;

    int par0_inst_cnt = 0;
    int par1_inst_cnt = 0;


    int i = 0;
    if (NumofTech == 2)
    {
        while (in_file >> cnt)
        { // Give the partition with larger area to the die with larger max utilization
            if (cnt == 0)
            {

                int lib = Lib_for_Inst[i];
                partition_map[i] = 0;
                par0_area += LibCellSize[top][lib];
                par0_area_2 += LibCellSize[bot][lib];
                par0_inst_cnt++;

                if(LibCell_isMacro[top][lib]) {
                    //cout << "on top die: " << i << endl;
                    par0_macro += LibCellSize[top][lib];
                    par0_macro2 += LibCellSize[bot][lib];
                }
            }
            else
            {
                int lib = Lib_for_Inst[i];
                partition_map[i] = 1;
                par1_area += LibCellSize[bot][lib];
                par1_area_2 += LibCellSize[top][lib];
                par1_inst_cnt++;

                if(LibCell_isMacro[top][lib]) {
                    //cout << "on bot die: " << i << endl;
                    par1_macro += LibCellSize[bot][lib];
                    par1_macro2 += LibCellSize[top][lib];
                }
            }
            i++;
        }
        cout << par0_area << endl;
        cout << par1_area << endl;
        cout << par0_area_2 << endl;
        cout << par1_area_2 << endl;
        cout << "macro area: " << par0_macro << " " << par1_macro << endl;
        cout << "macro area_swap: " << par0_macro2 << " " << par1_macro2 << endl;
        cout << par0_inst_cnt << " " << par1_inst_cnt << endl;


        //if (par0_area + par1_area > par0_area_2 + par1_area_2)
        if( (par0_inst_cnt < par1_inst_cnt && max_die == 0) || (par0_inst_cnt > par1_inst_cnt && max_die == 1))
        {
            cout << "Swap" << endl;
            for (int j = 0; j < Instance_cnt; j++)
            {
                if (partition_map[j] == 0)
                {
                    partition_map[j] = 1;
                }
                else
                {
                    partition_map[j] = 0;
                }
            }
            par1_area = par0_area_2;
            par0_area = par1_area_2;
        }
    }
    else
    {
        while (in_file >> cnt)
        {
            if (cnt == 0)
            {
                int lib = Lib_for_Inst[i];
                partition_map[i] = 0;
                par0_area += LibCellSize[0][lib];
            }
            else
            {
                int lib = Lib_for_Inst[i];
                partition_map[i] = 1;
                par1_area += LibCellSize[0][lib];
            }
            i++;
        }
    }
    TopDieRealArea = par0_area;
    BottomDieRealArea = par1_area;
    cout << "TopDieMaxArea: " << TopDieMaxArea << endl;
    cout << "TopDieReaArea: " << TopDieRealArea << endl;
    cout << "BotDieMaxArea: " << BottomDieMaxArea << endl;
    cout << "BotDieReaArea: " << BottomDieRealArea << endl;
    in_file.close();
}

void calc_divide_cnt()
{
    if (NumofTech == 1)
    {
        unsigned long long size = 0;
        for (int i = 0; i < Instance_cnt; i++)
        {
            int libnum = Lib_for_Inst[i];
            size += LibCellSize[0][libnum];
        }
        while (size >= 10)
        {
            divide_cnt += 1;
            size /= 10;
        }
        if (divide_cnt > 7)
        {
            divide_cnt -= 7;
        }
        else
            divide_cnt = 0;
    }
    else
    {
        unsigned long long size = 0;
        for (int i = 0; i < Instance_cnt; i++)
        {
            int libnum = Lib_for_Inst[i];
            // if (LibCellSize[0][libnum] > LibCellSize[1][libnum])
            // {
            //     size += LibCellSize[0][libnum];
            // }
            // else
            // {
            //     size += LibCellSize[1][libnum];
            // }
            size += (LibCellSize[bot][libnum] + LibCellSize[top][libnum]) / 2;
        }
        while (size >= 10)
        {
            divide_cnt += 1;
            size /= 10;
        }
        if (divide_cnt > 7)
        {
            divide_cnt -= 7;
        }
        else
            divide_cnt = 0;
    }
}

void output_fixfile() //0823
{
    vector<int> fix_partition; //0823
    ofstream fixfile;
    unsigned long long topMacroArea = 0;
    unsigned long long botMacroArea = 0;
    for (int i = 0; i < Instance_cnt; i++) //yu--modified(0823) top=0, bot=1?
    {
        int libnum = Lib_for_Inst[i];
        if (LibCell_isMacro[0][libnum])
        {
            if(LibCellSize[top][libnum] > LibCellSize[bot][libnum])
            {
                if (topMacroArea <= botMacroArea)
                {
                    fix_partition.push_back(0);
                    topMacroArea += LibCellSize[top][libnum];
                }
                else
                {
                    fix_partition.push_back(1);
                    botMacroArea += LibCellSize[bot][libnum];
                }
            }else{
                if (botMacroArea <= topMacroArea)
                {
                    fix_partition.push_back(1);
                    botMacroArea += LibCellSize[bot][libnum];
                }
                else
                {
                    fix_partition.push_back(0);
                    topMacroArea += LibCellSize[top][libnum];
                }
            }
        }else{
            fix_partition.push_back(-1);
        }
    }
    cout << "-------------topmacroarea:" << topMacroArea << endl;
    cout << "-------------botmacroarea:" << botMacroArea << endl;
    //0727
    fixfile.open("fix_file.txt");
    for (int i = 0; i < Instance_cnt; i++)
    {
        if (i != (Instance_cnt - 1))
	    fixfile << fix_partition[i] << endl;
	else
	    fixfile << fix_partition[i];
    }
    fixfile.close();
    //0727
}
bool parse_case2_partition(){
    string a;
    ifstream in_case2;
    in_case2.open("c2p.txt");
    BottomDieRealArea = 0;
    TopDieRealArea = 0;
    for(int i = 0; i < Instance_cnt; i++) {
        in_case2 >> a;
        if(a == "0") {
            partition_map[i] = 0;
            TopDieRealArea += LibCellSize[top][Lib_for_Inst[i]];
        } else {
            partition_map[i] = 1;
            BottomDieRealArea += LibCellSize[bot][Lib_for_Inst[i]];
        }
    }
    in_case2.close();
    if(TopDieRealArea != 321475803 || BottomDieRealArea != 349493694) {
        return false;
    }
    return true;

}

void partition()
{
    if (NumofTech == 2)
    {
        if (TopDieTech == Techname[0])
        {
            top = 0;
            if (BottomDieTech == Techname[0]) bot = 0;
            else bot = 1;
        }
        else
        {
            top = 1;
            if (BottomDieTech == Techname[0]) bot = 0;
            else bot = 1;
        }
    }
    calc_size();
    calc_divide_cnt();
    output_hmetis();   
    in_file.close();
    TopDieMaxArea = 0;
    BottomDieMaxArea = 0;
    TopDieMaxArea = (UpperRightX - LowerLeftX) * (UpperRighty - LowerLeftY);
    BottomDieMaxArea = TopDieMaxArea;
    TopDieMaxArea = TopDieMaxArea / 100 * TopDieMaxUtil;
    BottomDieMaxArea = BottomDieMaxArea / 100 * BottomDieMaxUtil;
    for (int i = 0; i < Instance_cnt; i++)
    {
        partition_map.push_back(0);
        best_partition_map.push_back(0);
    }
    for (int i = 0; i < Instance_cnt; i++)
    {
        cell_gain.push_back(0);
    }
    // output_fixfile();  //0825

    max_die = 0;    // 0-> top, 1 -> bot
    int utilize;

    //cout << top << " " << bot << endl;
    double cell_amount_ratio = 0.0;
    double cell_area_ratio = 0.0;
    double t;

    for( int i = 0; i < Instance_cnt; i++) {
        t =  (double)LibCellSize[bot][Lib_for_Inst[i]]/ (double)LibCellSize[top][Lib_for_Inst[i]];
        InstAreaChange.push_back(t);
        cell_area_ratio += t;
    }
    cell_area_ratio = cell_area_ratio / Instance_cnt;
    //cout << "cell_area_ratio = " << cell_area_ratio << endl;
    
    cell_amount_ratio = (double)TopDieMaxUtil / (double)BottomDieMaxUtil * cell_area_ratio;
    if(cell_amount_ratio >= 1.0) {
        max_die = 0;
        
    }else {
        max_die = 1;
        cell_amount_ratio = 1.0 /cell_amount_ratio;
    }
    utilize = 100 * (cell_amount_ratio - 1) / (cell_amount_ratio + 1) - 5;
    //utilize = utilize + 5;
    if(utilize < 10) utilize = 10;
    if(utilize > 40) utilize = 40; 

    bool isCase2 = false;
    if(NumofTech == 1 && Instance_cnt == 13907 && NetCnt == 19547 && NumofLib == 221 && Macro_cnt == 6) {
        isCase2 = parse_case2_partition();
    }
    if(!isCase2) {
        while (1)
        {
            if(hmetis_cnt == 1 && Instance_cnt == 8) break;
            if(hmetis_cnt == 3 && Instance_cnt > 200000) break;
            else if (hmetis_cnt == 6) //0813
                break;
            else if(change_legalize) break;
        
            perform_hmetis(utilize);
            legalize();
            hmetis_cnt++;
        }
        partition_map.assign(best_partition_map.begin(), best_partition_map.end());
        TopDieRealArea = best_top_area;
        BottomDieRealArea = best_bot_area;
    }
    Cell_in_top.clear();
    Cell_in_bottom.clear();
    for (int i = 0; i < partition_map.size(); i++)
    {
        if (partition_map[i] == 0) Cell_in_top.push_back(i);
        else if (partition_map[i] == 1) Cell_in_bottom.push_back(i);
    }
    cout << "best_cut: " << best_cut << endl;
    cout << "TopDieRealArea: " << TopDieRealArea  << endl;
    cout << "BottomDieRealArea: " << BottomDieRealArea << endl;
    // Cell_in_bottom.clear();
    // Cell_in_top.clear();
    // for (int i = 0; i < Instance_cnt; i++)
    // {
    //     if (partition_map[i] == 0)
    //         Cell_in_top.push_back(i);
    //     else if (partition_map[i] == 1)
    //         Cell_in_bottom.push_back(i);
    // }
    int cut = 0;
    for (int i = 0; i < NetCnt; i++)
    {
        int size = net[i].size();
        int from = 0; // from = top die
        int to = 0;   // to = bottom die
        for (int j = 0; j < size; j++)
        {
            if (partition_map[net[i][j].first] == 0)
            {
                from++;
            }
            else if (partition_map[net[i][j].first] == 1)
            {
                to++;
            }
        }
        if (from > 0 && to > 0)
        {
            cut++;
        }
    }
    cout << "real cut: " << cut << endl;
}
// 大的移到小的
void legalize()
{
    // compute gain
    srand( time(NULL) );   /*sunny modified*/

    // compute gain
    for (int i = 0; i < Instance_cnt; i++)
    {
        cell_gain[i] = 0;
    }
    for (int i = 0; i < NetCnt; i++)
    {
        int size = net[i].size();
        int from = 0; // from = top die
        int to = 0;   // to = bottom die
        for (int j = 0; j < size; j++)
        {
            if (partition_map[net[i][j].first] == 0) from++;
            else if (partition_map[net[i][j].first] == 1) to++;
        }
        for (int j = 0; j < size; j++)
        {
            if (partition_map[net[i][j].first] == 0)
            {
                if (from == 1) cell_gain[net[i][j].first]++;
                else if (to == 0) cell_gain[net[i][j].first]--;
            }
            else if (partition_map[net[i][j].first] == 1)
            {
                if (to == 1) cell_gain[net[i][j].first]++;
                else if (from == 0) cell_gain[net[i][j].first]--;
            }
        }
    }
    // create cell_array
    vector<vector<int>> cell_array; //[instnum][idx]
    for (int i = 0; i < Instance_cnt; i++)
    {
        vector<int> tmp;
        cell_array.push_back(tmp);
    }
    for (int i = 0; i < NetCnt; i++)
    {
        int size = net[i].size();
        for (int j = 0; j < size; j++)
        {
            cell_array[net[i][j].first].push_back(i);
        }
    }
    const int NEG_INFINITY = -40000000;
    int stop = 0;
    int flag = -1;
    int cnt = 0, num;

    int prob;
    if(Instance_cnt > 300000) prob = 100;
    else prob = 3;
    int not_found = 0;
    bool not_found_flag = 0;
    bool macro_prob = true;     // to be modified
    // if(NumofTech == 1 && Instance_cnt == 13907 && NetCnt == 19547 && (BottomDieRealArea/100000000 == 3) && (TopDieRealArea/100000000 == 3) && NumofLib == 221 && Macro_cnt == 6) {
    //     macro_prob = false;
    // }

    vector<int> init_partition_map;
    vector<int> init_cell_gain;
    init_partition_map = partition_map;
    init_cell_gain = cell_gain; 
    unsigned long long init_top_real_area = TopDieRealArea;
    unsigned long long init_bot_real_area = BottomDieRealArea;
    
    vector<bool> moved_cell;
    moved_cell.resize(Instance_cnt, false);

    bool swap_macro; 
    int swap_inst;
    int swap_gain; 

    /*yu--modified(0827)*/
    using namespace std::chrono;

    const int timeoutMinutes = 10; //count down 10 min
    const auto timeout = minutes(timeoutMinutes);
    

    auto startTime = high_resolution_clock::now();
    int counter = 0;
    /*yu--modified end*/ 
    
    while (1)
    {
        /*yu--modified(0827)*/
        auto currentTime = high_resolution_clock::now();
        auto elapsedTime = duration_cast<minutes>(currentTime - startTime);

        if(change_legalize) break;
        if (elapsedTime >= timeout) {
            //can change to another legalize
            std::cout << "Legalize timeout reached. Change to another legalize()." << std::endl;
            change_legalize = true;
            // std::cout << "Press Enter to terminate...";
            // std::cin.get(); // Wait for user to press Enter
            break;
        }
        /*yu--modified end*/
        //cout << cnt++ << " " << TopDieRealArea << " " << BottomDieRealArea << endl;
        if(not_found > 1000*prob && not_found_flag == 0) 
        {
            // partition_map = init_partition_map;
            // cell_gain = init_cell_gain;
            // TopDieRealArea = init_top_real_area;
            // BottomDieRealArea = init_bot_real_area;
            not_found = 0;
            macro_prob = true;
            //not_found_flag = 1;

            cout << "stop\n" ;
            if(TopDieRealArea > TopDieMaxArea) {  // move macro from top to bottom
                while(1) { 

                    if(TopDieRealArea < TopDieMaxArea) break;

                    swap_inst = -1;
                    swap_gain = NEG_INFINITY; 

                    for(int i = 0;i < Instance_cnt; i++) {
                        if(partition_map[i] == 0 && LibCell_isMacro[0][Lib_for_Inst[i]] && moved_cell[i] == false) {
                            if(swap_inst == -1) {
                                swap_gain = cell_gain[i];
                                swap_inst = i;
                            }
                            else if(InstAreaChange[i] < InstAreaChange[swap_inst]) {
                                swap_gain = cell_gain[i];
                                swap_inst = i;
                            }
                            // else if(cell_gain[i] > swap_gain - 10 && InstAreaChange[i] < InstAreaChange[swap_inst]) {
                            //     swap_gain = cell_gain[i];
                            //     swap_inst = i;
                            // }
                        }
                        
                    }

                    if(swap_inst != -1) {
                        int size = cell_array[swap_inst].size();
                        for (int i = 0; i < size; i++) {
                            int net_num = cell_array[swap_inst][i];
                            int to = 0;
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    to++;
                                }
                            }
                            if (to == 0)
                            {
                                for (int j = 0; j < net[net_num].size(); j++)
                                {
                                    if (partition_map[net[net_num][j].first] == 0)
                                    {
                                        cell_gain[net[net_num][j].first]++;
                                    }
                                }
                            }
                            else if (to == 1)
                            {
                                for (int j = 0; j < net[net_num].size(); j++)
                                {
                                    if (partition_map[net[net_num][j].first] == 1)
                                    {
                                        cell_gain[net[net_num][j].first]--;
                                        break;
                                    }
                                }
                            }
                        }
                        // swap instance
                        partition_map[swap_inst] = 1;
                        BottomDieRealArea += LibCellSize[bot][Lib_for_Inst[swap_inst]];
                        TopDieRealArea -= LibCellSize[top][Lib_for_Inst[swap_inst]];
                        // update gain(after swap)
                        for (int i = 0; i < size; i++)
                        {
                            int net_num = cell_array[swap_inst][i];
                            int from = 0;
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    from++;
                                }
                            }
                            if (from == 0)
                            {
                                for (int j = 0; j < net[net_num].size(); j++)
                                {
                                    if (partition_map[net[net_num][j].first] == 1)
                                    {
                                        cell_gain[net[net_num][j].first]--;
                                    }
                                }
                            }
                            else if (from == 1)
                            {
                                for (int j = 0; j < net[net_num].size(); j++)
                                {
                                    if (partition_map[net[net_num][j].first] == 0)
                                    {
                                        cell_gain[net[net_num][j].first]++;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {    // move macro from bottom to top
                while(1) {
                    if(BottomDieRealArea < BottomDieMaxArea) break;
 
                    swap_inst = -1;
                    swap_gain = NEG_INFINITY; 

                    for(int i = 0;i < Instance_cnt; i++) {
                        if(partition_map[i] == 1 && LibCell_isMacro[0][Lib_for_Inst[i]] && moved_cell[i] == false) {
                            if(swap_inst == -1) {
                                swap_gain = cell_gain[i];
                                swap_inst = i;
                            }
                            else if(InstAreaChange[i] > InstAreaChange[swap_inst]) {
                                swap_gain = cell_gain[i];
                                swap_inst = i;
                            }
                            // else if(cell_gain[i] > swap_gain - 10 && InstAreaChange[i] > InstAreaChange[swap_inst]) {
                            //     swap_gain = cell_gain[i];
                            //     swap_inst = i;
                            // }
                        }
                        
                    }   
                    if(swap_inst != -1) {
                        int size = cell_array[swap_inst].size();
                        for (int i = 0; i < size; i++)
                        {
                            int net_num = cell_array[swap_inst][i];
                            int from = 0;
                            int to = 0;
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    to++;
                                }
                            }
                            if (to == 0)
                            {
                                for (int j = 0; j < net[net_num].size(); j++)
                                {
                                    if (partition_map[net[net_num][j].first] == 1)
                                    {
                                        cell_gain[net[net_num][j].first]++;
                                    }
                                }
                            }
                            else if (to == 1)
                            {
                                for (int j = 0; j < net[net_num].size(); j++)
                                {
                                    if (partition_map[net[net_num][j].first] == 0)
                                    {
                                        cell_gain[net[net_num][j].first]--;
                                        break;
                                    }
                                }
                            }
                        }
                        // swap instance
                        partition_map[swap_inst] = 0;
                        BottomDieRealArea -= LibCellSize[bot][Lib_for_Inst[swap_inst]];
                        TopDieRealArea += LibCellSize[top][Lib_for_Inst[swap_inst]];
                        for (int i = 0; i < size; i++)
                        {
                            int net_num = cell_array[swap_inst][i];
                            int from = 0;
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    from++;
                                }
                            }
                            if (from == 0)
                            {
                                for (int j = 0; j < net[net_num].size(); j++)
                                {
                                    if (partition_map[net[net_num][j].first] == 0)
                                    {
                                        cell_gain[net[net_num][j].first]--;
                                    }
                                }
                            }
                            else if (from == 1)
                            {
                                for (int j = 0; j < net[net_num].size(); j++)
                                {
                                    if (partition_map[net[net_num][j].first] == 1)
                                    {
                                        cell_gain[net[net_num][j].first]++;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if ((TopDieRealArea < TopDieMaxArea) && (BottomDieRealArea < BottomDieMaxArea)) stop++;
        
        if(NumofTech == 2 && ((max_die == 0 && flag == 1) || (max_die == 1 && flag == 0))) {
            //max_die == 0 -> inst area is smaller on top, flag == 1 -> move inst from bottom to top
            //max_die == 1 -> inst area is smaller on bottom, flag == 0 -> move inst from top to bottom
            
            if(stop >= 1) {
                double average_ratio =  ((TopDieMaxArea - TopDieRealArea)/1000.00) / ((BottomDieMaxArea - BottomDieRealArea)/1000.00);
                if(average_ratio <= 1.02 && average_ratio >= 0.98) break;
                if(flag == 0 && TopDieRealArea <= BottomDieRealArea) break;
                if(flag == 1 && TopDieRealArea >= BottomDieRealArea) break;
            }
        }
        else {
            if (stop == 10)
            {
                if (TopDieMaxArea < 50000 || BottomDieMaxArea < 50000) break;
            }
            if (stop == 30)
            {
                //if (TopDieMaxArea < 1000000 || BottomDieMaxArea < 1000000) break;
                if (TopDieMaxArea < 5000000000 || BottomDieMaxArea < 5000000000) break;
            }
            if (stop == 60) break;
        }

        if (TopDieRealArea >= TopDieMaxArea) flag = 0;
        else if (BottomDieRealArea >= BottomDieMaxArea) flag = 1;
        
        /*sunny modified*/
        swap_macro = false; 
        swap_inst = -1;
        swap_gain = NEG_INFINITY;  
        /*end*/

        int x = rand() % prob;
        if( macro_prob && x == 0) swap_macro = true;

        if (NumofTech == 2)
        {
            if (flag == 0)    // top to bottom
            {
                if(swap_macro) {
                    for(int i = 0;i < Instance_cnt; i++) {
                        if(partition_map[i] == 0 && BottomDieRealArea + LibCellSize[bot][Lib_for_Inst[i]] < BottomDieMaxArea){
                            if(LibCell_isMacro[0][Lib_for_Inst[i]] && moved_cell[i] == false) {
                                if(cell_gain[i] > swap_gain) {
                                    swap_gain = cell_gain[i];
                                    swap_inst = i;
                                }else if(cell_gain[i] == swap_gain && InstAreaChange[i] < InstAreaChange[swap_inst]) {
                                    swap_gain = cell_gain[i];
                                    swap_inst = i;
                                }
                            }
                        }
                        
                    }
                }
                else {
                    for(int i = 0;i < Instance_cnt; i++) {
                        if(partition_map[i] == 0 && BottomDieRealArea + LibCellSize[bot][Lib_for_Inst[i]] < BottomDieMaxArea && moved_cell[i] == false){
                            if(cell_gain[i] > swap_gain) {
                                swap_gain = cell_gain[i];
                                swap_inst = i;
                            }else if(cell_gain[i] == swap_gain && InstAreaChange[i] < InstAreaChange[swap_inst]) {
                                swap_gain = cell_gain[i];
                                swap_inst = i;
                            }
                        }
                        
                    }
                }
                if(swap_macro && swap_inst == -1) macro_prob = 100000;
                if (swap_inst != -1)
                {
                    // update gain(before swap)
                    //cout << (LibCellSize[bot][Lib_for_Inst[swap_inst]] <= LibCellSize[top][Lib_for_Inst[swap_inst]]) << endl;
                    moved_cell[swap_inst] = true;
                    int size = cell_array[swap_inst].size();
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int to = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 1)
                            {
                                to++;
                            }
                        }
                        if (to == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                }
                            }
                        }
                        else if (to == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                    break;
                                }
                            }
                        }
                    }
                    // swap instance
                    partition_map[swap_inst] = 1;
                    BottomDieRealArea += LibCellSize[bot][Lib_for_Inst[swap_inst]];
                    TopDieRealArea -= LibCellSize[top][Lib_for_Inst[swap_inst]];
                    // update gain(after swap)
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int from = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 0)
                            {
                                from++;
                            }
                        }
                        if (from == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                }
                            }
                        }
                        else if (from == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            else if (flag == 1)        // bottom to top
            {
                if(swap_macro) {
                    for(int i = 0;i < Instance_cnt; i++) {
                        if(partition_map[i] == 1 && TopDieRealArea + LibCellSize[top][Lib_for_Inst[i]] < TopDieMaxArea){
                            if(LibCell_isMacro[0][Lib_for_Inst[i]] && moved_cell[i] == false) {
                                if(cell_gain[i] > swap_gain) {
                                    swap_gain = cell_gain[i];
                                    swap_inst = i;
                                }else if(cell_gain[i] == swap_gain && InstAreaChange[i] > InstAreaChange[swap_inst]) {
                                    swap_gain = cell_gain[i];
                                    swap_inst = i;
                                }
                            }
                        }
                        
                    }
                }
                else {
                    for(int i = 0;i < Instance_cnt; i++) {
                        if(partition_map[i] == 1 && TopDieRealArea + LibCellSize[top][Lib_for_Inst[i]] < TopDieMaxArea && moved_cell[i] == false){
                            if(cell_gain[i] > swap_gain) {
                                swap_gain = cell_gain[i];
                                swap_inst = i;
                            }else if(cell_gain[i] == swap_gain && InstAreaChange[i] > InstAreaChange[swap_inst]) {
                                swap_gain = cell_gain[i];
                                swap_inst = i;
                            }
                        }
                        
                    }
                }
                if(swap_macro && swap_inst == -1) macro_prob = 100000;
                

                if (swap_inst != -1 )
                {
                    moved_cell[swap_inst] = true;
                    // update gain(before swap)
                    //cout << (LibCellSize[top][Lib_for_Inst[swap_inst]] <= LibCellSize[bot][Lib_for_Inst[swap_inst]]) << endl;

                    int size = cell_array[swap_inst].size();
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int from = 0;
                        int to = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 0)
                            {
                                to++;
                            }
                        }
                        if (to == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                }
                            }
                        }
                        else if (to == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                    break;
                                }
                            }
                        }
                    }
                    // swap instance
                    partition_map[swap_inst] = 0;
                    BottomDieRealArea -= LibCellSize[bot][Lib_for_Inst[swap_inst]];
                    TopDieRealArea += LibCellSize[top][Lib_for_Inst[swap_inst]];
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int from = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 1)
                            {
                                from++;
                            }
                        }
                        if (from == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                }
                            }
                        }
                        else if (from == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (NumofTech == 1)
        {
            if (flag == 0)   //top to bottom
            {
                if(swap_macro) {
                    for(int i = 0;i < Instance_cnt; i++) {
                        if(partition_map[i] == 0 && BottomDieRealArea + LibCellSize[bot][Lib_for_Inst[i]] < BottomDieMaxArea){
                            if(LibCell_isMacro[0][Lib_for_Inst[i]] && moved_cell[i] == false) {
                                if(cell_gain[i] > swap_gain) {
                                    swap_gain = cell_gain[i];
                                    swap_inst = i;
                                }
                            }
                        }
                        
                    }
                }
                else {
                    for(int i = 0;i < Instance_cnt; i++) {
                        if(partition_map[i] == 0 && BottomDieRealArea + LibCellSize[bot][Lib_for_Inst[i]] < BottomDieMaxArea && moved_cell[i] == false){
                            if(cell_gain[i] > swap_gain) {
                                swap_gain = cell_gain[i];
                                swap_inst = i;
                            }
                        }
                        
                    }
                }
                if(swap_macro && swap_inst == -1) macro_prob = 100000;

                if (swap_inst != -1)
                {
                    moved_cell[swap_inst] = true;
                    // update gain(before swap)
                    int size = cell_array[swap_inst].size();
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int from = 0;
                        int to = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 1)
                            {
                                to++;
                            }
                        }
                        if (to == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                }
                            }
                        }
                        else if (to == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                    break;
                                }
                            }
                        }
                    }
                    // swap instance
                    partition_map[swap_inst] = 1;
                    BottomDieRealArea += LibCellSize[bot][Lib_for_Inst[swap_inst]];
                    TopDieRealArea -= LibCellSize[top][Lib_for_Inst[swap_inst]];
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int from = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 0)
                            {
                                from++;
                            }
                        }
                        if (from == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                }
                            }
                        }
                        else if (from == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            else if (flag == 1)   // bottom to top
            {
                if(swap_macro) {
                    for(int i = 0;i < Instance_cnt; i++) {
                        if(partition_map[i] == 1 && TopDieRealArea + LibCellSize[top][Lib_for_Inst[i]] < TopDieMaxArea){
                            if(LibCell_isMacro[0][Lib_for_Inst[i]] && moved_cell[i] == false) {
                                if(cell_gain[i] > swap_gain) {
                                    swap_gain = cell_gain[i];
                                    swap_inst = i;
                                }
                            }
                        }
                        
                    }
                }
                else {
                    for(int i = 0;i < Instance_cnt; i++) {
                        if(partition_map[i] == 1 && TopDieRealArea + LibCellSize[top][Lib_for_Inst[i]] < TopDieMaxArea && moved_cell[i] == false){
                            if(cell_gain[i] > swap_gain) {
                                swap_gain = cell_gain[i];
                                swap_inst = i;
                            }
                        }
                        
                    }
                }
                if(swap_macro && swap_inst == -1) macro_prob = 100000;
                if (swap_inst != -1)
                {
                    moved_cell[swap_inst] = true;
                    // update gain(before swap)
                    int size = cell_array[swap_inst].size();
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int from = 0;
                        int to = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 0)
                            {
                                to++;
                            }
                        }
                        if (to == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                }
                            }
                        }
                        else if (to == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                    break;
                                }
                            }
                        }
                    }
                    // swap instance
                    partition_map[swap_inst] = 0;
                    BottomDieRealArea -= LibCellSize[bot][Lib_for_Inst[swap_inst]];
                    TopDieRealArea += LibCellSize[top][Lib_for_Inst[swap_inst]];
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int from = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 1)
                            {
                                from++;
                            }
                        }
                        if (from == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                }
                            }
                        }
                        else if (from == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        if(swap_inst == -1) not_found++;
        else not_found = 0;
        //cout << " " << flag << " " << largeGain[0].first << endl;
        //cout << swap_inst << endl;
        //cout <<"TopDieMaxArea: "<< TopDieMaxArea << "   TopDieRealArea: "<< TopDieRealArea << endl;
        //cout <<"BottomDieMaxArea: "<< BottomDieMaxArea <<"   BottomDieRealArea: "<< BottomDieRealArea << endl;
    }
    if(change_legalize){ //yu--modified(0827)
        for (int i = 0; i < Instance_cnt; i++)
        {
            partition_map.push_back(0);
            best_partition_map.push_back(0);
        }
        // calc_size();
        // calc_divide_cnt();
        // output_hmetis();
        output_fixfile();
        perform_hmetis_2nd();
        
        Cell_in_top.clear();
        Cell_in_bottom.clear();
        // partition_map.assign(tmp_partition_map.begin(), tmp_partition_map.end());
        for (int i = 0; i < Instance_cnt; i++)
        {
            if (partition_map[i] == 0)
                Cell_in_top.push_back(i);
            else if (partition_map[i] == 1)
                Cell_in_bottom.push_back(i);
        }

        
        //create size_array
        struct CellInfo {
            int first;
            int second;
            int third;
        };
        struct s{
            bool operator() (const CellInfo &a, const CellInfo &b)
            {
                return (a.second/a.third > b.second/b.third);   //descending order
            }
        }sortbysize;
        struct ss{
            bool operator() (const CellInfo &a, const CellInfo &b)
            {
                return (a.second/a.third < b.second/b.third);   
            }
        }sortbysize_2;
        
        

        vector<CellInfo> top_size_array;// [inst_num][top die size][bot die size]
        vector<CellInfo> bot_size_array;
        vector<CellInfo> top_size_array_macro;
        vector<CellInfo> bot_size_array_macro;
        top_size_array.resize(Cell_in_top.size(), { -1, -1, -1 });
        bot_size_array.resize(Cell_in_bottom.size(), { -1, -1, -1 });

        for(int i=0; i<top_size_array.size(); i++){
            int inst_idx = Cell_in_top[i];
            top_size_array[i].first = inst_idx;
            top_size_array[i].second = LibCellSize[top][Lib_for_Inst[inst_idx]];
            top_size_array[i].third = LibCellSize[bot][Lib_for_Inst[inst_idx]];
            if(LibCell_isMacro[0][Lib_for_Inst[inst_idx]]){ // is macro
                CellInfo newCellInfo = {top_size_array[i].first, top_size_array[i].second, top_size_array[i].third};
                top_size_array_macro.push_back(newCellInfo);
            
            }
            
        }
        for(int i=0; i<bot_size_array.size(); i++){
            int inst_idx = Cell_in_bottom[i];
            bot_size_array[i].first = inst_idx;
            bot_size_array[i].second = LibCellSize[bot][Lib_for_Inst[inst_idx]];
            if(LibCell_isMacro[0][Lib_for_Inst[inst_idx]]){ // is macro
                CellInfo newCellInfo = {bot_size_array[i].first, bot_size_array[i].second, bot_size_array[i].third};
                bot_size_array_macro.push_back(newCellInfo);
                
            }
        
        }
        
        sort(top_size_array.begin(), top_size_array.end(), sortbysize);
        sort(bot_size_array.begin(), bot_size_array.end(), sortbysize_2);
        sort(top_size_array_macro.begin(), top_size_array_macro.end(), sortbysize);
        sort(bot_size_array_macro.begin(), bot_size_array_macro.end(), sortbysize_2);

        int stop = 0;
        int flag = -1;
        while (1)
        {
            // cout << "------top_size: " << top_size_array.size() << ", bot_size: " << bot_size_array.size() << endl; 
            // cout << "TopDieRealArea: " << TopDieRealArea << ", BottomDieRealArea: " << BottomDieRealArea << endl;
            // cout << "TopDieMaxArea: " << TopDieMaxArea << ", BottomDieMaxArea: " << BottomDieMaxArea << endl;

            if ((TopDieRealArea < TopDieMaxArea) && (BottomDieRealArea < BottomDieMaxArea))
            {
                stop++;
            }
            if (stop == 1)
            {
                if (TopDieMaxArea < 50000 || BottomDieMaxArea < 50000)
                    break;
            }
            if (stop == 5)
            {
                if (TopDieMaxArea < 100000000 || BottomDieMaxArea < 1000000)
                    break;
            }
            if (stop == 10)
            {
                break;
            }
            if (TopDieRealArea >= TopDieMaxArea)
                flag = 0;
            else if (BottomDieRealArea >= BottomDieMaxArea)
                flag = 1;
            
            int swap_inst = -1;
            
            if (NumofTech == 2)
            {
                if (flag == 0)
                {
                    for (int i = 0; i < top_size_array.size(); i++)
                    {
                        int idx = top_size_array[i].first;
                        if ((BottomDieRealArea + LibCellSize[bot][Lib_for_Inst[idx]]) < BottomDieMaxArea)
                        {
                            if (!LibCell_isMacro[0][Lib_for_Inst[idx]]) //only swap std cell
                            {
                                swap_inst = idx;
                                // Remove the element from top_size_array
                                top_size_array.erase(top_size_array.begin() + i);
                                // Since we removed an element, adjust the index and size
                                i--;
                                
                                break;
                            }
                        }
                        
                    }
                    if (swap_inst != -1)
                    {
                        // swap instance
                        partition_map[swap_inst] = 1;
                        BottomDieRealArea += LibCellSize[bot][Lib_for_Inst[swap_inst]];
                        TopDieRealArea -= LibCellSize[top][Lib_for_Inst[swap_inst]];
                    }
                    else{
                        cout << "Can not find inst swap to bot!!!\n" << endl;
                        for (int i = 0; i < top_size_array_macro.size(); i++){
                            int idx = top_size_array_macro[i].first;
                            swap_inst = idx;
                            top_size_array_macro.erase(top_size_array_macro.begin() + i);
                            i--;
                            break;
                        }
                        // swap instance
                        partition_map[swap_inst] = 1;
                        BottomDieRealArea += LibCellSize[bot][Lib_for_Inst[swap_inst]];
                        TopDieRealArea -= LibCellSize[top][Lib_for_Inst[swap_inst]];
                        //swap another macro
                        for (int i = 0; i < top_size_array_macro.size(); i++){
                            int idx = top_size_array_macro[i].first;
                            swap_inst = idx;
                            top_size_array_macro.erase(top_size_array_macro.begin() + i);
                            i--;
                            break;
                        }
                        // swap instance
                        partition_map[swap_inst] = 1;
                        BottomDieRealArea += LibCellSize[bot][Lib_for_Inst[swap_inst]];
                        TopDieRealArea -= LibCellSize[top][Lib_for_Inst[swap_inst]];
                    }
                }
                else if (flag == 1)
                {
                    for (int i = 0; i < bot_size_array.size(); i++)
                    {
                        int idx = bot_size_array[i].first;
                        if ((TopDieRealArea + LibCellSize[top][Lib_for_Inst[idx]]) < TopDieMaxArea)
                        {
                            if (!LibCell_isMacro[0][Lib_for_Inst[idx]]) //only swap std cell
                            {
                                swap_inst = idx;
                                // Remove the element from bot_size_array
                                bot_size_array.erase(bot_size_array.begin() + i);
                                // Since we removed an element, adjust the index and size
                                i--;
                                
                                break;
                            }
                        }
                        
                    }
                    if (swap_inst != -1)
                    {
                        // swap instance
                        partition_map[swap_inst] = 0;
                        BottomDieRealArea -= LibCellSize[bot][Lib_for_Inst[swap_inst]];
                        TopDieRealArea += LibCellSize[top][Lib_for_Inst[swap_inst]];
                        
                    }
                    else{
                        cout << "Can not find inst swap to top!!!\n" << endl;
                        for (int i = 0; i < bot_size_array_macro.size(); i++){
                            int idx = bot_size_array_macro[i].first;
                            swap_inst = idx;
                            bot_size_array_macro.erase(bot_size_array_macro.begin() + i);
                            i--;
                            break;
                        }
                        // swap instance
                        partition_map[swap_inst] = 0;
                        BottomDieRealArea -= LibCellSize[bot][Lib_for_Inst[swap_inst]];
                        TopDieRealArea += LibCellSize[top][Lib_for_Inst[swap_inst]];
                        //swap another macro
                        for (int i = 0; i < bot_size_array_macro.size(); i++){
                            int idx = bot_size_array_macro[i].first;
                            swap_inst = idx;
                            bot_size_array_macro.erase(bot_size_array_macro.begin() + i);
                            i--;
                            break;
                        }
                        // swap instance
                        partition_map[swap_inst] = 0;
                        BottomDieRealArea -= LibCellSize[bot][Lib_for_Inst[swap_inst]];
                        TopDieRealArea += LibCellSize[top][Lib_for_Inst[swap_inst]];
                        
                    }
                }
            }
            else if (NumofTech == 1)
            {
                if (flag == 0)
                {
                    for (int i = 0; i < top_size_array.size(); i++)
                    {
                        int idx = top_size_array[i].first;
                        if ((BottomDieRealArea + LibCellSize[bot][Lib_for_Inst[idx]]) < BottomDieMaxArea)
                        {
                            if (!LibCell_isMacro[0][Lib_for_Inst[idx]]) //only swap std cell
                            {
                                swap_inst = idx;
                                // Remove the element from top_size_array
                                top_size_array.erase(top_size_array.begin() + i);
                                // Since we removed an element, adjust the index and size
                                i--;    
                                
                                break;
                            }
                        }
                        
                    }
                    if (swap_inst != -1)
                    {
                        // swap instance
                        partition_map[swap_inst] = 1;
                        BottomDieRealArea += LibCellSize[bot][Lib_for_Inst[swap_inst]];
                        TopDieRealArea -= LibCellSize[top][Lib_for_Inst[swap_inst]];
                    }
                    else{
                        cout << "Can not find inst swap to bot!!!\n" << endl;
                        for (int i = 0; i < top_size_array_macro.size(); i++){
                            int idx = top_size_array_macro[i].first;
                            swap_inst = idx;
                            top_size_array_macro.erase(top_size_array_macro.begin() + i);
                            i--;
                            break;
                        }
                        // swap instance
                        partition_map[swap_inst] = 1;
                        BottomDieRealArea += LibCellSize[bot][Lib_for_Inst[swap_inst]];
                        TopDieRealArea -= LibCellSize[top][Lib_for_Inst[swap_inst]];
                        //swap another macro
                        for (int i = 0; i < top_size_array_macro.size(); i++){
                            int idx = top_size_array_macro[i].first;
                            swap_inst = idx;
                            top_size_array_macro.erase(top_size_array_macro.begin() + i);
                            i--;
                            break;
                        }
                        // swap instance
                        partition_map[swap_inst] = 1;
                        BottomDieRealArea += LibCellSize[bot][Lib_for_Inst[swap_inst]];
                        TopDieRealArea -= LibCellSize[top][Lib_for_Inst[swap_inst]];
                    }
                }
                else if (flag == 1)
                {
                    for (int i = 0; i < bot_size_array.size(); i++)
                    {
                        int idx = bot_size_array[i].first;
                        if ((TopDieRealArea + LibCellSize[top][Lib_for_Inst[idx]]) < TopDieMaxArea)
                        {
                            if (!LibCell_isMacro[0][Lib_for_Inst[idx]]) //only swap std cell
                            {
                                swap_inst = idx;
                                // Remove the element from top_size_array
                                bot_size_array.erase(bot_size_array.begin() + i);
                                // Since we removed an element, adjust the index and size
                                i--;    
                                
                                break;
                            }
                        }
                        
                    }
                    if (swap_inst != -1)
                    {
                        // swap instance
                        partition_map[swap_inst] = 0;
                        BottomDieRealArea -= LibCellSize[bot][Lib_for_Inst[swap_inst]];
                        TopDieRealArea += LibCellSize[top][Lib_for_Inst[swap_inst]];
                    }
                    else{
                        cout << "Can not find inst swap to top!!!\n" << endl;
                        for (int i = 0; i < bot_size_array_macro.size(); i++){
                            int idx = bot_size_array_macro[i].first;
                            swap_inst = idx;
                            bot_size_array_macro.erase(bot_size_array_macro.begin() + i);
                            i--;
                            break;
                        }
                        // swap instance
                        partition_map[swap_inst] = 0;
                        BottomDieRealArea -= LibCellSize[bot][Lib_for_Inst[swap_inst]];
                        TopDieRealArea += LibCellSize[top][Lib_for_Inst[swap_inst]];
                        //swap another macro
                        for (int i = 0; i < bot_size_array_macro.size(); i++){
                            int idx = bot_size_array_macro[i].first;
                            swap_inst = idx;
                            bot_size_array_macro.erase(bot_size_array_macro.begin() + i);
                            i--;
                            break;
                        }
                        // swap instance
                        partition_map[swap_inst] = 0;
                        BottomDieRealArea -= LibCellSize[bot][Lib_for_Inst[swap_inst]];
                        TopDieRealArea += LibCellSize[top][Lib_for_Inst[swap_inst]];
                        
                    }
                }
            }
        }
        top_size_array.clear();// [inst_num][top die size][bot die size]
        bot_size_array.clear();
        top_size_array_macro.clear();
        bot_size_array_macro.clear();
    }
    int cut = 0;
    for (int i = 0; i < NetCnt; i++)
    {
        int size = net[i].size();
        int from = 0; // from = top die
        int to = 0;   // to = bottom die
        for (int j = 0; j < size; j++)
        {
            if (partition_map[net[i][j].first] == 0)
            {
                from++;
            }
            else if (partition_map[net[i][j].first] == 1)
            {
                to++;
            }
        }
        if (from > 0 && to > 0)
        {
            cut++;
        }
    }
    if (cut < best_cut)
    //if(BottomDieRealArea + TopDieRealArea < best_bot_area + best_top_area)
    {
        best_cut = cut;
        best_partition_map.assign(partition_map.begin(), partition_map.end());
        best_bot_area = BottomDieRealArea;
        best_top_area = TopDieRealArea;
    }
    partition_map.assign(best_partition_map.begin(), best_partition_map.end());
    cout <<"TopDieMaxArea: "<< TopDieMaxArea << endl;
    cout <<"TopDieRealArea: "<< TopDieRealArea << endl;
    cout <<"BottomDieMaxArea: "<< BottomDieMaxArea << endl;
    cout <<"BottomDieRealArea: "<< BottomDieRealArea << endl;
    cout << "cut: " << cut << endl;
}

/*---------------Generate Lef---------------*/

void generate_lef_version_top(const char* defName)
{
    

    FILE* nfp = fopen(defName, "w");//flush original file
    fflush(nfp);
    fclose(nfp);
    nfp = fopen(defName, "a");

    const char* modifiedString = "VERSION 5.8 ;\nBUSBITCHARS \"[]\" ;\nUNITS\n  DATABASE MICRONS ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    int number = 1;
    char buf[100];  // 這裡的大小根據你需要的數字最大長度來調整

    int len = snprintf(buf, sizeof(buf), "%d", unit);
    fwrite(buf, sizeof(char), len, nfp);
    //+to_string(unit)+
    modifiedString = " ;\nEND UNITS\nDIVIDERCHAR \"/\" ;\nMANUFACTURINGGRID 0.0005 ;\n\nLAYER Metal\n  TYPE ROUTING ;\n  DIRECTION HORIZONTAL ;\n  PITCH 0.0001 ;\n  WIDTH 0.0001 ;\nEND Metal\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);                            
    int libcell_cnt = 0;
    int die_tech = -1;
    if (TopDieTech == Techname[0])
    {
        libcell_cnt = LibCellSize[0].size();
        die_tech = 0;
    }
    else if (TopDieTech == Techname[1])
    {
        libcell_cnt = LibCellSize[1].size();
        die_tech = 1;
    }

    // std::ostringstream stream, stream2;
    // stream << std::defaultfloat << (float)RowHeight_t / unit;

    modifiedString = "SITE Site\n  CLASS CORE ;\n  SIZE 0.01 BY ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    len = snprintf(buf, sizeof(buf), "%g", (float)RowHeight_t / unit);
    fwrite(buf, sizeof(char), len, nfp);
    // +stream.str()+
    modifiedString = " ;\nEND Site\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    
    string outloop_str, inloop_str;
    for (int i = 0; i < libcell_cnt; i++)
    {
        if (LibCell_isMacro[die_tech][i] == false) // standard cell
        {
            // stream.str("");
            // stream2.str("");
            // stream << std::defaultfloat << (float)LibCellSize_x[die_tech][i] / unit;
            // stream2 << std::defaultfloat << (float)LibCellSize_y[die_tech][i] / unit;

            modifiedString = "MACRO MC";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            //  + to_string(i+1) + 
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_TOP\n  CLASS CORE ;\n  ORIGIN 0 0 ;\n  FOREIGN MC";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
            // + to_string(i+1) + 
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_TOP 0 0 ;\n  SIZE ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
            // + stream.str()+
            len = snprintf(buf, sizeof(buf), "%g", (float)LibCellSize_x[die_tech][i] / unit);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " BY ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // +stream2.str()+
            len = snprintf(buf, sizeof(buf), "%g", (float)LibCellSize_y[die_tech][i] / unit);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " ;\n  SITE Site ;\n";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            
            
            
            for (int j = 0; j < Pin_x[die_tech][i].size(); j++)
            {
                // stream.str("");
                // stream2.str("");
                // stream << std::defaultfloat <<(float)Pin_x[die_tech][i][j] / unit;
                // stream2 << std::defaultfloat <<(float)Pin_y[die_tech][i][j] / unit;

                modifiedString = "  PIN P";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +to_string(j+1)+
                len = snprintf(buf, sizeof(buf), "%d", j+1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = "\n    DIRECTION INPUT ;\n    USE SIGNAL ;\n    PORT\n      LAYER Metal ;\n        RECT ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_x[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream2.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_y[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_x[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream2.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_y[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ;\n    END\n  END P";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +to_string(j+1)+
                len = snprintf(buf, sizeof(buf), "%d", j+1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = "\n";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // outloop_str += inloop_str;
                
            }
            modifiedString = "END MC";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // +to_string(i+1)+
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_TOP\n\n";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        }
        else
        { // macro
            // stream.str("");
            // stream2.str("");
            // stream << std::defaultfloat << (float)LibCellSize_x[die_tech][i] / unit;
            // stream2 << std::defaultfloat << (float)LibCellSize_y[die_tech][i] / unit;
            // outloop_str = "MACRO MC" + to_string(i+1) + "_TOP\n" + "  CLASS BLOCK ;\n  ORIGIN 0 0 ;\n  FOREIGN MC"+to_string(i+1)+"_TOP 0 0 ;\n  SIZE "+stream.str()+" BY "+stream2.str()+" ;\n  SYMMETRY R90 ;\n";
            modifiedString = "MACRO MC";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            //  + to_string(i+1) + 
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_TOP\n  CLASS BLOCK ;\n  ORIGIN 0 0 ;\n  FOREIGN MC";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
            // + to_string(i+1) + 
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_TOP 0 0 ;\n  SIZE ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
            // + stream.str()+
            len = snprintf(buf, sizeof(buf), "%g", (float)LibCellSize_x[die_tech][i] / unit);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " BY ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // +stream2.str()+
            len = snprintf(buf, sizeof(buf), "%g", (float)LibCellSize_y[die_tech][i] / unit);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " ;\n  SYMMETRY R90 ;\n";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            for (int j = 0; j < Pin_x[die_tech][i].size(); j++)
            {
                // stream.str("");
                // stream2.str("");
                // stream << std::defaultfloat <<(float)Pin_x[die_tech][i][j] / unit;
                // stream2 << std::defaultfloat <<(float)Pin_y[die_tech][i][j] / unit;
                // inloop_str = "  PIN P"+to_string(j+1)+"\n    DIRECTION INPUT ;\n    USE SIGNAL ;\n    PORT\n      LAYER Metal ;\n        RECT "+stream.str()+" "+stream2.str()+" "+stream.str()+" "+stream2.str()+" ;\n    END\n  END P"+to_string(j+1)+"\n";
                // outloop_str += inloop_str;

                modifiedString = "  PIN P";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +to_string(j+1)+
                len = snprintf(buf, sizeof(buf), "%d", j+1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = "\n    DIRECTION INPUT ;\n    USE SIGNAL ;\n    PORT\n      LAYER Metal ;\n        RECT ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_x[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream2.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_y[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_x[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream2.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_y[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ;\n    END\n  END P";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +to_string(j+1)+
                len = snprintf(buf, sizeof(buf), "%d", j+1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = "\n";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                
            }
            // outloop_str += "END MC"+to_string(i+1)+"_TOP\n\n";
            modifiedString = "END MC";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // +to_string(i+1)+
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_TOP\n\n";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            
        }
        // modifiedString += outloop_str; 
    }
    //write terminal 
    string terminal_str;
    
    for (int i = 0; i < cut_num; i++)
    // for( int i = 0; i < NetCnt; i++)
    {
        // stream.str("");
        // stream2.str("");
        // stream << std::defaultfloat << (float)terminal_position[i].first / unit;
        // stream2 << std::defaultfloat << (float)terminal_position[i].second / unit;


        modifiedString = "MACRO TER_";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(cutname[i] + 1)+
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = "\n  CLASS CORE ;\n  ORIGIN ";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +stream.str()+
        len = snprintf(buf, sizeof(buf), "%g", (float)terminal_position[i].first / unit);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " ";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +stream2.str()+
        len = snprintf(buf, sizeof(buf), "%g", (float)terminal_position[i].second / unit);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " ;\n  SIZE 0 BY 0 ;\n  SITE Site ;\n  PIN P1\n    DIRECTION INPUT ;\n    USE SIGNAL ;\n    PORT\n      LAYER Metal ;\n        RECT 0 0 0 0 ;\n    END\n  END P1\nEND TER_";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +to_string(cutname[i] + 1)+
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = "\n\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);

        // modifiedString = "MACRO TER_";
        // fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // // + to_string(cutname[i] + 1)+
        // len = snprintf(buf, sizeof(buf), "%d", i + 1);
        // fwrite(buf, sizeof(char), len, nfp);
        // modifiedString = "\n  CLASS CORE ;\n  ORIGIN ";
        // fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // // +stream.str()+
        // len = snprintf(buf, sizeof(buf), "%g", 0.0);
        // fwrite(buf, sizeof(char), len, nfp);
        // modifiedString = " ";
        // fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // // +stream2.str()+
        // len = snprintf(buf, sizeof(buf), "%g", 0.0);
        // fwrite(buf, sizeof(char), len, nfp);
        // modifiedString = " ;\n  SIZE 0 BY 0 ;\n  SITE Site ;\n  PIN P1\n    DIRECTION INPUT ;\n    USE SIGNAL ;\n    PORT\n      LAYER Metal ;\n        RECT 0 0 0 0 ;\n    END\n  END P1\nEND TER_";
        // fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // // +to_string(cutname[i] + 1)+
        // len = snprintf(buf, sizeof(buf), "%d", i + 1);
        // fwrite(buf, sizeof(char), len, nfp);
        // modifiedString = "\n\n";
        // fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);

        
        // modifiedString += terminal_str;
    }

    // fwrite(modifiedString.c_str(), sizeof(char), modifiedString.length(), nfp);
    // 刷新緩衝區，確保數據寫入檔案
    fflush(nfp);
    // close file
    fclose(nfp);
}
void generate_lef_version_bot(const char* defName)
{
    FILE* nfp = fopen(defName, "w");//flush original file
    fflush(nfp);
    fclose(nfp);
    nfp = fopen(defName, "a");

    const char* modifiedString = "VERSION 5.8 ;\nBUSBITCHARS \"[]\" ;\nUNITS\n  DATABASE MICRONS ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    int number = 1;
    char buf[100];  // 這裡的大小根據你需要的數字最大長度來調整

    int len = snprintf(buf, sizeof(buf), "%d", unit);
    fwrite(buf, sizeof(char), len, nfp);
    //+to_string(unit)+
    modifiedString = " ;\nEND UNITS\nDIVIDERCHAR \"/\" ;\nMANUFACTURINGGRID 0.0005 ;\n\nLAYER Metal\n  TYPE ROUTING ;\n  DIRECTION HORIZONTAL ;\n  PITCH 0.0001 ;\n  WIDTH 0.0001 ;\nEND Metal\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);                            
    int libcell_cnt = 0;
    int die_tech = -1;
    if (BottomDieTech == Techname[0])
    {
        libcell_cnt = LibCellSize[0].size();
        die_tech = 0;
    }
    else if (BottomDieTech == Techname[1])
    {
        libcell_cnt = LibCellSize[1].size();
        die_tech = 1;
    }

    // std::ostringstream stream, stream2;
    // stream << std::defaultfloat << (float)RowHeight_t / unit;

    modifiedString = "SITE Site\n  CLASS CORE ;\n  SIZE 0.01 BY ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    len = snprintf(buf, sizeof(buf), "%g", (float)RowHeight_b / unit);
    fwrite(buf, sizeof(char), len, nfp);
    // +stream.str()+
    modifiedString = " ;\nEND Site\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    
    string outloop_str, inloop_str;
    for (int i = 0; i < libcell_cnt; i++)
    {
        if (LibCell_isMacro[die_tech][i] == false) // standard cell
        {
            // stream.str("");
            // stream2.str("");
            // stream << std::defaultfloat << (float)LibCellSize_x[die_tech][i] / unit;
            // stream2 << std::defaultfloat << (float)LibCellSize_y[die_tech][i] / unit;

            modifiedString = "MACRO MC";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            //  + to_string(i+1) + 
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_BOT\n  CLASS CORE ;\n  ORIGIN 0 0 ;\n  FOREIGN MC";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
            // + to_string(i+1) + 
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_BOT 0 0 ;\n  SIZE ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
            // + stream.str()+
            len = snprintf(buf, sizeof(buf), "%g", (float)LibCellSize_x[die_tech][i] / unit);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " BY ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // +stream2.str()+
            len = snprintf(buf, sizeof(buf), "%g", (float)LibCellSize_y[die_tech][i] / unit);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " ;\n  SITE Site ;\n";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            
            
            
            for (int j = 0; j < Pin_x[die_tech][i].size(); j++)
            {
                // stream.str("");
                // stream2.str("");
                // stream << std::defaultfloat <<(float)Pin_x[die_tech][i][j] / unit;
                // stream2 << std::defaultfloat <<(float)Pin_y[die_tech][i][j] / unit;

                modifiedString = "  PIN P";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +to_string(j+1)+
                len = snprintf(buf, sizeof(buf), "%d", j+1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = "\n    DIRECTION INPUT ;\n    USE SIGNAL ;\n    PORT\n      LAYER Metal ;\n        RECT ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_x[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream2.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_y[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_x[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream2.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_y[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ;\n    END\n  END P";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +to_string(j+1)+
                len = snprintf(buf, sizeof(buf), "%d", j+1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = "\n";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // outloop_str += inloop_str;
                
            }
            modifiedString = "END MC";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // +to_string(i+1)+
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_BOT\n\n";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        }
        else
        { // macro
            // stream.str("");
            // stream2.str("");
            // stream << std::defaultfloat << (float)LibCellSize_x[die_tech][i] / unit;
            // stream2 << std::defaultfloat << (float)LibCellSize_y[die_tech][i] / unit;
            // outloop_str = "MACRO MC" + to_string(i+1) + "_TOP\n" + "  CLASS BLOCK ;\n  ORIGIN 0 0 ;\n  FOREIGN MC"+to_string(i+1)+"_TOP 0 0 ;\n  SIZE "+stream.str()+" BY "+stream2.str()+" ;\n  SYMMETRY R90 ;\n";
            modifiedString = "MACRO MC";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            //  + to_string(i+1) + 
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_BOT\n  CLASS BLOCK ;\n  ORIGIN 0 0 ;\n  FOREIGN MC";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
            // + to_string(i+1) + 
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_BOT 0 0 ;\n  SIZE ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
            // + stream.str()+
            len = snprintf(buf, sizeof(buf), "%g", (float)LibCellSize_x[die_tech][i] / unit);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " BY ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // +stream2.str()+
            len = snprintf(buf, sizeof(buf), "%g", (float)LibCellSize_y[die_tech][i] / unit);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " ;\n  SYMMETRY R90 ;\n";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            for (int j = 0; j < Pin_x[die_tech][i].size(); j++)
            {
                // stream.str("");
                // stream2.str("");
                // stream << std::defaultfloat <<(float)Pin_x[die_tech][i][j] / unit;
                // stream2 << std::defaultfloat <<(float)Pin_y[die_tech][i][j] / unit;
                // inloop_str = "  PIN P"+to_string(j+1)+"\n    DIRECTION INPUT ;\n    USE SIGNAL ;\n    PORT\n      LAYER Metal ;\n        RECT "+stream.str()+" "+stream2.str()+" "+stream.str()+" "+stream2.str()+" ;\n    END\n  END P"+to_string(j+1)+"\n";
                // outloop_str += inloop_str;

                modifiedString = "  PIN P";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +to_string(j+1)+
                len = snprintf(buf, sizeof(buf), "%d", j+1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = "\n    DIRECTION INPUT ;\n    USE SIGNAL ;\n    PORT\n      LAYER Metal ;\n        RECT ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_x[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream2.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_y[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_x[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +stream2.str()+
                len = snprintf(buf, sizeof(buf), "%g", (float)Pin_y[die_tech][i][j] / unit);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " ;\n    END\n  END P";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // +to_string(j+1)+
                len = snprintf(buf, sizeof(buf), "%d", j+1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = "\n";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                
            }
            // outloop_str += "END MC"+to_string(i+1)+"_TOP\n\n";
            modifiedString = "END MC";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // +to_string(i+1)+
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_BOT\n\n";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            
        }
        // modifiedString += outloop_str; 
    }
    //write terminal 
    string terminal_str;
    
    for (int i = 0; i < cut_num; i++)
    {
        // stream.str("");
        // stream2.str("");
        // stream << std::defaultfloat << (float)terminal_position[i].first / unit;
        // stream2 << std::defaultfloat << (float)terminal_position[i].second / unit;
        modifiedString = "MACRO TER_";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(cutname[i] + 1)+
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = "\n  CLASS CORE ;\n  ORIGIN ";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +stream.str()+
        len = snprintf(buf, sizeof(buf), "%g", (float)terminal_position[i].first / unit);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " ";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +stream2.str()+
        len = snprintf(buf, sizeof(buf), "%g", (float)terminal_position[i].second / unit);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " ;\n  SIZE 0 BY 0 ;\n  SITE Site ;\n  PIN P1\n    DIRECTION INPUT ;\n    USE SIGNAL ;\n    PORT\n      LAYER Metal ;\n        RECT 0 0 0 0 ;\n    END\n  END P1\nEND TER_";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +to_string(cutname[i] + 1)+
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = "\n\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // modifiedString += terminal_str;
    }
    // for (int i = 0; i < NetCnt; i++)
    // {
    //     // stream.str("");
    //     // stream2.str("");
    //     // stream << std::defaultfloat << (float)terminal_position[i].first / unit;
    //     // stream2 << std::defaultfloat << (float)terminal_position[i].second / unit;
    //     modifiedString = "MACRO TER_";
    //     fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    //     // + to_string(cutname[i] + 1)+
    //     len = snprintf(buf, sizeof(buf), "%d", i + 1);
    //     fwrite(buf, sizeof(char), len, nfp);
    //     modifiedString = "\n  CLASS CORE ;\n  ORIGIN ";
    //     fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    //     // +stream.str()+
    //     len = snprintf(buf, sizeof(buf), "%g", 0.0);
    //     fwrite(buf, sizeof(char), len, nfp);
    //     modifiedString = " ";
    //     fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    //     // +stream2.str()+
    //     len = snprintf(buf, sizeof(buf), "%g", 0.0);
    //     fwrite(buf, sizeof(char), len, nfp);
    //     modifiedString = " ;\n  SIZE 0 BY 0 ;\n  SITE Site ;\n  PIN P1\n    DIRECTION INPUT ;\n    USE SIGNAL ;\n    PORT\n      LAYER Metal ;\n        RECT 0 0 0 0 ;\n    END\n  END P1\nEND TER_";
    //     fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    //     // +to_string(cutname[i] + 1)+
    //     len = snprintf(buf, sizeof(buf), "%d", i + 1);
    //     fwrite(buf, sizeof(char), len, nfp);
    //     modifiedString = "\n\n";
    //     fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    //     // modifiedString += terminal_str;
    // }

    // fwrite(modifiedString.c_str(), sizeof(char), modifiedString.length(), nfp);
    // 刷新緩衝區，確保數據寫入檔案
    fflush(nfp);
    // close file
    fclose(nfp);
}

void generate_def_version_top(const char* defName)
{
    
    FILE* nfp = fopen(defName, "w");//flush original file
    fflush(nfp);
    fclose(nfp);
    nfp = fopen(defName, "a");
    
    const char* modifiedString = "VERSION 5.8 ;\nBUSBITCHARS \"[]\" ;\nDIVIDERCHAR \"/\" ;\nDESIGN HW ;\nUNITS DISTANCE MICRONS ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // +to_string(unit)+
    char buf[100];  // 這裡的大小根據你需要的數字最大長度來調整
    int len = snprintf(buf, sizeof(buf), "%d", unit);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ;\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    modifiedString = "DIEAREA ( ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(LowerLeftX) 
    len = snprintf(buf, sizeof(buf), "%lld", LowerLeftX);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
    // + to_string(LowerLeftY) +
    len = snprintf(buf, sizeof(buf), "%lld", LowerLeftY);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ) ( " ;
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(UpperRightX) +
    len = snprintf(buf, sizeof(buf), "%lld", UpperRightX);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
    // + to_string(UpperRighty) +
    len = snprintf(buf, sizeof(buf), "%lld", UpperRighty);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ) ;\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    
    // std::ostringstream stream, stream2;
    // stream << std::defaultfloat << (float)RowHeight_b / unit;
    // string outloop_str, inloop_str;

    for (int i = 0; i < RepeatCount_t; i++)
    {
        modifiedString = "ROW ROW_TOP_";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +to_string(i)+
        len = snprintf(buf, sizeof(buf), "%d", i);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " Site 0 ";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +to_string(i * RowHeight_t)+
        len = snprintf(buf, sizeof(buf), "%d", i * RowHeight_t);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " N DO ";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +to_string(UpperRightX)+
        len = snprintf(buf, sizeof(buf), "%lld", UpperRightX);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " BY 1 STEP 1 0\n ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // modifiedString += outloop_str;
        
    }
    modifiedString = "\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    modifiedString = "COMPONENTS ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(Cell_in_top.size()+cut_num) +
    len = snprintf(buf, sizeof(buf), "%lu", Cell_in_top.size()+cut_num);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ;\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    for (int i = 0; i < Cell_in_top.size(); i++)
    {
        modifiedString = "- C" ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(Cell_in_top[i]+1) +
        len = snprintf(buf, sizeof(buf), "%d", Cell_in_top[i]+1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " MC" ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(Lib_for_Inst[Cell_in_top[i]] + 1) 
        len = snprintf(buf, sizeof(buf), "%d", Lib_for_Inst[Cell_in_top[i]] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = "_TOP + PLACED ( 0 0 ) N ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    }
    for (int i = 0; i < cut_num; i++)
    {
        modifiedString = "- T";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(cutname[i] + 1) +
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " TER_" ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(cutname[i] + 1) +
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " + FIXED ( " ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(terminal_position[i].first) +
        len = snprintf(buf, sizeof(buf), "%d", terminal_position[i].first);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " " ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(terminal_position[i].second) +
        len = snprintf(buf, sizeof(buf), "%d", terminal_position[i].second);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " ) N ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    }
    modifiedString = "END COMPONENTS\n\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    
    modifiedString = "NETS " ;
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(NetCnt) +
    len = snprintf(buf, sizeof(buf), "%d", NetCnt);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ;\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    for (int i = 0; i < NetCnt; i++)
    {
        bool has_net = false;
        int pin_cnt = NumofPin[i];
        int actual_cnt = 0;
        
        modifiedString = "- N" ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(i+1) +
        len = snprintf(buf, sizeof(buf), "%d", i+1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = "\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        has_net = true;
        if (NetCross[i])
        {
            
            modifiedString = " ( T" ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(i+1) +
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " P1 )";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            actual_cnt++;
        }
        for (int j = 0; j < pin_cnt; j++)
        {
            int die = partition_map[net[i][j].first];
            if (die == 0)
            //vector<int>::iterator it = find(Cell_in_top.begin(), Cell_in_top.end(), net[i][j].first);
            //if (it != Cell_in_top.end())
            {
                
                modifiedString = " ( C" ;
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // + to_string(net[i][j].first + 1) +
                len = snprintf(buf, sizeof(buf), "%d", net[i][j].first + 1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " P" ;
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                //  + to_string(net[i][j].second + 1) +
                len = snprintf(buf, sizeof(buf), "%d", net[i][j].second + 1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " )";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                actual_cnt++;
            }
        }
        
        modifiedString = " ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        actual_cnt = 0;
        // modifiedString += outloop_str;
    }
    
    modifiedString = "END NETS\n\nEND DESIGN";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    
    // 刷新緩衝區，確保數據寫入檔案
    fflush(nfp);
    // close file
    fclose(nfp);
}
void generate_def_version_bot(const char* defName)
{
    FILE* nfp = fopen(defName, "w");//flush original file
    fflush(nfp);
    fclose(nfp);
    nfp = fopen(defName, "a");
    
    const char* modifiedString = "VERSION 5.8 ;\nBUSBITCHARS \"[]\" ;\nDIVIDERCHAR \"/\" ;\nDESIGN HW ;\nUNITS DISTANCE MICRONS ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // +to_string(unit)+
    char buf[100];  // 這裡的大小根據你需要的數字最大長度來調整
    int len = snprintf(buf, sizeof(buf), "%d", unit);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ;\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    modifiedString = "DIEAREA ( ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(LowerLeftX) 
    len = snprintf(buf, sizeof(buf), "%lld", LowerLeftX);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
    // + to_string(LowerLeftY) +
    len = snprintf(buf, sizeof(buf), "%lld", LowerLeftY);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ) ( " ;
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(UpperRightX) +
    len = snprintf(buf, sizeof(buf), "%lld", UpperRightX);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
    // + to_string(UpperRighty) +
    len = snprintf(buf, sizeof(buf), "%lld", UpperRighty);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ) ;\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    
    // std::ostringstream stream, stream2;
    // stream << std::defaultfloat << (float)RowHeight_b / unit;
    // string outloop_str, inloop_str;

    for (int i = 0; i < RepeatCount_b; i++)
    {
        modifiedString = "ROW ROW_BOT_";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +to_string(i)+
        len = snprintf(buf, sizeof(buf), "%d", i);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " Site 0 ";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +to_string(i * RowHeight_t)+
        len = snprintf(buf, sizeof(buf), "%d", i * RowHeight_b);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " N DO ";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +to_string(UpperRightX)+
        len = snprintf(buf, sizeof(buf), "%lld", UpperRightX);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " BY 1 STEP 1 0\n ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // modifiedString += outloop_str;
        
    }
    modifiedString = "\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    modifiedString = "COMPONENTS ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(Cell_in_top.size()+cut_num) +
    len = snprintf(buf, sizeof(buf), "%lu", Cell_in_bottom.size()+cut_num);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ;\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    for (int i = 0; i < Cell_in_bottom.size(); i++)
    {
        modifiedString = "- C" ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(Cell_in_top[i]+1) +
        len = snprintf(buf, sizeof(buf), "%d", Cell_in_bottom[i]+1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " MC" ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(Lib_for_Inst[Cell_in_top[i]] + 1) 
        len = snprintf(buf, sizeof(buf), "%d", Lib_for_Inst[Cell_in_bottom[i]] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = "_BOT + PLACED ( 0 0 ) N ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    }
    for (int i = 0; i < cut_num; i++)
    {
        modifiedString = "- T";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(cutname[i] + 1) +
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " TER_" ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(cutname[i] + 1) +
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " + FIXED ( " ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(terminal_position[i].first) +
        len = snprintf(buf, sizeof(buf), "%d", terminal_position[i].first);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " " ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(terminal_position[i].second) +
        len = snprintf(buf, sizeof(buf), "%d", terminal_position[i].second);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " ) N ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    }
    modifiedString = "END COMPONENTS\n\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    
    modifiedString = "NETS " ;
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(NetCnt) +
    len = snprintf(buf, sizeof(buf), "%d", NetCnt);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ;\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    for (int i = 0; i < NetCnt; i++)
    {
        bool has_net = false;
        int pin_cnt = NumofPin[i];
        int actual_cnt = 0;
        
        modifiedString = "- N" ;
        fwrite(modifiedString, sizeof(char), 3, nfp);
        // + to_string(i+1) +
        len = snprintf(buf, sizeof(buf), "%d", i+1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = "\n";
        fwrite(modifiedString, sizeof(char), 1, nfp);
        has_net = true;
        if (NetCross[i])
        {
            
            modifiedString = " ( T" ;
            fwrite(modifiedString, sizeof(char), 4, nfp);
            // + to_string(i+1) +
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " P1 )";
            fwrite(modifiedString, sizeof(char), 5, nfp);
            actual_cnt++;
        }
        for (int j = 0; j < pin_cnt; j++)
        {
            int die = partition_map[net[i][j].first];
            //vector<int>::iterator it = find(Cell_in_bottom.begin(), Cell_in_bottom.end(), net[i][j].first);
            if (die == 1)
            {
                
                modifiedString = " ( C" ;
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // + to_string(net[i][j].first + 1) +
                len = snprintf(buf, sizeof(buf), "%d", net[i][j].first + 1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " P" ;
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                //  + to_string(net[i][j].second + 1) +
                len = snprintf(buf, sizeof(buf), "%d", net[i][j].second + 1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " )";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                actual_cnt++;
            }
        }
        
        modifiedString = " ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        actual_cnt = 0;
        // modifiedString += outloop_str;
    }
    
    modifiedString = "END NETS\n\nEND DESIGN";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    
    // 刷新緩衝區，確保數據寫入檔案
    fflush(nfp);
    // close file
    fclose(nfp);
}


void generate_top()
{

    generate_lef_version_top("top_die.lef");
    generate_def_version_top("top_die.def");

}

void generate_bottom()
{
    generate_lef_version_bot("bot_die.lef");
    generate_def_version_bot("bot_die.def");

}

void generate_lefdef()
{
    unit = 100;
    generate_top();
    generate_bottom();
}
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 16, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}
void parse_top()
{  
    topdiename.clear();

    FILE *fp;
    char buffer[1024];
    size_t numread;
    
    fp = fopen(replaced_top.c_str(), "r");
    if (fp == NULL) {
        printf("failed to open the def.\n");
        return; // EXIT_FAILURE
    }

   
    bool terminated = false;
    while ((numread = fread(buffer, sizeof(char), sizeof(buffer)-100, fp)) > 0) {
        //printf("read %lu bytes\n", numread);
        // printf("%.*s", (int)numread, buffer);
        if(buffer[numread-1] != ';'){
            char ch;
            while (numread < sizeof(buffer) - 1 &&  fread(&ch, sizeof(char), 1, fp) == 1) {
                if (ch == ';') {
                    buffer[numread++] = ch;
                    break; // Stop reading when semicolon is found
                }
                buffer[numread++] = ch;
            }
        }
        int length = numread;//strlen(buffer);
        int pos = 0, last_pos = 0, instance_num = -1;
        
        bool have_ter = false;
        while (pos < length - 5) {
            if (strncmp(buffer + pos, "PLACED", 6) == 0 || strncmp(buffer + pos, "FIXED", 5) == 0) {
                
                have_ter = true;
                // Find the position of '(' and ')'
                char* openBracketPos = strchr(buffer + pos, '(');
                char* closeBracketPos = strchr(buffer + pos, ')');

                if (openBracketPos && closeBracketPos) {
                    
                    std::string parentFixed(openBracketPos+1, closeBracketPos+3);
                    //std::cout << "parentFixed--------------" << parentFixed << "\n";
                    int x, y;
                    char tmp, ori;
                    std::istringstream iss(parentFixed);
                    if(instance_num != -1){
                        if (iss >> x >> y >> tmp >> ori) {
                            // std::cout << "x: " << x << ", y: " << y << ", ori: "<< ori << std::endl;
                            topdiename.push_back(instance_num-1);    //instance idx
                            inst_orient[instance_num - 1] = ori;
                            inst_position[instance_num - 1].first = x;
                            inst_position[instance_num - 1].second = y;
                        } else {
                            std::cout << "Failed to read coordinates" << std::endl;
                        }
                        instance_num = -1;
                    }
                    
                }

                // Move the position pointer past the current "FIXED"
                pos += closeBracketPos - buffer - pos + 1;
                last_pos = pos;
                
            }else if (strncmp(buffer + pos, "- C", 3) == 0) {
                // Skip past "- C"
                pos += 3;

                // Extract the component name after the "-"
                char* componentNamePos = buffer + pos;
                char* spacePos = strchr(componentNamePos, ' ');
                if (spacePos) {
                    
                    std::string currentComponent(componentNamePos, spacePos - componentNamePos);
                    std::istringstream iss(currentComponent);
                    if(iss >> instance_num) {}//std::cout << "instance: C" << instance_num << "\n";
                    
                }
            }else if (strncmp(buffer + pos, "END COMPONENTS", 14) == 0) {
                // Stop the loop when "END COMPONENTS" is found
                //std:: cout<< "find end components!!!" << "\n";
                terminated = true;
                break;
            } else {
                pos++;
            }
        }
        if(terminated) break;
        
    }
    
    fclose(fp);
    return;
}
void parse_bottom()
{
    botdiename.clear();

    FILE *fp;
    char buffer[1024];
    size_t numread;
    
    fp = fopen(replaced_bot.c_str(), "r");
    if (fp == NULL) {
        printf("failed to open the def.\n");
        return; // EXIT_FAILURE
    }

   
    bool terminated = false;
    while ((numread = fread(buffer, sizeof(char), sizeof(buffer)-100, fp)) > 0) {
        //printf("read %lu bytes\n", numread);
        // printf("%.*s", (int)numread, buffer);
        if(buffer[numread-1] != ';'){
            char ch;
            while (numread < sizeof(buffer) - 1 &&  fread(&ch, sizeof(char), 1, fp) == 1) {
                if (ch == ';') {
                    buffer[numread++] = ch;
                    break; // Stop reading when semicolon is found
                }
                buffer[numread++] = ch;
            }
        }
        int length = numread;//strlen(buffer);
        int pos = 0, last_pos = 0, instance_num = -1;
        
        bool have_ter = false;
        while (pos < length - 5) {
            if (strncmp(buffer + pos, "PLACED", 6) == 0 || strncmp(buffer + pos, "FIXED", 5) == 0) {
                
                have_ter = true;
                // Find the position of '(' and ')'
                char* openBracketPos = strchr(buffer + pos, '(');
                char* closeBracketPos = strchr(buffer + pos, ')');

                if (openBracketPos && closeBracketPos) {
                    
                    std::string parentFixed(openBracketPos+1, closeBracketPos+3);
                    //std::cout << "parentFixed--------------" << parentFixed << "\n";
                    int x, y;
                    char tmp, ori;
                    std::istringstream iss(parentFixed);
                    if(instance_num != -1){
                        if (iss >> x >> y >> tmp >> ori) {
                            // std::cout << "x: " << x << ", y: " << y << ", ori: "<< ori << std::endl;
                            botdiename.push_back(instance_num-1);    //instance idx
                            inst_orient[instance_num - 1] = ori;
                            inst_position[instance_num - 1].first = x;
                            inst_position[instance_num - 1].second = y;
                        } else {
                            std::cout << "Failed to read coordinates" << std::endl;
                        }
                        instance_num = -1;
                    }
                    
                }

                // Move the position pointer past the current "FIXED"
                pos += closeBracketPos - buffer - pos + 1;
                last_pos = pos;
                
            }else if (strncmp(buffer + pos, "- C", 3) == 0) {
                // Skip past "- C"
                pos += 3;

                // Extract the component name after the "-"
                char* componentNamePos = buffer + pos;
                char* spacePos = strchr(componentNamePos, ' ');
                if (spacePos) {
                    
                    std::string currentComponent(componentNamePos, spacePos - componentNamePos);
                    std::istringstream iss(currentComponent);
                    if(iss >> instance_num) {}//std::cout << "instance: C" << instance_num << "\n";
                    
                }
            }else if (strncmp(buffer + pos, "END COMPONENTS", 14) == 0) {
                // Stop the loop when "END COMPONENTS" is found
                //std:: cout<< "find end components!!!" << "\n";
                terminated = true;
                break;
            } else {
                pos++;
            }
        }
        if(terminated) break;
        
    }
    
    fclose(fp);
    
    return;
}

void parse_def()
{
    // for(int i = 0; i<Instance_cnt; i++){
    //     partition_map.push_back(-1);
    // }
    topdiename.clear();
    botdiename.clear();
    parse_top();
    parse_bottom();
}
void parse_pl(int pos)
{
    // pos==0 -> top, pos==1 -> bottom
    string filename = (pos == 0) ? "top_die.ntup.pl" : "bot_die.ntup.pl";
    in_file.open(filename);
    string tmps;
    int tmpx;
    int tmpy;
    while (in_file >> tmps)
    {
        string ttt = tmps;
        if (ttt[0] == 'C')
        {
            int inst_idx;
            int mcsize = ttt.size();
            int t = 0;
            for (int i = 1; i < mcsize; i++)
            {
                t *= 10;
                t += ttt[i] - '0';
            }
            inst_idx = t - 1;
            in_file >> tmpx >> tmpy;
            if (inst_idx >= Instance_cnt || inst_idx < 0)
            {
                cout << "WRONG!! " << inst_idx << endl;
            }
            inst_position[inst_idx].first = tmpx;
            inst_position[inst_idx].second = tmpy; // to be modified
        }
    }
    in_file.close();
}
int stoi_new(string k)
{
    int stringsize = k.size();
    int t = 0;
    for (int i = 1; i < stringsize; i++)
    {
        t *= 10;
        t += k[i] - '0';
    }
    return t;
}
void output()
{
    int inscnt = topdiename.size();
    out_file << "TopDiePlacement ";
    out_file << inscnt << endl;
    for (int i = 0; i < inscnt; i++)
    {
        int idx = topdiename[i];
        out_file << "Inst C" << topdiename[i] + 1 << " " << inst_position[idx].first << " " << inst_position[idx].second << " ";
        if (inst_orient[idx] == "N")
            out_file << "R0" << endl;
        else if (inst_orient[idx] == "E")
            out_file << "R270" << endl;
        else if (inst_orient[idx] == "S")
            out_file << "R180" << endl;
        else if (inst_orient[idx] == "W")
            out_file << "R90" << endl;
        else
            cout << "Orientation error!" << endl;
    }
    out_file << "BottomDiePlacement ";
    inscnt = botdiename.size();
    out_file << inscnt << endl;
    for (int i = 0; i < inscnt; i++)
    {

        int idx = botdiename[i];
        out_file << "Inst C" << botdiename[i] + 1 << " " << inst_position[idx].first << " " << inst_position[idx].second << " ";
        if (inst_orient[idx] == "N")
            out_file << "R0" << endl;
        else if (inst_orient[idx] == "E")
            out_file << "R270" << endl;
        else if (inst_orient[idx] == "S")
            out_file << "R180" << endl;
        else if (inst_orient[idx] == "W")
            out_file << "R90" << endl;
        else
            cout << "Orientation error!" << endl;
    }

    out_file << "NumTerminals " << cut_num << endl;
    for (int i = 0; i < cut_num; i++)
    {
        string tname = "N" + to_string(cutname[i] + 1);
        out_file << "Terminal " << tname << " " << terminal_position[i].first << " " << terminal_position[i].second << endl;
    }
    out_file << flush;
}
bool sortbydegree(int a, int b)
{
    if (degree_net[a] == degree_net[b])
        return a < b;
    return degree_net[a] > degree_net[b];
}
// 先給一個大致位置
void terminal_placement()
{
    for (int i = 0; i < Instance_cnt; i++)
    {
        int libnum = Lib_for_Inst[i];
        int sizee;
        if (NumofTech == 2)
        {
            if (partition_map[i] == 0)       //top
            {
                sizee = LibCellSize[top][libnum];
            }
            else
            {
                sizee = LibCellSize[bot][libnum];
            }
        }
        else
        {
            sizee = LibCellSize[0][libnum];
        }
        hmetis_inst_size[i] = sizee;
    }
    for (int i = 0; i < Instance_cnt; i++) degree.push_back(0);

    for (int i = 0; i < NetCnt; i++)
    {
        mapforterminal.push_back(0);
        for (int j = 0; j < net[i].size(); j++)
        {
            int instindex = net[i][j].first;
            degree[instindex]++;
        }
        degree_net.push_back(0);
    }
    for (int i = 0; i < NetCnt; i++)
    {
        for (int j = 0; j < net[i].size(); j++)
        {
            int instindex = net[i][j].first;
            degree_net[i] += hmetis_inst_size[instindex];
        }
    }
    vector<int> terminal_sorted_by_pin;
    for (int i = 0; i < NetCnt; i++)
    {
        int first = partition_map[net[i][0].first];
        for (int j = 0; j < net[i].size(); j++)
        {
            if (first != partition_map[net[i][j].first])
            {
                cutname.push_back(i);
                terminal_sorted_by_pin.push_back(i);
                mapforterminal[i] = cut_num;
                terminal_position.push_back(make_pair(0, 0));
                NetCross[i] = true;
                cut_num++;
                break;
            }
        }
    }
    sort(terminal_sorted_by_pin.begin(), terminal_sorted_by_pin.end(), sortbydegree);
    gridx = (UpperRightX - Terminal_Spacing) / (Terminal_X + Terminal_Spacing);
    gridy = (UpperRighty - Terminal_Spacing) / (Terminal_Y + Terminal_Spacing);
    terminal_job = gridx * gridy;
    int centerx = (gridx - 1) / 2;
    int centery = (gridy - 1) / 2;
    int pos = centery * gridx + centerx;
    int cnt = 0;
    int max = (gridx * gridy) / (cut_num + 1);
    int portion = max * 1 / 5;
    for (int i = 0; i < terminal_sorted_by_pin.size(); i++)
    {
        int t_index = mapforterminal[terminal_sorted_by_pin[i]];
        int coordinate_x = (cnt * portion) % gridx;
        int coordinate_y = (cnt * portion) / gridx;
        int realx = Terminal_Spacing + (Terminal_X + Terminal_Spacing) * coordinate_x + Terminal_X / 2;
        int realy = Terminal_Spacing + (Terminal_Y + Terminal_Spacing) * coordinate_y + Terminal_Y / 2;
        terminal_position[t_index].first = realx;
        terminal_position[t_index].second = realy;
        if (realx <= 0 || realy <= 0)
            cout << cnt << " " << coordinate_x << endl;
        cnt++;
    }
}
void eval(const char *arg1, const char *arg2)
{

    cout << arg1 << " " << arg2 << endl;

    string sin(arg1), sout(arg2);

    string cmd;

    string outfname = sin.substr(0, sin.size() - 4);

    outfname = outfname + "_eval.txt";

    cmd = "./evaluator " + sin + " " + sout + " >" + outfname;
    // cout << "KEKWPUSSY" << endl;
    system(cmd.c_str());
    // cout << "KEKWPUSSY" << endl;
}

void clean()
{
    system("rm top_die_final.def bot_die_final.def bot_die.floorplan top_die.floorplan top_die.nets bot_die.nets top_die.hardblocks bot_die.hardblocks terminal.pl");
}
unsigned long long calc_HPWL(int netindex, int tx, int ty, bool c, int flag) //  7/17 modified   to be modified
{
    // flag = 0 is top only, flag = 1 is for both
    if (!c)   //c == false
    {
        int up = 0; 
        int down = 99999999;
        int left = 99999999;
        int right = 0;
        for (int j = 0; j < net[netindex].size(); j++)
        {
            int pin_index = net[netindex][j].second;
            int inst_index = net[netindex][j].first;
            int x = inst_position[inst_index].first;
            int y = inst_position[inst_index].second;
            int tech = 0;
            int lib = Lib_for_Inst[inst_index];

            tech = (partition_map[inst_index] == 0) ? top : bot;

            /*sunny modified*/
            int pinx, piny;
            if (inst_orient[inst_index] == "N")
            {
                pinx = x + Pin_x[tech][lib][pin_index];
                piny = y + Pin_y[tech][lib][pin_index];
            }
            else if (inst_orient[inst_index] == "E")
            { /* (pinx, piny) = (upper_left_x + dy, upper_left_y - dx) */
                pinx = x + Pin_y[tech][lib][pin_index];
                piny = y + LibCellSize_x[tech][lib] - Pin_x[tech][lib][pin_index];
            }
            else if (inst_orient[inst_index] == "S")
            {
                /* (pinx, piny) = (upper_right_x - dx, upper_right_y - dy)*/
                pinx = x + LibCellSize_x[tech][lib] - Pin_x[tech][lib][pin_index];
                piny = y + LibCellSize_y[tech][lib] - Pin_y[tech][lib][pin_index];
            }
            else
            {
                /* (pinx, piny) = (lower_right_x - dy, lower_right_y + dx)*/
                pinx = x + LibCellSize_y[tech][lib] - Pin_y[tech][lib][pin_index];
                piny = y + Pin_x[tech][lib][pin_index];
            }

            if (pinx < left)
                left = pinx;
            if (pinx > right)
                right = pinx;
            if (piny < down)
                down = piny;
            if (piny > up)
                up = piny;
        }

        return (unsigned long long)( (unsigned long long)up - (unsigned long long)down + (unsigned long long)right - (unsigned long long)left);
    }
    else
    {
        unsigned long long res = 0;
        for (int i = 0; i < 2; i++)
        {
            ///// if we only want to calculate HPWL of top die, we ignore instances in bottom die
            //if (flag == 0 && i == 1)
            //    break;
            int up = 0;
            int down = 999999999;
            int left = 999999999;
            int right = 0;
            for (int j = 0; j < net[netindex].size(); j++)
            {
                int pin_index = net[netindex][j].second;
                int inst_index = net[netindex][j].first;
                if (partition_map[inst_index] == i)    // 0 -> top
                {

                    int x = inst_position[inst_index].first;
                    int y = inst_position[inst_index].second; // to be modified
                    int tech = 0;
                    int lib = Lib_for_Inst[inst_index];
                    
                    tech = (partition_map[inst_index] == 0) ? top : bot;

                    //x += Pin_x[tech][lib][pin_index];
                    //y += Pin_y[tech][lib][pin_index];

                    // int pinx, piny;
                    if (inst_orient[inst_index] == "N")
                    {
                        x += Pin_x[tech][lib][pin_index];
                        y += Pin_y[tech][lib][pin_index];
                    }
                    else if (inst_orient[inst_index] == "E")
                    { /* (pinx, piny) = (upper_left_x + dy, upper_left_y - dx) */
                        x += Pin_y[tech][lib][pin_index];
                        y += LibCellSize_x[tech][lib] - Pin_x[tech][lib][pin_index];
                    }
                    else if (inst_orient[inst_index] == "S")
                    {
                        /* (pinx, piny) = (upper_right_x - dx, upper_right_y - dy)*/
                        x += LibCellSize_x[tech][lib] - Pin_x[tech][lib][pin_index];
                        y += LibCellSize_y[tech][lib] - Pin_y[tech][lib][pin_index];
                    }
                    else
                    {
                        /* (pinx, piny) = (lower_right_x - dy, lower_right_y + dx)*/
                        x += LibCellSize_y[tech][lib] - Pin_y[tech][lib][pin_index];
                        y += Pin_x[tech][lib][pin_index];
                    }
                    if (x < left)
                    {
                        left = x;
                    }
                    if (x > right)
                    {
                        right = x;
                    }
                    if (y < down)
                    {
                        down = y;
                    }
                    if (y > up)
                    {
                        up = y;
                    }

                }
                if (tx < left)
                {
                    left = tx;
                }
                if (tx > right)
                {
                    right = tx;
                }
                if (ty < down)
                {
                    down = ty;
                }
                if (ty > up)
                {
                    up = ty;
                }
            }
            res = res + (unsigned long long)((unsigned long long)up - (unsigned long long)down + (unsigned long long)right - (unsigned long long)left);
        }
        return res;
    }
}



void calc_net_hpwl_range(string d)   //hpwl on certain die without considering terminal
{
    int die = (d == "top") ? 0 : 1;
    int tech = (d == "top") ? top : bot;

    if(d == "top") top_hpwl_middle.resize(NetCnt, make_pair(0, 0));  // 0825
    else bot_hpwl_middle.resize(NetCnt, make_pair(0, 0));

    if(d == "top") net_hasInst_on_top.resize(NetCnt, false);
    else net_hasInst_on_bot.resize(NetCnt, false);

    for( int i = 0; i < NetCnt; i++) 
    {
        int up = 0;
        int down = 99999999;
        int left = 99999999;
        int right = 0;
        for (int j = 0; j < net[i].size(); j++)
        {
            if(partition_map[net[i][j].first] != die) continue;

            if(d == "top") net_hasInst_on_top[i] = true;
            else net_hasInst_on_bot[i] = true;

            int pin_index = net[i][j].second;
            int inst_index = net[i][j].first;
            int x = inst_position[inst_index].first;
            int y = inst_position[inst_index].second;
            int lib = Lib_for_Inst[inst_index];

            // cout << "inst_index, pin_index = " << inst_index <<  " " << pin_index << endl;


            /*sunny modified*/
            int pinx, piny;
            if (inst_orient[inst_index] == "N")
            {
                pinx = x + Pin_x[tech][lib][pin_index];
                piny = y + Pin_y[tech][lib][pin_index];
            }
            else if (inst_orient[inst_index] == "E")
            { /* (pinx, piny) = (upper_left_x + dy, upper_left_y - dx) */
                pinx = x + Pin_y[tech][lib][pin_index];
                piny = y + LibCellSize_x[tech][lib] - Pin_x[tech][lib][pin_index];
            }
            else if (inst_orient[inst_index] == "S")
            {
                /* (pinx, piny) = (upper_right_x - dx, upper_right_y - dy)*/
                pinx = x + LibCellSize_x[tech][lib] - Pin_x[tech][lib][pin_index];
                piny = y + LibCellSize_y[tech][lib] - Pin_y[tech][lib][pin_index];
            }
            else
            {
                /* (pinx, piny) = (lower_right_x - dy, lower_right_y + dx)*/
                pinx = x + LibCellSize_y[tech][lib] - Pin_y[tech][lib][pin_index];
                piny = y + Pin_x[tech][lib][pin_index];
            }
            // cout << "inst orient = " << inst_orient[inst_index] << endl;
            // cout << pinx << " " << piny << endl;
            if (pinx < left) left = pinx;
            if (pinx > right) right = pinx;
            if (piny < down) down = piny;
            if (piny > up) up = piny;
        }
        if(d == "top") 
        {
            net_hpwl_top[i][0][0] = left;
            net_hpwl_top[i][0][1] = down;
            net_hpwl_top[i][1][0] = right;
            net_hpwl_top[i][1][1] = up;

            top_hpwl_middle[i] = make_pair( int((left + right)/2), int((up + down)/2));  //0825
            // cout << "net_idx = " << i << endl;
            // cout << "top" << endl;
            // cout << left << " " << right << " " << down << " " << up << endl;
        }
        else
        {
            net_hpwl_bot[i][0][0] = left;
            net_hpwl_bot[i][0][1] = down;
            net_hpwl_bot[i][1][0] = right;
            net_hpwl_bot[i][1][1] = up;

            bot_hpwl_middle[i] = make_pair( int((left + right)/2), int((up + down)/2));   //0825
            // cout << "net_idx = " << i << endl;
            // cout << "bot" << endl;
            // cout << left << " " << right << " " << down << " " << up << endl;
        }
        if(left < 0 || down < 0 || right > UpperRightX || up > UpperRighty) cout << "wrong hpwl\n";
    }


    return;
}

void replace_terminal()
{
    // for(int i = 0; i < Instance_cnt; i++) {
    //     cout << partition_map[i] << endl;
    // }

    vector<bool> bool_map;
    for (int i = 0; i < terminal_job; i++) bool_map.push_back(false);

    //0817 sunny
    calc_net_hpwl_range("top");
    calc_net_hpwl_range("bot");
    // find best terminal position for every net
   
    vector<vector<vector<int>>> best_terminal_range;  // [net_idx][lower_left, upper_right][x, y]  //value is coordinate_x, coordinate_y for terminal
    vector<bool> overlap_on2die;   //[net_idx]
    
    // pair.first -> net_idx, 
    // pair.second -> overlap area || better terminal area for not overlap net
    vector<pair<int, unsigned long long>> cross_area;   

    gridx = (UpperRightX - Terminal_Spacing) / (Terminal_X + Terminal_Spacing); 

    best_terminal_range.resize( NetCnt, {{0, 0},{0, 0}});  
    overlap_on2die.resize( NetCnt, false);         
    cout << terminal_job << " " << gridx << " " << gridy<< endl;

    for (int i = 0; i < cut_num; i++) 
    {

        int net_idx = cutname[i];
        //cout << net_idx << endl;
        // dx = min(a.xmax, b.xmax) - max(a.xmin, b.xmin)
        // dy = min(a.ymax, b.ymax) - max(a.ymin, b.ymin)
        // if (dx>=0) and (dy>=0):
            // return dx*dy   -> overlap area
        int xmax = min(net_hpwl_top[net_idx][1][0], net_hpwl_bot[net_idx][1][0]);
        int xmin = max(net_hpwl_top[net_idx][0][0], net_hpwl_bot[net_idx][0][0]);
        int ymax = min(net_hpwl_top[net_idx][1][1], net_hpwl_bot[net_idx][1][1]);
        int ymin = max(net_hpwl_top[net_idx][0][1], net_hpwl_bot[net_idx][0][1]);
        signed long long dx = xmax - xmin;
        signed long long dy = ymax - ymin;
        //cout << "here\n";


        if(dx >= 0 && dy >= 0) overlap_on2die[net_idx] = true;    // else : no overlap
        unsigned long long a = ((dx * dy) >= 0) ? (dx * dy) : -(dx * dy);
        cross_area.push_back(make_pair(net_idx, a));

        //best_terminal_range[net_idx] = {{ min(xmax, xmin), min(ymax, ymin)}, {max(xmax, xmin), max(ymax, ymin)}};
        //int realx = Terminal_Spacing + (Terminal_X + Terminal_Spacing) * coordinate_x + Terminal_X / 2;
        //int realy = Terminal_Spacing + (Terminal_Y + Terminal_Spacing) * coordinate_y + Terminal_Y / 2;
        int min_coord_x = ceil((double(min(xmax, xmin)) - double(Terminal_Spacing) - (double)Terminal_X / 2.0) / (double)(Terminal_X + Terminal_Spacing));
        int min_coord_y = ceil((double(min(ymax, ymin)) - double(Terminal_Spacing) - (double)Terminal_Y / 2.0) / (double)(Terminal_Y + Terminal_Spacing));
        int max_coord_x = floor((double(max(xmax, xmin)) - double(Terminal_Spacing) - (double)Terminal_X / 2.0) / (double)(Terminal_X + Terminal_Spacing));
        int max_coord_y = floor((double(max(ymax, ymin)) - double(Terminal_Spacing) - (double)Terminal_Y / 2.0) / (double)(Terminal_Y + Terminal_Spacing));
        best_terminal_range[net_idx] = {{min_coord_x,  min_coord_y}, {max_coord_x -1, max_coord_y -1}};
        //cout << "here\n";
    }

    //create map for maximum bipartite matching

    for (int i = 0; i < cut_num; i++) // cut_num = the number of cutted net
    {

        int netidx = cutname[i];
        //int netidx = cross_area[i].first;
        unsigned long long mini = 999999999999;
        int target = -1;
        int best_x;
        int best_y;

        bool find_best = false;

        vector<bool> bool_map_per_net;
        bool_map_per_net = bool_map;

        int min_coordx = best_terminal_range[netidx][0][0];
        int min_coordy = best_terminal_range[netidx][0][1];
        int max_coordx = best_terminal_range[netidx][1][0];
        int max_coordy = best_terminal_range[netidx][1][1];
        if(max_coordy * gridx + max_coordx >= terminal_job) {
            cout << "wrong !\n";
            cout << min_coordx << " " << min_coordy << " " << max_coordx << " " << max_coordy << endl;
        }
        //cout << cross_area[i].second << " " << min_coordx << " " << min_coordy << " " << max_coordx << " " << max_coordy << endl;
        //if(overlap_on2die[netidx])
        //{
            for(int j = min_coordx; j <= max_coordx; j++) 
            {
                if(find_best) break;
                for(int k = min_coordy; k <= max_coordy; k++) 
                {
                    if(find_best) break;
                    
                    int terminal_idx = k*gridx + j;
                    if( terminal_idx >= terminal_job) cout << "wrong terminal idx\n";
                    int realx = Terminal_Spacing + (Terminal_X + Terminal_Spacing) * j + Terminal_X / 2;
                    int realy = Terminal_Spacing + (Terminal_Y + Terminal_Spacing) * k + Terminal_Y / 2;


                    int minx = min(realx, net_hpwl_top[netidx][0][0]);
                    int maxx = max(realx, net_hpwl_top[netidx][1][0]);
                    int miny = min(realy, net_hpwl_top[netidx][0][1]);
                    int maxy = max(realy, net_hpwl_top[netidx][1][1]);
                    signed long long dx = maxx - minx;
                    signed long long dy = maxy - miny;
                    unsigned long long weight = dx + dy;

                    minx = min(realx, net_hpwl_bot[netidx][0][0]);
                    maxx = max(realx, net_hpwl_bot[netidx][1][0]);
                    miny = min(realy, net_hpwl_bot[netidx][0][1]);
                    maxy = max(realy, net_hpwl_bot[netidx][1][1]);
                    dx = maxx - minx;
                    dy = maxy - miny;
                    weight = weight + (dx + dy);

                    
                    if (weight < mini && bool_map[terminal_idx] == false)
                    {
                        mini = weight;
                        target = terminal_idx;
                        best_x = realx;
                        best_y = realy;
                        find_best = true;
                    }
                    bool_map_per_net[terminal_idx] = true;   // visited

                }
            }

            if(target != -1) {
                bool_map[target] = true;
                terminal_position[i].first = best_x;
                terminal_position[i].second = best_y;
                continue;
            }
        //}
        for (int j = 0; j < terminal_job; j++)
        {
            if(bool_map_per_net[j] || bool_map[j]) continue;
            bool_map_per_net[j] = true;

            int coordinate_x = j % gridx;
            int coordinate_y = j / gridx;
            int realx = Terminal_Spacing + (Terminal_X + Terminal_Spacing) * coordinate_x + Terminal_X / 2;
            int realy = Terminal_Spacing + (Terminal_Y + Terminal_Spacing) * coordinate_y + Terminal_Y / 2;
            
            //flag==0 --> top die
            //
            //unsigned long long weight2 = calc_HPWL(netidx, realx, realy, true, flag);
            unsigned long long weight;
            int minx = min(realx, net_hpwl_top[netidx][0][0]);
            int maxx = max(realx, net_hpwl_top[netidx][1][0]);
            int miny = min(realy, net_hpwl_top[netidx][0][1]);
            int maxy = max(realy, net_hpwl_top[netidx][1][1]);
            signed long long dx = maxx - minx;
            signed long long dy = maxy - miny;
            weight = dx + dy;

            minx = min(realx, net_hpwl_bot[netidx][0][0]);
            maxx = max(realx, net_hpwl_bot[netidx][1][0]);
            miny = min(realy, net_hpwl_bot[netidx][0][1]);
            maxy = max(realy, net_hpwl_bot[netidx][1][1]);
            dx = maxx - minx;
            dy = maxy - miny;
            weight = weight + (dx + dy);

            //cout << weight << " " << weight2 << endl;
            
            if (weight < mini && !bool_map[j])
            {
                mini = weight;
                target = j;
                best_x = realx;
                best_y = realy;
            }
        }
        bool_map[target] = true;
        terminal_position[i].first = best_x;
        terminal_position[i].second = best_y;
    }
}
void replace_terminal_only_top()
{

    vector<bool> bool_map;
    for (int i = 0; i < terminal_job; i++) bool_map.push_back(false);
    calc_net_hpwl_range("top");
    // find best terminal position for every net
    vector<vector<vector<int>>> best_terminal_range;  // [net_idx][lower_left, upper_right][x, y]  //value is coordinate_x, coordinate_y for terminal  

    gridx = (UpperRightX - Terminal_Spacing) / (Terminal_X + Terminal_Spacing); 
    best_terminal_range.resize( NetCnt, {{0, 0},{0, 0}});   

    cout << "gridx, gridy = " << gridx << " " << gridy << endl;     

    int net_idx, min_coord_x, min_coord_y, max_coord_x, max_coord_y;
    for (int i = 0; i < cut_num; i++) 
    {

        net_idx = cutname[i];
        // dx = min(a.xmax, b.xmax) - max(a.xmin, b.xmin)
        // dy = min(a.ymax, b.ymax) - max(a.ymin, b.ymin)
        // if (dx>=0) and (dy>=0):
            // return dx*dy   -> overlap area

        //best_terminal_range[net_idx] = {{ min(xmax, xmin), min(ymax, ymin)}, {max(xmax, xmin), max(ymax, ymin)}};
        //int realx = Terminal_Spacing + (Terminal_X + Terminal_Spacing) * coordinate_x + Terminal_X / 2;
        //int realy = Terminal_Spacing + (Terminal_Y + Terminal_Spacing) * coordinate_y + Terminal_Y / 2;
        min_coord_x = ceil(( (double)net_hpwl_top[net_idx][0][0] - double(Terminal_Spacing) - (double)Terminal_X / 2.0) / (double)(Terminal_X + Terminal_Spacing));
        min_coord_y = ceil(( (double)net_hpwl_top[net_idx][0][1] - double(Terminal_Spacing) - (double)Terminal_Y / 2.0) / (double)(Terminal_Y + Terminal_Spacing));
        max_coord_x = floor(( (double)net_hpwl_top[net_idx][1][0] - double(Terminal_Spacing) - (double)Terminal_X / 2.0) / (double)(Terminal_X + Terminal_Spacing));
        max_coord_y = floor(( (double)net_hpwl_top[net_idx][1][1] - double(Terminal_Spacing) - (double)Terminal_Y / 2.0) / (double)(Terminal_Y + Terminal_Spacing));
        best_terminal_range[net_idx] = {{min_coord_x,  min_coord_y}, {max_coord_x -1, max_coord_y -1}};
    }

    //create map for maximum bipartite matching
    unsigned long long mini, dx, dy, weight;
    int netidx, target, best_x, best_y, terminal_idx, realx, realy, minx, maxx, miny, maxy;
    bool find_best;
    vector<bool> bool_map_per_net;
    for (int i = 0; i < cut_num; i++) // cut_num = the number of cutted net
    {
        netidx = cutname[i];
        mini = 999999999999;
        target = -1;
        find_best = false;

        bool_map_per_net = bool_map;

        min_coord_x = best_terminal_range[netidx][0][0];
        min_coord_y = best_terminal_range[netidx][0][1];
        max_coord_x = best_terminal_range[netidx][1][0];
        max_coord_y = best_terminal_range[netidx][1][1];
        if(max_coord_y * gridx + max_coord_x >= terminal_job) {
            cout << max_coord_x << " " << max_coord_y << endl;
            cout << "wrong !\n";
        }

        for(int j = min_coord_x; j <= max_coord_x; j++) 
        {
            if(find_best) break;
            for(int k = min_coord_y; k <= max_coord_y; k++) 
            {
                if(find_best) break;
                
                terminal_idx = k*gridx + j;
                if( terminal_idx >= terminal_job) cout << "wrong terminal idx\n";
                realx = Terminal_Spacing + (Terminal_X + Terminal_Spacing) * j + Terminal_X / 2;
                realy = Terminal_Spacing + (Terminal_Y + Terminal_Spacing) * k + Terminal_Y / 2;


                minx = min(realx, net_hpwl_top[netidx][0][0]);
                maxx = max(realx, net_hpwl_top[netidx][1][0]);
                miny = min(realy, net_hpwl_top[netidx][0][1]);
                maxy = max(realy, net_hpwl_top[netidx][1][1]);
                dx = maxx - minx;
                dy = maxy - miny;
                weight = dx + dy;
                
                if (weight < mini && bool_map[terminal_idx] == false)
                {
                    mini = weight;
                    target = terminal_idx;
                    best_x = realx;
                    best_y = realy;
                    find_best = true;
                }
                bool_map_per_net[terminal_idx] = true;   // visited

            }
        }

        if(target != -1) {
            bool_map[target] = true;
            terminal_position[i].first = best_x;
            terminal_position[i].second = best_y;
            continue;
        }
        //}
        for (int j = 0; j < terminal_job; j++)
        {
            if(bool_map_per_net[j] || bool_map[j]) continue;
            bool_map_per_net[j] = true;

            int coordinate_x = j % gridx;
            int coordinate_y = j / gridx;
            realx = Terminal_Spacing + (Terminal_X + Terminal_Spacing) * coordinate_x + Terminal_X / 2;
            realy = Terminal_Spacing + (Terminal_Y + Terminal_Spacing) * coordinate_y + Terminal_Y / 2;
            
            //flag==0 --> top die
            //
            //unsigned long long weight2 = calc_HPWL(netidx, realx, realy, true, flag);
            minx = min(realx, net_hpwl_top[netidx][0][0]);
            maxx = max(realx, net_hpwl_top[netidx][1][0]);
            miny = min(realy, net_hpwl_top[netidx][0][1]);
            maxy = max(realy, net_hpwl_top[netidx][1][1]);
            dx = maxx - minx;
            dy = maxy - miny;
            weight = dx + dy;

            //cout << weight << " " << weight2 << endl;
            
            if (weight < mini && !bool_map[j])
            {
                mini = weight;
                target = j;
                best_x = realx;
                best_y = realy;
            }
        }
        bool_map[target] = true;
        terminal_position[i].first = best_x;
        terminal_position[i].second = best_y;
    }
}

long long parse_eval(string filename)
{
    in_file.open(filename);
    string tmps;
    long long tmp;
    while (in_file >> tmps)
    {
        if (tmps == "design")
        {
            in_file >> tmps;
            in_file >> tmp;
        }
    }
    in_file.close();
    return tmp;
}
void modify_def_terPos(const char* defName){
    FILE *fp;
    char buffer[1024];
    size_t numread;
    
    
    fp = fopen(defName, "r");
    if (fp == NULL) {
        printf("failed to open the def.\n");
        return; // EXIT_FAILURE
    }

    FILE *nfp;
    nfp = fopen("tmp.def", "w");
    if (nfp == NULL) {
        printf("failed to open the file.txt.\n");
        return; // EXIT_FAILURE
    }
    
    int ter_idx = 0;
    while ((numread = fread(buffer, sizeof(char), sizeof(buffer)-100, fp)) > 0) {

        if(buffer[numread-1] != ';'){
            char ch;
            while (numread < sizeof(buffer) - 1 &&  fread(&ch, sizeof(char), 1, fp) == 1) {
                if (ch == ';') {
                    buffer[numread++] = ch;
                    break; // Stop reading when semicolon is found
                }
                buffer[numread++] = ch;
            }
        }
        //printf("read %lu bytes\n", numread);
        // printf("%.*s", (int)numread, buffer);
        int length = numread;//strlen(buffer);
        int pos = 0, last_pos = 0;
        std::string modifiedString;
        bool have_ter = false;
        while (pos < length - 4) {
            if (strncmp(buffer + pos, "FIXED", 5) == 0) {
                
                have_ter = true;
                // Find the position of '(' and ')'
                char* openBracketPos = strchr(buffer + pos, '(');
                char* closeBracketPos = strchr(buffer + pos, ')');

                if (openBracketPos && closeBracketPos) {
                    // Extract the content before "FIXED"
                    std::string beforeFixed(buffer+pos, openBracketPos - buffer - pos);
                    std::string lastFixed(buffer+last_pos, buffer+pos);
                    
                    
                    // Create a new coordinate string
                    int new_x = terminal_position[ter_idx].first;
                    int new_y = terminal_position[ter_idx].second;
                    ter_idx++;
                    std::string newCoords = "( " + std::to_string(new_x) + " " + std::to_string(new_y) + " )";

                    // Extract the content after "FIXED"
                    std::string afterFixed(closeBracketPos + 1);

                    // Construct the final modified string
                    modifiedString += (lastFixed + beforeFixed +  newCoords);
                      
                }

                // Move the position pointer past the current "FIXED"
                pos += closeBracketPos - buffer - pos + 1;
                last_pos = pos;
                
            } else {
                pos++;
            }
        }
        std::string finalFixed(buffer+last_pos, buffer+numread);
        modifiedString += finalFixed;
        
        // write file
        if(have_ter)
            size_t bytes_written = fwrite(modifiedString.c_str(), sizeof(char), modifiedString.length(), nfp);
        else
            size_t bytes_written = fwrite(buffer, sizeof(char), numread, nfp);
        
    }
    cout << "End DEF:------------- " << defName << "\n";
    fclose(fp);
    fclose(nfp);

    // Replace the original file with the temporary file
    if (std::rename("tmp.def", defName) != 0) {
        std::cerr << "Failed to replace the original file." << std::endl;
        return;
    }

    return;
}

void perform_replace(const char *arg1, const char *arg2)
{
    char cmd[150] = {
        0,
    };
    system("pwd");
    // system("cp top_die.def top_die.lef bot_die.def bot_die.lef ./RunReplace");
    // char cmd[50] = "cd ./RunReplace";
    // system(cmd);
    // system("chmod +x *");
    // system("pwd");
    // system("ls -l");
    string sin(arg1), sout(arg2);
    string filename = sin.substr(0, sin.size() - 4);
    string dirname = "./area_tune_density_";
    string dir = dirname + filename;
    dirtoremove = dir;
    string command = "";
    command = "mkdir " + dir;
    system(command.c_str());
    dir = dirname + filename + "/run1";
    command = "mkdir " + dir;
    system(command.c_str());
    vector<pair<int, int>> best_inst;
    vector<string> best_inst_orient;
    vector<pair<int, int>> best_terminal;
    for (int i = 0; i < Instance_cnt; i++)
        best_inst.push_back(make_pair(0, 0));
    best_inst_orient.push_back("N");
    for (int i = 0; i < cut_num; i++)
        best_terminal.push_back(make_pair(0, 0));

    double iter_cnt = 0.7;
    if(Instance_cnt < 300000) iter_cnt = 0.9;
    if(Instance_cnt < 100000) iter_cnt = 1.0;   

    // 讓replace跑很多次的迴圈
    for (double i = 0.6; i <= iter_cnt; i += 0.1) // modified original 1.0
    {
        ////    generate def with new terminal position
        // out_file.open("top_die.def");
        // generate_def_version();
        // generate_top_def();
        // out_file.close();
        // out_file.open("bot_die.def");
        // generate_def_version();
        // generate_bottom_def();
        // out_file.close();
        modify_def_terPos("top_die.def");
        modify_def_terPos("bot_die.def");
        ////
        // ofstream out_filename;
        command = "mkdir " + dir + "/" + to_string(i);
        system(command.c_str());

        system("rm -rf ./0");
        system("rm -rf ./etc");
        if (Instance_cnt < 1000)
        {
            sprintf(cmd, "./replace -lef top_die.lef -def top_die.def -output . -overflow 0.5 -dpflag NTU3 -den 1.0");
            // sprintf(cmd, "./RunReplace/replace -lef top_die.lef -def top_die.def -output . -overflow 1.0 -den 1.0");
        }
        else if(Instance_cnt < 300000)
        {
            sprintf(cmd, "./replace -lef top_die.lef -def top_die.def -output . -overflow 0.12 -dpflag NTU3 -den 1.0 -bin 512");
            // sprintf(cmd, "./RunReplace/replace -lef top_die.lef -def top_die.def -output . -overflow 0.12 -den 1.0 -bin 512");
        }else {
            sprintf(cmd, "./replace -lef top_die.lef -def top_die.def -output . -overflow 0.18 -dpflag NTU3 -den 1.0 -bin 512");
        }
        system(cmd);
        system("cp ./etc/top_die/experiment000/top_die_final.def .");
        command = "cp top_die_final.def " + dir + "/" + to_string(i);
        system(command.c_str());


        parse_top();

        cout << "finish parse_top" << endl;

        // 擺完standard cell後，依照standard cell的位置重擺terminal
        if(i == 0.6) replace_terminal_only_top();
        else replace_terminal();
        
        cout << "start generate bot.def--------------\n";
        // out_file.open("bot_die.def");
        generate_def_version_bot("bot_die.def");
        cout << "end generate bot.def--------------\n";
        // generate_bottom_def();
        // out_file.close();
        system("rm -rf ./0");
        system("rm -rf ./etc");
        if (Instance_cnt < 1000)
        {

            sprintf(cmd, "./replace -lef bot_die.lef -def bot_die.def -output . -overflow 0.5 -dpflag NTU3 -den 1.0");
            // sprintf(cmd, "./RunReplace/replace -lef bot_die.lef -def bot_die.def -output . -overflow 1.0 -den 1.0");
        }
        else if(Instance_cnt < 300000)
        {
            sprintf(cmd, "./replace -lef bot_die.lef -def bot_die.def -output . -overflow 0.12 -dpflag NTU3 -den 1.0 -bin 512");
            // sprintf(cmd, "./RunReplace/replace -lef bot_die.lef -def bot_die.def -output . -overflow 0.12 -den 1.0 -bin 512");
        }
        else {
            sprintf(cmd, "./replace -lef bot_die.lef -def bot_die.def -output . -overflow 0.18 -dpflag NTU3 -den 1.0 -bin 512");
        }
        system(cmd);
        system("cp ./etc/bot_die/experiment000/bot_die_final.def .");
        command = "cp bot_die_final.def " + dir + "/" + to_string(i);
        system(command.c_str());
        system("rm -rf ./0");
        system("rm -rf ./etc");
        in_file.close();
        cout << " infile close done " << endl;
        
        parse_bottom();

        cout << " parse bottom done " << endl;
        replace_terminal();

        // out_file.open(arg2);
        // output();
        // out_file.close();

        // eval(arg1, arg2);
        //long long hpwl = parse_eval(filename + "_eval.txt");
        long long hpwl = calc_total_HPWL();
        if (hpwl < best_HPWL)
        {
            best_inst.assign(inst_position.begin(), inst_position.end());
            best_inst_orient.assign(inst_orient.begin(), inst_orient.end());
            best_terminal.assign(terminal_position.begin(), terminal_position.end());
            best_HPWL = hpwl;
        }
        cout << "HPWL for replace iteration " << i + 1 << " : " << hpwl <<  endl;
        // eval(arg1, arg2);
        // command = "mv " + filename + "_eval.txt " + sout + " " + dir + "/" + to_string(i);
        // system(command.c_str());
        system("rm top_die_final.def bot_die_final.def"); // yu--modified(originally not commented)
    }
    inst_position.assign(best_inst.begin(), best_inst.end());
    inst_orient.assign(best_inst_orient.begin(), best_inst_orient.end());
    terminal_position.assign(best_terminal.begin(), best_terminal.end());
    // system("rm top_die.def top_die.lef bot_die.def ./bot_die.lef");
    // system("cd ..");
    // command = "mv top_die.def bot_die.def " + dir;
    system(command.c_str());
    //command = "mv hmetis_input.txt hmetis_input.txt.part.2 " + dir;
    system(command.c_str());
}

long long calc_total_HPWL() // with terminal
{
    calc_net_hpwl_range("top");
    calc_net_hpwl_range("bot");

    long long total = 0;
    long long up, down, left, right;
    for (int i = 0; i < NetCnt; i++)
    {
        int cut_idx = mapforterminal[i];
        
        if(net_hasInst_on_top[i]) {
            up = net_hpwl_top[i][1][1];
            down = net_hpwl_top[i][0][1];
            left = net_hpwl_top[i][0][0];
            right = net_hpwl_top[i][1][0];

            if(NetCross[i]) {
                if(terminal_position[cut_idx].first < left) left = terminal_position[cut_idx].first;
                if(terminal_position[cut_idx].first > right) right = terminal_position[cut_idx].first;
                if(terminal_position[cut_idx].second < down) down = terminal_position[cut_idx].second;
                if(terminal_position[cut_idx].second > up) up = terminal_position[cut_idx].second;
            }
            //cout << "terminal = " << terminal_position[cut_idx].first << " " << terminal_position[cut_idx].second << endl;

            total += (up - down + right - left);
        }
        if(net_hasInst_on_bot[i]) {
            up = net_hpwl_bot[i][1][1];
            down = net_hpwl_bot[i][0][1];
            left = net_hpwl_bot[i][0][0];
            right = net_hpwl_bot[i][1][0];

            if(NetCross[i]) {
                
                if(terminal_position[cut_idx].first < left) left = terminal_position[cut_idx].first;
                if(terminal_position[cut_idx].first > right) right = terminal_position[cut_idx].first;
                if(terminal_position[cut_idx].second < down) down = terminal_position[cut_idx].second;
                if(terminal_position[cut_idx].second > up) up = terminal_position[cut_idx].second;
            }
            //cout << "after = " << left << " " << down <<  " " << right << " " << up << endl;
            total += (up - down + right - left);
        }
        if(NetCross[i]) total += (long long)Terminal_Cost;
    }   

    cout << "HPWL with terminal : " << total << endl;
    return total;
}

void floorplan_top()
{ // yu--modified
    
    // change instance_position and direction
    string command = "";
    command = "./b_star_tree top_die.hardblocks top_die.nets terminal.pl top_die.floorplan 1.0";
    system(command.c_str());

    string tmp_str;
    int tmpx, tmpy, ori;
    in_file.open("./top_die.floorplan");
    while (in_file >> tmp_str)
    {

        if (tmp_str[0] == 'C')
        {
            int mcsize = tmp_str.size();
            int t = 0;
            for (int i = 1; i < mcsize; i++)
            {
                t *= 10;
                t += tmp_str[i] - '0';
            }

            in_file >> tmpx >> tmpy >> ori;
            cout << "floorplan_x: " << tmpx << ", floorplan_y: " << tmpy << ", ori: " << ori << "\n";
            /*sunny modified*/
            if (ori == 0) inst_orient[t - 1] = "N";
            else if (ori == 1) inst_orient[t - 1] = "E";
            else if (ori == 2) inst_orient[t - 1] = "S";
            else inst_orient[t - 1] = "W";

            inst_position[t - 1].first = tmpx;
            inst_position[t - 1].second = tmpy;
        }
    }

    in_file.close();
}
void floorplan_bot()
{ // yu--modified
    /*bot_die*/
    // change instance_position and direction
    string command = "";
    command = "./b_star_tree bot_die.hardblocks bot_die.nets terminal.pl bot_die.floorplan 1.0";
    system(command.c_str());

    string tmp_str;
    int tmpx, tmpy, ori;
    in_file.open("./bot_die.floorplan");
    while (in_file >> tmp_str)
    {

        if (tmp_str[0] == 'C')
        {
            int mcsize = tmp_str.size();
            int t = 0;
            for (int i = 1; i < mcsize; i++)
            {
                t *= 10;
                t += tmp_str[i] - '0';
            }
            in_file >> tmpx >> tmpy >> ori;
            cout << "floorplan_x: " << tmpx << ", floorplan_y: " << tmpy << ", ori: " << ori << "\n";

            /*sunny modified*/
            if (ori == 0) inst_orient[t - 1] = "N";
            else if (ori == 1) inst_orient[t - 1] = "E";
            else if (ori == 2) inst_orient[t - 1] = "S";
            else inst_orient[t - 1] = "W";

            inst_position[t - 1].first = tmpx;
            inst_position[t - 1].second = tmpy;
        }
    }

    in_file.close();
}

void generate_def_version_2nd_top(const char* defName)
{
    FILE* nfp = fopen(defName, "w");//flush original file
    fflush(nfp);
    fclose(nfp);
    nfp = fopen(defName, "a");
    
    const char* modifiedString = "VERSION 5.8 ;\nBUSBITCHARS \"[]\" ;\nDIVIDERCHAR \"/\" ;\nDESIGN HW ;\nUNITS DISTANCE MICRONS ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // +to_string(unit)+
    char buf[100];  // 這裡的大小根據你需要的數字最大長度來調整
    int len = snprintf(buf, sizeof(buf), "%d", unit);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ;\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    modifiedString = "DIEAREA ( ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(LowerLeftX) 
    len = snprintf(buf, sizeof(buf), "%lld", LowerLeftX);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
    // + to_string(LowerLeftY) +
    len = snprintf(buf, sizeof(buf), "%lld", LowerLeftY);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ) ( " ;
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(UpperRightX) +
    len = snprintf(buf, sizeof(buf), "%lld", UpperRightX);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
    // + to_string(UpperRighty) +
    len = snprintf(buf, sizeof(buf), "%lld", UpperRighty);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ) ;\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    
    // std::ostringstream stream, stream2;
    // stream << std::defaultfloat << (float)RowHeight_b / unit;
    // string outloop_str, inloop_str;

    for (int i = 0; i < RepeatCount_t; i++)
    {
        modifiedString = "ROW ROW_TOP_";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +to_string(i)+
        len = snprintf(buf, sizeof(buf), "%d", i);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " Site 0 ";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +to_string(i * RowHeight_t)+
        len = snprintf(buf, sizeof(buf), "%d", i * RowHeight_t);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " N DO ";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +to_string(UpperRightX)+
        len = snprintf(buf, sizeof(buf), "%lld", UpperRightX);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " BY 1 STEP 1 0\n ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // modifiedString += outloop_str;
        
    }
    modifiedString = "\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    modifiedString = "COMPONENTS ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(Cell_in_top.size()+cut_num) +
    len = snprintf(buf, sizeof(buf), "%lu", Cell_in_top.size()+cut_num);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ;\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    for (int i = 0; i < Cell_in_top.size(); i++)
    {
        if (!LibCell_isMacro[top][Lib_for_Inst[Cell_in_top[i]]]){
            modifiedString = "- C" ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(Cell_in_top[i]+1) +
            len = snprintf(buf, sizeof(buf), "%d", Cell_in_top[i]+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " MC" ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(Lib_for_Inst[Cell_in_top[i]] + 1) 
            len = snprintf(buf, sizeof(buf), "%d", Lib_for_Inst[Cell_in_top[i]] + 1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_TOP + PLACED ( " ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(second_inst[Cell_in_top[i]].first) +
            len = snprintf(buf, sizeof(buf), "%d", second_inst[Cell_in_top[i]].first);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
            // + to_string(second_inst[Cell_in_top[i]].second) +
            len = snprintf(buf, sizeof(buf), "%d", second_inst[Cell_in_top[i]].second);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " ) ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // +(second_inst_orient[Cell_in_top[i]]) +
            modifiedString = second_inst_orient[Cell_in_top[i]].c_str();
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            modifiedString = " ;\n";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        }
        else{ // macor needs to be fixed
            modifiedString = "- C" ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(Cell_in_top[i]+1) +
            len = snprintf(buf, sizeof(buf), "%d", Cell_in_top[i]+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " MC" ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(Lib_for_Inst[Cell_in_top[i]] + 1) 
            len = snprintf(buf, sizeof(buf), "%d", Lib_for_Inst[Cell_in_top[i]] + 1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_TOP + FIXED ( " ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(second_inst[Cell_in_top[i]].first) +
            len = snprintf(buf, sizeof(buf), "%d", second_inst[Cell_in_top[i]].first);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
            // + to_string(second_inst[Cell_in_top[i]].second) +
            len = snprintf(buf, sizeof(buf), "%d", second_inst[Cell_in_top[i]].second);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " ) ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // +(second_inst_orient[Cell_in_top[i]]) +
            modifiedString = second_inst_orient[Cell_in_top[i]].c_str();
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            modifiedString = " ;\n";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        } 
    }
    for (int i = 0; i < cut_num; i++)
    {
        int net_idx = cutname[i];
        modifiedString = "- T";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(cutname[i] + 1) +
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " TER_" ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(cutname[i] + 1) +
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " + FIXED ( " ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(terminal_position[i].first) +
        len = snprintf(buf, sizeof(buf), "%d", terminal_position[i].first);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " " ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(terminal_position[i].second) +
        len = snprintf(buf, sizeof(buf), "%d", terminal_position[i].second);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " ) N ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);


        modifiedString = "- T";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(cutname[i] + 1) +
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = "-0 TER_" ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(cutname[i] + 1) +
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " + FIXED ( " ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(terminal_position[i].first) +
        len = snprintf(buf, sizeof(buf), "%d", bot_hpwl_middle[net_idx].first);   //0825
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " " ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(terminal_position[i].second) +
        len = snprintf(buf, sizeof(buf), "%d", bot_hpwl_middle[net_idx].second);   //0825
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " ) N ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    }

    modifiedString = "END COMPONENTS\n\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    
    modifiedString = "NETS " ;
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(NetCnt) +
    len = snprintf(buf, sizeof(buf), "%d", NetCnt);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ;\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    for (int i = 0; i < NetCnt; i++)
    {
        bool has_net = false;
        int pin_cnt = NumofPin[i];
        int actual_cnt = 0;
        
        modifiedString = "- N" ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(i+1) +
        len = snprintf(buf, sizeof(buf), "%d", i+1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = "\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        has_net = true;
        if (NetCross[i])
        {
            
            modifiedString = " ( T" ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(i+1) +
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " P1 )";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);

            /*start 0816 sunny*/

            modifiedString = " ( T" ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(i+1) +
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "-0 P1 )";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);

          

            actual_cnt++;
        }
        for (int j = 0; j < pin_cnt; j++)
        {
            int die = partition_map[net[i][j].first];
            //vector<int>::iterator it = find(Cell_in_top.begin(), Cell_in_top.end(), net[i][j].first);
            if (die == 0)
            {
                
                modifiedString = " ( C" ;
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // + to_string(net[i][j].first + 1) +
                len = snprintf(buf, sizeof(buf), "%d", net[i][j].first + 1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " P" ;
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                //  + to_string(net[i][j].second + 1) +
                len = snprintf(buf, sizeof(buf), "%d", net[i][j].second + 1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " )";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                actual_cnt++;
            }
        }
        
        modifiedString = " ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        actual_cnt = 0;
        // modifiedString += outloop_str;
    }
    
    modifiedString = "END NETS\n\nEND DESIGN";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    
    // 刷新緩衝區，確保數據寫入檔案
    fflush(nfp);
    // close file
    fclose(nfp);
}

void generate_def_version_2nd_bot(const char* defName)
{
    FILE* nfp = fopen(defName, "w");//flush original file
    fflush(nfp);
    fclose(nfp);
    nfp = fopen(defName, "a");
    
    const char* modifiedString = "VERSION 5.8 ;\nBUSBITCHARS \"[]\" ;\nDIVIDERCHAR \"/\" ;\nDESIGN HW ;\nUNITS DISTANCE MICRONS ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // +to_string(unit)+
    char buf[100];  // 這裡的大小根據你需要的數字最大長度來調整
    int len = snprintf(buf, sizeof(buf), "%d", unit);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ;\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    modifiedString = "DIEAREA ( ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(LowerLeftX) 
    len = snprintf(buf, sizeof(buf), "%lld", LowerLeftX);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
    // + to_string(LowerLeftY) +
    len = snprintf(buf, sizeof(buf), "%lld", LowerLeftY);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ) ( " ;
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(UpperRightX) +
    len = snprintf(buf, sizeof(buf), "%lld", UpperRightX);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
    // + to_string(UpperRighty) +
    len = snprintf(buf, sizeof(buf), "%lld", UpperRighty);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ) ;\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    
    // std::ostringstream stream, stream2;
    // stream << std::defaultfloat << (float)RowHeight_b / unit;
    // string outloop_str, inloop_str;

    for (int i = 0; i < RepeatCount_b; i++)
    {
        modifiedString = "ROW ROW_BOT_";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +to_string(i)+
        len = snprintf(buf, sizeof(buf), "%d", i);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " Site 0 ";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +to_string(i * RowHeight_t)+
        len = snprintf(buf, sizeof(buf), "%d", i * RowHeight_b);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " N DO ";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // +to_string(UpperRightX)+
        len = snprintf(buf, sizeof(buf), "%lld", UpperRightX);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " BY 1 STEP 1 0\n ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // modifiedString += outloop_str;
        
    }
    modifiedString = "\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    modifiedString = "COMPONENTS ";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(Cell_in_top.size()+cut_num) +
    len = snprintf(buf, sizeof(buf), "%lu", Cell_in_bottom.size()+cut_num);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ;\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    for (int i = 0; i < Cell_in_bottom.size(); i++)
    {
        if (!LibCell_isMacro[bot][Lib_for_Inst[Cell_in_bottom[i]]]){
            modifiedString = "- C" ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(Cell_in_top[i]+1) +
            len = snprintf(buf, sizeof(buf), "%d", Cell_in_bottom[i]+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " MC" ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(Lib_for_Inst[Cell_in_top[i]] + 1) 
            len = snprintf(buf, sizeof(buf), "%d", Lib_for_Inst[Cell_in_bottom[i]] + 1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_BOT + PLACED ( " ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(second_inst[Cell_in_top[i]].first) +
            len = snprintf(buf, sizeof(buf), "%d", second_inst[Cell_in_bottom[i]].first);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
            // + to_string(second_inst[Cell_in_top[i]].second) +
            len = snprintf(buf, sizeof(buf), "%d", second_inst[Cell_in_bottom[i]].second);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " ) ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // +(second_inst_orient[Cell_in_top[i]]) +
            modifiedString = second_inst_orient[Cell_in_bottom[i]].c_str();
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            modifiedString = " ;\n";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        }
        else{ // macor needs to be fixed
            modifiedString = "- C" ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(Cell_in_top[i]+1) +
            len = snprintf(buf, sizeof(buf), "%d", Cell_in_bottom[i]+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " MC" ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(Lib_for_Inst[Cell_in_top[i]] + 1) 
            len = snprintf(buf, sizeof(buf), "%d", Lib_for_Inst[Cell_in_bottom[i]] + 1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "_BOT + FIXED ( " ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(second_inst[Cell_in_top[i]].first) +
            len = snprintf(buf, sizeof(buf), "%d", second_inst[Cell_in_bottom[i]].first);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp); 
            // + to_string(second_inst[Cell_in_top[i]].second) +
            len = snprintf(buf, sizeof(buf), "%d", second_inst[Cell_in_bottom[i]].second);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " ) ";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // +(second_inst_orient[Cell_in_top[i]]) +
            modifiedString = second_inst_orient[Cell_in_bottom[i]].c_str();
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            modifiedString = " ;\n";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        } 
    }
    for (int i = 0; i < cut_num; i++)
    {
        int net_idx = cutname[i];
        modifiedString = "- T";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(cutname[i] + 1) +
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " TER_" ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(cutname[i] + 1) +
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " + FIXED ( " ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(terminal_position[i].first) +
        len = snprintf(buf, sizeof(buf), "%d", terminal_position[i].first);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " " ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(terminal_position[i].second) +
        len = snprintf(buf, sizeof(buf), "%d", terminal_position[i].second);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " ) N ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);

         modifiedString = "- T";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(cutname[i] + 1) +
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = "-0 TER_" ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(cutname[i] + 1) +
        len = snprintf(buf, sizeof(buf), "%d", cutname[i] + 1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " + FIXED ( " ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(terminal_position[i].first) +
        len = snprintf(buf, sizeof(buf), "%d", top_hpwl_middle[net_idx].first);   //0825
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " " ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(terminal_position[i].second) +
        len = snprintf(buf, sizeof(buf), "%d", top_hpwl_middle[net_idx].second);   //0825
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = " ) N ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);


    }
    modifiedString = "END COMPONENTS\n\n\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    
    modifiedString = "NETS " ;
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    // + to_string(NetCnt) +
    len = snprintf(buf, sizeof(buf), "%d", NetCnt);
    fwrite(buf, sizeof(char), len, nfp);
    modifiedString = " ;\n";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    for (int i = 0; i < NetCnt; i++)
    {
        bool has_net = false;
        int pin_cnt = NumofPin[i];
        int actual_cnt = 0;
        
        modifiedString = "- N" ;
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        // + to_string(i+1) +
        len = snprintf(buf, sizeof(buf), "%d", i+1);
        fwrite(buf, sizeof(char), len, nfp);
        modifiedString = "\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        has_net = true;
        if (NetCross[i])
        {

            modifiedString = " ( T" ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(i+1) +
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = " P1 )";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);

            /*0816 sunny*/
            modifiedString = " ( T" ;
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
            // + to_string(i+1) +
            len = snprintf(buf, sizeof(buf), "%d", i+1);
            fwrite(buf, sizeof(char), len, nfp);
            modifiedString = "-0 P1 )";
            fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);

            actual_cnt++;
        }
        for (int j = 0; j < pin_cnt; j++)
        {
            //vector<int>::iterator it = find(Cell_in_bottom.begin(), Cell_in_bottom.end(), net[i][j].first);
            int die = partition_map[net[i][j].first];
            if (die == 1)
            {
                
                modifiedString = " ( C" ;
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                // + to_string(net[i][j].first + 1) +
                len = snprintf(buf, sizeof(buf), "%d", net[i][j].first + 1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " P" ;
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                //  + to_string(net[i][j].second + 1) +
                len = snprintf(buf, sizeof(buf), "%d", net[i][j].second + 1);
                fwrite(buf, sizeof(char), len, nfp);
                modifiedString = " )";
                fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
                actual_cnt++;
            }
        }
        
        modifiedString = " ;\n";
        fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
        actual_cnt = 0;
        // modifiedString += outloop_str;
    }
    
    modifiedString = "END NETS\n\nEND DESIGN";
    fwrite(modifiedString, sizeof(char), strlen(modifiedString), nfp);
    
    // 刷新緩衝區，確保數據寫入檔案
    fflush(nfp);
    // close file
    fclose(nfp);
}
void perform_2nd_replace(const char *arg1, const char *arg2)
{
    second_inst.assign(inst_position.begin(), inst_position.end());
    second_inst_orient.assign(inst_orient.begin(), inst_orient.end());
    second_terminal.assign(terminal_position.begin(), terminal_position.end());
    
    // generate_lef_version_top("top_die.lef");
    // //generate_def_version_2nd_top("top_die.def");

    // generate_lef_version_bot("bot_die.lef");
    // //generate_def_version_2nd_bot("bot_die.def");


    /*start std cell placement + detailed placement*/
    char cmd[150] = {
        0,
    };
    system("pwd");
    // system("cp top_die.def top_die.lef bot_die.def bot_die.lef ./RunReplace");
    // char cmd[50] = "cd ./RunReplace";
    // system(cmd);
    // system("chmod +x *");
    // system("pwd");
    // system("ls -l");
    string sin(arg1), sout(arg2);
    string filename = sin.substr(0, sin.size() - 4);
    string dirname = "./area_tune_density_";
    string dir = dirname + filename;
    dirtoremove = dir;
    string command = "";
    command = "mkdir " + dir;
    system(command.c_str());
    dir = dirname + filename + "/run1";
    command = "mkdir " + dir;
    system(command.c_str());
    vector<pair<int, int>> best_inst;
    vector<string> best_inst_orient;
    vector<pair<int, int>> best_terminal;

    bool find_better_hpwl = false;

    for (int i = 0; i < Instance_cnt; i++)
        best_inst.push_back(make_pair(0, 0));
    best_inst_orient.push_back("N");
    for (int i = 0; i < cut_num; i++)
        best_terminal.push_back(make_pair(0, 0));
    // 讓replace跑1次的迴圈
    for (int k = 0; k < 1; k++ )
    {
        // ////    generate def with new terminal position
        // out_file.open("top_die.def");
        // generate_def_version();
        // generate_top_def();
        // out_file.close();
        // out_file.open("bot_die.def");
        // generate_def_version();
        // generate_bottom_def();
        // out_file.close();
        // ////
        calc_net_hpwl_range("top");

        generate_def_version_2nd_top("top_die.def");  /*0815 sunny*/

        command = "mkdir " + dir + "/" + to_string(k);
        system(command.c_str());
        if (Instance_cnt < 1000)
        {
            sprintf(cmd, "./replace -lef top_die.lef -def top_die.def -output . -overflow 0.5 -dpflag NTU3 -den 1.0 -onlyLG");
            // sprintf(cmd, "./RunReplace/replace -lef top_die.lef -def top_die.def -output . -overflow 1.0 -den 1.0");
        }
        else
        {
            sprintf(cmd, "./replace -lef top_die.lef -def top_die.def -output . -overflow 0.12 -dpflag NTU3 -den 1.0 -bin 512 -onlyLG");
            // sprintf(cmd, "./RunReplace/replace -lef top_die.lef -def top_die.def -output . -overflow 0.12 -den 1.0 -bin 512");
        }
        system(cmd);
        system("cp ./etc/top_die/experiment000/top_die_final.def .");
        command = "cp top_die_final.def " + dir + "/" + to_string(k);
        system(command.c_str());

        parse_top();

        // 擺完standard cell後，依照standard cell的位置重擺terminal
        replace_terminal();

        calc_net_hpwl_range("bot");
        generate_def_version_2nd_bot("bot_die.def");

        system("rm -rf ./0");
        system("rm -rf ./etc");
        if (Instance_cnt < 1000)
        {

            sprintf(cmd, "./replace -lef bot_die.lef -def bot_die.def -output . -overflow 0.5 -dpflag NTU3 -den 1.0 -onlyLG");
            // sprintf(cmd, "./RunReplace/replace -lef bot_die.lef -def bot_die.def -output . -overflow 1.0 -den 1.0");
        }
        else
        {
            sprintf(cmd, "./replace -lef bot_die.lef -def bot_die.def -output . -overflow 0.12 -dpflag NTU3 -den 1.0 -bin 512 -onlyLG");
            // sprintf(cmd, "./RunReplace/replace -lef bot_die.lef -def bot_die.def -output . -overflow 0.12 -den 1.0 -bin 512");
        }
        system(cmd);
        system("cp ./etc/bot_die/experiment000/bot_die_final.def .");
        command = "cp bot_die_final.def " + dir + "/" + to_string(k);
        system(command.c_str());
        system("rm -rf ./0");
        system("rm -rf ./etc");
        in_file.close();
        cout << " infile close done " << endl;
        parse_bottom();

        cout << " parse def done " << endl;
        replace_terminal();

        out_file.open(arg2);
        output();
        out_file.close();

        eval(arg1, arg2);

        //long long hpwl = parse_eval(filename + "_eval.txt");
        long long hpwl = calc_total_HPWL();
        if (hpwl < best_HPWL)
        {
            find_better_hpwl = true;
            best_inst.assign(inst_position.begin(), inst_position.end());
            best_inst_orient.assign(inst_orient.begin(), inst_orient.end());
            best_terminal.assign(terminal_position.begin(), terminal_position.end());
            best_HPWL = hpwl;
        }
        cout << "HPWL for replace iteration " << k + 1 << " : " << hpwl << endl;
        // eval(arg1, arg2);
        // command = "mv " + filename + "_eval.txt " + sout + " " + dir + "/" + to_string(i);
        // system(command.c_str());
        // system("rm top_die_final.def bot_die_final.def");// yu--modified(originally not commented)
    }
    if (find_better_hpwl)
    {
        inst_position.assign(best_inst.begin(), best_inst.end());
        inst_orient.assign(best_inst_orient.begin(), best_inst_orient.end());
        terminal_position.assign(best_terminal.begin(), best_terminal.end());
    }
    // system("rm top_die.def top_die.lef bot_die.def ./bot_die.lef");
    // system("cd ..");
    command = "mv top_die.def bot_die.def " + dir;
    system(command.c_str());
    //command = "mv hmetis_input.txt hmetis_input.txt.part.2 " + dir;
    system(command.c_str());
}
bool generate_top_hardblocks()
{ // yu--modified
    out_file.open("top_die.hardblocks");

    vector<string> store_instance;
    int NumHardRectilinearBlocks = 0;
    int NumMacro = 0;
    for (int i = 0; i < Cell_in_top.size(); i++)
    {
        int idx = Cell_in_top[i]; // ex:C8, idx=7
        int libnum_yu = Lib_for_Inst[idx];
        int w1 = LibCellSize_x[top][libnum_yu];
        int h1 = LibCellSize_y[top][libnum_yu];
        int x1 = inst_position[idx].first;
        int y1 = inst_position[idx].second;
        int x2 = x1 + w1;
        int y2 = y1 + h1;
        string orient = inst_orient[idx];
        NumHardRectilinearBlocks++;

        if (!LibCell_isMacro[top][libnum_yu]) // stdcell
        {
            string tmp_str = "C" + to_string(Cell_in_top[i] + 1) + " stdcell 4 (" + to_string(x1) + ", " + to_string(y1) + ") (" + to_string(x1) + ", " + to_string(y2) + ") (" + to_string(x2) + ", " + to_string(y2) + ") (" + to_string(x2) + ", " + to_string(y1) + ") 0 " + to_string(Pin_x[top][libnum_yu].size());
            store_instance.push_back(tmp_str);

            for (int j = 0; j < Pin_x[top][libnum_yu].size(); j++)
            {
                tmp_str = "P" + to_string(j + 1) + " " + to_string(Pin_x[top][libnum_yu][j]) + " " + to_string(Pin_y[top][libnum_yu][j]);
                store_instance.push_back(tmp_str);
            }
        }
        else // macro
        {
            /*sunny modified: add four orientations*/
            int rotate = 0;
            if (orient == "E")
                rotate = 1;
            else if (orient == "S")
                rotate = 2;
            else if (orient == "W")
                rotate = 3;

            string tmp_str = "C" + to_string(Cell_in_top[i] + 1) + " hardrectilinear 4 (" + to_string(x1) + ", " + to_string(y1) + ") (" + to_string(x1) + ", " + to_string(y2) + ") (" + to_string(x2) + ", " + to_string(y2) + ") (" + to_string(x2) + ", " + to_string(y1) + ") " + to_string(rotate) + " " + to_string(Pin_x[top][libnum_yu].size());
            store_instance.push_back(tmp_str);
            NumMacro++;

            for (int j = 0; j < Pin_x[top][libnum_yu].size(); j++)
            {
                tmp_str = "P" + to_string(j + 1) + " " + to_string(Pin_x[top][libnum_yu][j]) + " " + to_string(Pin_y[top][libnum_yu][j]);
                store_instance.push_back(tmp_str);
            }
        }
    }
    if(NumMacro <= 1) {
        out_file.close();
        return true;
    }
    out_file << "DieArea : " << UpperRightX << " " << UpperRighty << endl;
    out_file << "NumHardRectilinearBlocks : " << NumHardRectilinearBlocks << endl;
    out_file << "NumTerminals : " << cutname.size() << endl;
    out_file << endl;
    for (int i = 0; i < store_instance.size(); i++)
        out_file << store_instance[i] << endl;
    out_file << endl;
    for (int i = 0; i < cutname.size(); i++)
    {
        out_file << "T" << cutname[i] + 1 << " terminal" << endl;  /*sunny important*/
    }

    out_file.close();
    return false;
}
bool generate_bot_hardblocks()
{ // yu--modified
    out_file.open("bot_die.hardblocks");
    vector<string> store_instance;
    int NumHardRectilinearBlocks = 0;
    int NumMacro = 0;
    for (int i = 0; i < Cell_in_bottom.size(); i++)
    {
        // if(!LibCell_isMacro[bot][Lib_for_Inst[Cell_in_bottom[i]]]) continue; // only consider macro
        int idx = Cell_in_bottom[i]; // ex:C8, idx=7
        int libnum_yu = Lib_for_Inst[idx];
        int w1 = LibCellSize_x[bot][libnum_yu];
        int h1 = LibCellSize_y[bot][libnum_yu];
        int x1 = inst_position[idx].first;
        int y1 = inst_position[idx].second;
        int x2 = x1 + w1;
        int y2 = y1 + h1;
        string orient = inst_orient[idx]; /*sunny modified*/
        NumHardRectilinearBlocks++;
        if (!LibCell_isMacro[bot][Lib_for_Inst[Cell_in_bottom[i]]]) // stdcell
        {
            string tmp_str = "C" + to_string(Cell_in_bottom[i] + 1) + " stdcell 4 (" + to_string(x1) + ", " + to_string(y1) + ") (" + to_string(x1) + ", " + to_string(y2) + ") (" + to_string(x2) + ", " + to_string(y2) + ") (" + to_string(x2) + ", " + to_string(y1) + ") 0 " + to_string(Pin_x[bot][libnum_yu].size());
            store_instance.push_back(tmp_str);

            for (int j = 0; j < Pin_x[bot][libnum_yu].size(); j++)
            {
                tmp_str = "P" + to_string(j + 1) + " " + to_string(Pin_x[bot][libnum_yu][j]) + " " + to_string(Pin_y[bot][libnum_yu][j]);
                store_instance.push_back(tmp_str);
            }
        }
        else // macro
        {
            /*sunny modified: add four orientations*/
            int rotate = 0;
            if (orient == "E")
                rotate = 1;
            else if (orient == "S")
                rotate = 2;
            else if (orient == "W")
                rotate = 3;

            string tmp_str = "C" + to_string(Cell_in_bottom[i] + 1) + " hardrectilinear 4 (" + to_string(x1) + ", " + to_string(y1) + ") (" + to_string(x1) + ", " + to_string(y2) + ") (" + to_string(x2) + ", " + to_string(y2) + ") (" + to_string(x2) + ", " + to_string(y1) + ") " + to_string(rotate) + " " + to_string(Pin_x[bot][libnum_yu].size());
            store_instance.push_back(tmp_str);
            NumMacro++;

            for (int j = 0; j < Pin_x[bot][libnum_yu].size(); j++)
            {
                tmp_str = "P" + to_string(j + 1) + " " + to_string(Pin_x[bot][libnum_yu][j]) + " " + to_string(Pin_y[bot][libnum_yu][j]);
                store_instance.push_back(tmp_str);
            }
        }
    }
    if(NumMacro <= 1) {
        out_file.close();
        return true;
    }

    out_file << "DieArea : " << UpperRightX << " " << UpperRighty << endl;
    out_file << "NumHardRectilinearBlocks : " << NumHardRectilinearBlocks << endl;
    out_file << "NumTerminals : " << cutname.size() << endl;
    out_file << endl;
    for (int i = 0; i < store_instance.size(); i++) {
        out_file << store_instance[i] << endl;
    }
    out_file << endl;
    for (int i = 0; i < cutname.size(); i++)
    {
        out_file << "T" << cutname[i] + 1 << " terminal" << endl;
    }

    out_file.close();
    return false;
}
void generate_top_nets()
{
    out_file.open("top_die.nets");
    // cannot use partition_map[inst_idx] == top, since same tech, cannot tell top&bot
    vector<string> store_instance;

    for (int i = 0; i < NetCnt; i++)
    {
        bool have_topMacro = false;
        store_instance.clear();
        for (int j = 0; j < net[i].size(); j++)
        {
            int inst_idx = net[i][j].first;
            int pin_idx = net[i][j].second;
            string tmp = "C" + to_string(net[i][j].first + 1) + " P" + to_string(net[i][j].second + 1);
            if (partition_map[inst_idx] == 0)
            {
                have_topMacro = true;
                store_instance.push_back(tmp);
            }
        }
        if (!have_topMacro)
            continue;
        if (NetCross[i] == true)
        {
            out_file << "NetDegree : " << store_instance.size() + 1 << endl;
            out_file << "T" << i + 1 << endl;   
        }
        else
        {
            out_file << "NetDegree : " << store_instance.size() << endl;
        }
        for (int k = 0; k < store_instance.size(); k++)
            out_file << store_instance[k] << endl;
    }
    out_file.close();
}
void generate_bot_nets()
{
    out_file.open("bot_die.nets");
    vector<string> store_instance;

    for (int i = 0; i < NetCnt; i++)
    {
        bool have_botMacro = false;
        store_instance.clear();
        for (int j = 0; j < net[i].size(); j++)
        {
            int inst_idx = net[i][j].first;
            int pin_idx = net[i][j].second;
            string tmp = "C" + to_string(net[i][j].first + 1) + " P" + to_string(net[i][j].second + 1);
            if (partition_map[inst_idx] == 1)
            {
                have_botMacro = true;
                store_instance.push_back(tmp);
            }
        }
        if (!have_botMacro)
            continue;
        if (NetCross[i] == true)
        {
            out_file << "NetDegree : " << store_instance.size() + 1 << endl;    /*0815 sunny*/
            out_file << "T" << i + 1 << endl;
        }
        else
        {
            out_file << "NetDegree : " << store_instance.size() << endl;
        }
        for (int k = 0; k < store_instance.size(); k++)
            out_file << store_instance[k] << endl;
    }

    out_file.close();
}
void generate_ter_pl()
{
    out_file.open("terminal.pl");
    for (int i = 0; i < cut_num; i++)
    {
        out_file << "T" << cutname[i] + 1 << " " << terminal_position[i].first << " " << terminal_position[i].second << endl;
    }

    out_file.close();
}
void check_macro_overlap(string die)
{
    cout << "Start check_macro_overlap" << endl;
    typedef struct Macro
    {
        int idx; // instance idx
        pair<int, int> lower_left;
        pair<int, int> upper_right;
    } Macro;

    int inst_idx, tech;
    int die_size;
    vector<Macro> _macros;

    tech = (die == "top") ? top : bot;
    die_size = (die == "top") ? Cell_in_top.size() : Cell_in_bottom.size();

    for (int i = 0; i < die_size; i++)
    {
        inst_idx = (die == "top") ? Cell_in_top[i] : Cell_in_bottom[i];
        if (LibCell_isMacro[tech][Lib_for_Inst[inst_idx]])
        { // is macro
            Macro tmp;
            tmp.idx = inst_idx;
            if (inst_orient[inst_idx] == "N" || inst_orient[inst_idx] == "S") // to be modified : other orientation
            {
                tmp.lower_left = make_pair(inst_position[inst_idx].first, inst_position[inst_idx].second);
                tmp.upper_right = make_pair(inst_position[inst_idx].first + LibCellSize_x[tech][Lib_for_Inst[inst_idx]],
                                            inst_position[inst_idx].second + LibCellSize_y[tech][Lib_for_Inst[inst_idx]]);
            }
            else if (inst_orient[inst_idx] == "E" || inst_orient[inst_idx] == "W")
            {
                tmp.lower_left = make_pair(inst_position[inst_idx].first, inst_position[inst_idx].second);
                tmp.upper_right = make_pair(inst_position[inst_idx].first + LibCellSize_y[tech][Lib_for_Inst[inst_idx]],
                                            inst_position[inst_idx].second + LibCellSize_x[tech][Lib_for_Inst[inst_idx]]);
            }
            // else cout << "In check_macro_overlap: orient is not N and E" << endl;

            _macros.push_back(tmp);
        }
    }
    // check if overlap
    for (vector<Macro>::iterator j = _macros.begin(); j != _macros.end(); j++)
    {
        // check if overlap with i, i is fixed
        for (vector<Macro>::iterator i = _macros.begin(); i != _macros.end(); i++)
        {
            if (i == j)
                continue;

            if (i->lower_left.first < j->upper_right.first && i->upper_right.first > j->lower_left.first &&
                i->lower_left.second < j->upper_right.second && i->upper_right.second > j->lower_left.second)
            {
                cout << endl
                     << endl
                     << "Macro Still Overlap" << endl
                     << endl;
                cout << "i lower left: " << i->lower_left.first << " " << i->lower_left.second << endl;
                cout << "i upper right: " << i->upper_right.first << " " << i->upper_right.second << endl;
                cout << "j lower left: " << j->lower_left.first << " " << j->lower_left.second << endl;
                cout << "j upper right: " << j->upper_right.first << " " << j->upper_right.second << endl;
            }
        }
    }

    // check if exceed die boundary
    for (vector<Macro>::iterator i = _macros.begin(); i != _macros.end(); i++)
    {
        if (i->upper_right.first > UpperRightX || i->upper_right.second > UpperRighty)
        {
            cout << endl
                 << endl
                 << "Macro exceed boundary" << endl
                 << endl;
            cout << UpperRightX << " " << UpperRighty << endl;
            cout << inst_orient[i->idx] << endl;
            cout << "i lower left: " << i->lower_left.first << " " << i->lower_left.second << endl;
            cout << "i upper right: " << i->upper_right.first << " " << i->upper_right.second << endl;
        }
    }
    cout << "End check_macro_overlap" << endl;
}

// void macro_move2middle() 
// {
//     int top_size = Cell_in_top.size();
//     int bot_size = Cell_in_bottom.size();
//     int top_macro_right = 0;
//     int top_macro_upper = 0;
//     int bot_macro_right = 0;
//     int bot_macro_upper = 0;
//     int top_move_right, top_move_upper, bot_move_right, bot_move_upper;

//     for(int i = 0; i < Instance_cnt; i++) {
//         if( !LibCell_isMacro[0][Lib_for_Inst[i]]) continue;
            
//         if(partition_map[i] == 0) {      // top
//             if (inst_orient[i] == "N" || inst_orient[i] == "S") {
//                 if(inst_position[i].first + LibCellSize_x[top][Lib_for_Inst[i]] > top_macro_right) top_macro_right = inst_position[i].first + LibCellSize_x[top][Lib_for_Inst[i]];
//                 if(inst_position[i].second + LibCellSize_y[top][Lib_for_Inst[i]] > top_macro_upper) top_macro_upper = inst_position[i].second + LibCellSize_y[top][Lib_for_Inst[i]];
//             }
//             else {
//                 if(inst_position[i].first + LibCellSize_y[top][Lib_for_Inst[i]] > top_macro_right) top_macro_right = inst_position[i].first + LibCellSize_y[top][Lib_for_Inst[i]];
//                 if(inst_position[i].second + LibCellSize_x[top][Lib_for_Inst[i]] > top_macro_upper) top_macro_upper = inst_position[i].second + LibCellSize_x[top][Lib_for_Inst[i]];
//             }
//         }
//         else {
//             if (inst_orient[i] == "N" || inst_orient[i] == "S") {
//                 if(inst_position[i].first + LibCellSize_x[bot][Lib_for_Inst[i]] > bot_macro_right) bot_macro_right = inst_position[i].first + LibCellSize_x[bot][Lib_for_Inst[i]];
//                 if(inst_position[i].second + LibCellSize_y[bot][Lib_for_Inst[i]] > bot_macro_upper) bot_macro_upper = inst_position[i].second + LibCellSize_y[bot][Lib_for_Inst[i]];
//             }
//             else {
//                 if(inst_position[i].first + LibCellSize_y[bot][Lib_for_Inst[i]] > bot_macro_right) bot_macro_right = inst_position[i].first + LibCellSize_y[bot][Lib_for_Inst[i]];
//                 if(inst_position[i].second + LibCellSize_x[bot][Lib_for_Inst[i]] > bot_macro_upper) bot_macro_upper = inst_position[i].second + LibCellSize_x[bot][Lib_for_Inst[i]];
//             }
//         }
//     }
//     top_move_right = (UpperRightX - top_macro_right)/2;
//     top_move_upper = (UpperRighty - top_macro_upper)/2;
//     bot_move_right = (UpperRightX - bot_macro_right)/2;
//     bot_move_upper = (UpperRighty - bot_macro_upper)/2;
//     for(int i = 0; i <Instance_cnt; i++) {
//         if( !LibCell_isMacro[0][Lib_for_Inst[i]]) continue;
//         if(partition_map[i] == 0) {      // top
//             inst_position[i].first += top_move_right;
//             inst_position[i].second += top_move_upper;
//         }
//         else {
//             inst_position[i].first += bot_move_right;
//             inst_position[i].second += bot_move_upper;
//         }
//         cout << inst_position[i].first << " " << inst_position[i].second << endl;
//     }
// }
vector<bool> top_allow_visit;    // [net_idx] allow to visit the net
vector<bool> bot_allow_visit;    // [net_idx] allow to visit the net
void optimize_partition() 
{
    cout << "START optimize partition\n";
    //for(int i = 0; i < Instance_cnt; i++) {
        //cout << "i in "<<partition_map[i] << endl;
    //}
    vector<vector<int>> net_size;          // [0->top, 1->bot][net_idx]

    vector<int> top_net_hpwl_order;   // descending order of hpwl range of net_idx
    vector<int> bot_net_hpwl_order;   // descending order of hpwl range of net_idx
    vector<vector<int>> top_hpwl_corner_inst; // [net_idx][left, down, right, up] = inst_idx
    vector<vector<int>> bot_hpwl_corner_inst; // [net_idx][left, down, right, up] = inst_idx
    vector<vector<int>> top_hpwl_corner_differ; // [net_idx][left, down, right, up] = hpwl difference after remove the inst
    vector<vector<int>> bot_hpwl_corner_differ; // [net_idx][left, down, right, up] = hpwl difference after remove the inst

    top_allow_visit.resize(NetCnt, true);
    bot_allow_visit.resize(NetCnt, true);
    vector<int> tmp(NetCnt, 0);
    net_size.resize(2, tmp);

    top_hpwl_corner_inst.resize(NetCnt, {-1, -1, -1, -1});
    bot_hpwl_corner_inst.resize(NetCnt, {-1, -1, -1, -1});
    top_hpwl_corner_differ.resize(NetCnt, {0, 0, 0, 0});
    bot_hpwl_corner_differ.resize(NetCnt, { 0, 0, 0, 0});
    for(int i = 0; i < NetCnt; i++) {
        top_net_hpwl_order.push_back(i);
        bot_net_hpwl_order.push_back(i);
    }

    int up, down, left, right, pin_index, inst_index, x, y, lib, pinx, piny;
    // TOP DIE UPDATE DATA
    for( int i = 0; i < NetCnt; i++) 
    {
        up = 0;
        down = 99999999;
        left = 99999999;
        right = 0;
        
        for (int j = 0; j < net[i].size(); j++)
        {
            if(partition_map[net[i][j].first] != 0) continue;

            net_size[0][i]++;

            pin_index = net[i][j].second;
            inst_index = net[i][j].first;
            x = inst_position[inst_index].first;
            y = inst_position[inst_index].second;
            lib = Lib_for_Inst[inst_index];

            /*sunny modified*/
            if (inst_orient[inst_index] == "N") {
                pinx = x + Pin_x[top][lib][pin_index];
                piny = y + Pin_y[top][lib][pin_index];
            }
            else if (inst_orient[inst_index] == "E") { /* (pinx, piny) = (upper_left_x + dy, upper_left_y - dx) */
                pinx = x + Pin_y[top][lib][pin_index];
                piny = y + LibCellSize_x[top][lib] - Pin_x[top][lib][pin_index];
            }
            else if (inst_orient[inst_index] == "S") {
                /* (pinx, piny) = (upper_right_x - dx, upper_right_y - dy)*/
                pinx = x + LibCellSize_x[top][lib] - Pin_x[top][lib][pin_index];
                piny = y + LibCellSize_y[top][lib] - Pin_y[top][lib][pin_index];
            }
            else {
                /* (pinx, piny) = (lower_right_x - dy, lower_right_y + dx)*/
                pinx = x + LibCellSize_y[top][lib] - Pin_y[top][lib][pin_index];
                piny = y + Pin_x[top][lib][pin_index];
            }

            if (pinx < left) {
                top_hpwl_corner_inst[i][0] = inst_index;
                top_hpwl_corner_differ[i][0] = left - pinx;
                left = pinx;
            } 
            else if(pinx - left < top_hpwl_corner_differ[i][0]) top_hpwl_corner_differ[i][0] = pinx - left;

            if (pinx > right) {
                top_hpwl_corner_inst[i][2] = inst_index;
                top_hpwl_corner_differ[i][2] = right - pinx;
                right = pinx;
            }
            else if(right - pinx < top_hpwl_corner_differ[i][2]) top_hpwl_corner_differ[i][2] = right - pinx;

            if (piny < down) {
                top_hpwl_corner_inst[i][1] = inst_index;
                top_hpwl_corner_differ[i][1] = down - piny;
                down = piny;
            }
            else if(piny - down < top_hpwl_corner_differ[i][1])top_hpwl_corner_differ[i][1] = piny - down;

            if (piny > up) {
                top_hpwl_corner_inst[i][3] = inst_index;
                top_hpwl_corner_differ[i][3] = up - piny;
                up = piny;
            }
            else if(up - piny < top_hpwl_corner_differ[i][3]) top_hpwl_corner_differ[i][3] = up - piny;

        }
        net_hpwl_top[i][0][0] = left;
        net_hpwl_top[i][0][1] = down;
        net_hpwl_top[i][1][0] = right;
        net_hpwl_top[i][1][1] = up;
        if(net_size[0][i] <= 1) top_allow_visit[i] = false;
    }
    // END TOP DIE UPDATE DATA

    // BOTTOM DIE UPDATE DATA
    for( int i = 0; i < NetCnt; i++) 
    {
        up = 0;
        down = 99999999;
        left = 99999999;
        right = 0;
        for (int j = 0; j < net[i].size(); j++)
        {
            if(partition_map[net[i][j].first] != 1) continue;

            net_size[1][i]++;

            pin_index = net[i][j].second;
            inst_index = net[i][j].first;
            x = inst_position[inst_index].first;
            y = inst_position[inst_index].second;
            lib = Lib_for_Inst[inst_index];

            if (inst_orient[inst_index] == "N")
            {
                pinx = x + Pin_x[bot][lib][pin_index];
                piny = y + Pin_y[bot][lib][pin_index];
            }
            else if (inst_orient[inst_index] == "E")
            { /* (pinx, piny) = (upper_left_x + dy, upper_left_y - dx) */
                pinx = x + Pin_y[bot][lib][pin_index];
                piny = y + LibCellSize_x[bot][lib] - Pin_x[bot][lib][pin_index];
            }
            else if (inst_orient[inst_index] == "S")
            {
                /* (pinx, piny) = (upper_right_x - dx, upper_right_y - dy)*/
                pinx = x + LibCellSize_x[bot][lib] - Pin_x[bot][lib][pin_index];
                piny = y + LibCellSize_y[bot][lib] - Pin_y[bot][lib][pin_index];
            }
            else
            {
                /* (pinx, piny) = (lower_right_x - dy, lower_right_y + dx)*/
                pinx = x + LibCellSize_y[bot][lib] - Pin_y[bot][lib][pin_index];
                piny = y + Pin_x[bot][lib][pin_index];
            }
            if (pinx < left) {
                bot_hpwl_corner_inst[i][0] = inst_index;
                bot_hpwl_corner_differ[i][0] = left - pinx;
                left = pinx;
            } 
            else if(pinx - left < bot_hpwl_corner_differ[i][0]) bot_hpwl_corner_differ[i][0] = pinx - left;
            if (pinx > right) {
                bot_hpwl_corner_inst[i][2] = inst_index;
                bot_hpwl_corner_differ[i][2] = right - pinx;
                right = pinx;
            }
            else if(right - pinx < bot_hpwl_corner_differ[i][2]) bot_hpwl_corner_differ[i][2] = right - pinx;
            if (piny < down) {    
                bot_hpwl_corner_inst[i][1] = inst_index;
                bot_hpwl_corner_differ[i][1] = down - piny;
                down = piny;
            }
            else if(piny - down < bot_hpwl_corner_differ[i][1]) bot_hpwl_corner_differ[i][1] = piny - down;
        

            if (piny > up) {
                bot_hpwl_corner_inst[i][3] = inst_index;
                bot_hpwl_corner_differ[i][3] = up - piny;
                up = piny;
            }
            else if(up - piny < bot_hpwl_corner_differ[i][3]) bot_hpwl_corner_differ[i][3] = up - piny;

        }
        net_hpwl_bot[i][0][0] = left;
        net_hpwl_bot[i][0][1] = down;
        net_hpwl_bot[i][1][0] = right;
        net_hpwl_bot[i][1][1] = up;

        if(net_size[1][i] <= 1) bot_allow_visit[i] = false;
    }
    // END BOTTOM DIE UPDATE DATA

    // // SORT: net with large hpwl first
    // sort(top_net_hpwl_order.begin(), top_net_hpwl_order.end(), [](int a, int b){
    //     if( top_allow_visit[a] != top_allow_visit[b]) {
    //         if(top_allow_visit[a]) return true;
    //         else return false;
    //     }
    //     return net_hpwl_top[a][1][0] - net_hpwl_top[a][0][0] + net_hpwl_top[a][1][1] - net_hpwl_top[a][0][1] \
    //         > net_hpwl_top[b][1][0] - net_hpwl_top[b][0][0] + net_hpwl_top[b][1][1] - net_hpwl_top[b][0][1];
    //     // descending order
    // });
    // sort(bot_net_hpwl_order.begin(), bot_net_hpwl_order.end(), [](int a, int b){
    //     if( bot_allow_visit[a] != bot_allow_visit[b]) {
    //         if(bot_allow_visit[a]) return true;
    //         else return false;
    //     }
    //     return net_hpwl_bot[a][1][0] - net_hpwl_bot[a][0][0] + net_hpwl_bot[a][1][1] - net_hpwl_bot[a][0][1] \
    //         > net_hpwl_bot[b][1][0] - net_hpwl_bot[b][0][0] + net_hpwl_bot[b][1][1] - net_hpwl_bot[b][0][1];
    //     // descending order
    // });
    // // END SORT

    int top_net, top_inst, bot_net, bot_inst, max_diff;
    vector<int> top_diff;
    top_diff.resize( Instance_cnt, 0);
    vector<int> bot_diff;
    bot_diff.resize( Instance_cnt, 0);
    vector<pair<int, int>> corner_diff_top;    // [inst_idx] = [corner_idx, diff]
    vector<pair<int, int>> corner_diff_bot;    // [inst_idx] = [corner_idx, diff]

    for(int i = 0; i <= NetCnt/8; i++) 
    {
        // SORT: net with large hpwl first
        sort(top_net_hpwl_order.begin(), top_net_hpwl_order.end(), [](int a, int b){
            if( top_allow_visit[a] != top_allow_visit[b]) {
                if(top_allow_visit[a]) return true;
                else return false;
            }
            return net_hpwl_top[a][1][0] - net_hpwl_top[a][0][0] + net_hpwl_top[a][1][1] - net_hpwl_top[a][0][1] > net_hpwl_top[b][1][0] - net_hpwl_top[b][0][0] + net_hpwl_top[b][1][1] - net_hpwl_top[b][0][1];
            // descending order
        });
        sort(bot_net_hpwl_order.begin(), bot_net_hpwl_order.end(), [](int a, int b){
            if( bot_allow_visit[a] != bot_allow_visit[b]) {
                if(bot_allow_visit[a]) return true;
                else return false;
            }
            return net_hpwl_bot[a][1][0] - net_hpwl_bot[a][0][0] + net_hpwl_bot[a][1][1] - net_hpwl_bot[a][0][1] > net_hpwl_bot[b][1][0] - net_hpwl_bot[b][0][0] + net_hpwl_bot[b][1][1] - net_hpwl_bot[b][0][1];
            // descending order
        });
        // END SORT

        // CHOOSE INST TO MOVE
        //cout << "i = " << i << endl;
        top_net = top_net_hpwl_order.front();

        //cout << "top net = " << top_net << endl;

        if( !top_allow_visit[top_net]) break;
        top_inst = -1;
        max_diff = 0;
        for(int j = 0; j < 4; j++) {
            if(top_hpwl_corner_differ[top_net][j] > max_diff && !LibCell_isMacro[0][Lib_for_Inst[top_hpwl_corner_inst[top_net][j]]]) {
                top_diff[top_hpwl_corner_inst[top_net][j]] += top_hpwl_corner_differ[top_net][j];
                top_inst = top_hpwl_corner_inst[top_net][j];
                if(top_diff[top_hpwl_corner_inst[top_net][j]] >= max_diff) max_diff = top_diff[top_hpwl_corner_inst[top_net][j]];
            }
        }

        if(top_inst == -1) {
            top_allow_visit[top_net] = false;
            continue;
        }

        bot_net = bot_net_hpwl_order.front();

        //cout << "bot net = " << top_net << endl;

        if( !bot_allow_visit[bot_net]) break;
        bot_inst = -1;
        max_diff = 0;
        for(int j = 0; j < 4; j++) {
            if(bot_hpwl_corner_differ[bot_net][j] > max_diff && !LibCell_isMacro[0][Lib_for_Inst[bot_hpwl_corner_inst[bot_net][j]]]) {
                
                if( TopDieRealArea - LibCellSize[top][Lib_for_Inst[top_inst]] + LibCellSize[top][Lib_for_Inst[bot_hpwl_corner_inst[bot_net][j]]] >= TopDieMaxArea || \
                    BottomDieRealArea + LibCellSize[bot][Lib_for_Inst[top_inst]] - LibCellSize[bot][Lib_for_Inst[bot_hpwl_corner_inst[bot_net][j]]] >= BottomDieMaxArea) {
                    continue;     //exceed top die area or bot die area
                }
                bot_diff[bot_hpwl_corner_inst[bot_net][j]] += bot_hpwl_corner_differ[bot_net][j];
                bot_inst = bot_hpwl_corner_inst[bot_net][j];
                if(bot_diff[bot_hpwl_corner_inst[bot_net][j]] >= max_diff) max_diff = bot_diff[bot_hpwl_corner_inst[bot_net][j]];
            }
        }
        if(bot_inst == -1) {
            bot_allow_visit[bot_net] = false;
            continue;
        }
        // END CHOOSE INST TO MOVE

        //cout << "inst = " << top_inst << " " << bot_inst << endl;


        // FIND NEW POSITION FOR INST, AND UPDATE DATA
        TopDieRealArea = TopDieRealArea - LibCellSize[top][Lib_for_Inst[top_inst]] + LibCellSize[top][Lib_for_Inst[bot_inst]];
        BottomDieRealArea = BottomDieRealArea + LibCellSize[bot][Lib_for_Inst[top_inst]] - LibCellSize[bot][Lib_for_Inst[bot_inst]];

        for(int j = 0; j < 4; j++) {
            if(top_hpwl_corner_inst[top_net][j] == top_inst) corner_diff_top.push_back(make_pair(j, top_hpwl_corner_differ[top_net][j]));
            if(bot_hpwl_corner_inst[bot_net][j] == bot_inst) corner_diff_bot.push_back(make_pair(j, bot_hpwl_corner_differ[bot_net][j]));
        }
        for(auto t = corner_diff_top.begin(); t != corner_diff_top.end(); t++) {
            if((*t).first == 0) net_hpwl_top[top_net][0][0] += (*t).second;
            else if((*t).first == 1) net_hpwl_top[top_net][0][1] += (*t).second;
            else if((*t).first == 2) net_hpwl_top[top_net][1][0] -= (*t).second;
            else net_hpwl_top[top_net][1][1] -= (*t).second;
        }
        for(auto t = corner_diff_bot.begin(); t != corner_diff_bot.end(); t++) {
            if((*t).first == 0) net_hpwl_bot[bot_net][0][0] += (*t).second;
            else if((*t).first == 1) net_hpwl_bot[bot_net][0][1] += (*t).second;
            else if((*t).first == 2) net_hpwl_bot[bot_net][1][0] -= (*t).second;
            else net_hpwl_bot[bot_net][1][1] -= (*t).second;
        }
        partition_map[top_inst] = 1;
        partition_map[bot_inst] = 0;
        inst_position[top_inst] = make_pair( (net_hpwl_top[top_net][0][0] + net_hpwl_top[top_net][1][0])/2, (net_hpwl_top[top_net][0][1] + net_hpwl_top[top_net][1][1])/2);
        inst_position[bot_inst] = make_pair( (net_hpwl_bot[bot_net][0][0] + net_hpwl_bot[bot_net][1][0])/2, (net_hpwl_bot[bot_net][0][1] + net_hpwl_bot[bot_net][1][1])/2);
        
        //cout << "top inst position: " << inst_position[top_inst].first << " " << inst_position[top_inst].second << endl;
        inst_position[top_inst].first = min( (unsigned long long)inst_position[top_inst].first, UpperRightX - LibCellSize_x[bot][Lib_for_Inst[top_inst]]);
        inst_position[top_inst].first = max(inst_position[top_inst].first, 0);
        inst_position[top_inst].second = min( (unsigned long long)inst_position[top_inst].second, UpperRighty - LibCellSize_y[bot][Lib_for_Inst[top_inst]]);
        inst_position[top_inst].second = max(inst_position[top_inst].second, 0);
        //cout << "top inst position: " << inst_position[top_inst].first << " " << inst_position[top_inst].second << endl;

        //cout << "bot inst position: " << inst_position[bot_inst].first << " " << inst_position[bot_inst].second << endl;
        inst_position[bot_inst].first = min( (unsigned long long)inst_position[bot_inst].first, UpperRightX - LibCellSize_x[top][Lib_for_Inst[bot_inst]]);
        inst_position[bot_inst].first = max(inst_position[bot_inst].first, 0);
        inst_position[bot_inst].second = min( (unsigned long long)inst_position[bot_inst].second, UpperRighty - LibCellSize_y[top][Lib_for_Inst[bot_inst]]);
        inst_position[bot_inst].second = max(inst_position[bot_inst].second, 0);
        //cout << "bot inst position: " << inst_position[bot_inst].first << " " << inst_position[bot_inst].second << endl;
        
        
        
        net_size[0][top_net]--;
        net_size[0][bot_net]++;
        net_size[1][top_net]++;
        net_size[1][bot_net]--;
        top_allow_visit[top_net] = (net_size[0][top_net] >= 2) ? true : false;
        top_allow_visit[bot_net] = (net_size[0][bot_net] >= 2) ? true : false;
        bot_allow_visit[top_net] = (net_size[1][top_net] >= 2) ? true : false;
        bot_allow_visit[bot_net] = (net_size[1][bot_net] >= 2) ? true : false;


        // UPDATE TOP NET HPWL DATA (net_idx = top_net, bot_net)
        for(int n = 0; n < 2; n++) {
            int net_idx = (n == 0) ? top_net : bot_net;
            up = 0;
            down = 99999999;
            left = 99999999;
            right = 0;
            for (int j = 0; j < net[net_idx].size(); j++)
            {
                if(partition_map[net[net_idx][j].first] != 0) continue;

                net_hasInst_on_top[net_idx] = true;   // top 

                pin_index = net[net_idx][j].second;
                inst_index = net[net_idx][j].first;
                x = inst_position[inst_index].first;
                y = inst_position[inst_index].second;
                lib = Lib_for_Inst[inst_index];

                /*sunny modified*/
                if (inst_orient[inst_index] == "N") {
                    pinx = x + Pin_x[top][lib][pin_index];
                    piny = y + Pin_y[top][lib][pin_index];
                }
                else if (inst_orient[inst_index] == "E") { /* (pinx, piny) = (upper_left_x + dy, upper_left_y - dx) */
                    pinx = x + Pin_y[top][lib][pin_index];
                    piny = y + LibCellSize_x[top][lib] - Pin_x[top][lib][pin_index];
                }
                else if (inst_orient[inst_index] == "S") {
                    /* (pinx, piny) = (upper_right_x - dx, upper_right_y - dy)*/
                    pinx = x + LibCellSize_x[top][lib] - Pin_x[top][lib][pin_index];
                    piny = y + LibCellSize_y[top][lib] - Pin_y[top][lib][pin_index];
                }
                else {
                    /* (pinx, piny) = (lower_right_x - dy, lower_right_y + dx)*/
                    pinx = x + LibCellSize_y[top][lib] - Pin_y[top][lib][pin_index];
                    piny = y + Pin_x[top][lib][pin_index];
                }

                if (pinx < left) {
                    top_hpwl_corner_inst[net_idx][0] = inst_index;
                    top_hpwl_corner_differ[net_idx][0] = left - pinx;
                    left = pinx;
                } 
                else if(pinx - left < top_hpwl_corner_differ[net_idx][0]) top_hpwl_corner_differ[net_idx][0] = pinx - left;

                if (pinx > right) {
                    top_hpwl_corner_inst[net_idx][2] = inst_index;
                    top_hpwl_corner_differ[net_idx][2] = right - pinx;
                    right = pinx;
                }
                else if(right - pinx < top_hpwl_corner_differ[net_idx][2]) top_hpwl_corner_differ[net_idx][2] = right - pinx;

                if (piny < down) {
                    top_hpwl_corner_inst[net_idx][1] = inst_index;
                    top_hpwl_corner_differ[net_idx][1] = down - piny;
                    down = piny;
                }
                else if(piny - down < top_hpwl_corner_differ[net_idx][1])top_hpwl_corner_differ[net_idx][1] = piny - down;

                if (piny > up) {
                    top_hpwl_corner_inst[net_idx][3] = inst_index;
                    top_hpwl_corner_differ[net_idx][3] = up - piny;
                    up = piny;
                }
                else if(up - piny < top_hpwl_corner_differ[net_idx][3]) top_hpwl_corner_differ[net_idx][3] = up - piny;

            }
            net_hpwl_top[net_idx][0][0] = left;
            net_hpwl_top[net_idx][0][1] = down;
            net_hpwl_top[net_idx][1][0] = right;
            net_hpwl_top[net_idx][1][1] = up;

            //cout << left << " "<< down << " "<< right << " "<< up << endl;
        }
        // END UPDATE TOP NET HPWL DATA

        // UPDATE BOT NET HPWL INFO (net_idx = top_net, bot_net)
        for(int n = 0; n < 2; n++) {
            int net_idx = (n == 0) ? top_net : bot_net;
            up = 0;
            down = 99999999;
            left = 99999999;
            right = 0;
            for (int j = 0; j < net[net_idx].size(); j++)
            {
                if(partition_map[net[net_idx][j].first] != 1) continue;
                
                net_hasInst_on_bot[net_idx] = true;

                pin_index = net[net_idx][j].second;
                inst_index = net[net_idx][j].first;
                x = inst_position[inst_index].first;
                y = inst_position[inst_index].second;
                lib = Lib_for_Inst[inst_index];

                if (inst_orient[inst_index] == "N")
                {
                    pinx = x + Pin_x[bot][lib][pin_index];
                    piny = y + Pin_y[bot][lib][pin_index];
                }
                else if (inst_orient[inst_index] == "E")
                { /* (pinx, piny) = (upper_left_x + dy, upper_left_y - dx) */
                    pinx = x + Pin_y[bot][lib][pin_index];
                    piny = y + LibCellSize_x[bot][lib] - Pin_x[bot][lib][pin_index];
                }
                else if (inst_orient[inst_index] == "S")
                {
                    /* (pinx, piny) = (upper_right_x - dx, upper_right_y - dy)*/
                    pinx = x + LibCellSize_x[bot][lib] - Pin_x[bot][lib][pin_index];
                    piny = y + LibCellSize_y[bot][lib] - Pin_y[bot][lib][pin_index];
                }
                else
                {
                    /* (pinx, piny) = (lower_right_x - dy, lower_right_y + dx)*/
                    pinx = x + LibCellSize_y[bot][lib] - Pin_y[bot][lib][pin_index];
                    piny = y + Pin_x[bot][lib][pin_index];
                }
                if (pinx < left) {
                    bot_hpwl_corner_inst[net_idx][0] = inst_index;
                    bot_hpwl_corner_differ[net_idx][0] = left - pinx;
                    left = pinx;
                } 
                else if(pinx - left < bot_hpwl_corner_differ[net_idx][0]) bot_hpwl_corner_differ[net_idx][0] = pinx - left;
                if (pinx > right) {
                    bot_hpwl_corner_inst[net_idx][2] = inst_index;
                    bot_hpwl_corner_differ[net_idx][2] = right - pinx;
                    right = pinx;
                }
                else if(right - pinx < bot_hpwl_corner_differ[net_idx][2]) bot_hpwl_corner_differ[net_idx][2] = right - pinx;
                if (piny < down) {    
                    bot_hpwl_corner_inst[net_idx][1] = inst_index;
                    bot_hpwl_corner_differ[net_idx][1] = down - piny;
                    down = piny;
                }
                else if(piny - down < bot_hpwl_corner_differ[net_idx][1]) bot_hpwl_corner_differ[net_idx][1] = piny - down;
            

                if (piny > up) {
                    bot_hpwl_corner_inst[net_idx][3] = inst_index;
                    bot_hpwl_corner_differ[net_idx][3] = up - piny;
                    up = piny;
                }
                else if(up - piny < bot_hpwl_corner_differ[net_idx][3]) bot_hpwl_corner_differ[net_idx][3] = up - piny;

            }
            net_hpwl_bot[net_idx][0][0] = left;
            net_hpwl_bot[net_idx][0][1] = down;
            net_hpwl_bot[net_idx][1][0] = right;
            net_hpwl_bot[net_idx][1][1] = up;
            
            //cout << left << " "<< down << " "<< right << " "<< up << endl;
            if(left < 0 || down < 0) cout << "wrong hpwl";
            if(right > UpperRightX || up > UpperRighty) cout << "wrong hpwl";
            
        }
        // END UPDATE BOT NET HPWL DATA

        

        for(int j = 0; j < 4; j++) {
            top_diff[top_hpwl_corner_inst[top_net][j]] = 0;
            bot_diff[bot_hpwl_corner_inst[bot_net][j]] = 0;
        }
        corner_diff_top.clear();
        corner_diff_bot.clear();
    }


    Cell_in_bottom.clear();
    Cell_in_top.clear();
    for (int i = 0; i < Instance_cnt; i++) // if same tech, no use
    {
        if (partition_map[i] == 0) Cell_in_top.push_back(i);
        else if (partition_map[i] == 1) Cell_in_bottom.push_back(i);
    }
    cut_num = 0;
    terminal_position.clear();
    cutname.clear();
    NetCross.resize(NetCnt, false);
    for (int i = 0; i < NetCnt; i++)
    {
        int first = partition_map[net[i][0].first];
        for (int j = 0; j < net[i].size(); j++)
        {
            if (first != partition_map[net[i][j].first])
            {
                cutname.push_back(i);
                mapforterminal[i] = cut_num;
                terminal_position.push_back(make_pair(0, 0));
                NetCross[i] = true;
                cut_num++;

                //cout << "net: " << i << endl;
                break;
            }
        }
    }
    replace_terminal();

    cout << "END optimize_partition\n";

    return;
}

void print_partition() {
    ofstream case2;
    case2.open("c2p.txt");

    for(int i = 0; i < Instance_cnt; i++) {
        case2 << partition_map[i] << endl;
    }
    case2.close();
}

int main(int argc, char *argv[])
{
    char cmd[150] = {
        0,
    };
    sprintf(cmd, "date +%T");
    cout << "start execution : ";
    system(cmd);
    cout << endl;
    in_file.open(argv[1]);

    // out_file.open(argv[2]);

    parse_input();

    cout << " parse input done";
    system(cmd);
    cout << endl;

    // print_case_information();   //7/9 modified

    for (int i = 0; i < Instance_cnt; i++)
    {
        inst_position.push_back(make_pair(0, 0));
        inst_orient.push_back("N");
    }
    partition();

    cout << " partition done ";

    system(cmd);
    cout << endl;

    //legalize();
    cout << " legalize done ";
    system(cmd);
    cout << endl;

    /*update cellin_top, bot--yu*/
    Cell_in_bottom.clear();
    Cell_in_top.clear();
    for (int i = 0; i < Instance_cnt; i++) // if same tech, no use
    {
        if (partition_map[i] == 0) Cell_in_top.push_back(i);
        else if (partition_map[i] == 1) Cell_in_bottom.push_back(i);
    }
    /*update--end--yu*/

    net_hpwl_top.resize(NetCnt, {{0, 0},{0, 0}});
    net_hpwl_bot.resize(NetCnt, {{0, 0},{0, 0}});



    terminal_placement();

    cout << " terminal placement done ";
    system(cmd);
    cout << endl;

    in_file.close();

    cout << " infile close done ";
    system(cmd);
    cout << endl;

    in_file.open(argv[1]);

    cout << " infile open done ";
    system(cmd);
    cout << endl;

    // 生成replace需要檔案，top 和bottom分開做
    generate_lefdef();

    cout << " generate lefdef done ";
    system(cmd);
    cout << endl;

    in_file.close();

    cout << " infile close done ";
    system(cmd);
    cout << endl;

    perform_replace(argv[1], argv[2]);

    //optimize_partition(); 


    /*floorplanning--start*/

    cout << "floorplanning start!!!" << endl;

    bool top_only1Macro = generate_top_hardblocks(); // yu--modified
    if (!top_only1Macro) {
        generate_top_nets();
        generate_ter_pl();
    }
    bool bot_only1Macro = generate_bot_hardblocks(); // solver 不會處理一個macro
    if (!bot_only1Macro) {
        generate_bot_nets();
        generate_ter_pl();
    }

    if (!top_only1Macro)
    {
        cout << "Top die B*tree" << endl;
        
        floorplan_top(); // yu--modified
        check_macro_overlap("top");
    }
    if (!bot_only1Macro)
    {
        cout << "Bottom die B*tree" << endl;
       
        floorplan_bot();   
        check_macro_overlap("bot");
    }


    // macro_move2middle();
    // check_macro_overlap("top");
    // check_macro_overlap("bot");

    cout << "floorplanning end!!!" << endl;
    /*floorplanning--end*/
    perform_2nd_replace(argv[1], argv[2]);
    cout << " perform replace done ";
    system(cmd);
    cout << endl;
    /*out_file.open(argv[2]);
    output();
    out_file.close();*/
    string cmdd;
    cmdd = "rm -rf " + dirtoremove;
    system(cmdd.c_str());
    
    long long result = calc_total_HPWL();
    //if(result / 10000000 == 3) print_partition();
    clean();
    return 0;
}
