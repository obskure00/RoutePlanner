#ifndef IO_H
#define IO_H

#include "../include/graph.h"

/**
 * @brief Result structure returned by pathfinding wrapper functions.
 */
typedef struct {
    int     found;      /**< 1 if a path was found, 0 otherwise */
    int    *path;       /**< Dynamically allocated array of vertex indices (start → goal) */
    int     path_len;   /**< Number of vertices in the path (including start and goal) */
    int     distance;   /**< For BFS: number of hops, for others: sum of edge weights */
} PathResult;

/* ============================================================================
   Graph file loading
   ========================================================================== */

/**
 * @brief Loads a graph from a text file in the expected format.
 *
 * File format:
 *   First line:        num_vertices num_edges directed(0/1)
 *   Next num_vertices lines:  id name
 *   Next num_edges lines:     from to weight
 *
 * @param filename Path to the graph file
 * @return Newly allocated Graph*, or NULL on error
 */
Graph *load_graph_from_file(const char *filename);

/* ============================================================================
   PathResult creation wrappers
   ========================================================================== */

/**
 * @brief Runs BFS and returns a PathResult.
 * Distance = number of hops (edges).
 */
PathResult pathresult_from_bfs(Graph *g, int start, int goal);

/**
 * @brief Runs DFS (any path) and returns a PathResult.
 * Distance = number of hops (edges).
 */
PathResult pathresult_from_dfs(Graph *g, int start, int goal);

/**
 * @brief Runs Dijkstra and returns a PathResult.
 * Distance = total edge weight sum.
 */
PathResult pathresult_from_dijkstra(Graph *g, int start, int goal);

/**
 * @brief Runs A* (with NULL heuristic → Dijkstra behavior) and returns PathResult.
 */
PathResult pathresult_from_astar(Graph *g, int start, int goal, int (*heuristic)(int, int));

/**
 * @brief Runs Bellman-Ford and returns a PathResult.
 * Distance = -1 if negative cycle detected or unreachable.
 */
PathResult pathresult_from_bellman_ford(Graph *g, int start, int goal);

/* ============================================================================
   PathResult management & output
   ========================================================================== */

/**
 * @brief Frees memory allocated inside a PathResult struct.
 * Sets all fields to safe/empty values.
 */
void pathresult_free(PathResult *res);

/**
 * @brief Prints a human-readable representation of the path result.
 * Uses vertex names when available (via graphGetName).
 */
void print_path_result(Graph *g, const PathResult *res, int algo_type);

/**
 * @brief Prints an error message to stderr.
 */
void io_print_error(const char *msg);

#endif /* IO_H */