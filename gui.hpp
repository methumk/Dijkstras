/*
gui.hpp
    - managing the viewer
    -  Dijkstra alogrithm and the drawing functionalities
*/
#pragma once
#include <SFML/Graphics.hpp>
#include "algo.hpp"
#include <string>

#define REM_SHADOW_COLOR sf::Color::Red
#define SIMUL_STATE_DISPLAY_COLOR sf::Color(255, 156, 18)

class Gui{
    private:
        Graph* graphMan;
        Algos algoMan;
        sf::Font nodeFont;
        sf::Font simulStateFont;
        sf::Text simulStateDisplay;
        size_t win_width, win_height;
        std::vector<sf::Vertex> shadowLink;
        std::vector<sf::Vertex> shadowRemoveLink;
        std::vector<sf::Vertex> tempLink;

        //sf::Vertex* shadowLink[2];
        
        // NOTE: currently just supporting positive weights
        bool isNumber(const std::string& str)
        {
            for (int i=0; i < str.size(); ++i) {
                // if (i==0 && str[i] == '-'){
                //     continue;
                // }
                if (std::isdigit(str[i]) == 0){
                    return false;
                }
            }
            return true;
        }

    public:

        Gui(const int& w_width, const int& w_height, const std::string simulStateText){
            //try to load in font needed to display text
            if (!nodeFont.loadFromFile("./Dijkstras/open-sans/OpenSans-Semibold.ttf")){
                std::cerr << "GUI CONSTRUCTOR - Error while loading node font - EXITING\n";
                exit(EXIT_FAILURE);
            }

            win_width = w_width;
            win_height = w_height;
            graphMan = new Graph(win_width, win_height);

            if (!simulStateFont.loadFromFile("./Dijkstras/Mollen/Mollen-Bold.otf")){
                std::cerr << "GUI CONSTRUCTOR - Error while loading simul state font - EXITING\n";
                exit(EXIT_FAILURE);
            }
            simulStateDisplay.setFont(simulStateFont);
            simulStateDisplay.setString(simulStateText);
            simulStateDisplay.setCharacterSize(20);
            // simulStateDisplay.getLocalBounds().width/2., simulStateDisplay.getLocalBounds().height/2.
            simulStateDisplay.setOrigin(simulStateDisplay.getLocalBounds().width, 0);
            simulStateDisplay.setPosition(sf::Vector2f(win_width, 10));
            simulStateDisplay.setFillColor(SIMUL_STATE_DISPLAY_COLOR);
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
            Node** iloc = graphMan->getNodeIlocs();
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
                
                //std::cout << "1 - Searched\n" << "\tyStart: " << ys << " yEnd: " << ye << "\n\txStart: " << xs << " xEnd: " << xe << '\n';
                
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

        void checkNodeExist(Node* n){
            //This function is literally useless
            if (n){
                Node* x = graphMan->findNode(n->getNodeIdent());
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
            if (!mouseOverNode(win, NODE_RADIUS*4)){
                sf::Vector2i pos = sf::Mouse::getPosition(*win);
                std::cout << "2 - node created at position: " << win_width*pos.y + pos.x << " x: " << pos.x << " y: " << pos.y << '\n';
                graphMan->createNewNode(pos, nodeFont);
            }
        }

        //deletes the node
        void removeNode(const sf::RenderWindow* win){
            Node* NTD = mouseOverNode(win, NODE_RADIUS);
            if (NTD){
                std::cout << "3 - Deleting node: " << NTD->getNodeIdent() << "\n";
                graphMan->deleteNode(NTD);
            }
        }

        //unlink two linked nodes
        void removeLink(Node* n1, Node* n2){
            //check that nodes exist and aren't the same
            if (n2 != NULL && n1 != NULL && n1 != n2){
                std::cout << "UNLINKING NODES\n";
                graphMan->unJoinNodes(n1, n2);
            }
        }

        //attempt to link two not Null and different nodes with each other
        void linkNodes(Node* n1, Node* n2, const LinkStat& lstate, bool& textInputting){
            //check that nodes being joined aren't the same
            if (n2 != NULL && n1 != NULL && n1 != n2 && !textInputting){
                //have gui ask for node link weight

                //std::string title = "Set link weight for nodes" + std::to_string(n1->getNodeIdent()) + " and " + std::to_string(n2->getNodeIdent());
                graphMan->joinNodes(n1, n2, 20, lstate);
                textInputting = true;

                std::cout << "TEXT INPUTTING TRUE!!!\n";
                // sf::Thread linkNodesThread(std::bind(&renderLinkWeightBox, nodeInputting));
                // linkNodesThread.launch();
                // renderLinkWeightBox(nodeInputting);
            }
        }




        /* 
            Shadow Link
                - Displays the link the user is trying to make between two nodes
        */

        // Creates a vertex point to show a shadow link
        void setShadowLink(Node* n1, const LinkStat& lstate){
            if (n1){
                sf::Color lcolor;
                if (lstate == LinkStat::Doubly) lcolor = DOUBLY_COLOR;
                else                            lcolor = SINGLY_COLOR;

                shadowLink.push_back(sf::Vertex(n1->getNodePos(), lcolor));
                shadowLink.push_back(sf::Vertex(n1->getNodePos(), lcolor));
                
            }
        }

        // Moves the shadow link end point to show where the user is trying to link
        void moveShadowLink(Node* n1, const sf::RenderWindow* win, const LinkStat& lstate){
            if (n1){
                sf::Vector2f pos = sf::Vector2f(sf::Mouse::getPosition(*win));
                
                sf::Color lcolor;
                if (lstate == LinkStat::Doubly) lcolor = DOUBLY_COLOR;
                else                            lcolor = SINGLY_COLOR;

                if (shadowLink.size() == 2){
                    shadowLink[1] = sf::Vertex(pos, lcolor);
                }
            }
        }

        // resets the shadow link
        inline void resetShadowLink(){
            shadowLink.clear();
        }

        //Creates a shadow link to help the user delete a link between two nodes
        void setShadowRemoveLink(Node* n1){
            if (n1){
                shadowRemoveLink.push_back(sf::Vertex(n1->getNodePos(), REM_SHADOW_COLOR));
                shadowRemoveLink.push_back(sf::Vertex(n1->getNodePos(), REM_SHADOW_COLOR));
            }
        }

        //moves the shadow link to point to where user is pointing to
        void moveShadowRemoveLink(Node* n1, const sf::RenderWindow* win){
            if (n1){
                sf::Vector2f pos = sf::Vector2f(sf::Mouse::getPosition(*win));
                if (shadowRemoveLink.size() == 2){
                    shadowRemoveLink[1] = sf::Vertex(pos, REM_SHADOW_COLOR);
                }
                    
            }
        }

        // Creates a temporary link between two nodes without a weight (used before getting user input for link weight)
        void setTempLink(Node* n1, Node* n2, const LinkStat& lstate){
            sf::Color lcolor;
            if (lstate == LinkStat::Doubly) lcolor = DOUBLY_COLOR;
            else                            lcolor = SINGLY_COLOR;

            tempLink.push_back(sf::Vertex(n1->getNodePos(), lcolor));
            tempLink.push_back(sf::Vertex(n2->getNodePos(), lcolor));
        }

        // resets the shadow link
        inline void resetTempLink(){
            tempLink.clear();
        }

        inline void resetShadowRemoveLink(){
            shadowRemoveLink.clear();
        }
        



        /* 
            Rendering
                - render graph viewer
                - render graphs
        */
        void drawIMGraphViewer(){
            ImGui::Begin("Graph Viewer");
            graphMan->drawGraphViewer();
            ImGui::End();
        }


        void renderLinks(sf::RenderWindow* win){
            if (shadowLink.size() == 2){
                win->draw(shadowLink.data(), shadowLink.size(), sf::Lines);
            }
            if (shadowRemoveLink.size() == 2){
                win->draw(shadowRemoveLink.data(), shadowRemoveLink.size(), sf::Lines);
            }
            if (tempLink.size() == 2){
                win->draw(tempLink.data(), tempLink.size(), sf::Lines);
            }
        }

        //renders all elements of all the graphs
        void renderGraphs(sf::RenderWindow* win){
            graphMan->drawAllLinks(win);
            size_t ags = graphMan->getAllGraphSize();
            for (size_t i=0; i < ags; ++i){
                graphMan->drawAllNodesinGraph(i, win);
            }
        }

        void renderLinkWeightBox(Node* n1, Node* n2, const LinkStat& lstate, bool& textInputting, bool& checkLinking){
            if (checkLinking){
                std::cout << "TEXTINPUTTING: " << textInputting << "\n";
                if (n1) std::cout << "N1 ID: " << n1->getNodeIdent() << "'\t";
                else std::cout << "N1 NULL;\t";
                if (n2) std::cout << "N2 ID: " << n2->getNodeIdent() << "'\t";
                else std::cout << "N2 NULL;\t";
                std::cout << "n2 == n1?: " << (n2 == n1) << "\n\n";
                
                if (n2 != NULL && n1 != NULL && n1 != n2 && textInputting){
                    // textInputting = true;
                    if (tempLink.size() == 0){
                        setTempLink(n1, n2, lstate);
                    }
                }else{
                    std::cout << "EXITING INPUTTING\n";
                    checkLinking = false;
                    textInputting = false;
                }
                 resetShadowLink();

                if (textInputting){
                    char inputNode[256];
                    memset(inputNode, '\0', 256);
                    std::cout << "OPENING GUI\n";
                    
                    if (ImGui::Begin("Enter Link's Weight")){
                        if (ImGui::InputText("Enter Weight", inputNode, 255, ImGuiInputTextFlags_EnterReturnsTrue) ){
                            if (isNumber(inputNode)){
                                std::string input(inputNode);
                                ll weight = std::stoi(input);
                                std::cout << "ENTERED WEIGHT: " << weight << "\n";
                                graphMan->joinNodes(n1, n2, weight, lstate);
                                resetTempLink();
                                textInputting = false;
                                checkLinking = false;
                            }
                        }
                        ImGui::End();
                    }
                    
                    
                }
            }
        }


        void drawIMAlgoMenu(){
            ImGui::Begin("Run Algorithms");
            algoMan.displayAlgosMenu();
            ImGui::End();
        }

        void drawSimulStateIndicator(sf::RenderWindow* win, const std::string stateText){
            simulStateDisplay.setString(stateText);
            simulStateDisplay.setOrigin(simulStateDisplay.getLocalBounds().width, 0);
            simulStateDisplay.setPosition(sf::Vector2f(win_width-10, 10));
            win->draw(simulStateDisplay);
        }

        void runAlgoFromMenu(){
            algoMan.runFromAlgoMenu(graphMan);
        }

        //clears the entire screen of nodes and links
        void clearScreen(){
            delete graphMan;
            graphMan = new Graph(win_width, win_height);
        }

        ~Gui(){
            delete graphMan;
        }
};  