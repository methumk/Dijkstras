/*
BFSImpl.hpp
    - Implementation for BFS algorithm and measures to feed into animation steps
 */
#pragma once
#include <climits>
#include <queue>
#include "IAnimImpl.hpp"

class BFSImpl : public IAnimImpl
{
private:
    Node *curr;
    const Node *start;
    const Node *find;

    // Keeps track of steps for BFS
    std::queue<Node *> nextNodes;            // Child nodes to visit next
    std::unordered_set<Node *> nodesVisited; // Nodes that have been visited

private:
    Node *getNewCurrNode()
    {
        if (nextNodes.empty())
            return NULL;

        Node *newNode = nextNodes.front();
        nextNodes.pop();

        return newNode;
    }

    // Saves all unvisited, travelable nodes in curr nodes adj list as reachable
    void getNewReachableNodes(std::vector<Node *> &reachable)
    {
        const std::vector<ADJ_NODE> &links = curr->getNodeLinks();

        for (const ADJ_NODE &link : links)
        {
            Node *childNode = std::get<0>(link);
            bool canTravel = std::get<3>(link);

            if (!nodesVisited.count(childNode) && canTravel)
            {
                std::cout << "\t\tCan reach " << childNode->getNodeIdent() << " @ step: " << currStep << std::endl;
                reachable.push_back(childNode);
                nextNodes.push(childNode);
            }
        }
    }

public:
    BFSImpl()
    {
        curr = NULL;
        start = NULL;
        find = NULL;
        currAlgo = AlgoToRun::BFS;
    }

    void setStartNodes(const std::vector<Node *> &nodes) override
    {
        assert(nodes.size() == 2);
        start = nodes[0];
        curr = nodes[0];
        find = nodes[1];

        // Add start description
        addStepDescription(0, NULL, NULL, false);
    }

    void stepForward() override
    {
        if (!algoFinished)
        {
            // Uncolor border of previous curr node
            // Uncolor previous reachable nodes
            // Color previous curr node as visited
            toggleCurrNodesBorderColor(currStep, false);
            setVecNodesColor(VisNodesVec::reachable, currStep, ANIM_NODE_UNTOUCHED_COLOR);
            setVecNodesColor(VisNodesVec::visited, currStep, ANIM_NODE_VIS_COLOR);

            // Go to next step
            incCurrStep();

            // Null curr means no new nodes to traverse to
            if (!curr)
            {
                if (nextNodes.empty())
                {
                    algoFinished = true;
                    addNodesToVec(VisNodesVec::current, std::vector<Node *>{});
                    addNodesToVec(VisNodesVec::visited, std::vector<Node *>{});
                    addNodesToVec(VisNodesVec::reachable, std::vector<Node *>{});
                    addStepDescription(currStep, NULL, find, true);
                    std::cout << "\t\tFailed to complete algo\n";
                    return;
                }
            }

            // Color border of current node before finding next curr and it's reachables
            // Mark current node as visited
            nodesVisited.insert(curr);
            addNodesToVec(VisNodesVec::current, std::vector<Node *>{curr});
            toggleCurrNodesBorderColor(currStep, true);

            // Check if curr is node to find before looking at children
            if (curr == find)
            {
                algoFinished = true;
                setVecNodesColor(VisNodesVec::current, currStep, ANIM_NODE_FOUND_COLOR);
                addNodesToVec(VisNodesVec::reachable, std::vector<Node *>{});
                addNodesToVec(VisNodesVec::visited, std::vector<Node *>{});
                addStepDescription(currStep, curr, find, false);
                std::cout << "\t\tFound node: " << curr->getNodeIdent() << std::endl;
                return;
            }

            // Find new reachable and new curr nodes
            std::vector<Node *> newReachableNodes;
            getNewReachableNodes(newReachableNodes);
            Node *newCurr = getNewCurrNode();

            // Color nodes reachable from curr node
            // Mark curr node as visited so it will be visited on next forward click
            addNodesToVec(VisNodesVec::reachable, newReachableNodes);
            setVecNodesColor(VisNodesVec::reachable, currStep, ANIM_NODE_REACHABLE_COLOR);
            addNodesToVec(VisNodesVec::visited, std::vector<Node *>{curr});

            // Add step description
            addStepDescription(currStep, curr, NULL, false);

            // Update curr to next curr
            curr = newCurr;
        }
    }

    ~BFSImpl() override {}
};