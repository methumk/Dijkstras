/*
structure.hpp
    - Contains the structure for a single node
    - Contains the structure to contain multiple graphs and corresponding functionalities
*/
#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <string>

//Define ull for identifiers
typedef unsigned long long ull;
#define NODE_RADIUS 30


/*
Node class:
    - Node identifier
    - Circle shape??
    - links (tuple: curr node, link weight, link identifier)
*/
class Node{
public:
    typedef std::tuple<Node*, size_t, ull> ADJ_NODE;
    typedef std::vector<ADJ_NODE> NODE_VEC;
private:
    ull ident;                              //Nodes identifying number
    std::vector<ADJ_NODE> links;            //The nodes connections: vector of tuples<node, link weight, node identifier>
    sf::CircleShape GUInode;                //Circle used to represent node on the interface
    sf::Text id;                            //Interface text used to identify each node
public:
    Node(){}
    Node(ull i): ident(i){}
    Node(ull i, sf::Vector2f pos, sf::Font& font) : ident(i){
        //set information about gui node
        GUInode.setRadius(NODE_RADIUS);
        GUInode.setOrigin(NODE_RADIUS, NODE_RADIUS);
        GUInode.setPosition(pos);
        GUInode.setFillColor(sf::Color::Blue);
        GUInode.setOutlineColor(sf::Color::White);
        GUInode.setOutlineThickness(2.f);

        //set inofrmation about node text
        id.setFont(font);
        id.setString(std::to_string(ident));
        id.setCharacterSize(18);
        id.setOrigin(id.getLocalBounds().width/2., id.getLocalBounds().height/2.);
        id.setPosition(pos);
        id.setFillColor(sf::Color::Red);  
    }   

    //renders individual node and text
    void drawNode(sf::RenderWindow* win){
        win->draw(GUInode);       
        win->draw(id); 
    }

    //moves the gui node to a given position
    inline void moveNode(sf::Vector2i pos){
        GUInode.setPosition(sf::Vector2f(pos));
        id.setPosition(sf::Vector2f(pos));
    }

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
    void addNode(Node* addNode, size_t lw, ull li){
        links.push_back(std::make_tuple(addNode, lw, li));
    }

    //add a new linked up node by tuple
    inline void addNode(ADJ_NODE addNode){
        links.push_back(addNode);
    }

    inline void printCurrNode(){
        std::cout << "Node ident: " << ident << '\n';
    }

