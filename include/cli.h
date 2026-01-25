#ifndef CLI_H
#define CLI_H

#include "../include/graph.h"
#include "../include/io.h"

/**
 * @brief Enum for supported pathfinding algorithms in the CLI.
 */
typedef enum {
    ALGO_NONE = 0,          /**< Exit / no algorithm selected */
    ALGO_BFS,               /**< Breadth-First Search – fewest hops */
    ALGO_DFS,               /**< Depth-First Search – any path */
    ALGO_DIJKSTRA,          /**< Dijkstra – shortest weighted path (non-negative) */
    ALGO_ASTAR_NONE,        /**< A* without heuristic (falls back to Dijkstra) */
    ALGO_ASTAR_MANHATTAN,   /**< A* with Manhattan heuristic */
    ALGO_ASTAR_EUCLIDEAN,   /**< A* with Euclidean heuristic */
    ALGO_BELLMAN_FORD       /**< Bellman-Ford – negative weights & cycle detection */
} Algorithm;

/* ============================================================================
   Interactive CLI functions
   ========================================================================== */

/**
 * @brief Displays an interactive menu and returns the user's chosen algorithm.
 * Loops until valid input (0–5) is received.
 * @return Selected Algorithm value (ALGO_NONE = 0 means exit)
 */
Algorithm cli_select_algorithm(void);

/**
 * @brief Prompts the user to select a vertex (by ID or by name).
 * Displays available vertices and handles both selection modes.
 * @param g      The loaded graph
 * @param prompt Descriptive text (e.g. "Start vertex" or "Goal vertex")
 * @return Valid vertex index (0 to numVertices-1), or -1 on fatal error
 */
int cli_prompt_vertex(const Graph *g, const char *prompt);

/**
 * @brief Main interactive loop of the command-line interface.
 * Repeatedly:
 *   - asks for algorithm
 *   - asks for start and goal vertices
 *   - computes and prints the path using the selected algorithm
 *   - frees resources
 * Exits when user selects ALGO_NONE (0).
 *
 * @param g The graph to operate on (must not be NULL)
 */
void cli_main_loop(Graph *g);

#endif /* CLI_H */