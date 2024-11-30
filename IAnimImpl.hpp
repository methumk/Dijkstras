/* 
algoAnimation.hpp
    - Implementation for algorithms and measures to feed into animation steps
 */
#pragma once
#include "graph.hpp"
#include <sstream>

const std::string AlgoNames[] = {"DFS", "BFS", "Dijkstra", "No algorithm"};
enum AlgoToRun {DFS, BFS, Dijkstra, NoAlgo};
enum AlgoAnimationMode {Pause, Play, Close};        // TODO: forward, backward (necessary??)

#define ANIM_NODE_FOUND_COLOR               sf::Color(0, 255, 0)            // green
#define ANIM_NODE_VIS_COLOR                 sf::Color(0, 0, 0)              // black
#define ANIM_NODE_REACHABLE_COLOR           sf::Color(255, 138, 0)          // orange

#define ANIM_NODE_BORDER_CURR_COLOR         ANIM_NODE_FOUND_COLOR            
#define ANIM_NODE_UNTOUCHED_COLOR           NODE_FILL_COLOR                 
#define ANIM_NODE_BORDER_UNTOUCHED_COLOR    NODE_OUT_COLOR                  

typedef std::tuple<ll, Node *> CHILD_WEIGHT;                  // <Child, Weight between child and parent, Parent>

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
    
    // For step and data visualization
    std::vector<std::string> stepDescription;                    // Descriptions for each step
    std::map<ll, CHILD_WEIGHT> dijkTable;                        // Dijkstra weight table <Child ID, <parent to child weight, Parent node>>

    enum class VisNodesVec
    {
        current,
        reachable,
        visited
    };


    // For clean up
    std::unordered_set<Node*> touched;              // All nodes that have been touched to be cleaned after algo finishes

    void decCurrStep()
    {
        assert(currStep > 0);
        currStep--;
        std::cout << "Curr step decr to " << currStep << "/" << allSteps << std::endl;
    }

    void incCurrStep()
    {
        assert(currStep < allSteps);
        currStep++;
        // AllSteps should gets updated when currstep matches total number of steps
        if (currStep >= allSteps)
            allSteps++;
        std::cout << "Curr step incr to " << currStep << "/" << allSteps << std::endl;
    }

    // Add a vector of nodes to a given visualizing nodes vector
    void addNodesToVec(const VisNodesVec vecType, std::vector<Node*> nodes, bool markTouched = true)
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

        // Mark added nodes to the touched map
        if (markTouched) setNodesTouched(nodes);
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
        assert(allSteps > step && step >= 0);
        std::vector<Node*> nodes;

        switch(vecType)
        {
            case VisNodesVec::current:
                assert(currNodes.size() > step);
                nodes = currNodes[step];
                break;
            case VisNodesVec::reachable:
                assert(reachableNodes.size() > step);
                nodes = reachableNodes[step];
                break;
            case VisNodesVec::visited:
                assert(visitedNodes.size() > step);
                nodes = visitedNodes[step];
                break;
        }

        for (Node* n : nodes)
        {
            assert(n);
            n->setNodeFillColor(color);
        }
    }

    // Sets the border color of the curr node at step to be on if onCurr is true
    void toggleCurrNodesBorderColor(const size_t& step, bool onCurr)
    {
        assert(allSteps > step && step >= 0);
        assert(currNodes.size() >= step);

        std::vector<Node*> nodes;
        nodes = currNodes[step];
        const sf::Color color = onCurr ? ANIM_NODE_BORDER_CURR_COLOR : ANIM_NODE_BORDER_UNTOUCHED_COLOR;
        for (Node* n : nodes) 
        {
            assert(n);
            n->setNodeOutlineColor(color);
        }
    }

    void printStepInfo(const VisNodesVec vecType, const size_t& step)
    {
        std::vector<Node*> nodes;
        std::string vecTypeName = "";

        switch(vecType)
        {
            case VisNodesVec::current:
                assert(currNodes.size() > step);
                nodes = currNodes[step];
                vecTypeName = "C";
                std::cout << "C vecs: " << currNodes.size();
                break;
            case VisNodesVec::reachable:
                assert(reachableNodes.size() >= step);
                nodes = reachableNodes[step];
                vecTypeName = "R";
                std::cout << "R vecs: " << reachableNodes.size();
                break;
            case VisNodesVec::visited:
                assert(visitedNodes.size() >= step);
                nodes = visitedNodes[step];
                vecTypeName = "V";
                std::cout << "V vecs: " << visitedNodes.size();
                break;
        }

        std::cout << "\tAt step: " << step << " | " << vecTypeName << " Nodes are: ";

        for (Node* n : nodes)
        {
            assert(n);
            std::cout << "\t" << n->getNodeIdent();
        }
        std::cout << "\t| total: " << nodes.size() << std::endl;
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
        currNodes.push_back(std::vector<Node*>());
        reachableNodes.push_back(std::vector<Node*>());
        visitedNodes.push_back(std::vector<Node*>());
    }

    virtual void setStartNodes(const std::vector<Node*>& nodes) = 0;    // Nodes that the algorithm starts knowing (step 0) - derived objects will save these accordingly
    virtual void stepForward() = 0;      // Stepping forward; passed in nodes is current nodes to run the algo on

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

        currNodes.erase(currNodes.begin()+1, currNodes.end());
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

    void addStepDescription(const size_t& step, Node* curr, const Node* find, bool failed, std::string additionalMessage = "")
    {
        assert(step >= 0);
        std::stringstream ss;
        std::string start = (!curr) ? "-" : std::to_string(curr->getNodeIdent());
        ss << "Step " << step << "\n\tOn node: " << start;

        if (step < reachableNodes.size())
        {
            if (reachableNodes[step].size() > 0)
                ss << "\n\tCan reach: ";
            else
                ss << "\n\tNo reachable nodes\n";

            for (size_t i = 0; i < reachableNodes[step].size(); ++i)
            {
                ss << reachableNodes[step][i]->getNodeIdent();
                if (i != reachableNodes[step].size()-1) ss << ", ";
                else ss << "\n";
            }
        }

        if (additionalMessage != "")
            ss << "\t" << additionalMessage << "\n";
        
        if (find)
        {
            if (failed)
            {
                ss << "\t" << "Failed to find node " << find->getNodeIdent();
            } else
            {
                ss << "\t" << "Found node " << find->getNodeIdent() << " @ step: " << step;
            }
        }
        
        stepDescription.push_back(ss.str());
    }

    const std::vector<std::string>& getStepDescriptions() const
    {
        return stepDescription;
    }

    void addToDijkTable(const ll& childId, const CHILD_WEIGHT& weight)
    {
        if (dijkTable.count(childId))
        {
            dijkTable[childId] = weight;
        }else 
        {
            dijkTable.emplace(childId, weight);
        }
    }

    const std::map<ll, CHILD_WEIGHT>& getDijkTable() const
    {
        return dijkTable;
    }

    virtual ~IAnimImpl()
    {
        cleanSaved();
    }
};