    //print all information about nodes connected to current node
    void printAdjNodes(){
        for (size_t i = 0; i < links.size(); ++i){
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
    inline ADJ_NODE getNodeInfo(size_t i){
        return links[i];
    }

    //have to do dfs on graph to find correct identifier
    //retrieves node and link information at the given node identifier
    inline ADJ_NODE getNodeInfo(ull i){
        return links[i];
    }

    //retrieves Node for a node at the given links index
    inline Node* getAdjNode(size_t i){
        return std::get<0>(links[i]);
    }

    //have to do dfs on graph to find correct identifier
    //retrieves Node for the node with the given node identifier
    inline Node* getAdjNode(ull ident){
        return NULL;
    }

    //removes current node's link to a given node from its links vector
    void remLinktoNode(ull ident){
        for (size_t i=0; i < links.size(); ++i){
            Node* inspect = std::get<0>(links[i]);
            if (inspect->getNodeIdent() == ident){
                links.erase(links.begin()+i);
                break;
            }
        }
    }

    //returns the position of the node
    inline sf::Vector2f getNodePos(){
        return GUInode.getPosition();
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
    typedef std::tuple<Node*, size_t, ull> ADJ_NODE;
    size_t win_width, win_height;                                   //keeps track of the interface window width and height
    size_t num_graphs;                                              //keeps track of the total number of graphs
    ull curr_node_ident;                                            //used to create a new unique node identifier
    ull curr_link_ident;                                            //used to create a new unique link identifier
    std::vector<Node*> all_graphs;                                  //vector containing all graphs
    std::unordered_map<ull, size_t> node_locs;                      //<Node identifier, all_graphs index> Keeps track of a nodes location in all_graphs                  
    std::vector<size_t> open_locs;                                  //Keeps track of indices in all_graphs that are null
    Node** node_ilocs;                                              //Keeps track of the location of each node in the window interface
    sf::VertexArray GUIlinks;                            //Lines used to represent links between nodes on the interface
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
        std::cout << "interface max size: " << i_size << std::endl;

        //set the primitive type for the interface links
        GUIlinks.setPrimitiveType(sf::Lines);
    };

    //creates a new link represented as a ve
    inline void addNewLink(sf::Vector2i p1, sf::Vector2i p2, sf::Color c){
        GUIlinks.append(sf::Vertex(sf::Vector2f(p1), c));
        GUIlinks.append(sf::Vertex(sf::Vector2f(p2), c));
    }

    inline void drawAllLinks(sf::RenderWindow* win){
        win->draw(GUIlinks);
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
    inline size_t getNodeGraphsPos(ull ident){
        if (node_locs.count(ident))
            return node_locs[ident];
        else
            return -1;
    }

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

    inline Node** getNodeIlocs(){
        return node_ilocs;
    }

    //Runs a depth first search on the graph to find node with the given identifier (Returns NULL if node was not in the graph)
    Node* graphDFS(Node* curr, ull ident, std::unordered_set<ull>& visited){
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
    Node* findNode(size_t graph_index, ull ident){
        std::unordered_set<ull> visited;
        //write a map that determines which index in all_graphs the indentifier belongs to
        return graphDFS(all_graphs[graph_index], ident, visited);
    }

    //runs DFS to find node with given identifier
    //REVISED: you don't have to tell which all_graphs index to look into
    Node* REVISEDfindNode(ull ident){
        if (!node_locs.count(ident)){
            std::cout << "ERROR in graph.REVISEDfindNode - Node location not recorded in map - EXITING\n";
            exit(EXIT_FAILURE);
        }
        
        Node* findNode = all_graphs[node_locs[ident]];
        if (findNode == NULL){
            std::cout << "REVISEDfindNode: Error - node to find is not in all_graphs\n";
            std::cout << "\t Searching for node: " << ident;
            std::cout << "\n\t Searched for index: " << node_locs[ident] << " in all_graphs - EXITING\n";
            exit(EXIT_FAILURE);
        }

        std::unordered_set<ull> visited;
        Node* ret = graphDFS(findNode, ident, visited);
        if (ret == NULL) std::cout << "DFS returned null\n";
        return ret;
    }

    //creates a new individual node and implants it as its own graph
    void createNewNode(){
        ull ident = getNewNodeIdent();
        size_t openidx = -1;
        for (size_t i=0; i < all_graphs.size(); ++i){
            if (all_graphs[i] == NULL){
                openidx = i;
                break;
            }
        }

        //if there are no open spaces in all_graphs append the new node
        //otherwise append in found open spot
        if (openidx == -1){
            all_graphs.push_back(new Node(ident));
            node_locs.emplace(ident, all_graphs.size()-1);
        }else{
            all_graphs[openidx] = new Node(ident);
            node_locs.emplace(ident, openidx);
        }
        num_graphs++;
    }

    //creates a new individual node and implants it as its own graph at position pos
    void createNewNode(sf::Vector2i pos, sf::Font& font){
        size_t iloc = pos.y*win_width + pos.x;
        ull ident = getNewNodeIdent();
        int openidx = -1;
        //search for open index in all_graphs
        for (size_t i=0; i < all_graphs.size(); ++i){
            if (all_graphs[i] == NULL){
                openidx = i;
                break;
            }
        }

        //if there are no open spaces in all_graphs append the new node
        //otherwise append in found open spot
        Node* nn = new Node(ident, sf::Vector2f(pos), font);
        if (openidx == -1){
            all_graphs.push_back(nn);
            node_locs.emplace(ident, all_graphs.size()-1);
        }else{
            all_graphs[openidx] = nn;
            node_locs.emplace(ident, openidx);
        }
        
        //store location of node in the interface
        if (node_ilocs[iloc] == NULL){
            node_ilocs[iloc] = nn;
            std::cout << "\tGraph added at loc: " << iloc << " x: " << pos.x << " y: " << pos.y << '\n';
        }else{
            std::cout << "\tadding to iloc: " << iloc << " is not null - EXITING\n";
            exit(EXIT_FAILURE);
        }
        std::cout << "\tnode ident: " << ident << '\n';
        num_graphs++;
    }

    //takes an existing node an appends it as a new graph
    void existingNodeToIndependentGraph(Node* node){
        all_graphs.push_back(node);
        std::cout << "Pushing Node: " << node->getNodeIdent() << " - vec size: " << all_graphs.size();
        node_locs[node->getNodeIdent()] = all_graphs.size()-1;
        std::cout << " node is located at index: " << node_locs[node->getNodeIdent()] << '\n';
        num_graphs++;
    }

    //runs DFS and changes each identifier's to the new_loc in all_graphs
    //Arguments(new all_graphs location, graph head to move, set to keep track of visited node)
    //doesnt update num_graphs after move
    void moveGraphLoc(size_t new_loc, Node* curr, std::unordered_set<ull>& visited){
        //mark current node as visited
        visited.insert(curr->getNodeIdent());
        //move current node to the new all_graph locs
        node_locs[curr->getNodeIdent()] = new_loc;
        //get current node's links list
        std::vector<ADJ_NODE> links = curr->getNodeLinks();


        //check each node in currents links 
        for (size_t i = 0; i < links.size(); ++i){
            Node* inspect = std::get<0>(links[i]);
            if (!visited.count(inspect->getNodeIdent())){
                moveGraphLoc(new_loc, inspect, visited);
            }
        }
    }

    //runs DFS and changes nodes identifier's to the new_loc in all_graph
    void moveGraphLoconDelete(size_t new_loc, Node* curr, std::unordered_set<ull>& visited, std::unordered_map<ull, bool>& nodechanged){
        //mark current node as visited
        visited.insert(curr->getNodeIdent());
        //move current node to the new all_graph locs
        node_locs[curr->getNodeIdent()] = new_loc;
        //get current node's links list
        std::vector<ADJ_NODE> links = curr->getNodeLinks();

        //check each node in currents links 
        for (size_t i = 0; i < links.size(); ++i){
            Node* inspect = std::get<0>(links[i]);
            if (!visited.count(inspect->getNodeIdent())){
                //mark the node as changed graph if it was a node that was severed from the NTD
                if (nodechanged.count(inspect->getNodeIdent())) 
                    nodechanged[inspect->getNodeIdent()] = 1;
                
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
    void joinNodes(ull ident1, ull ident2, size_t link_weight){
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
                std::unordered_set<ull> visited;
                //update node_locs map for the graph thats getting moved
                moveGraphLoc(loc1, all_graphs[loc2], visited);
                num_graphs--;
            }

            //Check to see if nodes are connected
            int alreadyConnected = 0;
            std::vector<ADJ_NODE> links = n1->getNodeLinks();
            for (size_t i =0; i < links.size(); ++i){
                Node* check = std::get<0>(links[i]);
                if (check->getNodeIdent() == n2->getNodeIdent()){
                    alreadyConnected = 1;
                    break;
                }
            }

            //connect the two nodes together if not already conencted
            if (!alreadyConnected){
                ull link_ident = getNewLinkIdent();
                n1->addNode(n2, link_weight, link_ident);
                n2->addNode(n1, link_weight, link_ident);
            }

            //remove graph getting moved from all_graphs if locations are different
            //record open position in open_locs
            if ((loc1 != loc2) && !alreadyConnected){
                all_graphs[loc2] = NULL;
                open_locs.push_back(loc2);
            }
            
        }

    }

    //unjoins two nodes
    void unJoinNodes(){

    }

    //given a node identifier, will delete that node and fix the (possibly) broken graph structure
    void REVISEDdeleteNode(ull NTDident){
        //Safety check to see if NTDidentifier should be in all_graphs
        if (!node_locs.count(NTDident)){
            std::cout << "ERROR in graph.deleteNodes: one of the nodes has not been created or appended into node_locs EXITING\n";
            exit(EXIT_FAILURE);
        }
        
        Node* NTD = REVISEDfindNode(NTDident);
        sf::Vector2i npos= sf::Vector2i(NTD->getNodePos());
        size_t ipos = win_width*npos.y + npos.x;

        //Try and check to see if Node to delete (NTD) was found
        if (!NTD){
            std::cout << "ERROR in graph.deleteNodes: node to be removed not found EXITING\n";
            exit(EXIT_FAILURE);
        }


        size_t NTDloc = node_locs[NTDident];                 //get NTD all_graphs location
        std::vector<ADJ_NODE> links = NTD->getNodeLinks();         //get NTD's node links
        
        //keeps track of the nodes identifier whose link with NTD was severed
        std::unordered_map<ull, bool> nodechanged;

        //search and remove link from child to NTD
        //save child node into map to update graph pos later
        for (size_t i=0; i < links.size(); ++i){
            Node* child = std::get<0>(links[i]);
            //remove the link to NTD from child
            child->remLinktoNode(NTDident);
            
            //save the child node in the map
            nodechanged[child->getNodeIdent()] = 0;
        }


        //update NTD's childrens graph positions
        for (size_t i=0; i < links.size(); ++i){
            //Attempt to change a node's graph position if it hasn't been changed by a previous node
            Node* child = std::get<0>(links[i]);
            if (!nodechanged[child->getNodeIdent()]){
                
                //determine first open position to move the graph to
                int newpos = all_graphs.size();
                if (open_locs.size() > 0){
                    newpos = open_locs[0];
                    open_locs.erase(open_locs.begin());
                }

                std::cout << "DELETEnode: \n\tCreating new graph for node: " << child->getNodeIdent() << " curr loc: " << node_locs[child->getNodeIdent()] << '\n';
                std::cout << "\tCurr graphs: " << num_graphs << " - node going to index: " << newpos << std::endl;
                std::unordered_set<ull> visited;
                //change the nodes in the graph to the new position
                moveGraphLoconDelete(newpos, child, visited, nodechanged);

                //make current child the head of the new graph
                //all_graphs.push_back(child);
                //move child head node to previously null location
                all_graphs[newpos] = child;
                num_graphs++;

                //ERROR CHECKING BELOW
                std::cout <<"\tNode: " << child->getNodeIdent() << " now located at graph: " << node_locs[child->getNodeIdent()] << "\n";
                Node * s = all_graphs[node_locs[child->getNodeIdent()]];
                if (s == NULL){
                    std::cout << "\tNode: " << child->getNodeIdent() << " not found as head of graph\n";
                }else{
                    std::cout << "\tNode: " << child->getNodeIdent() << " shud equal head: " << s->getNodeIdent() << "\n";
                }
            }
        }

        //remove NTD data from node_locs and all_graphs, decrement total number of graphs
        node_locs.erase(NTDident);
        delete NTD;
        all_graphs[NTDloc] = NULL;
        node_ilocs[ipos] = NULL;
        num_graphs--;

        //record NTDloc as open
        open_locs.push_back(NTDloc);
    }

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
        ADJ_NODE& change = l1[idx2];
        std::get<1>(change) = lw;
        l1[idx2] = change;

        //update link weight for n2
        change = l2[idx1];
        std::get<1>(change) = lw;
        l2[idx1] = change;
    }

    //updates the link connection weight between two nodes by identifier to a given link weight
    void updateNodeLink(ull ident1, ull ident2, size_t lw){
        std::cout << "UpdateNodeLin:\n\tSearching n1\n";
        Node* n1 = REVISEDfindNode(ident1);
        std::cout << "\tSearching n2\n";
        Node* n2 = REVISEDfindNode(ident2);
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
    void displayLinkWeight(ull ident1, ull ident2){
        Node* n1 = REVISEDfindNode(ident1);
        Node* n2 = REVISEDfindNode(ident2);

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
        curr = nullptr;
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
        /* std::cout << "Freeing all nodes\n";
        for (size_t i=0; i < win_width*win_height; ++i){
            if (node_ilocs[i] != nullptr){
                    std::cout << "\tFreeing node: " << node_ilocs[i]->getNodeIdent() << '\n';
                    delete node_ilocs[i];
                    node_ilocs[i] = NULL;
            }
        } */

        for (size_t i=0; i < win_width*win_height; ++i)
            delete node_ilocs[i];
        delete [] node_ilocs;
    }
};