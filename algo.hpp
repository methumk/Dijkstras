/* 
algo.hpp
    - The dijkstra algorithm implementation
 */
#pragma once
#include <climits>
#include "graph.hpp"

//List of possible algos the user can run
static const std::string algo_list[] = {"Graph DFS", "Graph BFS", "Dijkstra"};
static const std::string algo_init_menu[] = {"DFS Menu", "BFS Menu", "Dijkstra Menu"};
enum AlgoToRun {DFS, BFS, Dijkstra, NoAlgo};
enum AlgoAnimationMode {Pause, Play, Close};        // TODO: forward, backward
enum NodeSelectMode {NoSelected, StartSelected, FindSelected};
class Algos{
    public:
        //map<curr node identifier, tuple<distance from node to curr node, from node identifier>>
        typedef std::unordered_map<ll, std::tuple<long long, ll>> weight_map;
        //typedef for node links
        typedef std::tuple<Node*, ll, ll, bool> ADJ_NODE;
        AlgoToRun runAlgo;

        Node* startN;
        Node* findN;
        NodeSelectMode selectMode;
        AlgoAnimationMode animMode;

        // Node selection start menu options
        bool startSelectPressed;
        bool findSelectPressed;
    public:
        Algos(){
            //initialize to no algo running
            runAlgo = NoAlgo;   
            selectMode = NoSelected;
            animMode = Pause;

            // Stores the nodes to run the algo on
            startN = NULL;
            findN = NULL;

            startSelectPressed = false;
            findSelectPressed = false;
        }

        //Displays a Menu that allows user to pick which algorithm to run
        void displayAlgosMenu(AlgoToRun& guiRunAlgo){
            for (std::string algo : algo_list){
                if (ImGui::Button(algo.c_str())){
                    if (algo_list[(int)runAlgo] == algo)
                    {
                        std::cout << "Algo " << algo << " toggled off" << std::endl;
                        runAlgo = NoAlgo;
                        guiRunAlgo = runAlgo;
                    }else{
                        if (algo == algo_list[0]){
                            runAlgo = DFS;
                        }else if (algo == algo_list[1]){
                            runAlgo = BFS;
                        }else if (algo == algo_list[2]){
                            runAlgo = Dijkstra;                    
                        }
                        guiRunAlgo = runAlgo;
                        std::cout << "Clicked on button: " << algo << std::endl;
                    }
                }
            }
        }

        // Start menu to allow user to click on start/find nodes
        void displayAlgosStartMenu()
        {
            std::string startButtonName, findButtonName;
            std::string startText, findText;
            std::string algoName = runAlgo != AlgoToRun::NoAlgo ? algo_init_menu[(int)runAlgo] : "";
            bool startButton = false;
            bool findButton = false;
            switch(runAlgo){
                case DFS:
                case BFS:
                    ImGui::Begin(algoName.c_str(), NULL, ImGuiWindowFlags_NoMove);
                    
                    // Node button selection group
                    ImGui::BeginGroup();
                    startButtonName = startSelectPressed ? "X###1" : "O###1";
                    findButtonName = findSelectPressed ? "X###2" : "O###2";

                    if (ImGui::Button(startButtonName.c_str(), ImVec2(20, 20))){
                        if (startSelectPressed || findSelectPressed)
                        {
                            // Button pressed when start/find node is already selected -> clear button's node
                            selectMode = NodeSelectMode::NoSelected;
                            startSelectPressed = false;
                            startN = NULL;
                            std::cout << algoName+" start toggled off\n";
                        }else if (!startSelectPressed){
                            // Button pressed and so selecting start node
                            selectMode = NodeSelectMode::StartSelected;
                            startSelectPressed = true;
                            std::cout << algoName+" start toggled on\n";
                        }
                    }
                    if (ImGui::Button(findButtonName.c_str(), ImVec2(20, 20))){
                        if (startSelectPressed || findSelectPressed)
                        {
                            // Button pressed when start/find node is already selected -> clear button's node
                            selectMode = NodeSelectMode::NoSelected;
                            findSelectPressed = false;
                            findN = NULL;
                            std::cout << algoName+" find toggled off\n";
                        }else if (!findSelectPressed){
                            // Button pressed and so selecting find node
                            selectMode = NodeSelectMode::FindSelected;
                            findSelectPressed = true;
                            std::cout << algoName+" find toggled on\n";
                        }
                    }
                    ImGui::EndGroup();

                    // Button descriptions
                    ImGui::SameLine();
                    ImGui::BeginGroup();
                    startText = startN ? "Starting at node: "+std::to_string(startN->getNodeIdent()) : "Select starting node";  
                    findText = findN ? "Finding node: "+std::to_string(findN->getNodeIdent()) : "Select node to find"; 
                    ImGui::Text(startText.c_str());
                    ImGui::Text(findText.c_str());
                    ImGui::EndGroup();
                    ImGui::End();
                    break;
                case Dijkstra:
                    break;
                default:
                    break;
            }
        }

        // Helper to allow node to be selected as either start/find node for algorithm
        // Should only be called if start menu is open
        void setSelectedAlgoNode(Node* selected)
        {
            if (!selected)
            {
                std::cout << "\tNo node selected for algorithm to run\n";
                return;
            }

            if (selectMode == NodeSelectMode::StartSelected && startSelectPressed)
            {
                startN = selected;
                selectMode = NodeSelectMode::NoSelected;
                startSelectPressed = false; 
                // findSelectPressed = false; 
                std::cout << "Saved Node: " << std::to_string(startN->getNodeIdent()) << " as start\n";
            }else if (selectMode == NodeSelectMode::FindSelected && findSelectPressed)
            {
                findN = selected;
                selectMode = NodeSelectMode::NoSelected;
                findSelectPressed = false;
                // startSelectPressed = false; 
                std::cout << "Saved Node: " << std::to_string(findN->getNodeIdent()) << " as find\n";
            }
        }

        bool checkPossibleID(char* id, int len){
            for (int i=0; i < len; ++i){
                if (!std::isdigit(id[i]))
                    return false;
            }
            return true;
        }

        void displayClickNode(const std::string& title, const std::string& descrip, const size_t x, const size_t y)
        {
            ImGui::Begin(title.c_str());
            ImVec2 vec(x, y);
            ImGui::SetWindowPos(vec);
            ImGui::Text(descrip.c_str());
            ImGui::End();
        }

        void graphDFSMan()
        {   

        }

        
        // NOTE:
        // UPDATING
        // Not using weight map, will have table structure that gets printed in IMGUI??
        void graphDFSManager(Graph* graphMan){
            ImGui::Begin("DFS");
            ImGui::Text("Click on a node to start");

            char inputNode[256];
            memset(inputNode, '\0', 256);
            ImGui::InputText("Enter Id of node to start", inputNode, 255);
            std::cout << "\tDFS got: " << inputNode << std::endl;
            

            /* Node* start = NULL, *end = NULL;
            if (!checkPossibleID(inputNode, 256)){
                ll startIdent = std::stoull(inputNode);
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