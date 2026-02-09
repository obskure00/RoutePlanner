RoutePlanner

A modular, test-driven C implementation of a graph-based route planning system, featuring shortest-path algorithms, a clean command-line interface, and a structured, maintainable architecture. This project is designed as both a practical route planning tool, and a reference implementation for graph algorithms and data modeling.


Table of Contents:

- Overview
- Key Features
- Project Structure
- Architecture & Design
- Graph Model
- Algorithms
- Input File Format
- Building the Project
- Running the Application
- Testing


Overview:

RoutePlanner is a C-based application that computes optimal routes between nodes in a weighted graph. It reads graph data from a file, constructs an internal graph representation, and applies shortest-path algorithms to determine the most efficient route between two points.


Key Features:

- Graph representation using adjacency lists
- Shortest-path computation (e.g. Dijkstra-style algorithm)
- Command-line interface (CLI) for user interaction
- File-based graph input
- Automated unit tests
- Makefile-based build system
- Clean modular architecture (no monolithic C files)


Project Structure:

```plaintext

RoutePlanner/
│
├── include/                # Public header files
│   ├── algorithms.h        # Shortest-path algorithms
│   ├── cli.h               # Command-line interface
│   ├── graph.h             # Graph data structures
│   └── io.h                # File input utilities
│
├── src/                    # Implementation files
│   ├── algorithms.c
│   ├── cli.c
│   ├── graph.c
│   ├── io.c
│   └── main.c
│
├── tests/                  # Unit tests
│   ├── acutest.h           # Lightweight C testing framework
│   ├── run_all_tests.c
│   ├── test_algorithms.c
│   ├── test_graph.c
│   └── test_common.h
│
├── data/
│   └── sample_graph.txt    # Example graph input
│
├── build/                  # Compiled object files and binaries
│
├── Makefile                # Build and test automation
├── LICENSE
└── README.md

```


Architecture & Design:

```plaintext

+----------------------+
|      CLI Layer       |
|  (User Interaction)  |
+----------+-----------+
           |
+----------v-----------+
|   Algorithm Layer    |
| (Path Computation)   |
+----------+-----------+
           |
+----------v-----------+
|     Graph Layer      |
| (Data Structures)    |
+----------+-----------+
           |
+----------v-----------+
|   I/O Utilities      |
| (File Parsing)       |
+----------------------+

```

Graph Implementation:

The RoutePlanner project uses a dynamic adjacency-list graph representation, designed for flexibility, memory efficiency, and algorithmic correctness across a wide range of traversal and pathfinding algorithms. The graph layer is deliberately algorithm-agnostic: it provides only data storage and access primitives, leaving all traversal logic to the algorithm module.

The graph consists of:

- A fixed number of vertices
- One adjacency list per vertex
- A linked list of outgoing edges per adjacency list
- names associated with vertices

This structure allows the project to efficiently represent:

- sparse graphs
- directed and undirected graphs
- weighted and unweighted graphs
- graphs with negative edge weights (when supported by the algorithm)

Adjacency Lists vs. Matrix:

- Adjacency lists were chosen over an adjacency matrix for several reasons:
- Space usage scales with O(V + E) rather than O(V²).
- Most shortest-path and traversal algorithms naturally iterate over outgoing edges.
- Supports multigraphs and parallel edges without additional complexity.
- Edge insertion is constant-time, and traversal touches only existing edges.
- This choice aligns naturally with BFS, DFS, Dijkstra, A*, and Bellman–Ford.

Each edge stores:

- a destination vertex
- a weight (integer)
- a pointer to the next edge in the list

Edges are stored as singly linked lists, inserted at the head of each adjacency list.

Design Rationale:

Linked lists allow fast insertion without reallocation. The graph layer does not validate weight semantics:

- negative weights are allowed
- interpretation is left to the algorithm layer
This separation ensures algorithm correctness is enforced where it belongs.

The graph implementation supports both directed and undirected graphs.

Directed graphs:
Each edge represents a single directional connection.

Undirected graphs:
Represented internally by inserting two directed edges:

```plaintext
u → v
v → u
```

This approach:

- avoids special cases in algorithms
- keeps the underlying data structure uniform
- mirrors how most graph algorithms are defined in theory

Vertices are internally identified by integer IDs (0 … V-1) for:

- constant-time access
- compact storage
- cache-friendly behavior

Optionally, vertices may also names stored alongside the graph.
These names are not required for algorithms but enable:

- CLI-based lookup
- readable debugging output
- user-friendly routing interfaces

Memory Ownership & Safety:

The graph owns:

- all adjacency lists
- all edge structures
- all vertex name strings

No memory is shared with other modules and destruction of the graph cleanly releases all associated memory. This makes it safe to:

- pass graphs between algorithms
- create and destroy graphs dynamically
- test algorithms in isolation

