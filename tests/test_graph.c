#define TEST_NO_MAIN
#include "acutest.h"
#include <test_common.h>
#include <string.h>
#include "../include/graph.h"

/* createGraph / graph_free */

void test_create_and_free_empty_graph(void) {
    Graph *g = createGraph(0);
    TEST_CHECK_(g == NULL, "createGraph(0) must return NULL");

    g = createGraph(-5);
    TEST_CHECK_(g == NULL, "createGraph(negative) must return NULL");

    g = createGraph(1);
    TEST_CHECK_(g != NULL, "createGraph(1) should succeed");
    TEST_CHECK_(g->numVertices == 1, "numVertices should be 1");
    TEST_CHECK_(g->array != NULL, "adjacency array should be allocated");
    TEST_CHECK_(g->array[0].head == NULL, "no edges initially");
    TEST_CHECK_(g->names != NULL, "names array should be allocated");
    TEST_CHECK_(graphGetName(g, 0) == NULL, "initial name should be NULL");

    /* graph_free(NULL) must be safe */
    graph_free(NULL);
    graph_free(g);
}

/* directed / undirected edges */

void test_add_edge_directed_undirected(void) {
    Graph *g = createGraph(4);
    TEST_ASSERT_(g != NULL, "failed to create graph");

    /* directed edge 0 -> 1, weight 5 */
    addWeightedEdge(g, 0, 1, 5, 0);
    TEST_CHECK_(g->array[0].head != NULL, "head should not be NULL");
    TEST_CHECK_(g->array[0].head->to == 1, "to should be 1");
    TEST_CHECK_(g->array[0].head->weight == 5, "weight should be 5");
    TEST_CHECK_(g->array[1].head == NULL, "no back edge for directed");

    /* undirected edge 2 <-> 3, weight 7 */
    addWeightedEdge(g, 2, 3, 7, 1);
    TEST_CHECK_(g->array[2].head != NULL, "2->? head not NULL");
    TEST_CHECK_(g->array[3].head != NULL, "3->? head not NULL");
    TEST_CHECK_(g->array[2].head->to == 3, "2->3 to should be 3");
    TEST_CHECK_(g->array[3].head->to == 2, "3->2 to should be 2");
    TEST_CHECK_(g->array[2].head->weight == 7, "2->3 weight should be 7");
    TEST_CHECK_(g->array[3].head->weight == 7, "3->2 weight should be 7");

    graph_free(g);
}

/* multiple edges between same pair: newest first */

void test_multiple_edges_same_pair(void) {
    Graph *g = createGraph(3);
    TEST_ASSERT_(g != NULL, "failed to create graph");

    addWeightedEdge(g, 0, 1, 4, 0);
    addWeightedEdge(g, 0, 1, 7, 0);

    Edge *e = g->array[0].head;
    TEST_CHECK_(e != NULL, "at least one edge");
    TEST_CHECK_(e->to == 1 && e->weight == 7, "most recent edge first");
    TEST_CHECK_(e->next != NULL, "linked list has second edge");
    TEST_CHECK_(e->next->to == 1 && e->next->weight == 4, "older edge preserved");

    graph_free(g);
}

/* zero and negative weight edges are allowed for Bellman-Ford */

void test_zero_and_negative_weight(void) {
    Graph *g = createGraph(3);
    TEST_ASSERT_(g != NULL, "failed to create graph");

    addWeightedEdge(g, 0, 1, 0, 1);   /* zero-weight undirected */
    addWeightedEdge(g, 1, 2, -5, 1);  /* negative-weight undirected */

    /* Forward edges */
    TEST_CHECK_(g->array[0].head->to == 1, "0->1 exists");
    TEST_CHECK_(g->array[0].head->weight == 0, "0->1 weight 0");
    TEST_CHECK_(g->array[1].head->to == 2, "1->2 exists");
    TEST_CHECK_(g->array[1].head->weight == -5, "1->2 weight -5");

    /* Reverse edges (undirected insertion at head) */
    /* For vertex 1, head is 1->2(-5), next is 1->0(0) */
    TEST_CHECK_(g->array[1].head->next != NULL, "1 has reverse edge for 0");
    TEST_CHECK_(g->array[1].head->next->to == 0, "1->0 reverse of 0->1");
    TEST_CHECK_(g->array[1].head->next->weight == 0, "1->0 weight 0");

    /* Vertex 2, head is 2->1(-5) */
    TEST_CHECK_(g->array[2].head != NULL, "2 has reverse edge for 1");
    TEST_CHECK_(g->array[2].head->to == 1, "2->1 reverse of 1->2");
    TEST_CHECK_(g->array[2].head->weight == -5, "2->1 weight -5");

    graph_free(g);
}

