#pragma once
#include <string>
#include <vector>
#include <iostream>

struct Pin
{
    std::string name;
    int x, y, dx, dy;

    Pin(std::string const &name, int const &x, int const &y, int const &dx, int const &dy )
        : name(name), x(x), y(y), dx(dx), dy(dy) {}
};

struct Hardblock
{
    std::string name;
    int width, height, area, x, y;
    int isRotated;                // 0->N (R0), 1->E (R90), 2->S (R180), 3->W (R270)
    int bestRotated;
    std::vector<Pin*> pin;

    Hardblock(std::string const &name, int const &width, int const &height, int const &X, int const &Y, int rotate)
        : name(name), width(width), height(height), area(width*height), x(X), y(Y), isRotated(rotate), bestRotated(rotate) {}
    void rotate();
};

struct Net
{
    std::vector<Pin *> pins;

    signed long long HPWL();
};

struct ContourNode;
struct Node
{
    enum Type
    {
        RCHILD = -2,
        LCHILD = -1,
        ROOT = 0
    };

    Hardblock *hardblock;
    ContourNode *contourNode;
    Node *lchild, *rchild, *parent;
    int type;


    Node(Hardblock *hardblock)
        : hardblock(hardblock), contourNode(nullptr), lchild(nullptr), rchild(nullptr), parent(nullptr), type(0) {}

};

struct ContourNode
{
    Node *node;
    int x_l, x_r, y;
    ContourNode *left, *right;

    ContourNode(Node *node, int x_l, int x_r, int y)
        : node(node), x_l(x_l), x_r(x_r), y(y), left(nullptr), right(nullptr) {}

};

struct SAInput
{
    std::vector<Hardblock *> hardblocks;
    std::vector<Hardblock *> stdcells;    /*sunny modified*/
    std::vector<Net *> nets;
    double deadSpaceRatio;
    int initialSeed;

    int DieX, DieY;      /*sunny modified*/

    /*sunny modified*/
    SAInput(std::vector<Hardblock *> const &hardblocks, std::vector<Hardblock *> const &stdcells, std::vector<Net *> const &nets, double const &deadSpaceRatio, int x, int y)
        : hardblocks(hardblocks), stdcells(stdcells), nets(nets), deadSpaceRatio(deadSpaceRatio), DieX(x), DieY(y) {}
};