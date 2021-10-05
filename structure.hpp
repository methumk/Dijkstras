/*
structure.hpp
    - 
*/
#include <SFML/Graphics.hpp>
#include <vector>
#include <tuple>


/*
Node class:
    - Node identifier
    - Circle shape??
    - links (tuple: pointed node, link weight, link identifier)
*/
class Node{
private:
    typedef std::tuple<Node*, unsigned int, double> ADJ_NODE;
    typedef std::vector<ADJ_NODE> NODE_LIST;

    double ident;
    NODE_LIST links;
    sf::CircleShape GUInode;
public:
    Node(){}
    Node(double i): ident(i){}

    void addNodeShape(float radius, sf::Vector2f& pos, sf::Color color){
        GUInode.setRadius(radius);
        GUInode.setOrigin(radius, radius);
        GUInode.setPosition(pos);
        GUInode.setFillColor(color);
        GUInode.setOutlineColor(sf::Color::Blue);
        GUInode.setOutlineThickness(2.f);
    }

    //add a new linked up node
    void addNode(Node* addNode, unsigned int lw, double li){
        links.push_back(std::make_tuple(addNode, lw, li));
    }

    void addNode(ADJ_NODE addNode){
        links.push_back(addNode);
    }

    void printCurrNode(){
        std::cout << "Node ident: " << ident << '\n';
    }

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


    ADJ_NODE getNodeInfo(unsigned int i){
        return links[i];
    }

    //have to do dfs on graph to find correct identifier
    ADJ_NODE getNodeInfo(double i){
        return links[i];
    }

    Node* getAdjNode(unsigned int i){
        return std::get<0>(links[i]);
    }

    //have to do dfs on graph to find correct identifier
    Node* getAdjNode(double ident){
        return NULL;
    }
};


/*
Graph class:
    - vector of all UNJOINED graphs (holds the head of each graph)
    - if you want to iterate through an individual graph you need to check for already visited nodes (graph traversal)
    - Merging graphs and updating the all_graphs vector
*/
class Graph{
private:
    std::vector<Node*> all_graphs;
    double curr_node_ident;
    double curr_link_ident;

public:
    Graph(): curr_node_ident(0), curr_link_ident(0){};

    inline double getCurrNodeIdent(){
        return curr_node_ident++;
    }

    inline double getCurrLinkIdent(){
        return curr_link_ident++;
    }

    inline void addGraph(Node* n){
        all_graphs.push_back(n);
    }
};