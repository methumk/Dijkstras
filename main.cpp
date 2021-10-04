#include <iostream>
#include <SFML/Graphics.hpp>


int main(){
    int win_width = 1200;
    int win_height = 750;
    sf::RenderWindow window(sf::VideoMode(win_width, win_height), "Dijkstra", sf::Style::Close | sf::Style::Resize);

    while(window.isOpen()){
        sf::Event event; 
            while(window.pollEvent(event)){
                switch (event.type){
                    case sf::Event::Closed:
                        window.close();
                        break;
                }
            }

        // window.clear();
        // window.draw();
        // window.display();
    }
}