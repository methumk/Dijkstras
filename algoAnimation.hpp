/* 
algoAnimation.hpp
    - Implementation for algorithms and measures to feed into animation steps
 */
#pragma once
#include <climits>
#include "graph.hpp"

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

    void decStep()
    {
        assert(currStep != 0);
        currStep--;
    }

    void incStep()
    {
        currStep++;
    }

    void addCurrNodes(std::vector<Node*> currs)
    {
        currNodes.push_back(currs);
    }

    void addReachableNodes(std::vector<Node*> reachable)
    {
        reachableNodes.push_back(reachable);
    }

    void addVisNodes(std::vector<Node*> vis)
    {
        visitedNodes.push_back(vis);
    }

    // Sets the curr step to be the given color; can be either curr color or untouched color (for stepping)
    void setCurrNodesColor(const size_t& step, const sf::Color& color)
    {
        const std::vector<Node*>& currs = currNodes[step];
        for (size_t idx = 0; idx < currs.size(); ++idx)
        {
            if (currs[idx])
                currs[idx]->setNodeFillColor(color);
        }
    }

    // Sets the current step to be visited color
    // Should be used in tandem with stepForward() and stepBackward()
    void setVisNodesColor(const size_t& step, const sf::Color& color)
    {
        const std::vector<Node*>& currs = visitedNodes[currStep];
        for (size_t idx = 0; idx < currs.size(); ++idx)
        {
            if (currs[idx])
                currs[idx]->setNodeFillColor(color);
        }
    }

    // Can either color nodes as reachable or unreachable
    void setReachableNodesColor(const size_t& step, const sf::Color& color)
    {
        assert(step < allSteps);
        const std::vector<Node*>& currs = reachableNodes[currStep];
        for (size_t idx = 0; idx < currs.size(); ++idx)
        {
            if (currs[idx])
                currs[idx]->setNodeFillColor(color);
        }
    }

public:
    IAnimImpl()
    {
        currStep = 0;
        allSteps = 1;               // When algo starts step 0 should already be initialized
        currAlgo = AlgoToRun::NoAlgo;

        // On first step no nodes are marked as reachable or visited
        reachableNodes.push_back(std::vector<Node*>());
        visitedNodes.push_back(std::vector<Node*>());
    }

    virtual void setStartNodes(const std::vector<Node*>& nodes) = 0;    // Nodes that the algorithm starts knowing (step 0) - derived objects will save these accordingly
    virtual void stepForward() = 0;      // Stepping forward; passed in nodes is current nodes to run the algo on

    // Stepping backward requires unsetting current curr, reachable and vis, and setting previous curr and reachable nodes
    void stepBackward()
    {
        decStep();
        size_t aheadStep = currStep + 1;
        // Untouch current node, visited nodes, and reachable nodes
        setCurrNodesColor(aheadStep, ANIM_NODE_UNTOUCHED_COLOR);
        setReachableNodesColor(aheadStep, ANIM_NODE_UNTOUCHED_COLOR);
        setVisNodesColor(aheadStep, ANIM_NODE_UNTOUCHED_COLOR);
        
        // Current steps should now be highlighted
        // NOTE: don't have to set visited as it should have been visited before
        setCurrNodesColor(currStep, ANIM_NODE_UNTOUCHED_COLOR);
        setReachableNodesColor(currStep, ANIM_NODE_UNTOUCHED_COLOR);
    }

    virtual ~IAnimImpl(){}

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

    // Clean the saved node data and reset the touched node colors
    void cleanSaved()
    {
        resetTouchedColors();
        currStep = 0;
        allSteps = 0;
        currNodes.clear();
        reachableNodes.clear();
        visitedNodes.clear();
    }

    std::string getStepString()
    {
        if (currStep < allSteps-1)
            return "On Step: " + std::to_string(currStep+1) + "/" + std::to_string(allSteps+1);
        return "Curr Step: " + std::to_string(currStep);
    }
};

class DFSImpl : IAnimImpl
{
private:
    Node* curr;
    const Node* start;
    const Node* find;

public:
    DFSImpl()
    {
        start = NULL;
        find = NULL;
        curr = NULL;
        currAlgo = AlgoToRun::DFS;
    }

    // Passed in nodes assumed to be in order <start, find>
    void setStartNodes(const std::vector<Node*>& nodes) override 
    {
        assert(nodes.size() == 2);
        start = nodes[0];
        curr = nodes[0]; 
        find = nodes[1];

        // Set step 0 to be the start node
        setCurrNodesColor(currStep, ANIM_NODE_UNTOUCHED_COLOR);
    }

    void stepForward()
    {
        
    }

    ~DFSImpl() override {}
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