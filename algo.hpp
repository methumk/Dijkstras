/* 
algo.hpp
    - The dijkstra algorithm implementation
 */
#pragma once
#include "structure.hpp"


class Dijkstra{
public:
    typedef std::unordered_map<ull, unsigned int> weight_map;
private:
    std::unordered_set<ull> visited;
    std::unordered_map<ull, unsigned int> distances;

public:
};