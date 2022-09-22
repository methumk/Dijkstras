#include "imgui.h" 
#include "imgui-SFML.h" 
#include "gui.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <functional>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>

using namespace std::chrono;

sf::Mutex renderMutex;
bool rendering = true;

void renderThread(sf::RenderWindow* win, Gui* game){
    //renderMutex.lock();
    while(rendering && win->isOpen()){
        game->renderGraphs(win);
    }
    //renderMutex.unlock();
}

void mousePos(sf::RenderWindow* window){
    while(window->isOpen()){
        sf::Vector2i mpos = sf::Mouse::getPosition(*window);
        std::cout << mpos.x << " " << mpos.y << std::endl;
        sf::sleep(sf::milliseconds(2000));
    }
}

#define SIMUL 1

//Created seperate enums for removing nodes and links (don't know if this will be the case in the future)
enum class SimulState {AddNodeMode, AddLinkMode, RemoveNodeMode, RemoveLinkMode};
const std::string simulStateDisplay[4] = {"Adding Nodes", "Adding Links", "Removing Nodes", "Removing Links"};
const std::string simulStateLinkType[2] = {"Single Link", "Double Link"};

int main(){
#if SIMUL
    static bool leftPressed = false, rigthPressed = false, middlePressed = false, dragging = false, textInputting = false, algoMode = false, checkLinking = false;
    static SimulState state = SimulState::AddNodeMode;
    static AlgoToRun runningAlgo = NoAlgo;
    static LinkStat link_state = LinkStat::Doubly;
    sf::Vector2i left_mpos, curr_mpos;
    const int win_width = 1200;
    const int win_height = 750;
    Gui game(win_width, win_height, simulStateDisplay[(int) state], simulStateLinkType[(int) link_state]);
    Node *right_clicked_on_node = NULL, *left_clicked_on_node = NULL;
    Node *linkNode1 = NULL, *linkNode2 = NULL;

    sf::RenderWindow window(sf::VideoMode(win_width, win_height), "Dijkstra", sf::Style::Close);
    
    ImGui::SFML::Init(window);
    sf::Clock deltaClock;
    

    /* 
        algoMode - true: Disables creating and linking nodes
        algoMode - false: Allows creating and linking nodes 
     */
    // bool algoMode = false;
    while(window.isOpen()){
        sf::Event event; 
        while(window.pollEvent(event)){
            ImGui::SFML::ProcessEvent(event);
            switch (event.type){
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::MouseButtonPressed: 
                    if (!textInputting){
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                            if (state == SimulState::AddNodeMode){
                                game.addNode(&window);
                            }else if (state == SimulState::AddLinkMode){
                                //when mouse is clicked on a node, save the node position to mouse_on_node
                                std::cout << "HELD\n";
                                checkLinking = false;
                                linkNode1 = NULL;
                                linkNode2 = NULL;
                                left_clicked_on_node = game.mouseOverNode(&window, NODE_RADIUS);
                                game.setShadowLink(left_clicked_on_node, link_state);
                            }else if (state == SimulState::RemoveNodeMode){
                                game.removeNode(&window);
                            }else if (state == SimulState::RemoveLinkMode){
                                left_clicked_on_node = game.mouseOverNode(&window, NODE_RADIUS);
                                game.setShadowRemoveLink(left_clicked_on_node);
                            }
                            
                        }else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)){
                            //Left for dragging
                            leftPressed = true;
                            dragging = true;
                            
                            left_mpos = sf::Mouse::getPosition(window);
                            right_clicked_on_node = game.mouseOverNode(&window, NODE_RADIUS);
                            game.checkNodeExist(right_clicked_on_node);
                        }
                    }
                    break;

                case sf::Event::MouseButtonReleased:
                    if (!textInputting){
                        if (event.mouseButton.button == sf::Mouse::Left){
                            if (state == SimulState::AddLinkMode){
                                // game.linkNodes(left_clicked_on_node, game.mouseOverNode(&window, NODE_RADIUS), link_state, textInputting);
                                if (!checkLinking){
                                    if (left_clicked_on_node != NULL && game.mouseOverNode(&window, NODE_RADIUS) != NULL){
                                        linkNode1 = left_clicked_on_node;
                                        linkNode2 = (game.mouseOverNode(&window, NODE_RADIUS));
                                        textInputting = true;
                                    }
                                    checkLinking = true;
                                }
                            }else if (state == SimulState::RemoveLinkMode){
                                game.removeLink(left_clicked_on_node, game.mouseOverNode(&window, NODE_RADIUS));
                                game.clearShadowRemoveLink();
                            }
                            left_clicked_on_node = NULL;
                        }else if (event.mouseButton.button == sf::Mouse::Right){
                            //delete node, when delete button toggled
                            //game.onNodeLeftClick(&window);
                            //std::cout << "\tLeft is released\n";
                            leftPressed = false;
                            dragging = false;
                            if (right_clicked_on_node) std::cout << "was/is on node: " << right_clicked_on_node->getNodeIdent() << '\n';
                            right_clicked_on_node = NULL;
                        }
                    }
                    break;

                case sf::Event::KeyPressed:
                    if (!textInputting){
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
                            game.clearScreen();
                        }else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
                            if (state == SimulState::AddNodeMode){
                                std::cout << "\n\nMode Activated: Adding links\n";
                                state = SimulState::AddLinkMode;
                            }else{
                                std::cout << "\n\nMode Activated: Adding Nodes\n";
                                state = SimulState::AddNodeMode;
                            }
                        }else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
                            if (state == SimulState::RemoveNodeMode){
                                std::cout << "\n\nMode Activated: Removing Links\n";
                                state = SimulState::RemoveLinkMode;
                            }else{
                                std::cout << "\n\nMode Activated: Removing Nodes\n";
                                state = SimulState::RemoveNodeMode; 
                            }
                        }else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)){
                            if (link_state != LinkStat::SinglyTo) 
                                link_state = LinkStat::SinglyTo;
                            std::cout <<"\nLinking state: Singly To\n";

                        }else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)){
                            if (link_state != LinkStat::Doubly) 
                                link_state = LinkStat::Doubly;
                            std::cout <<"\nLinking state: Doubly\n";

                        }
                    }
                    break;
                default:
                    break;
                
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        // handle shadow links for adding and removing links
        if (state == SimulState::AddLinkMode){
            game.moveShadowLink(left_clicked_on_node, &window, link_state);
        }
        else if (state == SimulState::RemoveLinkMode){
            game.moveShadowRemoveLink(left_clicked_on_node, &window);
        }

        //game.onDragNode(&window, right_clicked_on_node, left_mpos, dragging);

        //draw ImGui objects
        game.drawIMGraphViewer();
        game.drawIMAlgoMenu();
        game.runAlgoFromMenu();
        game.renderLinkWeightBox(linkNode1, linkNode2, link_state, textInputting, checkLinking);

        //render SFML objects
        window.clear();
        game.drawSimulStateIndicator(&window, simulStateDisplay[(int) state]);
        game.drawSimulStateLinkType(&window, simulStateLinkType[(int) link_state]);
        game.renderLinks(&window);
        game.renderGraphs(&window);
        ImGui::SFML::Render(window);
        window.display();
    }


    ImGui::SFML::Shutdown();


