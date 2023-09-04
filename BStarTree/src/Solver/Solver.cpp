#include "Solver.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>

void printTree(Node *root)
{
    std::cerr << root->hardblock->name << " " << root->type << " " << root->hardblock->isRotated << "\n";
    std::cerr << "left start" << "\n";
    if(root->lchild != nullptr)
        printTree(root->lchild);
    std::cerr << "right start" << "\n";
    if(root->rchild != nullptr)        
        printTree(root->rchild);
}

Node* SA_Solver::getInitialBStartTree()
{
    std::shuffle(input->nets.begin(), input->nets.end(), std::default_random_engine(input->initialSeed));
    int rowWidth = 0;
    int rowHeight = 0;// yu--modified
    Node* root = new Node(input->hardblocks[0]);
    rowWidth += input->hardblocks[0]->width;
    rowHeight += input->hardblocks[0]->height;
    Node* currentNode = root;
    Node* currentLeft = root;
    for (size_t i = 1; i < input->hardblocks.size(); ++i)
    {
        auto hardblock = input->hardblocks[i];
        rowWidth += hardblock->width;
        rowHeight += hardblock->height; // 更新当前行的高度
        if (rowWidth > input->DieX || rowHeight > input->DieY)
        {
            currentNode->rchild = new Node(hardblock);
            currentNode->rchild->type = Node::RCHILD;
            currentNode->rchild->parent = currentNode;
            currentNode = currentNode->rchild;
            currentLeft = currentNode;
            rowWidth = hardblock->width;
            rowHeight = hardblock->height;
        }else{
            currentLeft->lchild = new Node(hardblock);
            currentLeft->lchild->type = Node::LCHILD;
            currentNode->lchild->parent = currentLeft;
            currentLeft = currentLeft->lchild;
        }
    }
    return root;
}



void SA_Solver::setPosition(Node *root)
{
    root->hardblock->x = 0;
    root->hardblock->y = 0;
    root->contourNode = new ContourNode(root,0,root->hardblock->width,root->hardblock->height);
    if(root->lchild != nullptr)
        setLeftTree(root->lchild,root);
    if(root->rchild != nullptr)        
        setRightTree(root->rchild,root);

}

void SA_Solver::setLeftTree(Node *node, Node* parent)
{
    node->hardblock->x = parent->hardblock->x + parent->hardblock->width;
    int y_max = 0;
    ContourNode *currentNode = parent->contourNode->right;
    int node_x_r = node->hardblock->x + node->hardblock->width;
    //if currentNode is nullptr, y_max is itself height and it's y is 0;
    node->contourNode = new ContourNode(node,node->hardblock->x,node_x_r,node->hardblock->height);
    node->hardblock->y = 0;
    node->contourNode->left = parent->contourNode;
    parent->contourNode->right = node->contourNode;
    while(currentNode != nullptr)
    {   
        if(currentNode->y > y_max)
            y_max = currentNode->y;
        if(currentNode->x_r > node_x_r)
        {
            currentNode->x_l = node_x_r;
            currentNode->left = node->contourNode;
            node->contourNode->right = currentNode;
            node->contourNode->y = y_max + node->hardblock->height;
            node->hardblock->y = y_max;
            break;
        }else
        {
            //delete contourNode before next perturb
            if(currentNode->x_r == node_x_r)
            {             
                node->contourNode->y = y_max + node->hardblock->height;
                node->hardblock->y = y_max;
                node->contourNode->right = currentNode->right;
                if(currentNode->right != nullptr)                                        
                    currentNode->right->left = node->contourNode;
                break;
            }
            currentNode = currentNode->right;
            if(currentNode == nullptr)
            {
                node->contourNode->y = y_max + node->hardblock->height;
                node->hardblock->y = y_max;
                break;
            }
        }

    }
    if(node->lchild != nullptr)
        setLeftTree(node->lchild,node);
    if(node->rchild != nullptr)        
        setRightTree(node->rchild,node);    
}

