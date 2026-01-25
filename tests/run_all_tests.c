// run_all_tests.c
// Single entry point to run ALL unit tests.
// acutest.h automatically provides main() when included.

#include "acutest.h"

// ───────────────────────────────────────────────
// Declare all test functions from test_graph.c
// ───────────────────────────────────────────────
extern void test_create_and_free_empty_graph(void);
extern void test_add_edge_directed_undirected(void);
extern void test_multiple_edges_same_pair(void);
extern void test_zero_and_negative_weight(void);
extern void test_name_management(void);
extern void test_name_null_and_invalid_id(void);
extern void test_print_graph_does_not_crash(void);
extern void test_add_edge_invalid_indices(void);

// ───────────────────────────────────────────────
// Declare all test functions from test_algorithms.c
// ───────────────────────────────────────────────
extern void test_bfs_disconnected(void);
extern void test_bfs_shortest_hops(void);
extern void test_dfs_any_path(void);
extern void test_dijkstra_basic(void);
extern void test_dijkstra_zero_weight(void);
extern void test_astar_equals_dijkstra_when_no_heuristic(void);
extern void test_dijkstra_unique_best_path(void);
extern void test_astar_manhattan(void);
extern void test_astar_euclidean(void);
extern void test_dijkstra_large_graph(void);
extern void test_bellman_ford_negative_weight(void);
extern void test_bellman_ford_negative_cycle(void);
extern void test_invalid_inputs(void);
extern void test_path_buffer_overflow(void);

// ───────────────────────────────────────────────
// The TEST_LIST — acutest discovers and runs these automatically
// ───────────────────────────────────────────────
TEST_LIST = {
    // Graph structure & basic operations
    { "graph/create_and_free_empty",              test_create_and_free_empty_graph },
    { "graph/add_edge_directed_undirected",       test_add_edge_directed_undirected },
    { "graph/multiple_edges_same_pair",           test_multiple_edges_same_pair },
    { "graph/zero_and_negative_weight",           test_zero_and_negative_weight },
    { "graph/name_management",                    test_name_management },
    { "graph/name_null_and_invalid_id",           test_name_null_and_invalid_id },
    { "graph/print_graph_does_not_crash",         test_print_graph_does_not_crash },
    { "graph/add_edge_invalid_indices",           test_add_edge_invalid_indices },

    // Pathfinding algorithms
    { "algo/bfs_disconnected",                    test_bfs_disconnected },
    { "algo/bfs_shortest_hops",                   test_bfs_shortest_hops },
    { "algo/dfs_any_path",                        test_dfs_any_path },
    { "algo/dijkstra_basic",                      test_dijkstra_basic },
    { "algo/dijkstra_zero_weight",                test_dijkstra_zero_weight },
    { "algo/astar_equals_dijkstra_no_heuristic",  test_astar_equals_dijkstra_when_no_heuristic },
    { "algo/dijkstra_unique_best_path",           test_dijkstra_unique_best_path },
    { "algo/astar_manhattan",                     test_astar_manhattan },
    { "algo/astar_euclidean",                     test_astar_euclidean },
    { "algo/dijkstra_large_graph",                test_dijkstra_large_graph },
    { "algo/bellman_ford_negative_weight",        test_bellman_ford_negative_weight },
    { "algo/bellman_ford_negative_cycle",         test_bellman_ford_negative_cycle },
    { "algo/invalid_inputs_all_algos",            test_invalid_inputs },
    { "algo/path_buffer_overflow",                test_path_buffer_overflow },

    { NULL, NULL }
};