#else

    //testing artifacts
    int win_width = 1200;
    int win_height = 750;
    sf::RenderWindow window(sf::VideoMode(win_width, win_height), "Dijkstra", sf::Style::Close);
    std::vector<sf::Vertex> links2;
    Links links;

    std::vector<ull> nodes;
    int node_start = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type){
                case sf::Event::Closed:
                    window.close();
                    
                    break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Right){

                        sf::Vector2f p1 = sf::Vector2f(sf::Mouse::getPosition(window));
                        sf::Vector2f p2 = p1;
                        p2.y += 50;

                        //add the link between two nodes
                        //set the link map between two nodes and set the link index to the last two appended
                        links.addLink(p1, p2, sf::Color::Red, sf::Color::Red);
                        std::cout << "link size: " << links.getLinksSize() << "\n";
                        links.setLinkMap(node_start, node_start+1, links.getLinksSize()-2);

                        nodes.push_back(node_start);
                        nodes.push_back(node_start+1);
                        node_start+=2;

                    }else{
                        if (nodes.size() > 0){
                            links.removeLink(nodes[1], nodes[0]);
                            links.removeLinkMap(nodes[0], nodes[1]);
                            nodes.erase(nodes.begin(), nodes.begin()+2);
                        }
                    }

                    break;
            }
        }

        window.clear();
        links.drawLinks(&window);

        //window.draw(links2.data(), links2.size(), sf::Lines);
        //window.draw(tl.all_links.data(), tl.all_links.size(), sf::Lines);
        window.display();
    }
    
    
    // while(window.isOpen()){
    //     sf::Event event; 
    //         while(window.pollEvent(event)){
    //             switch (event.type){
    //                 case sf::Event::Closed:
    //                     window.close();
                        
    //                     break;
    //                 case sf::Event::MouseButtonPressed:
    //                     if (event.mouseButton.button == sf::Mouse::Right){
    //                         sf::Vector2f p1 = sf::Vector2f(sf::Mouse::getPosition(window));
    //                         sf::Vector2f p2 = p1;
    //                         p2.y += 20;
    //                         links.push_back(sf::Vertex(p1, sf::Color::Blue));
    //                         links.push_back(sf::Vertex(p2, sf::Color::Blue));
    //                     }else{
                            
    //                     }

    //                     break;
    //             }
    //         }
    //     window.clear();
    //     window.draw(c);
    //     window.clear();
    // }
                        
    //Graph mygraph;

