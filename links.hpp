/*
links class:
    - class that manages links between two nodes
*/

#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <stdlib.h>
#include <string>

#define _USE_MATH_DEFINES
#include <cmath>

//Define the linkstatus possibilities between two nodes
enum class LinkStat {Doubly, SinglyTo};
#define ARROW_LEN 10
#define EDGE_WEIGHT_ORTH_DIST (ARROW_LEN+5)
#define DOUBLY_COLOR sf::Color::Green
#define SINGLY_COLOR sf::Color::Yellow
// #ifndef PI_4
// #define PI_4 0.785398163397448
// #endif

// Line helpers
namespace lh{
    //gets the angle between two points in radians (p1 is pointing to p2)
    //Angle ranges from 0 to 180 starting from second quad
    float getLineAngle(const sf::Vector2f& p1, const sf::Vector2f& p2){
        float lineX = p1.x > p2.x ? p1.x - p2.x : p2.x - p1.x;
                
        float lineangle = atan((p1.y - p2.y)/(lineX));
        //lineangle *= (180/M_PI);
        
        if (lineangle < 0){
            lineangle *= -1;
            if ((p1.x > p2.x) && (lineangle < M_PI_2)){
                lineangle = M_PI - lineangle;  
                //std::cout << "\tnegative adding\n";
            } 
        }else{
            if ((p1.x < p2.x) && (lineangle < M_PI_2)){
                lineangle = M_PI - lineangle;
                //std::cout << "\tpos adding\n";
            }
        }
        //return lineangle*(M_PI/180);
        return lineangle;
    }

    sf::Vector2f getLineMidpoint(const sf::Vector2f& p1, const sf::Vector2f& p2){
        return sf::Vector2f((p1.x + p2.x)/2, (p1.y + p2.y)/2);
    }

    //point p1 is node that is pointing to point p2
    void getArrowPositions(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& midpoint, sf::Vector2f& arrow1, sf::Vector2f& arrow2){
        float lineangle = getLineAngle(p1, p2);
        // std::cout << "lineangle: " << lineangle*(180/M_PI) << std::endl;

        /* std::cout << "TESTING RIGHT ANGLES..................\n";
        sf::Vector2f x1(0,0);
        sf::Vector2f x2(-10,0);
        std::cout << "X2 Left X1: " << getLineAngle(x1, x2)*(180/M_PI) << std::endl;
        x2.x = 0;
        x2.y = -10;
        std::cout << "X2 TOP X1: " << getLineAngle(x1, x2)*(180/M_PI) << std::endl;
        x2.x = 10;
        x2.y = 0;
        std::cout << "X2 RIGHT X1: " << getLineAngle(x1, x2)*(180/M_PI) << std::endl;
        x2.x = 0;
        x2.y = 10;
        std::cout << "X2 BOTTOM X1: " << getLineAngle(x1, x2)*(180/M_PI) << std::endl;
        std::cout << "END OF TEST RIGHT ANGLES...............\n"; */
        
        if (lineangle <= M_PI_2){
            // std::cout << "LINE NE\n";
            if (p1.x > p2.x){
                // std::cout << "ARROW POINTS LEFT\n";
                arrow1.x = midpoint.x - ARROW_LEN*cosf(3*M_PI_4 + lineangle);
                arrow1.y = midpoint.y - ARROW_LEN*sinf(3*M_PI_4 + lineangle);
                arrow2.x = midpoint.x - ARROW_LEN*cosf(5*M_PI_4 + lineangle);
                arrow2.y = midpoint.y - ARROW_LEN*sinf(5*M_PI_4 + lineangle);
            }else{
                // std::cout << "ARROW POINTS RIGHT\n";
                arrow1.x = midpoint.x - ARROW_LEN*cosf(M_PI_4 + lineangle);
                arrow1.y = midpoint.y - ARROW_LEN*sinf(M_PI_4 + lineangle);
                arrow2.x = midpoint.x - ARROW_LEN*cosf(lineangle - M_PI_4);
                arrow2.y = midpoint.y - ARROW_LEN*sinf(lineangle - M_PI_4);
            }
        }else{
            // std::cout << "LINE NW\n";
            if (p1.x <= p2.x){
                // std::cout << "ARROW POINTS RIGHT\n";
                arrow1.x = midpoint.x - ARROW_LEN*cosf(3*M_PI_4 + lineangle);
                arrow1.y = midpoint.y - ARROW_LEN*sinf(3*M_PI_4 + lineangle);
                arrow2.x = midpoint.x - ARROW_LEN*cosf(5*M_PI_4 + lineangle);
                arrow2.y = midpoint.y - ARROW_LEN*sinf(5*M_PI_4 + lineangle);
            }else{
                // std::cout << "ARROW POINTS LEFT\n";
                arrow1.x = midpoint.x - ARROW_LEN*cosf(M_PI_4 + lineangle);
                arrow1.y = midpoint.y - ARROW_LEN*sinf(M_PI_4 + lineangle);
                arrow2.x = midpoint.x - ARROW_LEN*cosf(lineangle - M_PI_4);
                arrow2.y = midpoint.y - ARROW_LEN*sinf(lineangle - M_PI_4);
            }
        }

    }

