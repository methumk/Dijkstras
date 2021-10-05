#include <iostream>
#include <SFML/Graphics.hpp>
#include "structure.hpp"


int main(){
    /* int win_width = 1200;
    int win_height = 750;
    sf::RenderWindow window(sf::VideoMode(win_width, win_height), "Dijkstra", sf::Style::Close | sf::Style::Resize);

    sf::CircleShape circle;
    circle.setRadius(30);
    circle.setOrigin(30, 30);
    circle.setPosition(win_height/2, win_height/2);
    circle.setFillColor(sf::Color::Red);
    circle.setOutlineColor(sf::Color::Green);
    circle.setOutlineThickness(2.f);
    while(window.isOpen()){
        sf::Event event; 
            while(window.pollEvent(event)){
                switch (event.type){
                    case sf::Event::Closed:
                        window.close();
                        break;
                }
            }

        window.clear();
        window.draw(circle);
        window.display();
    } */

    /* Graph mygraph;
    Node* hold;
    Node s1(mygraph.getCurrNodeIdent());

    s1.addNode(new Node(mygraph.getCurrNodeIdent()), 10, mygraph.getCurrLinkIdent());
    s1.addNode(new Node(mygraph.getCurrNodeIdent()), 12, mygraph.getCurrLinkIdent()); 
        hold = s1.getAdjNode((unsigned int)0);
        hold->addNode(s1.getNodeInfo((unsigned int)1));

    s1.printCurrNode();
    s1.printAdjNodes();
    std::cout << "\n\t";
    hold->printCurrNode();
    hold->printAdjNodes();

    Node s2(mygraph.getCurrNodeIdent());
    s2.addNode(new Node(mygraph.getCurrNodeIdent()), 2, mygraph.getCurrLinkIdent());
    s2.addNode(new Node(mygraph.getCurrNodeIdent()), 4, mygraph.getCurrLinkIdent());
    s2.addNode(new Node(mygraph.getCurrNodeIdent()), 8, mygraph.getCurrLinkIdent());

    s2.printCurrNode();
    s2.printAdjNodes();

    mygraph.addGraph(&s1);
    mygraph.addGraph(&s2);  */


    return 0;
}