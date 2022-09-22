/*
Graph class:
    - vector of all UNJOINED graphs (holds the head of each graph)
    - if you want to iterate through an individual graph you need to check for already visited nodes (graph traversal)
    - Merging graphs and updating the all_graphs vector
    - handle deletion of a Node (what happens to its links)
*/
#pragma once
#include "imgui.h" 
#include "imgui-SFML.h" 
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <math.h>

#include "node.hpp"


class Graph{
    private:
        typedef std::tuple<Node*, ll, ll, bool> ADJ_NODE;               //(tuple: curr node, link weight, link identifier, link type (can main node access curr node))
        size_t win_width, win_height;                                   //keeps track of the interface window width and height
        size_t num_graphs;                                              //keeps track of the total number of graphs
        ll curr_node_ident;                                            //used to create a new unique node identifier
        ll curr_link_ident;                                            //used to create a new unique link identifier
        std::vector<Node*> all_graphs;                                  //vector containing all graphs
        std::vector<size_t> open_locs;                                  //Keeps track of indices in all_graphs that are null
        std::unordered_map<ll, size_t> node_locs;                      //<Node identifier, all_graphs index> Keeps track of a nodes location in all_graphs                  
        Node** node_ilocs;                                              //Keeps track of the location of each node in the window interface                     
        Links GUIlinks;                                                 //Lines used to represent links between nodes on the interface
        //NOT YET IMPLEMENTED: Keeps track of open cells in all_graphs (might implement later - to deal with all_graphs space usage)
        //std::unordered_set<size_t> open_locs;
    public:
        Graph(): curr_node_ident(0), curr_link_ident(0), num_graphs(0){};

        Graph(const int& w_width, const int& w_height): curr_node_ident(0), curr_link_ident(0), num_graphs(0){
            //initialize node interface location array
            size_t i_size = w_width*w_height;
            node_ilocs = new Node*[i_size];
            for (size_t i=0; i < i_size; ++i)
                node_ilocs[i] = NULL;

            //save window dimensions
            win_width = w_width;
            win_height = w_height;

            //set the primitive type for the interface links
            //GUIlinks.setPrimitiveType(sf::Lines);
        };



        /* 
            Functions that draw to the gui
                - Drawing links
                - Drawing Graph nodes
                - Drawing graph viewing table
        */

        //Draws all links
        inline void drawAllLinks(sf::RenderWindow* win){
            GUIlinks.drawLinks(win);
        }

        //helper of function drawAllNodesinGraph
        //Runs DFS on graph to draw each node in the given graph
        void DFSGraphDraw(Node* curr, std::unordered_set<ll>& visited, sf::RenderWindow* win){
            visited.insert(curr->getNodeIdent());
            curr->drawNode(win);

            //inspect current node's links if they aren't already visited
            std::vector<ADJ_NODE> links = curr->getNodeLinks();
            for (size_t i=0; i < links.size(); ++i){
                Node* inspect = std::get<0>(links[i]);
                if (!visited.count(inspect->getNodeIdent())){
                    DFSGraphDraw(inspect, visited, win);
                }
            }
        }

        //draws all nodes in a given graph index
        void drawAllNodesinGraph(size_t graph_idx, sf::RenderWindow* win){
            Node* node_head = all_graphs[graph_idx];
            if (node_head){
                std::unordered_set<ll> visited;
                DFSGraphDraw(node_head, visited, win);
            }
        }

        // record nodes at a certain graph position to IMGUI graph table
        void recordAllNodes(Node* curr, std::unordered_set<ll>& visited, std::string& allnodes){
            ll currNode = curr->getNodeIdent();
            visited.insert(currNode);
            allnodes += std::to_string(currNode);

            std::vector<ADJ_NODE> links = curr->getNodeLinks();
            for (size_t i=0; i < links.size(); ++i){
                Node* inspect = std::get<0>(links[i]);
                if (!visited.count(inspect->getNodeIdent())){     
                    allnodes += ' ';
                    recordAllNodes(inspect, visited, allnodes);
                }
            }

        }

        // render IMGUI table to display nodes corresponding to each graph
        void drawGraphViewer(){
            for (size_t i=0; i < all_graphs.size(); ++i){
                std::unordered_set<ll> visited;
                std::string allnodes = "\t";

                ImGui::Text("Graph: %d", i);
                if (all_graphs[i]){
                    recordAllNodes(all_graphs[i], visited, allnodes);
                    ImGui::Text(allnodes.c_str());
                }else{
                    ImGui::Text(" ");
                }

                visited.clear();
            }
        }


