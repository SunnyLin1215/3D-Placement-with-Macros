#pragma once
#include "../GlobalTimer/GlobalTimer.hpp"
#include "../ResultWriter/ResultWriter.hpp"
#include "../Structure/Data.hpp"
#include <vector>
#include <queue>
#include <algorithm>    // std::swap


class SA_Solver
{
    SAInput *input;
    GlobalTimer &globalTimer;
    signed long long outline;
//delete b*tree for find seed
    Node* getInitialBStartTree();
    // std::vector<int> perturbNPE(std::vector<int> npe, int const &type);
    void setPosition(Node *root);
    void setLeftTree(Node *node, Node* parent);
    void setRightTree(Node *node, Node* parent);
    void clearContour(Node *root);
    signed long long calWirelength() const;
    signed long long calCost(bool const &focusWirelength);
    Node *chooseNode(Node *root, int seed);
    void swapNode(Node *node1, Node *node2);
    void swapTree(Node **node1, Node **node2);
    Node *perturbByRotate(Node *root);
    void perturbBySwap(Node *root);
    void perturbByMove(Node *root);
    void perturbBySubtree(Node *root);
    Node* copyTree(Node *root);
    void clearTree(Node *root);
    Node*  saProcess(double const &c, double const &r, int const &k,
                               Node *root, bool const &focusWirelength);

public:
    SA_Solver(SAInput *input, GlobalTimer &globalTimer);
    ResultWriter *solve();
};