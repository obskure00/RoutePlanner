#define TEST_NO_MAIN
#include "acutest.h"
#include <test_common.h>
#include <stdlib.h>
#include "../include/graph.h"
#include "../include/algorithms.h"

/* Helper to create a small connected graph (used in many tests) */
static Graph* create_small_test_graph(void) {
    Graph *g = createGraph(6);
    TEST_ASSERT_(g != NULL, "failed to allocate small test graph");

    /*
        0 --1--> 1 --4--> 3
         |        ^       ^
         2        |       |
         v        1       3
         2 --1--> 4 --2--> 5
                   |
                   7
                   v
                   3
    */

    addWeightedEdge(g, 0, 1, 1, 0);
    addWeightedEdge(g, 0, 2, 2, 0);
    addWeightedEdge(g, 1, 3, 4, 0);
    addWeightedEdge(g, 2, 4, 1, 0);
    addWeightedEdge(g, 4, 3, 2, 0);
    addWeightedEdge(g, 4, 5, 7, 0);
    addWeightedEdge(g, 2, 1, 1, 0); /* cycle */

    graphSetName(g, 0, "Start");
    graphSetName(g, 3, "Goal");
    graphSetName(g, 5, "End");

    return g;
}

void test_bfs_disconnected(void) {
    Graph *g = createGraph(5);
    TEST_ASSERT_(g != NULL, "failed to create graph");

    addEdge(g, 0, 1, 1);
    addEdge(g, 1, 2, 1);
    /* 3 and 4 disconnected */

    int path[10] = {0};
    int dist = 999;
    int len = bfs_shortest_hops(g, 0, 4, path, 10, &dist);

    TEST_CHECK_(len == 0, "BFS should not find path in disconnected graph");
    TEST_CHECK_(dist == -1, "distance should be -1 when no path exists");
    TEST_CHECK_(path[0] == 0, "path buffer should not be modified on failure");

    graph_free(g);
}

void test_bfs_shortest_hops(void) {
    Graph *g = create_small_test_graph();
    TEST_ASSERT_(g != NULL, "failed to create test graph");

    int path[10] = {0};
    int dist = -1;
    int len = bfs_shortest_hops(g, 0, 3, path, 10, &dist);

    TEST_CHECK_(len == 3, "BFS wrong path length (should be 3 vertices / 2 hops)");
    TEST_CHECK_(dist == 2, "BFS wrong hop count (should be 2)");
    TEST_CHECK_(path[0] == 0 && path[1] == 1 && path[2] == 3,
                "BFS should find shortest-hop path 0→1→3");

    graph_free(g);
}

void test_dfs_any_path(void) {
    Graph *g = create_small_test_graph();
    TEST_ASSERT_(g != NULL, "failed to create test graph");

    int path[10] = {0};
    int len = dfs_path(g, 0, 5, path, 10);

    TEST_CHECK_(len >= 3, "DFS should find at least one path to 5");
    TEST_CHECK_(path[0] == 0, "path must start with start vertex");
    TEST_CHECK_(path[len - 1] == 5, "path must end with goal vertex");

    graph_free(g);
}

void test_dijkstra_basic(void) {
    Graph *g = create_small_test_graph();
    TEST_ASSERT_(g != NULL, "failed to create test graph");

    int path[10] = {0};
    int dist = -1;
    int len = dijkstra_shortest_path(g, 0, 3, path, 10, &dist);

    TEST_CHECK_(len == 4 || len == 3,
                "Dijkstra path length should be 3 or 4 vertices");
    TEST_CHECK_(dist == 5, "Dijkstra shortest distance should be 5");

    int is_path1 = (len == 4 &&
                    path[0] == 0 && path[1] == 2 &&
                    path[2] == 4 && path[3] == 3);
    int is_path2 = (len == 3 &&
                    path[0] == 0 && path[1] == 1 &&
                    path[2] == 3);

    TEST_CHECK_(is_path1 || is_path2,
                "Dijkstra should return 0-2-4-3 or 0-1-3");

    graph_free(g);
}

void test_dijkstra_zero_weight(void) {
    Graph *g = createGraph(4);
    TEST_ASSERT_(g != NULL, "failed to create graph");

    addWeightedEdge(g, 0, 1, 0, 1);
    addWeightedEdge(g, 1, 2, 3, 1);
    addWeightedEdge(g, 0, 3, 10, 1);
    addWeightedEdge(g, 1, 3, 1, 1);

    int path[10] = {0};
    int dist = -1;
    int len = dijkstra_shortest_path(g, 0, 3, path, 10, &dist);

    TEST_CHECK_(len == 3, "path length should be 3 (0→1→3)");
    TEST_CHECK_(dist == 1, "zero-weight edge should be preferred → total distance 1");
    TEST_CHECK_(path[0] == 0 && path[1] == 1 && path[2] == 3,
                "expected path 0→1→3 using zero-weight edge");

    graph_free(g);
}