        //determines if it's legal to move node to a new position
        void moveNode(Node* node, sf::Vector2f& oldpos, sf::Vector2f& newpos){
            /* sf::Vector2i move_offset = sf::Vector2i(newpos - oldpos);
            sf::Vector2i npos = sf::Vector2i(node->getNodePos());
            bool move_right = move_offset.x > 0? true : false;
            bool move_down = move_offset.y > 0? true : false;
            size_t start_pos = npos.y*win_width + npos.x;
            size_t x_last_empty;

            //boundary check x-axis
            if (move_right){
                size_t x_travelled = move_offset.x-1;
                for (size_t x = start_pos+1; x < (npos.y*win_width + win_width)-NODE_RADIUS && x_travelled >= 0; ++x){
                    if (node_ilocs[x] == NULL){
                        x_last_empty = x;
                        x_travelled--;
                    }
                }
            }else{
                if (move_offset.x != 0){
                    if (newpos.x-NODE_RADIUS < 0) newpos.x = NODE_RADIUS;
                }
            }

            //boundary check y-axis  
            if (move_down){
                if (newpos.y+NODE_RADIUS >= win_height) newpos.y = win_height-NODE_RADIUS;
            }else{
                if (move_offset.y != 0){
                    if (newpos.y-NODE_RADIUS < 0) newpos.y = NODE_RADIUS;
                }
            } */
            sf::Vector2i move_offset = sf::Vector2i(newpos - oldpos);
            bool move_down = move_offset.y > 0? true : false;
            bool move_right = move_offset.x > 0? true : false;
            int farthest_empty;
            sf::Vector2i adjusted_newpos;
            sf::Vector2i adjusted_offset(0, 0);

            if (move_offset.x == 0){
                //dragging node only on the y-axis
                int remaining_ydist = move_offset.y;
                size_t curr_x = int(oldpos.x);
                
                if (move_down){
                    //determine farthest you can go right without interacting border or other node
                    for (size_t y = int(oldpos.y)+1; y < win_height-NODE_RADIUS && remaining_ydist > 0; ++y){
                        if (node_ilocs[win_width*y + curr_x] == NULL)
                            farthest_empty = y;
                        else 
                            break;
                        --remaining_ydist;
                    }

                    adjusted_offset.y = farthest_empty - int(oldpos.y);

                }else{
                    //determine farthest you can go left without interacting border or other node
                    for (size_t y = int(oldpos.y)-1; y >= 0+NODE_RADIUS && remaining_ydist < 0; --y){
                        if (node_ilocs[win_width*y + curr_x] == NULL)
                            farthest_empty = y;
                        else   
                            break;
                        ++remaining_ydist;
                    }

                    adjusted_offset.y = int(oldpos.y) - farthest_empty;

                }
            }else if (move_offset.y == 0){
                //dragging node only on the x-axis 
                int remaining_xdist = move_offset.x;
                size_t curr_y = int(oldpos.y)*win_width;

                if (move_right){
                    //determine farthest you can go right without interacting border or other node
                    for (size_t x = int(oldpos.x)+1; x < win_width-NODE_RADIUS && remaining_xdist > 0; ++x){
                        if (node_ilocs[curr_y + x] == NULL)
                            farthest_empty = x;
                        else 
                            break;
                        --remaining_xdist;
                    }

                    adjusted_offset.x = farthest_empty - int(oldpos.x);

                }else{
                    //determine farthest you can go left without interacting border or other node
                    for (size_t x = int(oldpos.x)-1; x >= 0+NODE_RADIUS && remaining_xdist < 0; --x){
                        if (node_ilocs[curr_y + x] == NULL)
                            farthest_empty = x;
                        else   
                            break;
                        ++remaining_xdist;
                    }

                    adjusted_offset.x = int(oldpos.x) - farthest_empty;

                }
            }else{
                int dx_dy = round(move_offset.x/move_offset.y);
                int dy_dx = round(move_offset.y/move_offset.x);

                if (dx_dy > dy_dx){

                }else if (dy_dx > dx_dy){

                }else{

                }

            }

            node->setNodePos(adjusted_offset + sf::Vector2i(node->getNodePos()));


        }

