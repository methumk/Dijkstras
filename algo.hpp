/*
algo.hpp
    - Algorithm viewer and play manager
 */
#pragma once
#include <climits>
#include "DFSImpl.hpp"
#include "BFSImpl.hpp"
#include "DijkImpl.hpp"

// List of possible algos the user can run
static const std::string algo_list[] = {"Graph DFS", "Graph BFS", "Dijkstra"};
static const std::string algo_init_menu[] = {"DFS Menu", "BFS Menu", "Dijkstra Menu"};
enum NodeSelectMode
{
    NoSelected,
    StartSelected,
    FindSelected
};

class Algos
{
public:
    // map<curr node identifier, tuple<distance from node to curr node, from node identifier>>
    typedef std::unordered_map<ll, std::tuple<long long, ll>> weight_map;
    // typedef for node links
    typedef std::tuple<Node *, ll, ll, bool> ADJ_NODE;

    Node *startN;
    Node *findN;
    AlgoToRun runAlgo;
    NodeSelectMode selectMode;
    IAnimImpl *algoAnim;

    // Node selection start menu options
    bool startSelectPressed;
    bool findSelectPressed;
    bool algoRunning;
    std::string runningAlgoName;

public:
    Algos()
    {
        // initialize to no algo running
        runAlgo = NoAlgo;
        selectMode = NoSelected;
        algoAnim = NULL;

        // Stores the nodes to run the algo on
        startN = NULL;
        findN = NULL;

        startSelectPressed = false;
        findSelectPressed = false;
        algoRunning = false;
        runningAlgoName = "";
    }

    // Display list of algos in menu the the user can select to run
    void displayAlgosListMenu(AlgoToRun &guiRunAlgo)
    {
        for (std::string algo : algo_list)
        {
            if (ImGui::Button(algo.c_str()))
            {
                if (algo_list[(int)runAlgo] == algo)
                {
                    std::cout << "Algo " << algo << " toggled off" << std::endl;
                    runAlgo = NoAlgo;
                    guiRunAlgo = runAlgo;
                }
                else
                {
                    if (algo == algo_list[0])
                    {
                        runAlgo = DFS;
                        algoAnim = new DFSImpl();
                    }
                    else if (algo == algo_list[1])
                    {
                        runAlgo = BFS;
                        algoAnim = new BFSImpl();
                    }
                    else if (algo == algo_list[2])
                    {
                        runAlgo = Dijkstra;
                        algoAnim = new DijkImpl();
                    }
                    guiRunAlgo = runAlgo;
                    std::cout << "Clicked on button: " << algo << std::endl;
                }
            }
        }
    }

