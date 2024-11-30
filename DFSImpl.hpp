/* 
DFSImpl.hpp
    - Implementation for DFS algo and measures to feed into animation steps
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
    std::stack<Node*> prevNodes;
    std::unordered_set<Node*> nodesVisited; // All nodes that have been stepped into as a "curr" node

private:

    Node* getReachableAndNextCurr(const std::vector<ADJ_NODE>& adjNodes, std::vector<Node*>& reachable)
    {
        Node* nextCurr = NULL;

        // Look at all adjacent nodes from curr node
        for (size_t i = 0; i < adjNodes.size(); ++i)
        {
            Node* n = std::get<0>(adjNodes[i]);
            // Node should never be null
            assert(n);

            bool canReach = std::get<3>(adjNodes[i]);
            bool visited = nodesVisited.count(n);

            if (canReach)
            {
                // Mark node as touched for clean up
                setNodeTouched(n);

                if (!visited && !nextCurr)
                {
                    // nextCurr set if we haven't visited
                    nextCurr = n;
                    nodesVisited.insert(nextCurr);       // Nodes that have been visited)
                    std::cout << "\t\tNode " << n->getNodeIdent() << " is now curr\n";          // REMOVE
                }else if (!visited)
                {
                    // If adjacent reachable nodes have not been visited
                    // then they are marked as reachable
                    reachable.push_back(n);
                    std::cout << "\t\tNode " << n->getNodeIdent() << " is now marked as reachable\n";   // REMOVE
                }
            }
        }

        return nextCurr;
    }

    // Finds the new curr node from the current curr node's adjacency list
    Node* getNewCurrNode()
    {
        const std::vector<ADJ_NODE>& links = curr->getNodeLinks();

        for (const ADJ_NODE& link : links)
        {
            Node* childNode = std::get<0>(link);
            bool canTravel = std::get<3>(link);

            if (!nodesVisited.count(childNode) && canTravel)
            {
                // Unvisited node in list becomes node to visit and also mark it as visited
                return childNode;
            }
        }
        return NULL;
    }

    // Saves all unvisited, travelable nodes in curr nodes adj list as reachable
    void getNewReachableNodes(std::vector<Node*>& reachable)
    {
        const std::vector<ADJ_NODE>& links = curr->getNodeLinks();

        for (const ADJ_NODE& link : links)
        {
            Node* childNode = std::get<0>(link);
            bool canTravel = std::get<3>(link);

            if (!nodesVisited.count(childNode) && canTravel)
            {
                std::cout << "\t\tCan reach " << childNode->getNodeIdent() << " @ step: " << currStep << std::endl;
                reachable.push_back(childNode);
            }
        }
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
        // Make sure start and find nodes passed in
        assert(nodes.size() == 2);
        start = nodes[0];
        curr = nodes[0]; 
        find = nodes[1];

        // Step description for step 0
        addStepDescription(0, NULL, NULL, false);
    }

    void stepForward() override
    {
        // Description for steps
        std::string description = "";

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

            if (!curr) 
            {
                // If current node is null try and recurse or end algo
                if (prevNodes.empty())
                {
                    // Failed to recurse and ending algo
                    algoFinished = true;
                    addNodesToVec(VisNodesVec::current, std::vector<Node*>{});
                    addNodesToVec(VisNodesVec::visited, std::vector<Node*>{});
                    addNodesToVec(VisNodesVec::reachable, std::vector<Node*>{});
                    addStepDescription(currStep, NULL, find, true);
                    std::cout << "\t\tFailed to complete algo\n";
                    return;
                } else 
                {
                    // Recurse to previous node
                    curr = prevNodes.top();
                    prevNodes.pop();

                    std::cout << "\t\tRecursing to: " << curr->getNodeIdent() << std::endl;
                    description = "Recursing to: " + std::to_string(curr->getNodeIdent());
                }
            }


            // Color border of current node before finding next curr and it's reachables
            // Mark current node as visited
            nodesVisited.insert(curr);
            addNodesToVec(VisNodesVec::current, std::vector<Node*>{curr});
            

            // Check if curr node is node found
            if (curr == find)
            {
                algoFinished = true;
                toggleCurrNodesBorderColor(currStep, false);
                setVecNodesColor(VisNodesVec::current, currStep, ANIM_NODE_FOUND_COLOR);
                addNodesToVec(VisNodesVec::reachable, std::vector<Node*>{});
                addNodesToVec(VisNodesVec::visited, std::vector<Node*>{});
                addStepDescription(currStep, curr, find, false);
                std::cout << "\t\tFound node: " << curr->getNodeIdent() << std::endl;
                return;
            }

            // Color curr border color
            toggleCurrNodesBorderColor(currStep, true);

            // Find the next curr and it's reachables if possible
            std::vector<Node*> newReachableNodes;
            getNewReachableNodes(newReachableNodes);
            Node* newCurr = getNewCurrNode();

            // Color new reachable nodes and mark current node as visited
            addNodesToVec(VisNodesVec::reachable, newReachableNodes);
            setVecNodesColor(VisNodesVec::reachable, currStep, ANIM_NODE_REACHABLE_COLOR);
            addNodesToVec(VisNodesVec::visited, std::vector<Node*>{curr});

            // Add step description
            addStepDescription(currStep, curr, NULL, false, description);

            // Update to new curr and save old curr if there is a new curr to go to
            if (newCurr) prevNodes.push(curr);
            curr = newCurr;
        }
    }

    ~DFSImpl() override {}
};