Implemented Algorithms:

RoutePlanner implements a comprehensive suite of graph traversal and shortest-path algorithms, each chosen to address a distinct class of routing problems.
All algorithms share a common path reconstruction mechanism and operate on the same adjacency-list graph representation.

Shared Path Reconstruction:

All path-producing algorithms rely on a shared internal helper. Each algorithm maintains a parent[] array mapping a vertex to its predecessor. Once the goal is reached, the path is reconstructed backwards from goal to start. The resulting path is reversed into user-facing order (start → goal).

Design Decisions:

Single reconstruction logic avoids duplication and subtle inconsistencies. A temporary buffer is used to ensure correctness before copying into user-provided memory. Hard limits (max_path_len, num_vertices) prevent buffer overflow and infinite loops. Invalid or disconnected paths return a clean failure signal (0).

Breadth-First Search (BFS):

Finds the shortest path in terms of number of edges (hops) in an unweighted graph or a graph where all edges have equal cost. Uses an explicit FIFO queue backed by a simple array. Maintains a visited[] bitmap to prevent revisiting nodes. Stops early once the goal is dequeued. Distance is computed as path_length - 1.

Complexity:

- Time: O(V + E)
- Space: O(V)

Design Rationale:

BFS is optimal for hop-minimization and avoids unnecessary weight handling. Chosen over Dijkstra for uniform-cost graphs due to lower overhead. Explicit queue avoids recursion and stack usage.

Depth-First Search (DFS):

Finds any valid path between two nodes (not guaranteed to be shortest). Recursive DFS with early termination when the goal is found. Tracks visited nodes to avoid cycles. Uses parent tracking to reconstruct a valid path.

Complexity:

- Time: O(V + E)
- Space: O(V) (recursion stack + visited array)

Design Rationale:

DFS is intentionally included for exploration and validation, not optimization. Recursive form improves clarity and mirrors textbook definitions. Useful for connectivity checks and debugging graph structure.

Dijkstra’s Algorithm:

Computes the shortest weighted path in graphs with non-negative edge weights. Uses a simple array-based minimum selection, not a heap. It maintains:

- dist[] for shortest known distances
- visited[] to finalize nodes
- parent[] for path reconstruction
and Carefully guards against integer overflow during relaxation.

Complexity:

- Time: O(V² + E)
- Space: O(V)

Design Rationale:

Chosen for clarity and predictability over raw performance. Array-based selection avoids complex heap logic for small–medium graphs. Explicit rejection of negative weights enforces algorithm correctness.

A* Search Algorithm:

Optimized shortest-path search using heuristic guidance, reducing explored nodes compared to Dijkstra.

Fully general A* with:

g(n) → actual distance from start

h(n) → heuristic estimate to goal

f(n) = g(n) + h(n)

Includes a custom binary min-heap priority queue:

- Supports decrease-key
- Tracks node positions for O(log V) updates
- Automatically degrades to Dijkstra when heuristic == NULL.
- Included Heuristics
- Manhattan distance (grid-based graphs)
- Euclidean distance (continuous-space approximation)

Complexity:

- Time: O((V + E) log V) (worst case)
- Space: O(V)

Design Rationale:

Custom priority queue avoids external dependencies. Heap index tracking enables efficient decrease-key operations. Heuristic is injected via function pointer for maximum flexibility. Safe fallback behavior ensures robustness.

Bellman–Ford Algorithm:

Handles graphs with negative edge weights and detects negative cycles. Performs V - 1 relaxation passes. Early termination if no updates occur. Explicit detection of nodes affected by negative cycles:

- Propagates cycle influence through reachable nodes
- Rejects paths affected by negative cycles
- Safely clamps integer overflow.

Complexity:

- Time: O(V × E)
- Space: O(V)

Design Rationale:

Necessary for correctness in graphs where Dijkstra/A* are invalid. Negative-cycle propagation ensures no invalid shortest paths are returned. Conservative failure behavior prevents undefined results.

Input File Format:

Graphs are loaded from plain text files.
Example (data/sample_graph.txt):

```plaintext

# number_of_nodes number_of_edges
5 6

# from to weight
0 1 7
0 2 9
0 4 14
1 2 10
2 3 11
3 4 6

```

Rules:

- Node IDs must be zero-based
- Edge weights must be non-negative
- Comments are allowed
- Invalid files are rejected with descriptive errors


Building and running the Project:

Requirements:

- C compiler (GCC or Clang)
- Make
- Standard C library

```

make all

```

This produces the main executable and all required object file.

```
./route_planner.exe <graph.txt>
```

Testing:

```
make tests
```

The test suite uses Acutest, a lightweight, dependency-free C testing framework and tests:

- Graph creation and destruction
- Edge insertion and validation
- Algorithm correctness
- Edge cases (invalid input, unreachable nodes)
