/*
DijkImpl.hpp
    - Implementation for Dijk algorithm and measures to feed into animation steps
 */
#pragma once
#include <climits>
#include "IAnimImpl.hpp"

class DijkImpl : public IAnimImpl
{
private:
     // <Child Node, <weight from parent to child, Parent Node>>

    Node *curr;
    const Node *start;

    std::string weightMessage = "";                   // Message for set description when getting new reachable nodes;

    std::unordered_set<Node *> nodesVisited;          // Nodes that have been visited
    std::unordered_map<Node *, CHILD_WEIGHT> toVisit; // Nodes that are should eventually be visited <Child Node, <weight from parent to child, Parent Node>> (Algo will end when this map is empty)
    std::unordered_map<Node *, CHILD_WEIGHT> weights; // For displaying weight map table, <Child Node, <weight from parent to child, Parent Node>>

    // Saves new reachable nodes into vector to color at a step
    // Marks and updates weights as needed into both the toVisit and weights map
    // NOTE: should run before getNewCurrNodes, to correctly have updated toVisit map
    void getNewReachableNodes(std::vector<Node *> &reachable)
    {
        assert(curr);
        
        const std::vector<ADJ_NODE> &links = curr->getNodeLinks();

        for (const ADJ_NODE &link : links)
        {
            std::stringstream ss;
            Node *childNode = std::get<0>(link);
            bool canTravel = std::get<3>(link);

            if (!nodesVisited.count(childNode) && canTravel)
            {
                // Save nodes to reachable vector to color
                std::cout << "\t\tCan reach " << childNode->getNodeIdent() << " @ step: " << currStep << std::endl;
                reachable.push_back(childNode);

                // Get curr's weight and weight of link
                const CHILD_WEIGHT &currWeight = weights[curr];
                ll weight = std::get<1>(link);
                ll newWeight = std::get<0>(currWeight) + weight;

                // If unvisited, reachable child node in weights
                // Use existing weights to determine if weights & parent node needs to be updated
                if (weights.count(childNode))
                {
                    assert(toVisit.count(childNode) != 0); // Shouldn't fail because child node needs to be unvisited to be in toVisit

                    // Determine weight from curr node to child node
                    const CHILD_WEIGHT &childWeight = toVisit[childNode];

                    // Update weights to child if new path leads to smaller weight, otherwise keep old path
                    if (newWeight < std::get<0>(childWeight))
                    {
                        // Add extra description and push to dijk table visualizer
                        ss << "Updating " << curr->getNodeIdent() << "->" << childNode->getNodeIdent() << " weight from " << weight << " to " << newWeight << "\n\t";
                        addToDijkTable(childNode->getNodeIdent(), std::tuple<ll, Node*>(newWeight, curr));
                        
                        std::get<0>(toVisit[childNode]) = newWeight;
                        std::get<1>(toVisit[childNode]) = curr;
                        std::get<0>(weights[childNode]) = newWeight;
                        std::get<1>(weights[childNode]) = curr;
                    }

                    weightMessage += ss.str();
                }
                else
                {
                    // Add extra description and push to dijk table visualizer
                    ss << "Adding " << curr->getNodeIdent() << "->" << childNode->getNodeIdent() << " weight as " << newWeight << "\n\t";
                    weightMessage += ss.str();
                    addToDijkTable(childNode->getNodeIdent(), std::tuple<ll, Node*>(newWeight, curr));

                    // Store weights for the first time from curr to child node
                    toVisit.emplace(childNode, std::tuple<ll, Node *>{newWeight, curr});
                    weights.emplace(childNode, std::tuple<ll, Node *>{newWeight, curr});
                }
                
            }
        }
    }

    // Returns the new curr node or null if algorithm has ended
    // New curr node is determined as the node in toVisit with the smallest weight
    Node *getNewCurrNode()
    {
        if (toVisit.empty())
            return NULL;

        Node *newCurr = NULL;
        ll smallestWeight = -1;

        // Loop through entire toVisit map for the node with the smallest weight
        for (auto it = toVisit.begin(); it != toVisit.end(); ++it)
        {
            const CHILD_WEIGHT &childWeight = it->second;
            if (smallestWeight == -1 || smallestWeight > std::get<0>(childWeight))
            {
                newCurr = it->first;
                smallestWeight = std::get<0>(childWeight);
            }
        }

        // Remove new curr from toVisit as that will be next curr
        if (newCurr)
            toVisit.erase(newCurr);

        return newCurr;
    }

public:
    DijkImpl()
    {
        curr = NULL;
        start = NULL;
        currAlgo = AlgoToRun::BFS;
    }

    void setStartNodes(const std::vector<Node *> &nodes) override
    {
        assert(nodes.size() == 1);
        start = nodes[0];
        curr = nodes[0];

        // Save start node in weights map
        // Weight to curr is 0 and it has no parents
        weights.emplace(curr, std::tuple<ll, Node *>{0, NULL});

        // Add step description
        addStepDescription(0, NULL, NULL, false);
        addToDijkTable(curr->getNodeIdent(), std::tuple<ll, Node*>(0, NULL));
    }

    void stepForward() override
    {
        weightMessage = "";
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
                algoFinished = true;
                addStepDescription(currStep, NULL, NULL, false, "Completed Dijkstra's Algorithm");
                std::cout << "Completed Dijkstra's Algorithm\n";
                return;
            }

            // Color border of current node before finding next curr and it's reachables
            // Mark current node as visited
            nodesVisited.insert(curr);
            addNodesToVec(VisNodesVec::current, std::vector<Node *>{curr});
            toggleCurrNodesBorderColor(currStep, true);

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
            addStepDescription(currStep, curr, NULL, false, weightMessage);

            // Update curr to next curr
            curr = newCurr;
        }
    }

    ~DijkImpl() override {}
};