/* name management and search */

void test_name_management(void) {
    Graph *g = createGraph(5);
    TEST_ASSERT_(g != NULL, "failed to create graph");

    graphSetName(g, 0, "Frankfurt");
    graphSetName(g, 0, "Berlin");   /* overwrite */
    graphSetName(g, 4, "Munich");

    TEST_CHECK_(strcmp(graphGetName(g, 0), "Berlin") == 0,
                "name overwritten correctly");
    TEST_CHECK_(strcmp(graphGetName(g, 4), "Munich") == 0,
                "second name set correctly");

    TEST_CHECK_(graph_find_vertex_by_name(g, "Berlin") == 0,
                "find by name works");
    TEST_CHECK_(graph_find_vertex_by_name(g, "Munich") == 4,
                "find second name works");
    TEST_CHECK_(graph_find_vertex_by_name(g, "Hamburg") == -1,
                "non-existing name -> -1");
    TEST_CHECK_(graph_find_vertex_by_name(g, "berlin") == -1,
                "search is case-sensitive");

    graph_free(g);
}

/* invalid ids / null handling for names */

void test_name_null_and_invalid_id(void) {
    Graph *g = createGraph(3);
    TEST_ASSERT_(g != NULL, "failed to create graph");

    /* invalid cases — should be ignored safely */
    graphSetName(g, -1, "Invalid");
    graphSetName(g, 5, "Out of range");
    graphSetName(NULL, 0, "Crash attempt");
    graphSetName(g, 1, NULL); /* should not crash, leaves name as NULL */

    TEST_CHECK_(graphGetName(g, 0) == NULL, "unset name is NULL");
    TEST_CHECK_(graphGetName(g, 1) == NULL, "explicit NULL name leaves NULL");
    TEST_CHECK_(graph_find_vertex_by_name(g, NULL) == -1,
                "NULL name search returns -1");
    TEST_CHECK_(graph_find_vertex_by_name(NULL, "test") == -1,
                "NULL graph search returns -1");

    graph_free(g);
}

/* printGraph basic smoke test */

void test_print_graph_does_not_crash(void) {
    Graph *g = createGraph(4);
    TEST_ASSERT_(g != NULL, "failed to create graph");

    addEdge(g, 0, 1, 0);  /* directed */
    addEdge(g, 1, 2, 1);  /* undirected */
    graphSetName(g, 0, "A");
    graphSetName(g, 2, "C");

    /* Just verify it doesn't crash */
    printGraph(g);
    printGraph(NULL);   /* must be safe */

    graph_free(g);
}

/* Optional: check that addWeightedEdge ignores invalid indices safely */

void test_add_edge_invalid_indices(void) {
    Graph *g = createGraph(3);
    TEST_ASSERT_(g != NULL, "failed to create graph");

    addWeightedEdge(g, -1, 1, 10, 0);
    addWeightedEdge(g, 0, -1, 10, 0);
    addWeightedEdge(g, 0, 3, 10, 0);
    addWeightedEdge(g, 3, 0, 10, 0);

    TEST_CHECK_(g->array[0].head == NULL, "no edges added for invalid indices");
    TEST_CHECK_(g->array[1].head == NULL, "vertex 1 still has no edges");
    TEST_CHECK_(g->array[2].head == NULL, "vertex 2 still has no edges");

    graph_free(g);
}