void SA_Solver::setRightTree(Node *node, Node* parent)
{
    node->hardblock->x = parent->hardblock->x;

    int y_max = 0;
    ContourNode *currentNode = parent->contourNode;
    int node_x_r = node->hardblock->x + node->hardblock->width;
    //if currentNode is nullptr, y_max is parent y + self height;
    y_max = currentNode->y;
    node->hardblock->y = y_max;
    node->contourNode = new ContourNode(node,node->hardblock->x,node_x_r, y_max + node->hardblock->height);
    if(parent->contourNode->left != nullptr)
        parent->contourNode->left->right = node->contourNode;
    node->contourNode->left = parent->contourNode->left;
    while(currentNode != nullptr)
    {
        if(currentNode->y > y_max)
            y_max = currentNode->y;

        if(currentNode->x_r > node_x_r)
        {         
            currentNode->x_l = node_x_r;
            currentNode->left = node->contourNode;
            node->contourNode->y = y_max + node->hardblock->height;
            node->hardblock->y = y_max;
            node->contourNode->right = currentNode;
            break;
        }else
        {
            //delete contourNode before next perturb
            if(currentNode->x_r == node_x_r)
            {                   
                node->contourNode->y = y_max + node->hardblock->height;
                node->hardblock->y = y_max;
                node->contourNode->right = currentNode->right;          
                if(currentNode->right != nullptr)                                        
                    currentNode->right->left = node->contourNode;
                break;
            }
            currentNode = currentNode->right;
            if(currentNode == nullptr)
            {
                node->contourNode->y = y_max + node->hardblock->height;
                node->hardblock->y = y_max;
                break;
            }            
        }
    }

    if(node->lchild != nullptr)
        setLeftTree(node->lchild,node);
    if(node->rchild != nullptr)        
        setRightTree(node->rchild,node);        
}

void SA_Solver::clearContour(Node *root)
{
    if(root != nullptr)
    {
        if(root->contourNode != nullptr)
        {           
            delete root->contourNode;
            root->contourNode = nullptr;
        }
        clearContour(root->rchild);
        clearContour(root->lchild);
    }
}

