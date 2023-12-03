/* 
algoAnimation.hpp
    - Implementation for algorithms and measures to feed into animation steps
 */
#pragma once
#include "graph.hpp"

const std::string AlgoNames[] = {"DFS", "BFS", "Dijkstra", "No algorithm"};
enum AlgoToRun {DFS, BFS, Dijkstra, NoAlgo};
enum AlgoAnimationMode {Pause, Play, Close};        // TODO: forward, backward (necessary??)

#define ANIM_NODE_CURR_COLOR        sf::Color(102, 255, 153)        // greenish
#define ANIM_NODE_FOUND_COLOR       sf::Color(104, 246, 255)        // cyan
#define ANIM_NODE_VIS_COLOR         sf::Color(255, 102, 217)        // purplish
#define ANIM_NODE_REACHABLE_COLOR    sf::Color(255, 217, 102)       // orangish
#define ANIM_NODE_UNTOUCHED_COLOR  NODE_FILL_COLOR                  // blue

class IAnimImpl
{
protected:
    size_t currStep;                                                    // Current step in algorithm (also index to vector array to get nodes; 0 index)
    size_t allSteps;                                                    // Total number of steps currently ran
    AlgoToRun currAlgo;                                                 // Current algo that is running - TODO: tbd necessary?
    bool algoFinished;                                                  // Indicate that we have reached the last step and the algo is finished

    // For visualizing node steps
    // <step index, nodes at that time>
    // NOTE: visited nodes are cumulative over steps
    // e.g. <0, <N1>>, <1, <N2, N3>>. This means by step 1, nodes 1,2, and 3 have all been visited
    std::vector<std::vector<Node*>> currNodes;                   // Current node(s) algorithm is on
    std::vector<std::vector<Node*>> reachableNodes;              // Nodes that can be visited at current iteration
    std::vector<std::vector<Node*>> visitedNodes;                // All nodes that have been visited at current step

    enum class VisNodesVec
    {
        current,
        reachable,
        visited
    };

    // TODO: table visualizer 

    // For clean up
    std::unordered_set<Node*> touched;              // All nodes that have been touched to be cleaned after algo finishes

    void decCurrStep()
    {
        assert(currStep != 0);
        currStep--;
    }

    void incCurrStep()
    {
        assert(currStep < allSteps);
        currStep++;
        // AllSteps should gets updated when currstep matches total number of steps
        if (currStep >= allSteps)
            allSteps++;
    }

    // Add a vector of nodes to a given visualizing nodes vector
    void addNodesToVec(const VisNodesVec vecType, std::vector<Node*> nodes)
    {
        switch(vecType)
        {
            case VisNodesVec::current:
                currNodes.push_back(nodes);
                break;
            case VisNodesVec::reachable:
                reachableNodes.push_back(nodes);
                break;
            case VisNodesVec::visited:
                visitedNodes.push_back(nodes);
                break;
        }
    }

    // Checks and marks each node as touched
    void setNodesTouched(const std::vector<Node*>& nodes)
    {
        for (Node* n : nodes)
        {
            setNodeTouched(n);
        }
    }

    // Marks a single node as touched
    void setNodeTouched(Node* node)
    {
        if (node && !touched.count(node))
        {
            touched.insert(node);
        }
    }

    // Sets every node in the given vis vec at the given step to the given color
    void setVecNodesColor(const VisNodesVec vecType, const size_t& step, const sf::Color& color)
    {
        assert(currNodes.size() > step);
        std::vector<Node*> nodes;
        switch(vecType)
        {
            case VisNodesVec::current:
                nodes = currNodes[step];
                break;
            case VisNodesVec::reachable:
                nodes = reachableNodes[step];
                break;
            case VisNodesVec::visited:
                nodes = visitedNodes[step];
                break;
        }

        for (Node* n : nodes)
        {
            if (n)
            {
                n->setNodeFillColor(color);
            }
        }
    }


public:
    IAnimImpl()
    {
        // Start off at index 0
        currStep = 0;
        allSteps = 1;               // When algo starts step 0 should already be initialized with setStartNodes()
        currAlgo = AlgoToRun::NoAlgo;
        algoFinished = false;

        // On first step no nodes are marked as reachable or visited
        reachableNodes.push_back(std::vector<Node*>());
        visitedNodes.push_back(std::vector<Node*>());
    }

    virtual void setStartNodes(const std::vector<Node*>& nodes) = 0;    // Nodes that the algorithm starts knowing (step 0) - derived objects will save these accordingly
    virtual void stepForward() = 0;      // Stepping forward; passed in nodes is current nodes to run the algo on

    // Stepping backward requires unsetting current curr, reachable and vis, and setting previous curr and reachable nodes
    void stepBackward()
    {   
        // Don't allow step back if currStep on index 0
        if (currStep == 0) return;

        // Untouch current node, visited nodes, and reachable nodes
        setVecNodesColor(VisNodesVec::current, currStep, ANIM_NODE_UNTOUCHED_COLOR);
        setVecNodesColor(VisNodesVec::reachable, currStep, ANIM_NODE_UNTOUCHED_COLOR);
        setVecNodesColor(VisNodesVec::visited, currStep, ANIM_NODE_UNTOUCHED_COLOR);

        decCurrStep();
        
        // Current steps should now be highlighted
        // NOTE: don't have to set visited as it should have been visited before
        setVecNodesColor(VisNodesVec::current, currStep, ANIM_NODE_CURR_COLOR);
        setVecNodesColor(VisNodesVec::reachable, currStep, ANIM_NODE_REACHABLE_COLOR);
    }

    // Reset the touched node colors
    void resetTouchedColors()
    {
        for (auto itr = touched.begin(); itr != touched.end(); ++itr)
        {
            Node* node = (*itr);
            node->setNodeFillColor(NODE_FILL_COLOR);
            node->setNodeOutlineColor(NODE_OUT_COLOR);      // NOTE: might be unnecessary
        }
    }
    
    // NOTE: if you are deleting the instance you might just need to reset the colors and delete this func
    // Clean the saved node data and reset the touched node colors
    void cleanSaved()
    {
        resetTouchedColors();
        currStep = 0;
        allSteps = 1;       // When algo starts step 0 should already be initialized with setStartNodes()
        algoFinished = false;
        currAlgo = AlgoToRun::NoAlgo;

        currNodes.clear();
        // On first step no nodes are marked as reachable or visited
        reachableNodes.erase(reachableNodes.begin()+1, reachableNodes.end());
        visitedNodes.erase(visitedNodes.begin()+1, visitedNodes.end());
    }

    std::string getStepString()
    {
        if (algoFinished)
            return AlgoNames[(int)currAlgo] + " algo has completed in " + std::to_string(allSteps) + " steps";
        if (currStep < allSteps-1)
            return "On Step: " + std::to_string(currStep+1) + "/" + std::to_string(allSteps+1);
        return "Curr Step: " + std::to_string(currStep);
    }

    virtual ~IAnimImpl()
    {
        std::cout << "\t\tbase dtor called\n";      // REMOVE
        resetTouchedColors();
    }
};