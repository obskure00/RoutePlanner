#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "../include/graph.h"

/**
 * @brief Finds the shortest path in terms of number of hops (edges) using BFS.
 * Works on unweighted graphs or when all edge weights are considered equal.
 *
 * @param graph       The graph to search in
 * @param start       Starting vertex index
 * @param goal        Target vertex index
 * @param out_path    Buffer to store the path (vertex indices)
 * @param max_path_len Maximum number of vertices the buffer can hold
 * @param out_distance Pointer to store the number of hops (edges) or -1 if no path
 * @return Number of vertices in the path (including start and goal), or 0 if no path
 */
int bfs_shortest_hops(Graph *graph, int start, int goal, int *out_path, int max_path_len, int *out_distance);

/**
 * @brief Finds **any** path from start to goal using Depth-First Search.
 * The path is not guaranteed to be shortest.
 *
 * @param graph       The graph
 * @param start       Start vertex
 * @param goal        Target vertex
 * @param out_path    Buffer for path vertices
 * @param max_path_len Size of the out_path buffer
 * @return Number of vertices in the path, or 0 if no path found
 */
int dfs_path(Graph *graph, int start, int goal, int *out_path, int max_path_len);

/**
 * @brief Finds the shortest path using Dijkstra's algorithm.
 * Assumes non-negative edge weights.
 *
 * @param graph       The graph
 * @param start       Start vertex
 * @param goal        Goal vertex
 * @param out_path    Output buffer for path
 * @param max_path_len Buffer size
 * @param out_distance Total path weight or -1 if unreachable
 * @return Path length (number of vertices) or 0 on failure
 */
int dijkstra_shortest_path(Graph *graph, int start, int goal, int *out_path, int max_path_len, int *out_distance);

/**
 * @brief A* search – shortest path using heuristic.
 * If heuristic is NULL, behaves like Dijkstra.
 *
 * @param graph       The graph
 * @param start       Start vertex
 * @param goal        Goal vertex
 * @param out_path    Path output buffer
 * @param max_path_len Buffer size
 * @param out_distance Total path cost or -1
 * @param heuristic   Heuristic function(from, to) → estimated cost (can be NULL)
 * @return Path length or 0 if no path
 */
int astar_shortest_path(Graph *graph, int start, int goal, int *out_path, int max_path_len, int *out_distance, int (*heuristic)(int from, int to));
int simple_euclidean_heuristic(int a, int b);
int simple_manhattan_heuristic(int a, int b);

/**
 * @brief Bellman-Ford algorithm – finds shortest paths with possible negative weights.
 * Also detects negative cycles reachable from start.
 *
 * @param graph       The graph
 * @param start       Source vertex
 * @param goal        Target vertex
 * @param out_path    Path buffer
 * @param max_path_len Buffer size
 * @param out_distance Total cost or -1 (no path or negative cycle detected)
 * @return Number of vertices in path or 0 on failure/no path/negative cycle
 */
int bellman_ford_shortest_path(Graph *graph, int start, int goal, int *out_path, int max_path_len, int *out_distance);

#endif /* ALGORITHMS_H */