        //gets the head node of a graph at index i
        inline Node* getGraphHead(size_t i){
            return all_graphs[i];
        }
        
        //returns the size of all the graphs in the all_graphs vector
        inline size_t getAllGraphSize(){
            return all_graphs.size();
        }
        
        //checks if a node exists and returns the position of a node in all_graphs 
        inline size_t getNodeGraphsPos(ll ident){
            if (node_locs.count(ident))
                return node_locs[ident];
            else
                return -1;
        }

        //returns a valid Node identifier and updates next available identifier
        inline ll getNewNodeIdent(){
            return curr_node_ident++;
        }

        //returns a valid Link identifier and updates next available identifier
        inline ll getNewLinkIdent(){
            return curr_link_ident++;
        }

        //returns size of the all_graphs vector
        inline size_t all_graphs_size(){
            return all_graphs.size();
        }

        //returns total number of existing graphs
        inline size_t getNumGraphs(){
            return num_graphs;
        }

        //returns the node interface location array
        inline Node** getNodeIlocs(){
            return node_ilocs;
        }

        //Runs a depth first search on the graph to find node with the given identifier (Returns NULL if node was not in the graph)
        Node* graphDFS(Node* curr, ll ident, std::unordered_set<ll>& visited){
            std::cout << "Curr node: " << curr->getNodeIdent() << '\n';
            if (curr->getNodeIdent() == ident){
                std::cout << "DFS found: " << curr->getNodeIdent() << " == " << ident << '\n';
                return curr;
            }else{
                //mark current node as visited
                visited.insert(curr->getNodeIdent());
                //get the current nodes links
                std::vector<ADJ_NODE> links = curr->getNodeLinks();

                //search for the next unvisited node in the current nodes links vector
                for (size_t i=0; i < links.size(); ++i){
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
        Node* findNode(size_t graph_index, ll ident){
            std::unordered_set<ll> visited;
            //write a map that determines which index in all_graphs the indentifier belongs to
            return graphDFS(all_graphs[graph_index], ident, visited);
        }

        //runs DFS to find node with given identifier
        //REVISED: you don't have to tell which all_graphs index to look into
        Node* findNode(ll ident){
            std::cout << "\nFINDING NODE" << ident << "\n";
            if (!node_locs.count(ident)){
                std::cout << "\tERROR in graph.REVISEDfindNode - Node location not recorded in map - EXITING\n";
                exit(EXIT_FAILURE);
            }
            
            Node* findNode = all_graphs[node_locs[ident]];
            if (findNode == NULL){
                std::cout << "\tREVISEDfindNode: Error - node to find is not in all_graphs\n";
                std::cout << "\t Searching for node: " << ident;
                std::cout << "\n\t Searched for index: " << node_locs[ident] << " in all_graphs - EXITING\n";
                exit(EXIT_FAILURE);
            }else{
                std::cout << "\tNode found in location: " << node_locs[ident] << "\n";
            }

            std::unordered_set<ll> visited;
            Node* ret = graphDFS(findNode, ident, visited);
            if (ret == NULL) std::cout << "\tDFS returned nll\n";
            return ret;
        }


        //creates a new individual node and implants it as its own graph at position pos
        void createNewNode(sf::Vector2i pos, sf::Font& font){
            ll ident = getNewNodeIdent();
            Node* nn = new Node(ident, sf::Vector2f(pos), font);

            // determine where to place the new node in all_graphs
            size_t open_idx = all_graphs.size();
            if (open_locs.size() > 0){
                open_idx = open_locs[0]; 
                open_locs.erase(open_locs.begin());
                all_graphs[open_idx] = nn;   
            }else{
                all_graphs.push_back(nn);
            }

            //store mapping from identity to all_graphs index
            node_locs[ident] = open_idx;

            
            // set the node in the interface array
            size_t iloc = win_width*pos.y + pos.x;
            if (node_ilocs[iloc] == NULL){
                node_ilocs[iloc] = nn;
            }else{
                std::cout << "\tCREATE NODE - adding to iloc position: " << iloc << " is not nll - EXITING\n";
                exit(EXIT_FAILURE);
            }

            num_graphs++;
        }

        // debugging graph deleting 
        // CAN DELETE LATER
        void displayOpenLocs(){
            std::cout << "DISPLAYING OPEN_LOCS\n\t";
            for (size_t i=0; i < open_locs.size(); ++i){
                std::cout << open_locs[i] << " ";
            }
            std::cout << "\n";
        }



    /* 
        - Joining Nodes
        - Unjoining Nodes

    */

        //should take in the points of two existing nodes and connect them together in their links vector
        //check if two nodes are in the same graph
            //if they are then just run DFS and connect the two
            //else connect 1 to 2 and 2 to 1 and remove node 2 index from the graph index (update node locs as well)
                //note if you connect any node from an index in all_graphs with another one, it becomes one graph so you can get rid of the entire pointer on one side
                //have to change node locs of the graph you are moving though
        //right now just going to give it node identifier parameter
        /* void joinNodes(ll ident1, ll ident2, size_t link_weight, const LinkStat& lstate){
            if (ident1 == ident2){
                std::cout << "ERROR in graph.joinNodes: cannot join node with itself EXITING\n";
                exit(EXIT_FAILURE);
            }
            if (!node_locs.count(ident1) || !node_locs.count(ident2)){
                std::cout << "ERROR in graph.joinNodes: One or both of nodes to join have not been created before EXITING\n";
                exit(EXIT_FAILURE);
            }else{
                size_t loc1 = node_locs[ident1], loc2 = node_locs[ident2];
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

                //update node 2's node_loc to be the same as node 1 if it isn't already
                if (loc1 != loc2){
                    std::unordered_set<ll> visited;
                    //update node_locs map for the graph thats getting moved
                    moveGraphLoc(all_graphs[loc2], visited, loc1);
                    num_graphs--;
                }

                //Check to see if nodes are connected
                bool alreadyConnected = 0;
                std::vector<ADJ_NODE> links = n1->getNodeLinks();
                for (size_t i =0; i < links.size(); ++i){
                    Node* check = std::get<0>(links[i]);
                    if (check->getNodeIdent() == n2->getNodeIdent()){
                        alreadyConnected = 1;
                        break;
                    }
                }

                //connect the two nodes together if not already connected
                if (!alreadyConnected){
                    ll link_ident = getNewLinkIdent();
                    n1->addLinktoNode(n2, link_weight, link_ident);
                    n2->addLinktoNode(n1, link_weight, link_ident);
                    //addNewLink(n1->getNodePos(), n2->getNodePos(), sf::Color::Green);
                    GUIlinks.addLink(n1->getNodePos(), n2->getNodePos(), n1->getNodeIdent(), n2->getNodeIdent(), lstate);
                    //GUIlinks.setLinkMap(n1->getNodeIdent(), n2->getNodeIdent(), GUIlinks.getLinksSize()-2);
                }

                //remove graph getting moved from all_graphs if locations are different
                //record open position in open_locs
                if ((loc1 != loc2) && !alreadyConnected){
                    all_graphs[loc2] = NULL;
                    open_locs.push_back(loc2);
                }
                
            }

        }

        //Join node by Node* values
        void joinNodes(Node* n1, Node* n2, size_t link_weight, const LinkStat& lstate){
            size_t loc1 = node_locs[n1->getNodeIdent()];
            size_t loc2 = node_locs[n2->getNodeIdent()];

            //update node 2's node_loc to be the same as node 1 if it isn't already
            if (loc1 != loc2){
                std::unordered_set<ll> visited;
                //update node_locs map for the graph thats getting moved
                moveGraphLoc(all_graphs[loc2], visited, loc1);
                all_graphs[loc2] = NULL;
                open_locs.push_back(loc2);
                num_graphs--;
            }

            //Check to see if nodes are connected
            bool alreadyConnected = 0;
            std::vector<ADJ_NODE> links = n1->getNodeLinks();
            for (size_t i =0; i < links.size(); ++i){
                Node* check = std::get<0>(links[i]);
                if (check->getNodeIdent() == n2->getNodeIdent()){
                    alreadyConnected = 1;
                    break;
                }
            }

            //connect the two nodes together if not already connected
            if (!alreadyConnected){
                ll link_ident = getNewLinkIdent();
                n1->addLinktoNode(n2, link_weight, link_ident);
                n2->addLinktoNode(n1, link_weight, link_ident);
                //addNewLink(n1->getNodePos(), n2->getNodePos(), sf::Color::Green);
                GUIlinks.addLink(n1->getNodePos(), n2->getNodePos(), n1->getNodeIdent(), n2->getNodeIdent(), lstate);
                //GUIlinks.setLinkMap(n1->getNodeIdent(), n2->getNodeIdent(), GUIlinks.getLinksSize()-2);
            }
        }
    */

        //joining nodes n1 (from node), to n2 (to node)
        void joinNodes(Node* n1, Node* n2, const ll& link_weight, const LinkStat& lstate){
            ll n1Ident = n1->getNodeIdent();
            ll n2Ident = n2->getNodeIdent();
            size_t loc1 = node_locs[n1Ident];
            size_t loc2 = node_locs[n2Ident];

            //update node 2's node_loc to be the same as node 1 if it isn't already
            if (loc1 != loc2){
                std::unordered_set<ll> visited;
                //update node_locs map for the graph thats getting moved
                moveGraphLoc(all_graphs[loc2], visited, loc1);
                all_graphs[loc2] = NULL;
                open_locs.push_back(loc2);
                num_graphs--;
            }

            //Check to see link status between both nodes
            bool alreadyConnected = 0;
            bool n1_con;
            ll currLinkWeight = 0;
            std::vector<ADJ_NODE> links = n1->getNodeLinks();
            size_t n1_l_idx;
            for (size_t i =0; i < links.size(); ++i){
                Node* check = std::get<0>(links[i]);
                if (check->getNodeIdent() == n2->getNodeIdent()){
                    alreadyConnected = 1;
                    n1_l_idx = i;
                    n1_con = std::get<3>(links[i]);
                    currLinkWeight = std::get<1>(links[i]);
                    break;
                }
            }

            
            //determine how to link n1 and n2 by link state
            bool n1ConnectionStat = true;
            bool n2ConnectionStat = true;
            if (lstate == LinkStat::SinglyTo)
                n2ConnectionStat = false;

            //connect the two nodes together if not already connected
            if (!alreadyConnected){
                ll link_ident = getNewLinkIdent();

                n1->addLinktoNode(n2, link_weight, link_ident, n1ConnectionStat);
                n2->addLinktoNode(n1, link_weight, link_ident, n2ConnectionStat);
                GUIlinks.addLink(n1->getNodePos(), n2->getNodePos(), n1Ident, n2Ident, link_weight, lstate);
                std::cout << "added node link\n";
            }else{
                //determine connection from n1 to n2
                size_t n2_l_idx;
                bool n2_con;
                std::vector<ADJ_NODE> links2 = n2->getNodeLinks();
                for (size_t i =0; i < links2.size(); ++i){
                    Node* check = std::get<0>(links2[i]);
                    if (check->getNodeIdent() == n1->getNodeIdent()){
                        n2_l_idx = i;
                        n2_con = std::get<3>(links2[i]);
                        break;
                    }
                }

                std::cout << "\n\tlinkage - already connected:" << n1_con << " " << n2_con << " \n";
                std::cout << "\tconnecting to: " << n1ConnectionStat << " " << n2ConnectionStat << " \n";

                //update link status of already created nodes
                if (n1ConnectionStat != n1_con || n2ConnectionStat != n2_con){
                    std::cout << "\tUPDATED LINK\n";
                    n1->changeLinkType(n1_l_idx, n1ConnectionStat);
                    n2->changeLinkType(n2_l_idx, n2ConnectionStat);

                    GUIlinks.removeLink(n1Ident, n2Ident);
                    GUIlinks.addLink(n1->getNodePos(), n2->getNodePos(), n1Ident, n2Ident, link_weight, lstate);

                    /* 
                    //DEBUGGING - CAN REMOVE LATER
                    std::vector<ADJ_NODE> links = n1->getNodeLinks();
                    for (size_t i =0; i < links.size(); ++i){
                        Node* check = std::get<0>(links[i]);
                        if (check->getNodeIdent() == n2->getNodeIdent()){
                            std::cout << "\tn1->n2: " << std::get<3>(links[i]) << " ";
                            break;
                        }
                    }

                    std::vector<ADJ_NODE> links2 = n2->getNodeLinks();
                    for (size_t i =0; i < links2.size(); ++i){
                        Node* check = std::get<0>(links2[i]);
                        if (check->getNodeIdent() == n1->getNodeIdent()){
                            std::cout << "n2->n1: " << std::get<3>(links2[i]) << "\n";
                            break;
                        }
                    } */

                }else if(currLinkWeight != link_weight){
                    std::cout << "\tUPDATING LINK WEIGHT\n";
                    n1->changeLinkWeight(n1_l_idx, link_weight);
                    n2->changeLinkWeight(n2_l_idx, link_weight);
                    GUIlinks.updateLinkWeight(n1->getNodeIdent(), n2->getNodeIdent(), link_weight);
                    
                }
            }
        }

        //removes the link between two nodes and manages graph movement for those nodes
        /* 
            @TODO 0->1->2->3, removing link from 3 to 1,
                                210 gets put in new loc,
                                012 exists in original loc,
                                and 3 removed from GUI
        */
        void unJoinNodes(Node* n1, Node* n2){
            //Don't attempt to unjoin nodes if they aren't in the same graph
            size_t graphLoc = node_locs[n2->getNodeIdent()];
            std::cout << "head of graph is " << all_graphs[graphLoc]->getNodeIdent() << std::endl;
            std::cout << "N1: " << n1->getNodeIdent() << " N2: " << n2->getNodeIdent() << std::endl;
            if (graphLoc != node_locs[n1->getNodeIdent()])
                return;    

            //erase instance of each node in each others links
            n1->remLinktoNode(n2->getNodeIdent());
            n2->remLinktoNode(n1->getNodeIdent());
            
            //determine if a node needs to be moved to a new graph
            std::unordered_set<ll> visited;
            Node* inGraph = graphDFS(n1, n2->getNodeIdent(), visited);
            visited.clear();

            //Check whether n2 or n1 should be moved
            Node* graphHead = all_graphs[graphLoc];
            bool moveN1 = graphDFS(graphHead, n2->getNodeIdent(), visited) == n2 ? true : false;
            visited.clear();

            //Establish n1 or n2 as its own graph if n1 and n2 are separate graphs after link removal
            if (inGraph == NULL){     
                //determine position of graph n1 or n2 will go to
                size_t new_loc = all_graphs.size();
                if (open_locs.size() > 0){
                    new_loc = open_locs[0];
                    open_locs.erase(open_locs.begin());
                }
                if (new_loc == all_graphs.size()){
                    all_graphs.push_back(NULL);
                }

                //if n2 is not head of graph move n2, otherwise move n1 to new graph
                if (moveN1){
                    std::cout << "MOVING N1\n";
                    std::cout << "MOVING NODE " << n1->getNodeIdent() << " TO GRAPH " << new_loc << std::endl;
                    all_graphs[new_loc] = n1;
                    moveGraphLoc(n1, visited, new_loc);
                }else{
                    std::cout << "N2(" << n2->getNodeIdent() <<") " "is NOT head of graph\n";
                    std::cout << "MOVING NODE " << n2->getNodeIdent() << " TO GRAPH " << new_loc << std::endl;
                    all_graphs[new_loc] = n2;
                    moveGraphLoc(n2, visited, new_loc);
                }

                num_graphs++;
            }

            //remove the UI links
            GUIlinks.removeLink(n1->getNodeIdent(), n2->getNodeIdent());
        }



    /* 
        - Deleting Node functions
        - Helper functions for delete method

    */

        // runs DFS move the graph so all nodes within that graph are found at new_loc
        void moveGraphLoc(Node* curr, std::unordered_set<ll>& visited, size_t new_loc){
            // change where to find node via node_locs
            ll curr_ident = curr->getNodeIdent();
            visited.emplace(curr_ident);
            node_locs[curr_ident] = new_loc;
            std::cout << "\tnode: " << curr_ident << " moved to: " << new_loc << '\n';

            // DFS to change all attached children to the new location in all_graphs
            std::vector<ADJ_NODE> links = curr->getNodeLinks();
            for (size_t i=0; i < links.size(); ++i){
                Node* inspect = std::get<0>(links[i]);
                if (!visited.count(inspect->getNodeIdent()))
                    moveGraphLoc(inspect, visited, new_loc);
            }
        }

        // deletes a given node and updates connected node's positions
        void deleteNode(Node* NTD){
            std::cout << "\nDELETING NODE " << NTD->getNodeIdent() << "\n";
            sf::Vector2i npos= sf::Vector2i(NTD->getNodePos());
            ll NTDident = NTD->getNodeIdent();
            
            std::vector<ADJ_NODE> links = NTD->getNodeLinks();
            std::unordered_set<ll> visited;
            visited.emplace(NTDident);

            // move each child node to a new graph positon & remove link from child to NTD
            for (size_t i=0; i < links.size(); ++i){
                Node* child = std::get<0>(links[i]);
                ll child_ident = child->getNodeIdent();

                if (!visited.count(child_ident)){
                    size_t new_loc = all_graphs.size();

                    // change location of graph head in all_graphs
                    if (open_locs.size() > 0){
                        new_loc = open_locs[0];
                        all_graphs[new_loc] = child;
                        open_locs.erase(open_locs.begin());
                        std::cout << "\tmoving to open loc pos\n";
                    }else{
                        all_graphs.push_back(child);
                        std::cout << "\tmoving to all_graph end\n";
                    }
                    
                    // change the graph so all nodes are found at the new_loc
                    moveGraphLoc(child, visited, new_loc);
                    num_graphs++;
                }

                // remove link from NTD to children node and GUI links
                child->remLinktoNode(NTDident);
                GUIlinks.removeLink(NTDident, child_ident);
                //GUIlinks.removeLinkMap(NTDident, child_ident);
            }


            // remove NTD from graph structure 
            size_t NTDloc = node_locs[NTDident];
            all_graphs[NTDloc] = NULL;
            open_locs.push_back(NTDloc);
            node_locs.erase(NTDident);
            num_graphs--;

            // remove NTD from GUI 
            size_t ipos = win_width*npos.y + npos.x;
            node_ilocs[ipos] = NULL;

            // free NTD
            delete NTD;
            NTD = NULL;
        }


    /* 
        - Updating Link weight
        - Displaying link Weight

    */

        //updates the link connection weight between two nodes by pointer to a given link weight
        void updateNodeLink(Node* n1, Node* n2, size_t lw){
            if (n1 == NULL){
                std::cout << "UpdateNodeLink: Error - n1 null\n";
                exit(EXIT_FAILURE);
            }
            if (n2 == NULL){
                std::cout << "UpdateNodeLink: Error - n2 null\n";
                exit(EXIT_FAILURE);
            }

            bool n1connected = 0, n2connected = 0;
            size_t idx1, idx2;
            //check that the two nodes are connected to each other and in the same graph
            std::vector<ADJ_NODE>& l1 = n1->getNodeLinks();
            std::vector<ADJ_NODE>& l2 = n2->getNodeLinks();
            for (size_t i=0; i < l1.size(); ++i){
                Node* check = std::get<0>(l1[i]);
                //search first node for link to second node
                if (check == n2){
                    idx2 = i;
                    n1connected =1;
                    break;
                }
            }
            for (size_t i=0; i < l2.size(); ++i){
                Node* check = std::get<0>(l2[i]);
                //search second node for link to first node
                if (check == n1){
                    idx1 = i;
                    n2connected =1;
                    break;
                }
            }


            // If the nodes aren't doubly linked to each other exit
            if (!n1connected || !n2connected){
                if (!n1connected)
                    std::cout << "UpdateNodeLink: Error - N1 isn't connected to N2\n";
                if (!n2connected)
                    std::cout << "UpdateNodeLink: Error - N2 isn't connected to N1\n";
                exit(EXIT_FAILURE);
            }

            //update link weight for n1
            // ADJ_NODE& change = l1[idx2];
            // std::get<1>(change) = lw;
            // l1[idx2] = change;
            n1->changeLinkWeight(idx2, lw);

            //update link weight for n2
            // change = l2[idx1];
            // std::get<1>(change) = lw;
            // l2[idx1] = change;
            n2->changeLinkWeight(idx1, lw);
        }

        //updates the link connection weight between two nodes by identifier to a given link weight
        void updateNodeLink(ll ident1, ll ident2, size_t lw){
            std::cout << "UpdateNodeLin:\n\tSearching n1\n";
            Node* n1 = findNode(ident1);
            std::cout << "\tSearching n2\n";
            Node* n2 = findNode(ident2);
            if (n1 == NULL){
                std::cout << "UpdateNodeLink: Error - n1 null\n";
                exit(EXIT_FAILURE);
            }
            if (n2 == NULL){
                std::cout << "UpdateNodeLink: Error - n2 null\n";
                exit(EXIT_FAILURE);
            }
            std::cout << "\t\tUpdate Found all nodes\n";

            bool n1connected = 0, n2connected = 0;
            size_t idx1, idx2;
            //check that the two nodes are connected to each other and in the same graph
            std::vector<ADJ_NODE>& l1 = n1->getNodeLinks();
            std::vector<ADJ_NODE>& l2 = n2->getNodeLinks();
            for (size_t i=0; i < l1.size(); ++i){
                Node* check = std::get<0>(l1[i]);
                //search first node for link to second node
                if (check == n2){
                    idx2 = i;
                    n1connected =1;
                    break;
                }
            }
            for (size_t i=0; i < l2.size(); ++i){
                Node* check = std::get<0>(l2[i]);
                //search second node for link to first node
                if (check == n1){
                    idx1 = i;
                    n2connected =1;
                    break;
                }
            }


            // If the nodes aren't doubly linked to each other exit
            if (!n1connected || !n2connected){
                if (!n1connected)
                    std::cout << "UpdateNodeLink: Error - N1 isn't connected to N2\n";
                if (!n2connected)
                    std::cout << "UpdateNodeLink: Error - N2 isn't connected to N1\n";
                exit(EXIT_FAILURE);
            }

            //update link weight for n1
            ADJ_NODE& change = l1[idx2];
            std::get<1>(change) = lw;

            //update link weight for n2
            ADJ_NODE& change2 = l2[idx1];
            std::get<1>(change2) = lw;
        }

        //debug function to see the link weight between two nodes
        void displayLinkWeight(ll ident1, ll ident2){
            Node* n1 = findNode(ident1);
            Node* n2 = findNode(ident2);

            std::vector<ADJ_NODE> l1 = n1->getNodeLinks();
            std::vector<ADJ_NODE> l2 = n2->getNodeLinks();
            for (size_t i=0; i < l1.size(); ++i){
                Node* check = std::get<0>(l1[i]);
                //search first node for link to second node
                if (check == n2){
                    std::cout << "displayLinkWeight:\n\t" << ident1 << " -> " << ident2 << " weight: " << std::get<1>(l1[i]);
                    break;
                }
            }

            for (size_t i=0; i < l2.size(); ++i){
                Node* check = std::get<0>(l2[i]);
                //search second node for link to first node
                if (check == n1){
                    std::cout << "\n\t" << ident2 << " -> " << ident1 << " weight: " << std::get<1>(l2[i]) << std::endl;
                    break;
                }
            }
        }



    /* 
        - Erasing Graph functions (DFS method)
        - No longer needed ?
        - Used before for deconstructor

    */

        //helper function for eraseGraph that uses visited set in order to recursively delete all nodes in a graph
        void eraseGraphHelper(Node* curr, std::unordered_set<Node*>& visited){
            std::cout << "\tNode " << curr->getNodeIdent() << " has been visited\n";
            //mark current node as visited
            visited.insert(curr);

            //inspect the links of the current node if they are not visited
            std::vector<ADJ_NODE> links = curr->getNodeLinks();
            for (size_t i = 0; i < links.size(); ++i){
                Node* inspect = std::get<0>(links[i]);

                if (!visited.count(inspect)){
                    std::cout << "\t\tinspecting Node: " << inspect->getNodeIdent() << "\n";
                    eraseGraphHelper(inspect, visited);
                }
            } 
            
            //after all current links have been visited delete memory of current node
            std::cout << "\tdeleting Node: " << curr->getNodeIdent() << std::endl;
            delete curr;
            curr = NULL;
            if (curr){
                std::cout << "\tNode exists?? - " << curr->getNodeIdent() << std::endl;
            }
            
        }
        
        //erases all nodes in a graph given the head node of that graph
        void eraseGraph(Node* graph_head){
            std::cout << "\n\nErasing Graph\n";
            //set to mark nodes as visited
            std::unordered_set<Node*> visited;

            //erase entire graph
            eraseGraphHelper(graph_head, visited);
        }

        //erases every node in all of the graphs
        void eraseAllGraphs(){
            for (size_t i=0; i < all_graphs.size(); i++){
                if (all_graphs[i] != NULL)
                    eraseGraph(all_graphs[i]);
        }
        }



        //deconstructor deletes every node for all the graphs
        ~Graph(){
            //eraseAllGraphs();

            std::cout << "Freeing all nodes\n";
            for (size_t i=0; i < win_width*win_height; ++i){
                if (node_ilocs[i] != nullptr){
                        std::cout << "\tFreeing node: " << node_ilocs[i]->getNodeIdent() << '\n';
                        delete node_ilocs[i];
                        node_ilocs[i] = NULL;
                }
            }
        }
};