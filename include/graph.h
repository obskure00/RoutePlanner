#ifndef GRAPH_H
#define GRAPH_H

/**
 * @brief Edge in the adjacency list — represents a neighbor and edge weight.
 */
typedef struct Edge {
    int to;             /**< Destination vertex index */
    int weight;         /**< Edge weight (can be negative, zero or positive) */
    struct Edge *next;      /**< Next edge in the linked list */
} Edge;

/**
 * @brief Adjacency list head for one vertex.
 */
typedef struct AdjList {
    Edge *head;             /**< Pointer to first edge / NULL if empty */
} AdjList;

/**
 * @brief Graph structure — adjacency list representation with optional vertex names.
 */
typedef struct Graph {
    int numVertices;  /**< Number of vertices */
    AdjList* array;        /**< Array of adjacency lists (size = numVertices) */
    char** names;        /**< Array of dynamically allocated names (can be NULL) */
} Graph;

/* ──────────────────────────────────────────────────────────────────────────────
   Core graph operations
   ────────────────────────────────────────────────────────────────────────────── */

/**
 * @brief Creates a new graph with the specified number of vertices.
 * @param vertices Number of vertices (must be > 0)
 * @return New Graph pointer or NULL on failure or invalid input
 */
Graph *createGraph(int vertices);

/**
 * @brief Frees all memory associated with the graph (including edges and names).
 * @param graph Graph to destroy (safe to pass NULL)
 */
void graph_free(Graph *graph);

/**
 * @brief Adds a weighted edge from source to destination.
 * @param graph The graph
 * @param source Source vertex index
 * @param destination Destination vertex index
 * @param weight Edge weight (may be negative)
 * @param undirected If non-zero, adds edge in both directions
 * @return 0 on success, -1 on failure (invalid indices, allocation error, NULL graph)
 */
void addWeightedEdge(Graph *graph, int source, int destination, int weight, int undirected);

/**
 * @brief Adds an unweighted edge (weight = 1).
 * Convenience wrapper around addWeightedEdge.
 */
void addEdge(Graph *graph, int source, int destination, int undirected);

/* ──────────────────────────────────────────────────────────────────────────────
   Vertex naming support
   ────────────────────────────────────────────────────────────────────────────── */

/**
 * @brief Assigns a name to a vertex (replaces any previous name).
 * @param graph The graph
 * @param id Vertex index
 * @param name New name (NULL is allowed → removes name)
 */
void graphSetName(Graph *graph, int id, const char *name);

/**
 * @brief Returns the name of a vertex (or NULL if not set).
 * @return Read-only pointer to name string or NULL
 */
const char *graphGetName(const Graph *graph, int id);

/**
 * @brief Finds vertex index by its name (exact match, case-sensitive).
 * @return Vertex index or -1 if not found or invalid input
 */
int graph_find_vertex_by_name(const Graph *graph, const char *name);

/* ──────────────────────────────────────────────────────────────────────────────
   Debugging / output
   ────────────────────────────────────────────────────────────────────────────── */

/**
 * @brief Prints the graph structure to stdout in a human-readable format.
 * Shows vertex id, name (if any) and all outgoing edges with weights.
 * @param graph Graph to print (safe to pass NULL)
 */
void printGraph(const Graph *graph);

#endif /* GRAPH_H */