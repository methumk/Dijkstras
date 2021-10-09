/*
structure.hpp
    - 
*/
#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

//Define ull for identifiers
typedef unsigned long long ull;


/*
Node class:
    - Node identifier
    - Circle shape??
    - links (tuple: curr node, link weight, link identifier)
*/
class Node{
public:
    typedef std::tuple<Node*, unsigned int, ull> ADJ_NODE;
    typedef std::vector<ADJ_NODE> NODE_VEC;
private:
    ull ident;
    std::vector<ADJ_NODE> links;
    sf::CircleShape GUInode;
public:
    Node(){}
    Node(ull i): ident(i){}

    //creates the node for node visualization in the GUI
    void addNodeShape(float radius, sf::Vector2f& pos, sf::Color color){
        GUInode.setRadius(radius);
        GUInode.setOrigin(radius, radius);
        GUInode.setPosition(pos);
        GUInode.setFillColor(color);
        GUInode.setOutlineColor(sf::Color::Blue);
        GUInode.setOutlineThickness(2.f);
    }

    //add a new linked up node manually
    void addNode(Node* addNode, unsigned int lw, ull li){
        links.push_back(std::make_tuple(addNode, lw, li));
    }

    //add a new linked up node by tuple
    void addNode(ADJ_NODE addNode){
        links.push_back(addNode);
    }

    void printCurrNode(){
        std::cout << "Node ident: " << ident << '\n';
    }

    //print all information about nodes connected to current node
    void printAdjNodes(){
        for (unsigned int i = 0; i < links.size(); ++i){
            Node* hold = std::get<0>(links[i]);
            std::cout << "Adj Node " << i 
                        << " ident: " << hold->ident 
                        << " lw: " << std::get<1>(links[i]) 
                        << " li: " << std::get<2>(links[i]) 
                        << '\n';
        }
    }

    //returns current nodes identifier
    inline ull getNodeIdent(){
        return ident;
    }

    //returns vector of all nodes connected to current node
    inline std::vector<ADJ_NODE>& getNodeLinks(){
        return links;
    }

    //retrieves node and link information at the given links index
    ADJ_NODE getNodeInfo(unsigned int i){
        return links[i];
    }

    //have to do dfs on graph to find correct identifier
    //retrieves node and link information at the given node identifier
    ADJ_NODE getNodeInfo(ull i){
        return links[i];
    }

    //retrieves Node for a node at the given links index
    Node* getAdjNode(unsigned int i){
        return std::get<0>(links[i]);
    }

    //have to do dfs on graph to find correct identifier
    //retrieves Node for the node with the given node identifier
    Node* getAdjNode(ull ident){
        return NULL;
    }
};


/*
Graph class:
    - vector of all UNJOINED graphs (holds the head of each graph)
    - if you want to iterate through an individual graph you need to check for already visited nodes (graph traversal)
    - Merging graphs and updating the all_graphs vector
    - handle deletion of a Node (what happens to its links)
*/
class Graph{
private:
    typedef std::tuple<Node*, unsigned int, ull> ADJ_NODE;
    std::vector<Node*> all_graphs;
    size_t num_graphs;
    ull curr_node_ident;
    ull curr_link_ident;
    //For node_locs: <Node identifier, all_graphs index>
    std::unordered_map<ull, unsigned int> node_locs;
public:
    Graph(): curr_node_ident(0), curr_link_ident(0), num_graphs(0){};

    //returns a valid Node identifier and updates next available identifier
    inline ull getNewNodeIdent(){
        return curr_node_ident++;
    }

    //returns a valid Link identifier and updates next available identifier
    inline ull getNewLinkIdent(){
        return curr_link_ident++;
    }

    //Adds a Node to the all_graphs vector
    inline void addGraph(Node* n){
        all_graphs.push_back(n);
    }

    //returns size of the all_graphs vector
    inline size_t all_graphs_size(){
        return all_graphs.size();
    }

    //returns total number of existing graphs
    inline size_t getNumGraphs(){
        return num_graphs;
    }

    //Runs a depth first search on the graph to find node with the given identifier (Returns NULL if node was not in the graph)
    Node* graphDFS(Node* curr, ull ident, std::unordered_set<ull>& visited){
        std::cout << "Curr node: " << curr->getNodeIdent() << '\n';
        if (curr->getNodeIdent() == ident){
            return curr;
        }else{
            //mark current node as visited
            visited.insert(curr->getNodeIdent());
            //get the current nodes links
            std::vector<ADJ_NODE> links = curr->getNodeLinks();

            //search for the next unvisited node in the current nodes links vector
            for (unsigned int i=0; i < links.size(); ++i){
                Node* inspect = std::get<0>(links[i]);
                Node* retval = NULL;
                
                std::cout << "\tCN: " << curr->getNodeIdent() << " searching link: " << inspect->getNodeIdent() << '\n';
                if (!visited.count(inspect->getNodeIdent())){
                    std::cout << "\tCN: " << curr->getNodeIdent() << " inspecting link: " << inspect->getNodeIdent() << '\n';
                    retval = graphDFS(inspect, ident, visited);
                    if (retval) return retval;
                }

                std::cout << "\tCN: " << curr->getNodeIdent() << " leaving link: " << inspect->getNodeIdent() << '\n';
            }
        }

        //Couldn't find node with identity so return null adj_node
        return NULL;
    }

