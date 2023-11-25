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
#define CONTROL_BORDER_COLOR sf::Color(105, 105, 150)

//Created seperate enums for removing nodes and links (don't know if this will be the case in the future)
enum class SimulState {AddNodeMode, AddLinkMode, RemoveNodeMode, RemoveLinkMode, SelectNodeMode, ViewMode};
const std::string simulStateDisplay[] = {"Adding Nodes", "Adding Links", "Removing Nodes", "Removing Links", "Selecting Algo Node", "View Only"};
const std::string simulStateLinkType[] = {"Double Link", "Single Link"};

class Gui{
    private:
        Graph* graphMan;
        Algos algoMan;
        sf::Font nodeFont;
        sf::Font simulStateFont;
        sf::Text simulStateDisplay;
        sf::Text simulStateLinkType;
        sf::RectangleShape controlBorder;
        size_t win_width, win_height;
        size_t simul_width, simul_height;
        std::vector<sf::Vertex> shadowLink;
        std::vector<sf::Vertex> shadowArrows;
        std::vector<sf::Vertex> shadowRemoveLink;
        std::vector<sf::Vertex> tempInputLink;
        std::vector<sf::Vertex> tempInputArrows;

        //sf::Vertex* shadowLink[2];
        
        // NOTE: currently just supporting positive weights
        bool isNumber(const std::string& str)
        {
            if (str == "") return false;
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

        Gui(const int& w_width, const int& w_height, const int& s_width, const int& s_height, const std::string simulStateText, const std::string simulLinkText){
            //try to load in font needed to display text
            if (!nodeFont.loadFromFile("./Dijkstras/fonts/open-sans/OpenSans-Semibold.ttf")){
                std::cerr << "GUI CONSTRUCTOR - Error while loading node font - EXITING\n";
                exit(EXIT_FAILURE);
            }

            win_width = w_width;
            win_height = w_height;
            simul_width = s_width;
            simul_height = s_height;
            graphMan = new Graph(win_width, win_height, simul_width, simul_height);

            if (!simulStateFont.loadFromFile("./Dijkstras/fonts/Mollen/Mollen-Bold.otf")){
                std::cerr << "GUI CONSTRUCTOR - Error while loading simul state font - EXITING\n";
                exit(EXIT_FAILURE);
            }
            simulStateDisplay.setFont(simulStateFont);
            simulStateDisplay.setString(simulStateText);
            simulStateDisplay.setCharacterSize(20);
            // simulStateDisplay.getLocalBounds().width/2., simulStateDisplay.getLocalBounds().height/2.
            simulStateDisplay.setOrigin(simulStateDisplay.getLocalBounds().width, 0);
            simulStateDisplay.setPosition(sf::Vector2f(s_width, 10));
            simulStateDisplay.setFillColor(SIMUL_STATE_DISPLAY_COLOR);

            simulStateLinkType.setFont(simulStateFont);
            simulStateLinkType.setString(simulLinkText);
            simulStateLinkType.setCharacterSize(15);
            simulStateLinkType.setOrigin(simulStateLinkType.getLocalBounds().width, 0);
            simulStateLinkType.setPosition(sf::Vector2f(win_width-10, simulStateDisplay.getLocalBounds().height+20));
            simulStateLinkType.setFillColor(simulLinkText == "Single Link"? SINGLY_COLOR : DOUBLY_COLOR);

            controlBorder.setSize(sf::Vector2f(w_width - s_width, s_height));
            controlBorder.setFillColor(CONTROL_BORDER_COLOR);
            controlBorder.setPosition(s_width, 0);

        }

        //returns the euclidean distance between two integer points
        inline size_t euclidDist(const sf::Vector2i& p1, const sf::Vector2i& p2){
            return sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
        }

        //check if the mouse position is within a node boundary and the radius distance
        bool withinBoundary(Node* n, const sf::Vector2i& mpos, size_t node_radius){
            if (n){
                sf::Vector2i npos = sf::Vector2i(n->getNodePos());

                int node_xs = npos.x-node_radius, node_xe = npos.x+node_radius;
                int node_ys = npos.y-node_radius, node_ye = npos.y+node_radius;
                if (node_xs < 0) node_xs = 0;
                if (node_ys < 0) node_ys = 0;
                if (node_xe >= simul_width) node_xe = simul_width-1;
                if (node_ye >= simul_height) node_ye = simul_height-1;

                if (euclidDist(npos, mpos) <= node_radius && 
                    ((node_xs <= mpos.x) && (mpos.x <= node_xe)) && 
                    ((node_ys <= mpos.y) && (mpos.y <= node_ye))){
                    return true;
                }
            }
            
            return false;
        }

        //checks a reduced range based on the mouse position to determine if the mouse is over a node
        Node* mouseOverNode(const sf::RenderWindow* win, size_t node_radius){
            // Node** iloc = graphMan->getNodeIlocs();
            sf::Vector2i mpos = sf::Mouse::getPosition(*win);

            //determine if mouse is over valid range
            // UPDATING
            if (mpos.x >= 0 && mpos.y >=0 && mpos.x <= simul_width && mpos.y <= simul_height){
                //determine where to start looking for the node
                int ys = mpos.y-node_radius, ye=mpos.y+node_radius;
                int xs = mpos.x-node_radius, xe=mpos.x+node_radius;
                if (ys < 0) ys = 0;
                if (xs < 0) xs = 0;
                if (xe >= simul_width) xe = simul_width-1;
                if (ye >= simul_height) ye = simul_height-1;
                
                // std::cout << "1 - Searched\n" << "\tyStart: " << ys << " yEnd: " << ye << "\n\txStart: " << xs << " xEnd: " << xe << '\n';
                
                //search the interface reduced range
                for (int y = ys; y <= ye; ++y){
                    for (int x = xs; x <= xe; ++x){
                        size_t idx = simul_width*y + x;
                        // UPDATING
                        // if (iloc[idx] != NULL && withinBoundary(iloc[idx], mpos, node_radius))
                        //     return iloc[idx];
                        Node* cpn = graphMan->getNodeAtPosition(idx);
                        // std::cout << "searching: " << idx << "\n";
                        if (cpn != NULL && withinBoundary(cpn, mpos, node_radius)){
                            return cpn;
                        }
                    }
                }
            }

            return NULL;
        }

        bool nodeWithinSimulBoundary(const sf::RenderWindow* win){
            sf::Vector2i mpos = sf::Mouse::getPosition(*win);
            if (mpos.x-NODE_RADIUS >= 0 && mpos.y-NODE_RADIUS >=0 && mpos.x+NODE_RADIUS <= simul_width && mpos.y+NODE_RADIUS <= simul_height){
                return true;
            }else{
                return false;
            }
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
            if (nodeWithinSimulBoundary(win) && !mouseOverNode(win, NODE_RADIUS*4)){
                sf::Vector2i pos = sf::Mouse::getPosition(*win);
                std::cout << "2 - node created at position: " << simul_width*pos.y + pos.x << " x: " << pos.x << " y: " << pos.y << '\n';
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

        // Set the selected node for the algo to use
        void selectNodeForAlgo(const sf::RenderWindow* win, size_t node_radius)
        {
            // TODO: WORKING
            algoMan.setSelectedAlgoNode(mouseOverNode(win, node_radius));
        }




        /* 
            Shadow Link
                - Displays the link the user is trying to make between two nodes
        */

        // Creates a vertex point to show a shadow link
        void setShadowLink(Node* n1, const LinkStat& lstate){
            if (n1){
                sf::Color lcolor;
                sf::Vector2f n1Pos = n1->getNodePos();
                if (lstate == LinkStat::Doubly){
                    lcolor = DOUBLY_COLOR;
                }else{
                    lcolor = SINGLY_COLOR;
                    sf::Vector2f ap1, ap2;
                    sf::Vector2f midpoint = lh::getLineMidpoint(n1Pos, n1Pos);
                    lh::getArrowPositions(n1Pos, n1Pos, midpoint, ap1, ap2);
                    shadowArrows.push_back(sf::Vertex(n1Pos, lcolor));
                    shadowArrows.push_back(sf::Vertex(midpoint, lcolor));
                    shadowArrows.push_back(sf::Vertex(n1Pos, lcolor));
                    shadowArrows.push_back(sf::Vertex(midpoint, lcolor));
                }  

                shadowLink.push_back(sf::Vertex(n1Pos, lcolor));
                shadowLink.push_back(sf::Vertex(n1Pos, lcolor));                          
                
            }
        }

        // Moves the shadow link end point to show where the user is trying to link
        void moveShadowLink(Node* n1, const sf::RenderWindow* win, const LinkStat& lstate){
            if (n1){
                sf::Vector2f n2Pos = sf::Vector2f(sf::Mouse::getPosition(*win));
                
                sf::Color lcolor;
                if (lstate == LinkStat::Doubly) lcolor = DOUBLY_COLOR;
                else                            lcolor = SINGLY_COLOR;

                if (shadowLink.size() == 2){
                    shadowLink[1] = sf::Vertex(n2Pos, lcolor);
                }
                if (shadowArrows.size() == 4){
                    sf::Vector2f n1Pos = n1->getNodePos();
                    sf::Vector2f midpoint = lh::getLineMidpoint(n1Pos, n2Pos);
                    lh::getArrowPositions(n1Pos, n2Pos, midpoint, n1Pos, n2Pos);
                    shadowArrows[0] = sf::Vertex(n1Pos, lcolor);
                    shadowArrows[1] = sf::Vertex(midpoint, lcolor);
                    shadowArrows[2] = sf::Vertex(n2Pos, lcolor);
                    shadowArrows[3] = sf::Vertex(midpoint, lcolor);
                }
            }
        }

        // resets the shadow link
        inline void clearShadowLink(const LinkStat& lstate){
            shadowLink.clear();
            if (lstate == LinkStat::SinglyTo)
                shadowArrows.clear();
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
            sf::Vector2f n1Pos = n1->getNodePos(), n2Pos = n2->getNodePos();
            if (lstate == LinkStat::Doubly) lcolor = DOUBLY_COLOR;
            else{
                lcolor = SINGLY_COLOR;
                sf::Vector2f ap1, ap2;
                sf::Vector2f midpoint = lh::getLineMidpoint(n1Pos, n2Pos);
                lh::getArrowPositions(n1Pos, n2Pos, midpoint, ap1, ap2);
                tempInputArrows.push_back(sf::Vertex(ap1, lcolor));
                tempInputArrows.push_back(sf::Vertex(midpoint, lcolor));
                tempInputArrows.push_back(sf::Vertex(ap2, lcolor));
                tempInputArrows.push_back(sf::Vertex(midpoint, lcolor));
            }                            

            tempInputLink.push_back(sf::Vertex(n1Pos, lcolor));
            tempInputLink.push_back(sf::Vertex(n2Pos, lcolor));
        }

        // resets the shadow link
        inline void clearTempLink(const LinkStat& lstate){
            tempInputLink.clear();
            if (lstate == LinkStat::SinglyTo)
                tempInputArrows.clear();
        }

        inline void clearShadowRemoveLink(){
            shadowRemoveLink.clear();
        }
        



        /* 
            Rendering
                - render graph viewer
                - render graphs
        */
        void drawIMGraphViewer(){
            ImGui::Begin("Graph Viewer", NULL, ImGuiWindowFlags_NoMove);
            ImGui::SetWindowPos(ImVec2(simul_width+20, 20));
            graphMan->drawGraphViewer();
            ImGui::End();
        }


        void renderLinks(sf::RenderWindow* win){
            if (shadowLink.size() > 0){
                win->draw(shadowLink.data(), shadowLink.size(), sf::Lines);
            }
            if (shadowArrows.size() > 0){
                win->draw(shadowArrows.data(), shadowArrows.size(), sf::Lines);
            }
            if (shadowRemoveLink.size() > 0){
                win->draw(shadowRemoveLink.data(), shadowRemoveLink.size(), sf::Lines);
            }
            if (tempInputLink.size() > 0){
                win->draw(tempInputLink.data(), tempInputLink.size(), sf::Lines);
            }
            if (tempInputArrows.size() > 0){
                win->draw(tempInputArrows.data(), tempInputArrows.size(), sf::Lines);
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
                /* std::cout << "TEXTINPUTTING: " << textInputting << "\n";
                if (n1) std::cout << "N1 ID: " << n1->getNodeIdent() << "'\t";
                else std::cout << "N1 NULL;\t";
                if (n2) std::cout << "N2 ID: " << n2->getNodeIdent() << "'\t";
                else std::cout << "N2 NULL;\t";
                std::cout << "n2 == n1?: " << (n2 == n1) << "\n\n"; */
                
                if (n2 != NULL && n1 != NULL && n1 != n2 && textInputting){
                    // textInputting = true;
                    if (tempInputLink.size() == 0){
                        setTempLink(n1, n2, lstate);
                    }
                }else{
                    std::cout << "EXITING INPUTTING\n";
                    checkLinking = false;
                    textInputting = false;
                }
                clearShadowLink(lstate);

                if (textInputting){
                    char inputNode[256];
                    memset(inputNode, '\0', 256);
                    std::cout << "OPENING GUI\n";
                    
                    if (ImGui::Begin("Enter Link's Weight")){
                        if (ImGui::InputText("Enter Weight", inputNode, 255, ImGuiInputTextFlags_EnterReturnsTrue) ){
                            std::string input(inputNode);
                            if (input.empty()){
                                std::cout << "STRING EMPTY\n";
                                clearTempLink(lstate);
                                textInputting = false;
                                checkLinking = false;
                            }else if (isNumber(inputNode)){
                                std::cout << "IS NUMBER\n";
                                ll weight = std::stoi(input);
                                std::cout << "ENTERED WEIGHT: " << weight << "\n";
                                graphMan->joinNodes(n1, n2, weight, lstate);
                                clearTempLink(lstate);
                                textInputting = false;
                                checkLinking = false;
                            }
                        }
                        ImGui::End();
                    }
                    
                    
                }
            }
        }


        void drawIMAlgoMenu(AlgoToRun& runAlgo, SimulState& state){
            // Don't display run algo or algo menu if in view mode (algo is running)
            if(state == SimulState::ViewMode)
                return;

            ImGui::Begin("Run Algorithms", NULL, ImGuiWindowFlags_NoMove);
            ImVec2 vec(10, 10);
            ImGui::SetWindowPos(vec);
            algoMan.displayAlgosMenu(runAlgo);
            ImGui::End();

            if (runAlgo != AlgoToRun::NoAlgo)
            {
                algoMan.displayAlgosStartMenu();
                bool algoSelectingNode = algoMan.selectMode != NodeSelectMode::NoSelected;
                if (algoSelectingNode) {state = SimulState::SelectNodeMode;}
            }
        }

        void drawIMAlgoPlayButtons(SimulState& state)
        {
            // if algomode
                // draw buttons
                // if X button clicked algoMode goes off
            if (algoMan.algoRunning)
            {
                // NOTE: view mode should mean no other buttons able to be pressed
                state = SimulState::ViewMode;
                size_t w = simul_width+20, h = win_height/2, bh = 25, space = 7;
                ImVec2 buttonSize(23, 23);

                // Algo panel
                ImGui::Begin(algoMan.runningAlgoName.c_str(), NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
                ImGui::SetWindowPos(ImVec2(w, h));

                // Algo play butons
                ImGui::BeginGroup();
                ImGui::SetCursorPos(ImVec2(space, bh));
                bool play = ImGui::Button("|>", buttonSize);

                ImGui::SetCursorPos(ImVec2(space + buttonSize.x + space, bh));
                bool stepBack = ImGui::Button("<", buttonSize);

                ImGui::SetCursorPos(ImVec2(space + 2*(buttonSize.x + space), bh));
                bool stepForward = ImGui::Button(">", buttonSize);

                ImGui::SetCursorPos(ImVec2(space + 3*(buttonSize.x + space), bh));
                bool quit = ImGui::Button("X", buttonSize);

                if (quit)
                {
                    std::cout << "EXITING ALGO\n";
                    state = SimulState::AddNodeMode;
                    algoMan.quitAlgo();
                }
                ImGui::EndGroup();

                // Group to display table
                ImGui::BeginGroup();
                if (ImGui::BeginTable("table1", 3))
                {
                    ImGui::TableSetupColumn("0");
                    ImGui::TableSetupColumn("1");
                    ImGui::TableSetupColumn("2");
                    ImGui::TableHeadersRow();
                    for (int row = 0; row < 50; row++)
                    {
                        ImGui::TableNextRow();
                        for (int column = 0; column < 3; column++)
                        {
                            // ImGui::TableSetColumnIndex(column);
                            ImGui::TableNextColumn();
                            ImGui::Text("Row %d Column %d", row, column);
                        }
                    }
                    ImGui::EndTable();
                }
                ImGui::EndGroup();
                ImGui::End();
            }
        }

        void drawControlBorder(sf::RenderWindow* win){
            win->draw(controlBorder);
        }

        void drawSimulStateIndicator(sf::RenderWindow* win, const std::string stateText){
            simulStateDisplay.setString(stateText);
            simulStateDisplay.setOrigin(simulStateDisplay.getLocalBounds().width, 0);
            simulStateDisplay.setPosition(sf::Vector2f(win_width-10, 10));
            win->draw(simulStateDisplay);
        }

        void drawSimulStateLinkType(sf::RenderWindow* win, const std::string linkText){
            simulStateLinkType.setString(linkText);
            simulStateLinkType.setOrigin(simulStateLinkType.getLocalBounds().width, 0);
            simulStateLinkType.setPosition(sf::Vector2f(win_width-10, simulStateDisplay.getLocalBounds().height+20));
            simulStateLinkType.setFillColor(linkText == "Single Link"? SINGLY_COLOR : DOUBLY_COLOR);
            win->draw(simulStateLinkType);
        }

        // REMOVE FUNC? called in drawIMAlgoMenu only?
        // void displayAlgosStartMenu(bool& algoSelectingNode){
        //     algoMan.displayAlgosStartMenu();
        //     // TODO: remove after toggling algo menu
        //     algoSelectingNode = algoMan.selectMode != NodeSelectMode::NoSelected;
        // }

        //clears the entire screen of nodes and links
        void clearScreen(){
            // UPDATING
            // delete graphMan;
            // graphMan = new Graph(win_width, win_height, simul_width, simul_height);
            // graphMan->freeAllNodes();
            graphMan->eraseAllGraphs();
        }

        ~Gui(){
            delete graphMan;
        }
};  