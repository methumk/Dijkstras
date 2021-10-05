**GUI**
- Adding/Deleting Nodes
- Linking Nodes (user has to add weights, automatic, editable?)
- clearing everything (nodes and links)
- Toggling Dijkstra table
- Toggling Dijkstra drawing

**Intrinsic**
- Dijkstra algorithm (will it work on one node?)
    - Dijkstra would need to know structure of drawn nodes

- Implementation (Gui -> Dijkstra -> Node structure)
- Gui is going to control both the node structure and the toggling the Dijkstra algorithm on and off

**GUI In depth**
- When user left clicks on screen creates a new node (add visual node info to node -> push node in graph)
- When user holds right click (and on node to another node - can't be itself) need to store each node in each of their links, then graph needs to remove one of the node from all_graphs (graph should create visual edge and keep track of it)