/*
gui.hpp
    - managing the viewer
    -  Dijkstra alogrithm and the drawing functionalities
*/
#pragma once
#include "algo.hpp"
#include <cmath>

class Gui{
private:
    Graph* allgraphs;
    sf::Font font;
    size_t win_width, win_height;
public:
    Gui(){
        //try to load in font needed to display text
        if (!font.loadFromFile("./Dijkstras/OpenSans-Semibold.ttf")){
            std::cerr << "GUI CONSTRUCTOR - Error while loading font - EXITING\n";
            exit(EXIT_FAILURE);
        }

        allgraphs = new Graph;
    }

    Gui(const sf::RenderWindow* win){
        //try to load in font needed to display text
        if (!font.loadFromFile("./Dijkstras/OpenSans-Semibold.ttf")){
            std::cerr << "GUI CONSTRUCTOR - Error while loading font - EXITING\n";
            exit(EXIT_FAILURE);
        }

        allgraphs = new Graph(win);
        win_width = win->getSize().x;
        win_height = win->getSize().y;
    }

    //creates a new node
    void addNode(const sf::RenderWindow* win){
        allgraphs->createNewNode(sf::Mouse::getPosition(*win), font);
    }

    //returns the euclidean distance between two integer points
    inline size_t euclidDist(const sf::Vector2i& p1, const sf::Vector2i& p2){
        return sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
    }

    //check if the mouse position is within a node boundary
    bool withinBoundary(Node* n, const sf::Vector2i& mpos){
        sf::Vector2i npos = sf::Vector2i(n->getNodePos());
        if (euclidDist(npos, mpos) <= NODE_RADIUS && 
            ((npos.x+NODE_RADIUS >= mpos.x) && (npos.x-NODE_RADIUS <= mpos.x)) && 
            ((npos.y+NODE_RADIUS >= mpos.y) && (npos.y-NODE_RADIUS <= mpos.y))){
            
            return true;
        }
        return false;
    }

    //checks a reduced range based on the mouse position to determine if the mouse is over a node
    Node* mouseOverNode(const sf::RenderWindow* win){
        Node** iloc = allgraphs->getNodeIlocs();
        sf::Vector2i mpos = sf::Mouse::getPosition(*win);

        //determine where to start looking for the node
        int ys = mpos.y-NODE_RADIUS, ye=mpos.y+NODE_RADIUS;
        int xs = mpos.x-NODE_RADIUS, xe=mpos.x+NODE_RADIUS;
        if (ys < 0) ys = 0;
        if (xs < 0) xs = 0;
        if (xe >= win_width) xe = win_width-1;
        if (ye >= win_height) ye = win_height-1;
        
        //determine if mouse is over valid range
        if (mpos.x >= 0 && mpos.y >=0){
            //search the interface reduced range
            for (int y = ys; y <= ye; ++y){
                for (int x = xs; x <= xe; ++x){
                    int idx = x + win_width*y;
                    if (iloc[idx] != NULL && withinBoundary(iloc[idx], mpos))
                        return iloc[idx];
                }
            }
        }

        return NULL;
    }

    //determines if a node was clicked on by the left button and finds it
    void onNodeLeftClick(const sf::RenderWindow* win){
        Node* mouse_on_node = mouseOverNode(win);
        if (mouse_on_node){
            std::cout << "Currently over node: " << mouse_on_node->getNodeIdent() <<'\n';
        }else{
            std::cout << "Not over node\n";
        }
    }

    void renderAllGraphs(sf::RenderWindow* win){
        win->clear();
        unsigned int ags = allgraphs->getAllGraphSize();
        for (unsigned int i=0; i < ags; ++i){
            Node* gh = allgraphs->getGraphHead(i);
            if (gh != NULL)
                gh->renderNode(win);
        }
        win->display();
    }

    //clears the entire screen of nodes and links
    void clearScreen(){
        //allgraphs->eraseAllGraphs();
        delete allgraphs;
        allgraphs = new Graph;
    }

    ~Gui(){
        delete allgraphs;
    }
};  