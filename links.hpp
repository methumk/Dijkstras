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

//Define the linkstatus possibilities between two nodes
enum class LinkStat {Doubly, SinglyTo};

class Links{
private:
    typedef unsigned long long ull;
    std::vector<sf::Vertex> all_links;                  
    std::unordered_map<std::string, size_t> nodes_links;        //corresponding node identity to location of its link in all_links
public:
    //creates a link in the vector of vertices
    void addLink(sf::Vector2f p1, sf::Vector2f p2, sf::Color c1, sf::Color c2){
        all_links.push_back(sf::Vertex(p1, c1));
        all_links.push_back(sf::Vertex(p2, c2));
    }

    //new addlink, will take link state as arugment 
    void addLink(const sf::Vector2f& p1, const sf::Vector2f& p2, const ull& node1, const ull& node2, const LinkStat& lstate){

        //set link color based on link state
        sf::Color lcolor;
        if (lstate == LinkStat::Doubly) lcolor = sf::Color::Green;
        else                            lcolor = sf::Color::Yellow;

        //add based on color
        all_links.push_back(sf::Vertex(p1, lcolor));
        all_links.push_back(sf::Vertex(p2, lcolor));

        //create node identifiers
        std::string n_l_identifier1 = std::to_string(node1) + "_" + std::to_string(node2);
        std::string n_l_identifier2 = std::to_string(node2) + "_" + std::to_string(node1);

        size_t l_idx = all_links.size()-2;
        if (!nodes_links.count(n_l_identifier1) && !nodes_links.count(n_l_identifier2)){
            // set both identifiers so order which nodes given doesn't matter
            nodes_links[n_l_identifier1] = l_idx;
            nodes_links[n_l_identifier2] = l_idx;
        }else{
            std::cout << "LINKS - ERROR - setting identifiers: " << n_l_identifier1 << " or " << n_l_identifier2 << " already exists\n";
        }

    }

    //removes the shared link between two nodes
    void removeLink(ull node1, ull node2){
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
        }else{
            std::cout << "LINKS - ERROR - removing node linkage identifier: " << n_l_identifier1 << " or " << n_l_identifier2 <<" has not been recorded\n";
        }
    }

    //sets the mapping between two nodes and its link
    /* void setLinkMap(ull node1, ull node2, size_t l_idx){
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
    /* void removeLinkMap(ull node1, ull node2){
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

    //draws all links to interface
    inline void drawLinks(sf::RenderWindow* win){
        win->draw(all_links.data(), all_links.size(), sf::Lines);
    }
};
