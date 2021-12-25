#include <iostream>
#include <functional>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "gui.hpp"
#include <chrono>
#include <unistd.h>
#include <fcntl.h>

using namespace std::chrono;

sf::Mutex renderMutex;
bool rendering = true;

void renderThread(sf::RenderWindow* win, Gui* game){
    //renderMutex.lock();
    while(rendering && win->isOpen()){
        game->renderAllGraphs(win);
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

int main(){
#if SIMUL
    int win_width = 1200;
    int win_height = 750;
    sf::RenderWindow window(sf::VideoMode(win_width, win_height), "Dijkstra", sf::Style::Close);
    //window.setActive(false);

    Gui game(win_width, win_height);
    // sf::Thread t1(std::bind(&renderThread, &window, &game));
    // t1.launch();
    // sf::Thread mp(&mousePos, &window);
    // mp.launch();

    static bool leftPressed = false, rigthPressed = false, middlePressed = false;
    while(window.isOpen()){
        sf::Event event; 
            while(window.pollEvent(event)){
                switch (event.type){
                    case sf::Event::Closed:
                        window.close();
                        
                        break;
                    case sf::Event::MouseButtonPressed:  
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Right)){
                            game.addNode(&window);
                            
                        }else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                            std::cout << "\tLeft is pressed\n";
                            leftPressed = true;
                            game.onNodeLeftClick(&window);
                        }
                        
                        break;
                    case sf::Event::MouseButtonReleased:
                        if (event.mouseButton.button == sf::Mouse::Left){
                            //delete node, when delete button toggled
                            //game.onNodeLeftClick(&window);
                            std::cout << "\tLeft is released\n";
                            leftPressed = false;
                        }
                        break;
                    case sf::Event::KeyPressed:
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
                            game.clearScreen();
                        }
                        break;
                }
            }
        
        game.renderAllGraphs(&window);
    }

#else
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