    // Start menu to allow user to click on start/find nodes
    void displayAlgosStartMenu(const size_t &simul_width)
    {
        // No start menu if no algo selected
        if (runAlgo == AlgoToRun::NoAlgo)
            return;

        std::string startButtonName, findButtonName;
        std::string startText, findText;
        std::string algoName = runAlgo != AlgoToRun::NoAlgo ? algo_init_menu[(int)runAlgo] : "";
        bool startButton = false;
        bool findButton = false;

        ImGui::Begin(algoName.c_str(), NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImVec2 vec(simul_width + 20, 170);
        ImGui::SetWindowPos(vec);

        // Node button selection group
        ImGui::BeginGroup();
        startButtonName = startSelectPressed ? "X###1" : "O###1";
        if (ImGui::Button(startButtonName.c_str(), ImVec2(20, 20)))
        {
            if (startSelectPressed)
            {
                // Button pressed when start/find node is already selected -> clear button's node
                selectMode = NodeSelectMode::NoSelected;
                startSelectPressed = false;
                startN = NULL;
                std::cout << algoName + " start toggled off\n";
            }
            else
            {
                // Button pressed and so selecting start node
                selectMode = NodeSelectMode::StartSelected;
                startSelectPressed = true;
                std::cout << algoName + " start toggled on\n";
            }
        }

        // Find node only matters for BFS/DFS
        if (runAlgo < AlgoToRun::Dijkstra)
        {
            findButtonName = findSelectPressed ? "X###2" : "O###2";
            if (ImGui::Button(findButtonName.c_str(), ImVec2(20, 20)))
            {
                if (findSelectPressed)
                {
                    // Button pressed when start/find node is already selected -> clear button's node
                    selectMode = NodeSelectMode::NoSelected;
                    findSelectPressed = false;
                    findN = NULL;
                    std::cout << algoName + " find toggled off\n";
                }
                else
                {
                    // Button pressed and so selecting find node
                    selectMode = NodeSelectMode::FindSelected;
                    findSelectPressed = true;
                    std::cout << algoName + " find toggled on\n";
                }
            }
        }
        ImGui::EndGroup();

        // Button descriptions
        ImGui::SameLine();
        ImGui::BeginGroup();
        startText = startN ? "Starting at node: " + std::to_string(startN->getNodeIdent()) : "Select starting node";
        ImGui::Text(startText.c_str());
        if (runAlgo != AlgoToRun::Dijkstra)
        {
            findText = findN ? "Finding node: " + std::to_string(findN->getNodeIdent()) : "Select node to find";
            ImGui::Text(findText.c_str());
        }
        ImGui::EndGroup();

        // Create run algo button if algo nodes selected
        if (startN && (findN || runAlgo == AlgoToRun::Dijkstra))
        {
            std::string runAlgoMessage = "Run " + algo_list[(int)runAlgo] + " algorithm";
            if (ImGui::Button(runAlgoMessage.c_str(), ImVec2(250, 25)))
            {
                std::cout << runAlgoMessage + " clicked\n";
                algoRunning = true;
                runningAlgoName = "Step Descriptions for " + algo_list[(int)runAlgo];

                std::vector<Node *> startNodes{startN};

                if (runAlgo != AlgoToRun::Dijkstra)
                {
                    // BFS/DFS both need start and find nodes
                    startNodes.push_back(findN);
                }
                algoAnim->setStartNodes(startNodes);
            }
        }

        // Clear button when either start or find selected
        if (startN || (findN || runAlgo == AlgoToRun::Dijkstra))
        {
            std::string clearMessage = "Clear all selected nodes";
            if (ImGui::Button(clearMessage.c_str(), ImVec2(250, 25)))
            {
                startN = NULL;
                findN = NULL;
                startSelectPressed = false;
                findSelectPressed = false;
            }
        }

        if (ImGui::Button(std::string("Exit algorithm").c_str(), ImVec2(250, 25)))
        {
            quitAlgo();
        }
        ImGui::End();
    }

    // Helper to allow node to be selected as either start/find node for algorithm
    // Should only be called if start menu is open
    void setSelectedAlgoNode(Node *selected)
    {
        if (selectMode == NodeSelectMode::StartSelected && startSelectPressed)
        {
            startN = selected;
            selectMode = NodeSelectMode::NoSelected;
            startSelectPressed = false;
            std::cout << "Saved Node: " << (startN ? std::to_string(startN->getNodeIdent()) : "NULL") << " as start\n";
        }
        else if (selectMode == NodeSelectMode::FindSelected && findSelectPressed)
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
        delete algoAnim;

        // Clear saved nodes
        startN = NULL;
        findN = NULL;

        startSelectPressed = false;
        findSelectPressed = false;
        algoRunning = false;
        runningAlgoName = "";
    }

    bool checkPossibleID(char *id, int len)
    {
        for (int i = 0; i < len; ++i)
        {
            if (!std::isdigit(id[i]))
                return false;
        }
        return true;
    }

    void algoStepForward()
    {
        assert(algoAnim);
        algoAnim->stepForward();
    }

    const std::vector<std::string> &algoGetStepDescription()
    {
        assert(algoAnim);
        return algoAnim->getStepDescriptions();
    }

    const std::map<ll, CHILD_WEIGHT> &algoGetDijkTable()
    {
        assert(algoAnim);
        assert(runAlgo == AlgoToRun::Dijkstra);
        return algoAnim->getDijkTable();
    }
};