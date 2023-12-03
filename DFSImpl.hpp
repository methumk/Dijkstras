/* 
algoAnimation.hpp
    - Implementation for algorithms and measures to feed into animation steps
 */
#pragma once
#include <climits>
#include <stack>
#include "IAnimImpl.hpp"

class DFSImpl : public IAnimImpl
{
private:
    Node* curr;
    const Node* start;
    const Node* find;

    // Keeps track of the recursion for DFS
    // Contains all nodes that should be visited at a recursive layer
    std::stack<const std::vector<ADJ_NODE>*> visitNodes;   

private:

    // Helper to push visit nodes
    // Will only push if ADJ nodes not already at top
    void tryPushVisitNodes(const std::vector<ADJ_NODE>* adjnodes)
    {
        if (visitNodes.empty() || visitNodes.top() != adjnodes)
        {
            visitNodes.push(adjnodes);
        }
    }

    // Returns first untouched node from adjacency list, null if all visited
    Node* GetUnVisFromAdjNodes(const std::vector<ADJ_NODE>* adjnodes)
    {
        assert(adjnodes);
        for (size_t i = 0; i < adjnodes->size(); ++i)
        {
            Node* n = std::get<0>((*adjnodes)[i]);
            if (!touched.count(n))
                return n;
        }
        return NULL;
    }

    // Keep popping stack until Node that has not been visited is found or until stack empty
    Node* PopVisitNodes()
    {
        Node* nextNode = NULL;
        // Keep going through stack as long as visit nodes not empty and current stack adj nodes lead to null node
        while(!visitNodes.empty() && (nextNode = GetUnVisFromAdjNodes(visitNodes.top())) == NULL)
        {
            visitNodes.pop();
        }
        return NULL;
    }

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

        if (curr == find) algoFinished = true;
        currNodes.push_back(std::vector<Node*>{curr});

        // Set step 0 to be the start node
        // If the start and find node are the same then we don't have to do the algorithm
        setCurrNodesColor(currStep, algoFinished ? ANIM_NODE_FOUND_COLOR : ANIM_NODE_CURR_COLOR);
        std::cout << "\t\tfinished curr node color\n";
    }

    void stepForward() override
    {

    }

    ~DFSImpl() override {}
};