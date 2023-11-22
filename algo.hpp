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

#define ANIM_NODE_CURR_COLOR        sf::Color(102, 255, 153)    // greenish
#define ANIM_NODE_VIS_COLOR         sf::Color(255, 102, 217)    // purplish
#define ANIM_NODE_WILL_VIS_COLOR    sf::Color(255, 217, 102)    // orangish

struct AnimationSteps
{
    size_t step = 0;                                // Current step in algorithm (also index to vector array to get nodes)
    size_t allSteps = 0;                            // Total number of steps/ might not be necessary
    std::vector<Node*> currNodes;                   // Current node(s) algorithm is on
    std::vector<Node*> visitedNodes;                // Nodes that have been visited
    std::vector<Node*> willVisNodes;                // Nodes that will be visited
    std::unordered_map<ll, Node*> touched;          // All nodes that have been touched to be cleaned after algo finishes

    // TODO: algorithm needs to feed into this class??
    
    void clean()
    {
        for (auto node = touched.begin(); node != touched.end(); ++node)
        {
            node->second->setNodeFillColor(NODE_FILL_COLOR);
            node->second->setNodeOutlineColor(NODE_OUT_COLOR);      // NOTE: might not be necessary
        }
    }
};

struct AnimTable
{
    std::vector<std::string> tableHeader;
    std::vector<std::vector<std::string>> tableData;


};

class AlgoDFS
{
    public:
        AlgoDFS(Node* start, Node* find)
        {

        }
};

class Algos{
    public:
        //map<curr node identifier, tuple<distance from node to curr node, from node identifier>>
        typedef std::unordered_map<ll, std::tuple<long long, ll>> weight_map;
        //typedef for node links
        typedef std::tuple<Node*, ll, ll, bool> ADJ_NODE;

        Node* startN;
        Node* findN;
        AlgoToRun runAlgo;
        NodeSelectMode selectMode;
        AlgoAnimationMode animMode;

        // Node selection start menu options
        bool startSelectPressed;
        bool findSelectPressed;
        bool algoRunning;
        std::string runningAlgoName;
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
            algoRunning = false;
            runningAlgoName = "";
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
            // No start menu if no algo selected
            if (runAlgo == AlgoToRun::NoAlgo) return;

            std::string startButtonName, findButtonName;
            std::string startText, findText;
            std::string algoName = runAlgo != AlgoToRun::NoAlgo ? algo_init_menu[(int)runAlgo] : "";
            bool startButton = false;
            bool findButton = false;

            ImGui::Begin(algoName.c_str(), NULL, ImGuiWindowFlags_NoMove);
                    
            // Node button selection group
            ImGui::BeginGroup();
            startButtonName = startSelectPressed ? "X###1" : "O###1";
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

            // Find node only matters for BFS/DFS
            if (runAlgo < AlgoToRun::Dijkstra)
            {
                findButtonName = findSelectPressed ? "X###2" : "O###2";
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
            }
            ImGui::EndGroup();

            // Button descriptions
            ImGui::SameLine();
            ImGui::BeginGroup();
            startText = startN ? "Starting at node: "+std::to_string(startN->getNodeIdent()) : "Select starting node";  
            ImGui::Text(startText.c_str());
            if (runAlgo != AlgoToRun::Dijkstra)
            {
                findText = findN ? "Finding node: "+std::to_string(findN->getNodeIdent()) : "Select node to find"; 
                ImGui::Text(findText.c_str());
            }
            ImGui::EndGroup();

            // Create run algo button if algo nodes selected
            if (startN && (findN || runAlgo == AlgoToRun::Dijkstra))
            {
                std::string runAlgoMessage = "Run " + algo_list[(int) runAlgo] + " algorithm";
                if (ImGui::Button(runAlgoMessage.c_str(), ImVec2(250, 25)))
                {
                    std::cout << runAlgoMessage+" clicked\n";
                    algoRunning = true;
                    runningAlgoName = "Debugging "+algo_list[(int) runAlgo];
                }
            }
            ImGui::End();
        }

        // Helper to allow node to be selected as either start/find node for algorithm
        // Should only be called if start menu is open
        void setSelectedAlgoNode(Node* selected)
        {
            if (selectMode == NodeSelectMode::StartSelected && startSelectPressed)
            {
                startN = selected;
                selectMode = NodeSelectMode::NoSelected;
                startSelectPressed = false; 
                std::cout << "Saved Node: " << (startN ? std::to_string(startN->getNodeIdent()) : "NULL") << " as start\n";
            }else if (selectMode == NodeSelectMode::FindSelected && findSelectPressed)
            {
                findN = selected;
                selectMode = NodeSelectMode::NoSelected;
                findSelectPressed = false;
                std::cout << "Saved Node: " << (findN ? std::to_string(findN->getNodeIdent()) : "NULL") << " as find\n";
            }
        }

        // Clear saved algo settings
        void quitAlgo()
        {
            runAlgo = NoAlgo;   
            selectMode = NoSelected;
            animMode = Pause;   // is this even necessary

            // Clear saved nodes
            startN = NULL;
            findN = NULL;

            startSelectPressed = false;
            findSelectPressed = false;
            algoRunning = false;
            runningAlgoName = "";
        }

        bool checkPossibleID(char* id, int len){
            for (int i=0; i < len; ++i){
                if (!std::isdigit(id[i]))
                    return false;
            }
            return true;
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