    //Rotates a point aroung an origin point 90 degrees clock wise
    inline sf::Vector2f cwOrthRotation(const sf::Vector2f& point_to_rotate, const sf::Vector2f& origin){
        return sf::Vector2f((point_to_rotate.y - origin.y) + origin.x,-1*(point_to_rotate.x - origin.x) + origin.y);
    } 

}

class Links{
    private:
        typedef long long ll;
        sf::Font font;
        std::vector<sf::Vertex> all_links;                  
        std::unordered_map<std::string, size_t> nodes_links;        //corresponding node identity to location of its link in all_links
        std::vector<sf::Vertex> arrows;
        std::unordered_map<std::string, size_t> nodes_arrows;               //bool cooresponding to whether nodes have an arrow (for singly linked)
        std::vector<sf::Text> link_weights;
        std::unordered_map<std::string, size_t> nodes_weights;
    public:
        Links(){
            if (!font.loadFromFile("./Dijkstras/open-sans/OpenSans-Semibold.ttf")){
                std::cerr << "LINKS CONSTRUCTOR - Error while loading font - EXITING\n";
                exit(EXIT_FAILURE);
            }
        }

        sf::Text setTextInfo(const ll& weight, const int& charSize, const sf::Color& fillColor, const sf::Vector2f& pos){
            sf::Text text;
            text.setFont(font);
            text.setString(std::to_string(weight));
            text.setCharacterSize(charSize);
            text.setOrigin(text.getLocalBounds().width/2., text.getLocalBounds().height/2.);
            text.setPosition(pos);
            text.setFillColor(fillColor);  

            return text;
        }

        //creates a link in the vector of vertices
        /* void addLink(sf::Vector2f p1, sf::Vector2f p2, sf::Color c1, sf::Color c2){
            all_links.push_back(sf::Vertex(p1, c1));
            all_links.push_back(sf::Vertex(p2, c2));
        } */


        //new addlink, will take link state as arugment
        //point p1 is node that is pointing to point p2 if singly linked
        void addLink(const sf::Vector2f& p1, const sf::Vector2f& p2, const ll& node1, const ll& node2, const ll& weight, const LinkStat& lstate){
            sf::Vector2f midpoint = lh::getLineMidpoint(p1, p2);
            //set link color based on link state
            sf::Color lcolor;
            if (lstate == LinkStat::Doubly) lcolor = DOUBLY_COLOR;
            else                            lcolor = SINGLY_COLOR;

            //add link based on color
            all_links.push_back(sf::Vertex(p1, lcolor));
            all_links.push_back(sf::Vertex(p2, lcolor));

            //create node identifiers
            std::string n_l_identifier1 = std::to_string(node1) + "_" + std::to_string(node2);
            std::string n_l_identifier2 = std::to_string(node2) + "_" + std::to_string(node1);

            //update link weight values
            // float slope = (p2.y-p1.y)/(p2.x-p1.x);
            // float intercept = p2.y - slope*(p2.x);
            // sf::Vector2f weightPos(p1.x + ((p2.x - p1.x)/2.5), p1.y + ((p2.y - p1.y)/2.5));
            sf::Vector2f wp(midpoint.x+EDGE_WEIGHT_ORTH_DIST*cosf(lh::getLineAngle(p1, p2)), midpoint.y+EDGE_WEIGHT_ORTH_DIST*sinf(lh::getLineAngle(p1, p2)));
            //sf::Vector2f weightPos(p1.x+ARROW_LEN, slope*(p1.x+ARROW_LEN) + intercept);
            link_weights.push_back(setTextInfo(weight, 10, sf::Color(255, 0, 0), lh::cwOrthRotation(wp, midpoint)));
            size_t w_idx = link_weights.size()-1;
            nodes_weights[n_l_identifier1] = w_idx;
            nodes_weights[n_l_identifier2] = w_idx;

            // set node-link identifiers
            // set arrow if singly linked
            size_t l_idx = all_links.size()-2;
            if (!nodes_links.count(n_l_identifier1) && !nodes_links.count(n_l_identifier2)){
                nodes_links[n_l_identifier1] = l_idx;
                nodes_links[n_l_identifier2] = l_idx;

                //draw the arrow to show singly linked nodes (from -> to)
                if (lstate == LinkStat::SinglyTo){
                    sf::Vector2f ap1, ap2;
                    lh::getArrowPositions(p1, p2, midpoint, ap1, ap2);

                    arrows.push_back(sf::Vertex(ap1, lcolor));
                    arrows.push_back(sf::Vertex(midpoint, lcolor));

                    //bottom part of the arrow
                    //sf::Vector2f ap2(ARROW_LEN*(cosf(lineangle + M_PI_4)) + midpoint.x, -1*ARROW_LEN*(sinf(lineangle + M_PI_4)) + midpoint.y);
                    arrows.push_back(sf::Vertex(ap2, lcolor));
                    arrows.push_back(sf::Vertex(midpoint, lcolor));

                    std::cout << "ap1.x: " << ap1.x << " ap1.y" << ap1.y << std::endl;
                    std::cout << "ap2.x: " << ap2.x << " ap2.y" << ap2.y << std::endl;

                    //store location of the arrow from node identifier
                    nodes_arrows[n_l_identifier1] = arrows.size()-4;
                    nodes_arrows[n_l_identifier2] = arrows.size()-4;
                }
            }else{
                std::cout << "LINKS - ERROR - setting identifiers: " << n_l_identifier1 << " or " << n_l_identifier2 << " already exists\n";
                exit(EXIT_FAILURE);
            }

        }

