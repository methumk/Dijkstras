/* 
algo.hpp
    - The dijkstra algorithm implementation
 */
#pragma once
#include <climits>
#include "graph.hpp"

//List of possible algos the user can run
static const std::string algo_list[] = {"Graph DFS", "Graph BFS", "Dijkstra"};
enum AlgoToRun {DFS, BFS, Dijkstra, NoAlgo};

class Algos{
public:
    //map<curr node identifier, tuple<distance from node to curr node, from node identifier>>
    typedef std::unordered_map<ull, std::tuple<long long, ull>> weight_map;
    //typedef for node links
    typedef std::tuple<Node*, size_t, ull, bool> ADJ_NODE;
    AlgoToRun runAlgo;
public:
    Algos(){
        //initialize to no algo running
        runAlgo = NoAlgo;
    }

    //Displays a Menu that allows user to pick which algorithm to run
    void displayAlgosMenu(){
        for (std::string algo : algo_list){
            if (ImGui::Button(algo.c_str())){
                std::cout << "Clicked on button: " << algo << std::endl;
                if (algo == "Graph DFS"){
                    runAlgo = DFS;
                }else if (algo == "Graph BFS"){
                    runAlgo = BFS;
                }else if (algo == "Dijkstra"){
                    runAlgo = Dijkstra;                    
                }
                
            }
        }
    }

    void runFromAlgoMenu(Graph* graphMan){
        switch(runAlgo){
            case DFS:
                graphDFSManager(graphMan);
                break;
            case BFS:
                //Implement BFS
                ImGui::Begin("BFS");
                ImGui::End();
                break;
            case Dijkstra:
                //Implement Dijkstra
                ImGui::Begin("Dijkstra");
                ImGui::End();
                break;
            
            default: //Don't run any algorithm if NoAlgo
                break;
        }
    }

    bool checkPossibleID(char* id, int len){
        for (int i=0; i < len; ++i){
            if (!std::isdigit(id[i]))
                return false;
        }
        return true;
    }

    void graphDFSManager(Graph* graphMan){
        ImGui::Begin("DFS");
        ImGui::Text("Click on a node to start");

        char inputNode[256];
        memset(inputNode, '\0', 256);
        ImGui::InputText("Enter Id of node to start", inputNode, 255);
        std::cout << "\tDFS got: " << inputNode << std::endl;
        

        /* Node* start = NULL, *end = NULL;
        if (!checkPossibleID(inputNode, 256)){
            ull startIdent = std::stoull(inputNode);
            std::cout << "Algo DFS - start ident: " << startIdent << std::endl;
            start = graphMan->findNode(startIdent);
        } */

        //set the current running Algo to none
        //runAlgo = NoAlgo;
        ImGui::End();
    }

    void runGraphDFS(Node* start){

        runAlgo = NoAlgo;
    }

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