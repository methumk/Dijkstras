/* 
algo.hpp
    - The dijkstra algorithm implementation
 */
#pragma once
#include "structure.hpp"
#include <climits>


class Dijkstra{
public:
    //map<curr node identifier, tuple<distance from node to curr node, from node identifier>>
    typedef std::unordered_map<ull, std::tuple<long long, ull>> weight_map;
    //typedef for node links
    typedef std::tuple<Node*, size_t, ull> ADJ_NODE;
public:

    void nodesToWM(Node* curr, std::unordered_set<Node*>& visited, weight_map& wm){
        //mark current node as visited and save it in weight map
        visited.insert(curr);
        wm[curr->getNodeIdent()] = std::make_tuple(-1, ULLONG_MAX);

        std::vector<ADJ_NODE> links = curr->getNodeLinks();
        //add nodes to the weight map if they aren't already visited
        for(size_t i=0; i < links.size(); ++i){
            Node* inspect = std::get<0>(links[i]);
            if (!visited.count(inspect))
                nodesToWM(inspect, visited, wm);
        }


    }

    //returns the weight map from the start node to all other nodes in the graph
    weight_map runDijkstra(Node* start_node, Graph& graphs){
        std::cout << "Running Dijkstra\n";
    
        //find graph location of node
        size_t graph_loc = graphs.getNodeGraphsPos(start_node->getNodeIdent());
        if (graph_loc == -1) perror("\t\tDijkstra  - graph pos not found - exiting");

        weight_map wm;
        std::unordered_set<Node*> setwm_visited;
        //save all nodes to the weight map 
        nodesToWM(graphs.getGraphHead(graph_loc), setwm_visited, wm);

        

        


    }
};