/* 
    //testing that reviseddelete works properly - makes new graphs if needed
    //works
    int fd = open("./Dijkstras/saveout.txt", O_WRONLY | O_CREAT, 0644);
    int saveout = dup(1);
    int rfd = dup2(fd, 1);
    Graph mygraph;

    //Testing memory leaks from graph
    mygraph.createNewNode();    //0
    mygraph.createNewNode();    //1
    mygraph.createNewNode();    //2
    mygraph.createNewNode();    //3
    mygraph.createNewNode();    //4
    mygraph.createNewNode();    //5
    mygraph.createNewNode();    //6
    mygraph.createNewNode();    //7
    mygraph.createNewNode();    //8
    mygraph.createNewNode();    //9

    mygraph.joinNodes(0, 1, 20);
    mygraph.joinNodes(0, 2, 20);
    mygraph.joinNodes(2, 7, 20);
    mygraph.joinNodes(0, 3, 20);
    mygraph.joinNodes(1, 8, 20);
    mygraph.joinNodes(8, 9, 20);
    mygraph.joinNodes(4, 5, 20);
    mygraph.joinNodes(4, 6, 20);
    mygraph.joinNodes(5, 6, 20);

    fflush(stdout);
    close(fd);
    int crfd = dup2(saveout, 1);
    close(saveout);

    std::cout.setstate(std::ios_base::failbit);
    mygraph.REVISEDdeleteNode(4);
    std::cout.clear();

    for (int i=0; i < 10; ++i){
        if (i != 4){
            std::cout.setstate(std::ios_base::failbit);
            Node* n = mygraph.REVISEDfindNode(i);
            std::cout.clear();
            if (n){
                std::cout << "node: " << n->getNodeIdent() << " found at location: " << mygraph.getAllGraphsPos(n->getNodeIdent()) << std::endl;
            }
        }
    } */


    /* //Test UpdateLinkWeight
    //Worked
    mygraph.createNewNode();    //0
    mygraph.createNewNode();    //1

    std::cout << "~1\n";
    mygraph.joinNodes(0, 1, 10);
    mygraph.displayLinkWeight(0, 1);
    // std::cout << "~2\n";
     mygraph.updateNodeLink(1, 0, 30);
    mygraph.displayLinkWeight(0, 1);
    std::cout << "~3\n";
    mygraph.updateNodeLink(0, 1, 900);
    mygraph.displayLinkWeight(0, 1); */


    /*
        0->(1, 2, 3)
        1->(*0, 2, 4)
        2->(*0)
        3->(*0, 4)
        4->(*1, *3)
    */
    /* Node* hold, *t2;
    Node s1(mygraph.getNewNodeIdent());
    s1.addNode(new Node(mygraph.getNewNodeIdent()), 10, mygraph.getNewLinkIdent());
    s1.addNode(new Node(mygraph.getNewNodeIdent()), 12, mygraph.getNewLinkIdent()); 
    s1.addNode(new Node(mygraph.getNewNodeIdent()), 26, mygraph.getNewLinkIdent());
        //Node 1 -> (2, 4)
        hold = s1.getAdjNode(((unsigned int)0));
        hold->addNode(new Node(mygraph.getNewNodeIdent()), 3, mygraph.getNewLinkIdent());
        hold->addNode(s1.getNodeInfo(((unsigned int)1)));
    
        //Node 3 -> (4)
        t2 = s1.getAdjNode(((unsigned int)2));
        t2->addNode(hold->getNodeInfo(((unsigned int)0)));
    s1.printCurrNode();
    s1.printAdjNodes();
    
    mygraph.addGraph(&s1);


    std::cout << "\n\n";
    hold = mygraph.findNode(0, 99);

    if (hold == NULL) std::cout << "hold NULL\n";
    if (4 == hold->getNodeIdent()){
        std::cout << "found correct Node " << hold->getNodeIdent() << "\n";
    }else{
        std::cout << "INCORRECT NODE " << hold->getNodeIdent() << "\n";
    } */
    
    
    /* //(0, 1)
    mygraph.createNewNode(mygraph.getNewNodeIdent());
    mygraph.createNewNode(mygraph.getNewNodeIdent());
        std::cout<<"All graphs size: " << mygraph.all_graphs_size() <<'\n';
    //(0-1)
    mygraph.joinNodes(0, 1, 20);
        std::cout<<"All graphs size: " << mygraph.all_graphs_size() <<'\n';
    //(2)
    mygraph.createNewNode(mygraph.getNewNodeIdent());
        std::cout<<"All graphs size: " << mygraph.all_graphs_size() <<'\n';
    //(0-(1, 2))
    mygraph.joinNodes(0, 2, 30);  
    //(3)
    mygraph.createNewNode(mygraph.getNewNodeIdent());
        std::cout<<"All graphs size: " << mygraph.all_graphs_size() <<'\n';
    //(0-(1, 2, 3))
    mygraph.joinNodes(0, 3, 40);

    //(4, 5)
    mygraph.createNewNode(mygraph.getNewNodeIdent());
    mygraph.createNewNode(mygraph.getNewNodeIdent());
    std::cout<<"All graphs size: " << mygraph.all_graphs_size() <<'\n';
    //(4-5)
    mygraph.joinNodes(4, 5, 50);
    std::cout<<"All graphs size: " << mygraph.all_graphs_size() <<'\n';

    //(0-(1, 2, 3, 5-(4))))
    mygraph.joinNodes(0, 5, 40);
    std::cout<<"All graphs size: " << mygraph.all_graphs_size() <<'\n';

    //(6)
    mygraph.createNewNode(mygraph.getNewNodeIdent());
    mygraph.joinNodes(3, 6, 23); */

    //Think problem lies in vector, you join two nodes and the other vector nodes in the node_loc is messed up
    //probably from vector.erase, probably could just make pointer null but that would waste space
        //otherwise would have to move all the other graphs down - DFS to move all of them
        //this means all_graphs size is not reflective of how many graphs there are
    //(0, 1, 2, 3, 4)
    /* mygraph.createNewNode();
    mygraph.createNewNode();
    mygraph.createNewNode();
    mygraph.createNewNode();
    mygraph.createNewNode();
        std::cout<<"All graphs size: " << mygraph.getNumGraphs() <<'\n';
    //(0-1)
    mygraph.joinNodes(0, 1, 12);
        std::cout<<"All graphs size: " << mygraph.getNumGraphs() <<'\n';
    //(4-3)
    mygraph.joinNodes(4, 3, 12);
        std::cout<<"All graphs size: " << mygraph.getNumGraphs() <<'\n';
    //(0-1-[4-[3]])
    mygraph.joinNodes(1, 4, 12);
        std::cout<<"All graphs size: " << mygraph.getNumGraphs() <<'\n';
    //(0-1-[4-[3], 3-[4]])
    mygraph.joinNodes(1, 3, 12);
        std::cout<<"All graphs size: " << mygraph.getNumGraphs() <<'\n';
    mygraph.joinNodes(3, 2, 12);

    std::cout << "\n\n";
    Node* fn = mygraph.REVISEDfindNode(2);
    if (fn){
        std::cout << "Found Node: " << fn->getNodeIdent() <<'\n'; 
    }*/


    //Testing deletion of nodes
        //Test case 1: NTD(node to delete) is head in graph and no children
            //v2.0 WORKS
        /* mygraph.createNewNode();    //0
        mygraph.createNewNode();    //1
        mygraph.createNewNode();    //2
        mygraph.createNewNode();    //3
        std::cout<<"1 All graphs size: " << mygraph.getNumGraphs() <<'\n';
        mygraph.joinNodes(0, 2, 10);
        std::cout<<"2 All graphs size: " << mygraph.getNumGraphs() <<'\n';
        mygraph.joinNodes(2, 3, 12);
        std::cout<<"3 All graphs size: " << mygraph.getNumGraphs() <<'\n';
        mygraph.REVISEDdeleteNode(0);
        std::cout<<"4 DELETE 0 - All graphs size: " << mygraph.getNumGraphs() <<'\n';
        mygraph.joinNodes(1, 3, 5);
        std::cout<<"5 All graphs size: " << mygraph.getNumGraphs() <<'\n';
        Node* fn = mygraph.REVISEDfindNode(3);
        std::cout << "\nCheck\n";
        if (fn != NULL){
            std::cout << "Found Node: " << fn->getNodeIdent() <<'\n'; 
        }else{
            std::cout << "Node not found - incorrect\n";
        } 
        mygraph.REVISEDdeleteNode(2);
        std::cout<<"6 All graphs size: " << mygraph.getNumGraphs() <<'\n';
        fn = mygraph.REVISEDfindNode(3);
        std::cout << "\nCheck\n";
        if (fn != NULL){
            std::cout << "Found Node: " << fn->getNodeIdent() <<'\n'; 
        }else{
            std::cout << "Node not found - incorrect\n";
        } 
        mygraph.REVISEDdeleteNode(1);
        mygraph.REVISEDdeleteNode(3); */
        /* std::cout<<"8 All graphs size: " << mygraph.getNumGraphs() <<'\n';
        fn = mygraph.REVISEDfindNode(1);
        std::cout << "\nCheck\n";
        if (fn != NULL){
            std::cout << "Found Node: " << fn->getNodeIdent() <<'\n'; 
        }else{
            std::cout << "Node not found - incorrect\n";
        } */ 

        //Test case 2: NTD is head and and no node in graph is connected with each other
            //Found bug with joining node after deleting NTD
            //v2.0 WORKS
        /* mygraph.createNewNode();    //0
        mygraph.createNewNode();    //1
        mygraph.createNewNode();    //2
        mygraph.createNewNode();    //3
        mygraph.createNewNode();    //4
        mygraph.joinNodes(2, 0, 10);
        mygraph.joinNodes(2, 1, 11);
        mygraph.joinNodes(2, 3, 12);
        mygraph.joinNodes(2, 4, 10);
        std::cout<<"1 All graphs size: " << mygraph.getNumGraphs() <<'\n';
        mygraph.REVISEDdeleteNode(2);
        std::cout<<"2 DELETE 2 - All graphs size: " << mygraph.getNumGraphs() << "\n\n";
        mygraph.joinNodes(0, 1, 10);
        mygraph.joinNodes(3, 1, 10);
        std::cout<<"\nAll graphs size: " << mygraph.getNumGraphs() << "\n";
        for (int i=0; i<5; ++i){
            if (i!=2){
            Node* fn = mygraph.REVISEDfindNode(i);
            if (fn) std::cout << "\t\t\tFOUND Node: " << fn->getNodeIdent() <<'\n'; 
            else std::cout << "\t\t\tNode NOT FOUND: " << i <<'\n';

            }
            
        } */

        //Test case 3: NTD is head and some nodes in graph are connected w/ each other
        /* mygraph.createNewNode();
        mygraph.createNewNode();
        mygraph.createNewNode();
        mygraph.createNewNode(); */
        //Test case 4: NTD is not head and has no other link beside head
        //Test case 5: NTD is not head and has other links that are only connected to NTD
        //Test case 6: NTD is not head and has other links that connect to the head
#endif

    return 0;
}