void test_astar_equals_dijkstra_when_no_heuristic(void) {
    Graph *g = create_small_test_graph();
    TEST_ASSERT_(g != NULL, "failed to create test graph");

    int path_d[10] = {0}, path_a[10] = {0};
    int dist_d = -1, dist_a = -1;

    int len_d = dijkstra_shortest_path(g, 0, 5, path_d, 10, &dist_d);
    int len_a = astar_shortest_path(g, 0, 5, path_a, 10, &dist_a, NULL);

    TEST_CHECK_(len_d == len_a, "A* with NULL heuristic must match Dijkstra length");
    TEST_CHECK_(dist_d == dist_a, "A* with NULL heuristic must match Dijkstra distance");
    TEST_CHECK_(len_d > 0, "path should exist");

    graph_free(g);
}

void test_dijkstra_unique_best_path(void) {
    Graph *g = createGraph(5);
    TEST_ASSERT_(g != NULL, "failed to create graph");

    addWeightedEdge(g, 0, 1, 10, 0);
    addWeightedEdge(g, 1, 3, 1, 0);
    addWeightedEdge(g, 0, 2, 2, 0);
    addWeightedEdge(g, 2, 3, 10, 0);

    int path[10] = {0};
    int dist = -1;
    int len = dijkstra_shortest_path(g, 0, 3, path, 10, &dist);

    TEST_CHECK_(len == 3, "path length should be 3 vertices");
    TEST_CHECK_(dist == 11,
                "should choose 0→1→3 (cost 11) over 0→2→3 (cost 12)");
    TEST_CHECK_(path[0] == 0 && path[1] == 1 && path[2] == 3,
                "Dijkstra must select the unique best path 0→1→3");

    graph_free(g);
}

void test_astar_manhattan(void) {
    Graph *g = createGraph(25); /* 5×5 grid */
    TEST_ASSERT_(g != NULL, "failed to create 5×5 grid");

    /* Undirected grid */
    for (int i = 0; i < 25; i++) {
        if ((i % 5) < 4) {
            addWeightedEdge(g, i,   i + 1, 1, 1);
        }
        if (i < 20) {
            addWeightedEdge(g, i,   i + 5, 1, 1);
        }
    }

    int path[30] = {0};
    int dist = -1;
    int len = astar_shortest_path(g, 0, 24, path, 30,
                                  &dist, simple_manhattan_heuristic);

    TEST_CHECK_(len > 0, "A* with Manhattan should find path from 0 to 24");
    TEST_CHECK_(dist == 8,
                "Manhattan heuristic should give optimal cost (4 right + 4 down)");
    TEST_CHECK_(path[0] == 0 && path[len - 1] == 24,
                "path should start at 0 and end at 24");
    TEST_CHECK_(len <= 9,
                "path should not be longer than Manhattan distance + 1");

    graph_free(g);
}

void test_astar_euclidean(void) {
    Graph *g = createGraph(25);
    TEST_ASSERT_(g != NULL, "failed to create 5×5 grid");

    for (int i = 0; i < 25; i++) {
        if ((i % 5) < 4) {
            addWeightedEdge(g, i,   i + 1, 1, 1);
        }
        if (i < 20) {
            addWeightedEdge(g, i,   i + 5, 1, 1);
        }
    }

    int path_m[30] = {0}, path_e[30] = {0};
    int dist_m = -1, dist_e = -1;

    int len_e = astar_shortest_path(g, 0, 24, path_e, 30,
                                    &dist_e, simple_euclidean_heuristic);
    int len_m = astar_shortest_path(g, 0, 24, path_m, 30,
                                    &dist_m, simple_manhattan_heuristic);

    TEST_CHECK_(len_m > 0 && len_e > 0, "both heuristics should find path");
    TEST_CHECK_(dist_m == 8 && dist_e == 8, "both should give optimal cost 8");
    TEST_CHECK_(len_m == len_e,
                "both should find same-length path (optimal)");
    TEST_CHECK_(dist_e == dist_m,
                "Euclidean should not overestimate in grid");

    graph_free(g);
}