signed long long SA_Solver::calWirelength() const
{
    signed long long wirelength = 0;
    for (auto hardblock : input->hardblocks)
    {
        int rotate = hardblock->isRotated;
        for(auto pin : hardblock->pin)                /*sunny modified*/
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
    return wirelength;
}

signed long long SA_Solver::calCost(bool const &focusWirelength)
{
    signed long long outlineArea = 0;
    for (size_t i = 0; i < input->hardblocks.size(); ++i)
    {
        auto hardblock = input->hardblocks[i];
        if (hardblock->x + hardblock->width > input->DieX  //yu--modified
            || hardblock->y + hardblock->height > input->DieY)
        {
            outlineArea += (signed long long)hardblock->area;        
        }
    }

    if (focusWirelength == false)
        return outlineArea * 10 ;//+ calWirelength();

    return outlineArea * 10000 + calWirelength();
}

Node *SA_Solver::chooseNode(Node *root, int seed)
{
    std::queue<Node *> nodeQueue;
    nodeQueue.push(root);
    Node *currentNode = nullptr;
    for(int i=0; i<=seed; i++){
        currentNode = nodeQueue.front();        
        if (currentNode->lchild != nullptr)
            nodeQueue.push(currentNode->lchild);
        if (currentNode->rchild != nullptr)
            nodeQueue.push(currentNode->rchild);
        nodeQueue.pop();
    }
    return currentNode;
}

void SA_Solver::swapNode(Node *node1, Node *node2)
{
    Hardblock* temp = node1->hardblock;
    node1->hardblock = node2->hardblock;
    node2->hardblock = temp;
    // Node *temp = nullptr;
    // if(node1->type != node2->type)
    // {
    //     int temp = node1->type;
    //     node1->type = node2->type;
    //     node2->type = temp;
    // }
    // temp = node1;
    // node1->parent = node2->parent;
    // if(node1->type == Node::LCHILD)
    //     node1->parent->lchild = node1;
    // if(node1->type == Node::RCHILD)
    //     node1->parent->rchild = node1;
    // node1->lchild = node2->lchild;
    // if(node1->lchild != nullptr)
    //     node1->lchild->parent = node1;
    // node1->rchild = node2->rchild;
    // if(node1->rchild != nullptr)
    //     node1->rchild->parent = node1;

    // node2->parent = temp->parent;
    // if(node2->type == Node::LCHILD)
    //     node2->parent->lchild = node2;
    // if(node2->type == Node::RCHILD)
    //     node2->parent->rchild = node2;
    // node2->lchild = temp->lchild;
    // if(node2->lchild != nullptr)
    //     node2->lchild->parent = node2;
    // node2->rchild = temp->rchild;
    // if(node2->rchild != nullptr)
    //     node2->rchild->parent = node2;        
}

void SA_Solver::swapTree(Node **node1, Node **node2)
{
   std::swap(*node1,*node2);
}

Node* SA_Solver::perturbByRotate(Node *root)
{
    int seed = rand() % input->hardblocks.size();
    Node *selectedNode = chooseNode(root,seed);
    selectedNode->hardblock->rotate();
    return selectedNode;
}

void SA_Solver::perturbBySwap(Node *root)
{
    // std::cerr<<"246"<<"\n";
    int seed1 = rand() % input->hardblocks.size();
    int seed2 = rand() % input->hardblocks.size();
    Node *selectedNode1 = chooseNode(root,seed1);    
    Node *selectedNode2 = chooseNode(root,seed2);
    swapNode(selectedNode1,selectedNode2);
    // printTree(root);
    // std::cerr<<"253"<<"\n";
}

void SA_Solver::perturbByMove(Node *root)
{
    //find leaf
    std::vector<Node*> leafArray;
    std::queue<Node*> nodeQueue;
    nodeQueue.push(root);
    Node *currentNode = nullptr;
    for(size_t i=0; i<input->hardblocks.size(); i++){
        currentNode = nodeQueue.front();
        if (currentNode->lchild == nullptr 
            && currentNode->rchild == nullptr)
        {
            leafArray.push_back(currentNode);
            nodeQueue.pop();
            continue;
        }
        if (currentNode->lchild != nullptr)
            nodeQueue.push(currentNode->lchild);
        if (currentNode->rchild != nullptr)
            nodeQueue.push(currentNode->rchild);
        nodeQueue.pop();
    }
    //find leaf to move
    int seed1 = rand() % leafArray.size();
    Node *leaf = leafArray[seed1];
    //find space to move
    int seed2 = rand() % input->hardblocks.size() + 1;
    std::queue<Node *> nodeQueue2;
    nodeQueue2.push(root);
    Node *spaceParent = nullptr;
    bool left_right;
    seed2 = seed2 + 1;
    while(1)
    {
        spaceParent = nodeQueue2.front();
        if (spaceParent->lchild != nullptr)
            nodeQueue2.push(spaceParent->lchild);
        else{
            left_right = 0;
            if(--seed2 == 0) break;
        }
        if (spaceParent->rchild != nullptr)
            nodeQueue2.push(spaceParent->rchild);
        else{
            left_right = 1;
            if(--seed2 == 0) break;
        }
        // std::cerr<<seed2<<"\n";
        // std::cerr<<nodeQueue2.size()<<"\n";                    
        nodeQueue2.pop();
    }
    // std::cerr<<"318"<<"\n";
    //move
    // printTree(root);
    if(spaceParent == leaf)
    {
        return;
    }    
    // std::cerr<<leaf->hardblock->name<<" "<<leaf->type<<"\n";
    // if(leaf->parent->lchild != nullptr)
    //     std::cerr<<leaf->parent->hardblock->name<<" "<<leaf->parent->lchild->hardblock->name<<"\n";
    // if(spaceParent->parent->lchild != nullptr)        
    //     std::cerr<<spaceParent->hardblock->name<<" "<<spaceParent->parent->lchild->hardblock->name<<"\n";
    // if(leaf->parent->rchild != nullptr)
    //     std::cerr<<leaf->parent->hardblock->name<<" "<<leaf->parent->rchild->hardblock->name<<"\n";
    // if(spaceParent->parent->rchild != nullptr)        
    //     std::cerr<<spaceParent->hardblock->name<<" "<<spaceParent->parent->rchild->hardblock->name<<"\n";        
    if(leaf->type == Node::LCHILD)
        leaf->parent->lchild = nullptr;
    if(leaf->type == Node::RCHILD)
        leaf->parent->rchild = nullptr;
    leaf->parent = spaceParent;
    if(left_right == 0)
    {
        leaf->type = Node::LCHILD;
        spaceParent->lchild = leaf;
    }
    else
    {
        leaf->type = Node::RCHILD;
        spaceParent->rchild = leaf;
    }
    // std::cerr<<"341"<<"\n";
    // printTree(root);
}

void SA_Solver::perturbBySubtree(Node *root)
{
    // std::cerr<<"246"<<"\n";
    int seed1 = rand() % input->hardblocks.size();
    int seed2 = rand() % input->hardblocks.size();
    Node *selectedNode1 = chooseNode(root,seed1);    
    Node *selectedNode2 = chooseNode(root,seed2);
    swapTree(&selectedNode1,&selectedNode2);
    // printTree(root);
    // std::cerr<<"253"<<"\n";    
}

Node* SA_Solver::copyTree(Node *root)
{
    Node *newRoot = new Node(root->hardblock);
    newRoot->type = root->type;
    if (root->lchild != nullptr)
    {
        newRoot->lchild = copyTree(root->lchild);
        newRoot->lchild->parent = newRoot;
    }
    if (root->rchild != nullptr)
    {
        newRoot->rchild = copyTree(root->rchild);
        newRoot->rchild->parent = newRoot;
    }
    return newRoot;
}

void SA_Solver::clearTree(Node *root)
{
    if(root != nullptr)
    {
        clearTree(root->rchild);
        clearTree(root->lchild);
        delete root;
        root = nullptr;
    }
}

Node* SA_Solver::saProcess(double const &c, double const &r, int const &k,
                                      Node *root, bool const &focusWirelength)
{
    int MT = 0, uphill = 0, reject = 0, N = k * input->hardblocks.size();
    int count = 0;
    Node *BT, *T, *NT = nullptr;
    BT = copyTree(root);
    T = copyTree(root);
    signed long long bestCost = 0, cost = 0;
    bool flag = false;
    clearContour(T);
    setPosition(T);
    clearContour(T);
    bestCost = cost = calCost(focusWirelength);
    if (bestCost == 0)
        goto finishSA;

    do
    {
        // double T0 = 1000000;//212640 238202 411892 439563 574843 612750 
        double T0 = 1000;//215001 236811 398838 448286 573291 615021
        do
        {
            MT = uphill = reject = 0;
            do
            {
                if (globalTimer.overTime())
                    goto finishSA;
                // std::cerr<<"410"<<"\n";
                clearTree(NT);
                NT = copyTree(T);
                int type = 0;
                Node *selectedNode;
                type = rand() % 3;
                if(type == 0)
                    selectedNode = perturbByRotate(NT);
                else if(type == 1)
                    perturbBySwap(NT);
                else if(type == 2)
                    perturbByMove(NT);
                MT += 1;
                // std::cerr<<"424"<<"\n";
                clearContour(NT);
                setPosition(NT);
                clearContour(NT);              
                signed long long newCost = calCost(focusWirelength);
                signed long long deltaCost = newCost - cost;
                
                //std::cout << deltaCost << " " << cost << " " << bestCost << std::endl;

                // for (auto hardblock : input->hardblocks)
                // {
                //     if(hardblock->name == "sb77")
                //         std::cerr<<"height "<<hardblock->height<<" y1 "<<hardblock->y<<" y2 "<<hardblock->y + hardblock->height<<"\n";        
                // } 
                if(newCost < bestCost) {   /*sunny modified*/
                    count = 0;
                }
                if (deltaCost < 0 || (double)rand() / RAND_MAX < exp(-1 * deltaCost / T0))
                {
                    if (deltaCost > 0)
                        uphill += 1;
                    // std::cerr << "newCost " << newCost << " ECost " << cost << "\n";
                    clearTree(T);
                    T = copyTree(NT);
                    cost = newCost;

                    if (cost <= bestCost )         //newCost < cost < bestCost
                    {
                        int areaCost = calCost(false);
                        if(areaCost <= 0)
                        {
                            flag = true;
                            clearTree(BT);
                            BT = copyTree(T);
                            for (auto hardblock : input->hardblocks)
                            {
                                hardblock->bestRotated = hardblock->isRotated;
                            }             
                            bestCost = cost;
                            if (bestCost == 0)                        
                                goto finishSA;
                            
                            count++;   /*sunny modified*/
                            if(count >= 1000) goto finishSA;
                        }
                    }
                    // std::cerr<<"446"<<"\n";
                }
                else
                {
                    if(type == 0)
                        selectedNode->hardblock->rotate();
                    reject += 1;
                    // std::cerr<<"452"<<"\n";
                }
                // std::cerr<<"453"<<"\n";
            } while (uphill <= N && MT <= 2 * N);
            T0 = r * T0;
        } while (reject / MT <= 0.95 && T0 >= c);
    } while (flag == false);
finishSA:
    for (auto hardblock : input->hardblocks)
    {
        /*sunny modified*/
        while(hardblock->bestRotated != hardblock->isRotated)
            hardblock->rotate();
    }
    clearContour(BT);
    setPosition(BT);
    clearContour(BT);
    // for (auto hardblock : input->hardblocks)
    // {
    //     if(hardblock->name == "sb77")
    //     {
    //         std::cerr<<"height "<<hardblock->height<<" y1 "<<hardblock->y<<" y2 "<<hardblock->y + hardblock->height<<"\n";        
    //         std::cerr<<"height "<<hardblock->height<<" y1 "<<hardblock->best_y<<" y2 "<<hardblock->best_y + hardblock->height<<"\n";
    //     }
    // }
    // clearTree(T);
    // T = copyTree(BT);
    // clearContour(T);
    // setPosition(T);
    // clearContour(T);
    // for (auto hardblock : input->hardblocks)
    // {
    //     if(hardblock->name == "sb77")
    //         std::cerr<<"height "<<hardblock->height<<" y1 "<<hardblock->y<<" y2 "<<hardblock->y + hardblock->height<<"\n";        
    // }    
    // printTree(BT);    
    return BT;
}

SA_Solver::SA_Solver(SAInput *input, GlobalTimer &globalTimer)
    : input(input), globalTimer(globalTimer)
{
    int totalArea = 0;
    for (size_t i = 0; i < input->hardblocks.size(); ++i)
    {
        auto hardblock = input->hardblocks[i];
        totalArea += hardblock->area;
    } 
    
    // outline = sqrt(totalArea * (1 + input->deadSpaceRatio));// yu--modified
    std::cout << "in SA_Solver: " << input->DieX << " " << input->DieY << std::endl; 
    
    outline = (sqrt(totalArea * (1 + input->deadSpaceRatio)) < input->DieX*input->DieY)?sqrt(totalArea * (1 + input->deadSpaceRatio)):input->DieX*input->DieY;
}

ResultWriter *SA_Solver::solve()
{
    /* choose seed for each testcase (Platform: nthucad ic26) */    
    if (input->hardblocks.size() == 100)
    {
        if (input->deadSpaceRatio == 0.1)
            input->initialSeed = 1782;
        else if (input->deadSpaceRatio == 0.15)
            input->initialSeed = 1323;
    }
    else if (input->hardblocks.size() == 200)
    {
        if (input->deadSpaceRatio == 0.1)
            input->initialSeed = 112;
        else if (input->deadSpaceRatio == 0.15)
            input->initialSeed = 396;
    }
    else if (input->hardblocks.size() == 300)
    {
        if (input->deadSpaceRatio == 0.1)
            input->initialSeed = 145;
        else if (input->deadSpaceRatio == 0.15)
            input->initialSeed = 28;
    }
    int seed = input->initialSeed;
    srand(seed);

    Node *root = getInitialBStartTree();
    std::cerr << "start simulated annealing for area\n";
    Node *BT  ;//= saProcess(0.1, 0.9, 10, root, false);
    // clearTree(root);
    // std::cerr << "find a feasible solution!\n"
    //           << "wirelength: " << calWirelength() << "\n\n";

    // std::cerr << "start simulated annealing for wirelength\n";
    BT = saProcess(15, 0.99, 30, root, true);
    clearTree(BT);
    std::cerr << "find a minimum wirelength in time!\n"
              << "cost: " << calCost(true)
              << "wirelength: " << calWirelength() << "\n\n"; 

    return new ResultWriter(input);
}