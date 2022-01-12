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
#include <cmath>

#include "links.hpp"

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
    typedef std::tuple<Node*, size_t, ull, bool> ADJ_NODE2;
    typedef std::vector<ADJ_NODE> NODE_VEC;
private:
    ull ident;                              //Nodes identifying number
    std::vector<ADJ_NODE> links;            //The nodes connections: vector of tuples<node, link weight, node identifier>
    sf::CircleShape GUInode;                //Circle used to represent node on the interface
    sf::Text textId;                            //Interface text used to identify each node
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
        textId.setFont(font);
        textId.setString(std::to_string(ident));
        textId.setCharacterSize(18);
        textId.setOrigin(textId.getLocalBounds().width/2., textId.getLocalBounds().height/2.);
        textId.setPosition(pos);
        textId.setFillColor(sf::Color::Red);  
    }   

    //renders individual node and text
    void drawNode(sf::RenderWindow* win){
        win->draw(GUInode);       
        win->draw(textId); 
    }

    //moves the gui node to a given position
    inline void setNodePos(sf::Vector2i pos){
        GUInode.setPosition(sf::Vector2f(pos));
        textId.setPosition(sf::Vector2f(pos));
    }

    //returns the position of the node
    inline sf::Vector2f getNodePos(){
        return GUInode.getPosition();
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

    //checks whether a node is singly linked to another node
    //returns true if the from node is connected to the to node
    //returns false otherwise
    bool isSinglyLinked(Node* from_n, Node* to_n){
        std::vector<ADJ_NODE> links = from_n->getNodeLinks();
        size_t link_idx = 0;
        for (; link_idx < links.size(); ++link_idx){
            Node* inspect = std::get<0>(links[link_idx]);
            if (inspect->ident == to_n->ident)
                return true;
                //replace above with below 
                //return std::get<3>(links[link_idx]);
        }

    }

    //add a new linked up node manually
    inline void addLinktoNode(Node* addNode, size_t lw, ull li){
        links.push_back(std::make_tuple(addNode, lw, li));
    }

    //add a new linked up node by tuple
    inline void addLinktoNode(ADJ_NODE addNode){
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
};