void test_dijkstra_large_graph(void) {
    Graph *g = createGraph(30);
    TEST_ASSERT_(g != NULL, "failed to create 30-vertex graph");

    /* 5×6 grid (mostly) */
    for (int i = 0; i < 29; i++) {
        if (i % 6 != 5) addWeightedEdge(g, i, i + 1, 1, 0); /* right */
        if (i < 24)     addWeightedEdge(g, i, i + 6, 1, 0); /* down */
    }

    /* Some shortcuts */
    addWeightedEdge(g, 0, 10, 5, 0);
    addWeightedEdge(g, 15, 25, 3, 0);

    int path[40] = {0};
    int dist = -1;
    int len = dijkstra_shortest_path(g, 0, 29, path, 40, &dist);

    TEST_CHECK_(len > 0, "path from 0 to 29 should exist");
    TEST_CHECK_(path[0] == 0 && path[len - 1] == 29,
                "path must start at 0 and end at 29");
    TEST_CHECK_(dist >= 8,
                "minimal distance in 5×6 grid is 8 – shortcuts may reduce it");

    graph_free(g);
}

void test_bellman_ford_negative_weight(void) {
    Graph *g = createGraph(4);
    TEST_ASSERT_(g != NULL, "failed to create graph");

    addWeightedEdge(g, 0, 1, 6, 0);
    addWeightedEdge(g, 1, 2, 5, 0);
    addWeightedEdge(g, 2, 3, -4, 0); /* negative edge */
    addWeightedEdge(g, 0, 3, 8, 0);

    int path[10] = {0};
    int dist = -1;
    int len = bellman_ford_shortest_path(g, 0, 3, path, 10, &dist);

    TEST_CHECK_(len > 0, "Bellman-Ford should find a path");
    TEST_CHECK_(dist == 7,
                "shortest path should be 6 + 5 + (-4) = 7");
    TEST_CHECK_(path[len - 1] == 3, "path must reach goal");

    graph_free(g);
}

void test_bellman_ford_negative_cycle(void) {
    Graph *g = createGraph(3);
    TEST_ASSERT_(g != NULL, "failed to create graph");

    addWeightedEdge(g, 0, 1, 1, 0);
    addWeightedEdge(g, 1, 2, 1, 0);
    addWeightedEdge(g, 2, 1, -3, 0); /* negative cycle 1 ↔ 2 */

    int path[10] = {0};
    int dist = 999;
    int len = bellman_ford_shortest_path(g, 0, 2, path, 10, &dist);

    TEST_CHECK_(len == 0,
                "negative cycle reachable from start → no valid shortest path");
    TEST_CHECK_(dist == -1,
                "distance must be set to -1 when negative cycle detected");

    graph_free(g);
}

void test_invalid_inputs(void) {
    Graph *g = createGraph(3);
    TEST_ASSERT_(g != NULL, "failed to create graph");

    int path[5] = {0};
    int dist = 999;

    /* Invalid vertex indices */
    TEST_CHECK_(bfs_shortest_hops(g, -1, 1, path, 5, &dist) == 0,
                "negative start rejected");
    TEST_CHECK_(bfs_shortest_hops(g, 0, 5, path, 5, &dist) == 0,
                "out-of-range goal rejected");

    /* NULL graph */
    TEST_CHECK_(dijkstra_shortest_path(NULL, 0, 1, path, 5, &dist) == 0,
                "NULL graph rejected");
    TEST_CHECK_(bellman_ford_shortest_path(NULL, 0, 2, path, 5, &dist) == 0,
                "NULL graph rejected");

    /* Small buffer */
    TEST_CHECK_(bfs_shortest_hops(g, 0, 1, path, 1, &dist) == 0,
                "buffer too small rejected");

    graph_free(g);
}

void test_path_buffer_overflow(void) {
    Graph *g = createGraph(15);
    TEST_ASSERT_(g != NULL, "failed to create graph");

    /* Long chain: 0 → 1 → 2 → ... → 14 */
    for (int i = 0; i < 14; i++) {
        addWeightedEdge(g, i, i + 1, 1, 0);
    }

    int path[5] = {0}; /* buffer too small for 15-vertex path */
    int dist = -1;
    int len = dijkstra_shortest_path(g, 0, 14, path, 5, &dist);

    TEST_CHECK_(len == 0, "should return 0 when path buffer is too small");
    TEST_CHECK_(dist == -1, "distance should be -1 on buffer overflow");
    TEST_CHECK_(path[0] == 0, "path[0] should remain unchanged on failure");

    graph_free(g);
}