    //finds a given node in one of the graphs and the node identifier 
    //should only take ident as parameter
    Node* findNode(unsigned int graph_index, ull ident){
        std::unordered_set<ull> visited;
        //write a map that determines which index in all_graphs the indentifier belongs to
        return graphDFS(all_graphs[graph_index], ident, visited);
    }


    //runs DFS to find node with given identifier
    //REVISED: you don't have to tell which all_graphs index to look into
    Node* REVISEDfindNode(ull ident){
        if (!node_locs.count(ident)){
            std::cout << "ERROR in graph.REVISEDfindNode - Node location not recorded in map\n";
            exit(EXIT_FAILURE);
        }
        std::unordered_set<ull> visited;
        return graphDFS(all_graphs[node_locs[ident]], ident, visited);
    }

    //should take in a given point, and create a new node as its own graph
    //records the new node into the map and stores which index in the graphs vector it is in
    //right now just going to give it node identifier parameter
    void createNewNode(){
        ull ident = getNewNodeIdent();
        all_graphs.push_back(new Node(ident));
        node_locs.emplace(ident, all_graphs.size()-1);
        num_graphs++;
    }

    //runs DFS and changes each identifier's to the new_loc in all_graphs
    //Arguments(new all_graphs location, graph head to move, set to keep track of visited node)
    void moveGraphLoc(unsigned int new_loc, Node* curr, std::unordered_set<ull>& visited){
        //mark current node as visited
        visited.insert(curr->getNodeIdent());
        //move current node to the new all_graph locs
        node_locs[curr->getNodeIdent()] = new_loc;
        //get current node's links list
        std::vector<ADJ_NODE> links = curr->getNodeLinks();


        //check each node in currents links 
        for (unsigned int i = 0; i < links.size(); ++i){
            Node* inspect = std::get<0>(links[i]);
            if (!visited.count(inspect->getNodeIdent())){
                moveGraphLoc(new_loc, inspect, visited);
            }
        }
    }

    //should take in the points of two existing nodes and connect them together in their links vector
    //check if two nodes are in the same graph
        //if they are then just run DFS and connect the two
        //else connect 1 to 2 and 2 to 1 and remove node 2 index from the graph index (update node locs as well)
            //note if you connect any node from an index in all_graphs with another one, it becomes one graph so you can get rid of the entire pointer on one side
            //have to change node locs of the graph you are moving though
    //right now just going to give it node identifier parameter
    void joinNodes(ull ident1, ull ident2, unsigned int link_weight){
        if (!node_locs.count(ident1) || !node_locs.count(ident2)){
            std::cout << "ERROR in graph.joinNodes: One or both of nodes to join have not been created before EXITING\n";
            exit(EXIT_FAILURE);
        }else{
            unsigned int loc1 = node_locs[ident1], loc2 = node_locs[ident2];
            Node* n1 = REVISEDfindNode(ident1);
            Node* n2 = REVISEDfindNode(ident2);
            if (!n1){
                std::cout << "Join Nodes - N1(" << ident1 << ") WASNT FOUND but it should EXITING\n";
                exit(EXIT_FAILURE);
            }
            if (!n2){
                std::cout << "JOIN NODES - N2(" << ident2 << ") WASNT FOUND but it should EXITING\n";
                exit(EXIT_FAILURE);
            }

            //update node 2's node_loc to be the same as node 1
            if (loc1 != loc2){
                std::unordered_set<ull> visited;
                //update node_locs map for the graph thats getting moved
                moveGraphLoc(loc1, all_graphs[loc2], visited);
                //decrement total number of graphs
                num_graphs--;
            }

            //CHECK TO SEE IF NODES ALREADY CONNECTED
            //connect the two nodes together
            ull link_ident = getNewLinkIdent();
            n1->addNode(n2, link_weight, link_ident);
            n2->addNode(n1, link_weight, link_ident);

            //remove graph getting moved from all_graphs if locations are different
            // if (loc1 != loc2) all_graphs.erase(all_graphs.begin()+loc2);
            if (loc1 != loc2) all_graphs[loc2] = NULL;
            
        }

    }
};