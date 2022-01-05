/*
gui.hpp
    - managing the viewer
    -  Dijkstra alogrithm and the drawing functionalities
*/
#pragma once
#include "algo.hpp"

class Gui{
private:
    Graph* allgraphs;
    sf::Font font;
    size_t win_width, win_height;
public:
    Gui(const int& w_width, const int& w_height){
        //try to load in font needed to display text
        if (!font.loadFromFile("./Dijkstras/OpenSans-Semibold.ttf")){
            std::cerr << "GUI CONSTRUCTOR - Error while loading font - EXITING\n";
            exit(EXIT_FAILURE);
        }

        win_width = w_width;
        win_height = w_height;
        allgraphs = new Graph(win_width, win_height);
    }

    //returns the euclidean distance between two integer points
    inline size_t euclidDist(const sf::Vector2i& p1, const sf::Vector2i& p2){
        return sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
    }

    //check if the mouse position is within a node boundary and the radius distance
    bool withinBoundary(Node* n, const sf::Vector2i& mpos, size_t node_radius){
        sf::Vector2i npos = sf::Vector2i(n->getNodePos());

        int node_xs = npos.x-node_radius, node_xe = npos.x+node_radius;
        int node_ys = npos.y-node_radius, node_ye = npos.y+node_radius;
        if (node_xs < 0) node_xs = 0;
        if (node_ys < 0) node_ys = 0;
        if (node_xe >= win_width) node_xe = win_width-1;
        if (node_ye >= win_height) node_ye = win_height-1;

        if (euclidDist(npos, mpos) <= node_radius && 
            ((node_xs <= mpos.x) && (mpos.x <= node_xe)) && 
            ((node_ys <= mpos.y) && (mpos.y <= node_ye))){
            return true;
        }
        return false;
    }

    //checks a reduced range based on the mouse position to determine if the mouse is over a node
    Node* mouseOverNode(const sf::RenderWindow* win, size_t node_radius){
        Node** iloc = allgraphs->getNodeIlocs();
        sf::Vector2i mpos = sf::Mouse::getPosition(*win);

        //determine if mouse is over valid range
        if (mpos.x >= 0 && mpos.y >=0){
            //determine where to start looking for the node
            int ys = mpos.y-node_radius, ye=mpos.y+node_radius;
            int xs = mpos.x-node_radius, xe=mpos.x+node_radius;
            if (ys < 0) ys = 0;
            if (xs < 0) xs = 0;
            if (xe >= win_width) xe = win_width-1;
            if (ye >= win_height) ye = win_height-1;
            
            std::cout << "1 - Searched\n" << "\tyStart: " << ys << " yEnd: " << ye << "\n\txStart: " << xs << " xEnd: " << xe << '\n';
            
            //search the interface reduced range
            for (int y = ys; y <= ye; ++y){
                for (int x = xs; x <= xe; ++x){
                    int idx = win_width*y + x;
                    if (iloc[idx] != NULL && withinBoundary(iloc[idx], mpos, node_radius))
                        return iloc[idx];
                }
            }
        }

        return NULL;
    }

    //determines if a node was clicked on by the left button and finds it
    void onNodeLeftClick(const sf::RenderWindow* win){
        Node* mouse_on_node = mouseOverNode(win, NODE_RADIUS);
        if (mouse_on_node){
            std::cout << "Currently over node: " << mouse_on_node->getNodeIdent() <<'\n';
        }else{
            std::cout << "Not over node\n";
        }
    }

    //updates the node position the left mouse button is pressing on to a dragged postion
    void onDragNode(const sf::RenderWindow* win, Node* mouse_on_node, sf::Vector2i& prev_mpos, bool& dragging){
        if (mouse_on_node && dragging){
                sf::Vector2i curr_mpos = sf::Mouse::getPosition(*win);
                //std::cout << "\tcurr pos: (" << curr_mpos.x << ", " << curr_mpos.y << ")\tprev pos: (" << prev_mpos.x << ", " << prev_mpos.y << ")\n";
                if (curr_mpos != prev_mpos){
                    sf::Vector2i move_offset = curr_mpos - prev_mpos;
                    std::cout << "\tOFFSET x: " << move_offset.x << " y: " << move_offset.y << '\n';
                    //mouse_on_node->moveNode(move_offset + sf::Vector2i(mouse_on_node->getNodePos()));
                    prev_mpos = curr_mpos;
                }
        }
    }
    
    //attempts to create a new node displayed on the interface
    void addNode(const sf::RenderWindow* win){
        //adds node if new node doesn't overlap with existing nodes
        if (!mouseOverNode(win, NODE_RADIUS*2)){
            sf::Vector2i pos = sf::Mouse::getPosition(*win);
            std::cout << "2 - node created at position: " << win_width*pos.y + pos.x << " x: " << pos.x << " y: " << pos.y << '\n';
            allgraphs->createNewNode(pos, font);
        }
    }

    void removeNode(const sf::RenderWindow* win){
        Node* NTD = mouseOverNode(win, NODE_RADIUS*2);
        if (NTD){
            std::cout << "3 - Deleting node: " << NTD->getNodeIdent() << "\n";
            allgraphs->REVISEDdeleteNode(NTD);
        }
    }

    //attempt to link two not Null and different nodes with each other
    void linkNodes(Node* n1, Node* n2){
        //check that nodes being joined aren't the same
        if (n2 != NULL && n1 != NULL && n1 != n2){
            //have gui ask for node link weight
            int link_weight;
            allgraphs->joinNodes(n1, n2, 20);
        }
    }
    
    //renders all elements of all the graphs
    void renderAllGraphs(sf::RenderWindow* win){
        win->clear();

        allgraphs->drawAllLinks(win);
        size_t ags = allgraphs->getAllGraphSize();
        for (size_t i=0; i < ags; ++i){
            allgraphs->drawAllNodesinGraph(i, win);
        }


        win->display();
    }

    //clears the entire screen of nodes and links
    void clearScreen(){
        delete allgraphs;
        allgraphs = new Graph(win_width, win_height);
    }

    ~Gui(){
        delete allgraphs;
    }
};  