        //removes the shared link between two nodes
        void removeLink(const ll& node1, const ll& node2){
            std::string n_l_identifier1 = std::to_string(node1) + "_" + std::to_string(node2);
            std::string n_l_identifier2 = std::to_string(node2) + "_" + std::to_string(node1);

            if (nodes_links.count(n_l_identifier1) && nodes_links.count(n_l_identifier2)){
                size_t n_l_idx = nodes_links[n_l_identifier1];

                //update node_link mappings greater than current index+1 to be deleted
                for (auto it = nodes_links.begin(); it != nodes_links.end(); ++it){
                    if (it->second > n_l_idx+1)
                        it->second -= 2;
                }
                
                //erase link between two nodes
                all_links.erase(all_links.begin()+n_l_idx, all_links.begin()+n_l_idx+2);
                //remove link mapping 
                nodes_links.erase(n_l_identifier1);
                nodes_links.erase(n_l_identifier2);

                //erase edge weight
                size_t w_idx = nodes_weights[n_l_identifier1];
                for (auto it = nodes_weights.begin(); it != nodes_weights.end(); ++it){
                    if (it->second > w_idx)
                        it->second -= 1;
                }

                //remove gui edge weight
                link_weights.erase(link_weights.begin() + w_idx);
                //remove edge weight mapping
                nodes_weights.erase(n_l_identifier1);
                nodes_weights.erase(n_l_identifier2);


                //if node has arrows
                if (nodes_arrows.count(n_l_identifier1) && nodes_arrows.count(n_l_identifier2)){
                    size_t a_l_idx = nodes_arrows[n_l_identifier1];

                    //update node_arrow mapping
                    for (auto it = nodes_arrows.begin(); it != nodes_arrows.end(); ++it){
                        if (it->second > a_l_idx+3)
                            it->second -= 4;
                    }

                    //remove the arrows
                    arrows.erase(arrows.begin()+a_l_idx, arrows.begin()+a_l_idx+4);

                    //remove arrow mapping
                    nodes_arrows.erase(n_l_identifier1);
                    nodes_arrows.erase(n_l_identifier2);
                }
            }else{
                std::cout << "LINKS - ERROR - removing node linkage identifier: " << n_l_identifier1 << " or " << n_l_identifier2 <<" has not been recorded\n";
            }
        }

        //sets the mapping between two nodes and its link
        /* void setLinkMap(ll node1, ll node2, size_t l_idx){
            std::string n_l_identifier1 = std::to_string(node1) + "_" + std::to_string(node2);
            std::string n_l_identifier2 = std::to_string(node2) + "_" + std::to_string(node1);

            if (!nodes_links.count(n_l_identifier1) && !nodes_links.count(n_l_identifier2)){
                if (l_idx >= all_links.size()  && l_idx < 0){
                    std::cout << "LINKS - ERROR - setting node: index in vector of vertices is invalid\n";
                    exit(EXIT_FAILURE);
                }

                // set both identifiers so order which nodes given doesn't matter
                nodes_links[n_l_identifier1] = l_idx;
                nodes_links[n_l_identifier2] = l_idx;
            }else{
                std::cout << "LINKS - ERROR - setting identifiers: " << n_l_identifier1 << " or " << n_l_identifier2 << " already exists\n";
            }
        } */

        //removes mapping of link between two nodes
        /* void removeLinkMap(ll node1, ll node2){
            std::string n_l_identifier1 = std::to_string(node1) + "_" + std::to_string(node2);
            std::string n_l_identifier2 = std::to_string(node2) + "_" + std::to_string(node1);

            if (nodes_links.count(n_l_identifier1) && nodes_links.count(n_l_identifier2)){
                nodes_links.erase(n_l_identifier1);
                nodes_links.erase(n_l_identifier2);
            }else{
                std::cout << "LINKS - ERROR - removing node linkage: " << n_l_identifier1 << " or " << n_l_identifier2 << " has not been recorded\n";
            }
        } */


        //returns size of the vector of vertices
        /* inline size_t getLinksSize(){
            return all_links.size();
        } */

        //draws all links and arrows to interface
        inline void drawLinks(sf::RenderWindow* win){
            win->draw(all_links.data(), all_links.size(), sf::Lines);
            win->draw(arrows.data(), arrows.size(), sf::Lines);
            for (int i=0; i<link_weights.size(); ++i){
                win->draw(link_weights[i]);
            }
        }
};
