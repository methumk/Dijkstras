/* 
algoAnimation.hpp
    - Implementation for algorithms and measures to feed into animation steps
 */
#pragma once
#include <climits>
#include "graph.hpp"

enum AlgoToRun {DFS, BFS, Dijkstra, NoAlgo};
enum AlgoAnimationMode {Pause, Play, Close};        // TODO: forward, backward (necessary??)

#define ANIM_NODE_CURR_COLOR        sf::Color(102, 255, 153)    // greenish
#define ANIM_NODE_VIS_COLOR         sf::Color(255, 102, 217)    // purplish
#define ANIM_NODE_WILL_VIS_COLOR    sf::Color(255, 217, 102)    // orangish

class IAnimImpl
{
private:
    size_t currStep;                                                    // Current step in algorithm (also index to vector array to get nodes)
    size_t allSteps;                                                    // Total number of steps currently ran
    AlgoToRun currAlgo;                                                 // Current algo that is running - TODO: tbd necessary?

    // For visualizing node steps
    // <step index, nodes at that time>
    // NOTE: visited nodes are cumulative over steps
    // e.g. <0, <N1>>, <1, <N2, N3>>. This means by step 1, nodes 1,2, and 3 have all been visited
    std::vector<std::vector<Node*>> currNodes;                   // Current node(s) algorithm is on
    std::vector<std::vector<Node*>> reachableNodes;              // Nodes that can be visited at current iteration
    std::vector<std::vector<Node*>> visitedNodes;                // All nodes that have been visited at current step

    // TODO: table visualizer 

    // For clean up
    std::unordered_set<Node*> touched;              // All nodes that have been touched to be cleaned after algo finishes

public:
    IAnimImpl()
    {
        currStep = 0;
        allSteps = 0;
        currAlgo = AlgoToRun::NoAlgo;
    }
    virtual void stepForward(const std::vector<Node*>& nodes) = 0;      // Stepping forward; passed in nodes is current nodes to run the algo on
    virtual void stepBackward() = 0;
    void clean()
    {
        for (auto itr = touched.begin(); itr != touched.end(); ++itr)
        {
            Node* node = (*itr);
            node->setNodeFillColor(NODE_FILL_COLOR);
            node->setNodeOutlineColor(NODE_OUT_COLOR);      // NOTE: might be unnecessary
        }

        currNodes.clear();
        reachableNodes.clear();
        visitedNodes.clear();
    }
    virtual ~IAnimImpl(){}
};

class DFSImpl : IAnimImpl
{

};




/* 
//  previous impl delete when this class set up
class AnimationSteps
{
private:
    size_t step;                                    // Current step in algorithm (also index to vector array to get nodes)
    size_t allSteps;                                // Total number of steps/ might not be necessary
    AlgoToRun currAlgo;
    std::vector<Node*> currNodes;                   // Current node(s) algorithm is on
    std::vector<Node*> reachableNodes;              // Nodes that can be visited at current iteration
    std::vector<Node*> visitedNodes;                // All nodes that have been visited at current step
    std::unordered_set<Node*> touched;              // All nodes that have been touched to be cleaned after algo finishes

    void DFSHelper(Node* curr, const Node* find, std::unordered_set<Node*>& visited)
    {
        step++;
        if (curr == find)
        {
            return;
        }else
        {
            visited.insert(curr);
            std::vector<ADJ_NODE> links = curr->getNodeLinks();

            for (size_t i=0; i < links.size(); ++i)
            {
                Node* inspect = std::get<0>(links[i]);
                bool canVisit = std::get<3>(links[i]) && visited.count(inspect);
                if (canVisit)
                {
                    DFSHelper(inspect, find, visited);
                }
            }
        }
    }

public:
    AnimationSteps()
    {
        step = 0;
        allSteps = 0;
        currAlgo = AlgoToRun::NoAlgo;
    }

    void SaveDFS(Node* start, const Node* find)
    {
        std::unordered_set<Node*> visited;
        DFSHelper(start, find, visited);
    }

    void 

    void clean()
    {
        for (auto itr = touched.begin(); itr != touched.end(); ++itr)
        {
            Node* node = (*itr);
            node->setNodeFillColor(NODE_FILL_COLOR);
            node->setNodeOutlineColor(NODE_OUT_COLOR);      // NOTE: might not be necessary
        }

        step = 0;
        allSteps = 0;
        currNodes.clear();
        visitedNodes.clear();
        reachableNodes.clear();
        touched.clear();
    }
}; */