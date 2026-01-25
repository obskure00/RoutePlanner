#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/graph.h"

Graph *createGraph(int vertices) {
    if (vertices <= 0) {
        return NULL;
    }

    Graph *graph = malloc(sizeof(Graph));
    if (!graph) {
        return NULL;
    }

    graph->numVertices = vertices;
    graph->array = calloc(vertices, sizeof(AdjList));
    graph->names = calloc(vertices, sizeof(char *));
    if (!graph->array || !graph->names) {
        free(graph->array);
        free(graph->names);
        free(graph);
        return NULL;
    }

    return graph;
}

static Edge *createEdge(int destination, int weight) {
    Edge *e = malloc(sizeof(Edge));
    if (!e) {
        return NULL;
    }

    e->to = destination;
    e->weight = weight;
    e->next = NULL;
    return e;
}

void addWeightedEdge(Graph *graph, int source, int destination, int weight, int undirected) {
    if (!graph) {
        return;
    }

    if (source < 0 || destination < 0 || source >= graph->numVertices || destination >= graph->numVertices) {
        return;
    }

    Edge *v1 = createEdge(destination, weight);
    if (!v1) {
        return;
    }

    Edge *v2 = NULL;
    if (undirected) {
        v2 = createEdge(source, weight);
        if (!v2) {
            free(v1);
            return;
        }
    }

    v1->next = graph->array[source].head;
    graph->array[source].head = v1;

    if (undirected) {
        v2->next = graph->array[destination].head;
        graph->array[destination].head = v2;
    }
}

void addEdge(Graph *graph, int source, int destination, int undirected) {
    addWeightedEdge(graph, source, destination, 1, undirected);
}

void graphSetName(Graph *graph, int id, const char *name) {
    if (!graph || !name || id < 0 || id >= graph->numVertices) {
        return;
    }

    free(graph->names[id]);
    graph->names[id] = strdup(name);
}

const char *graphGetName(const Graph *graph, int id) {
    if (!graph || id < 0 || id >= graph->numVertices) {
        return NULL;
    }

    return graph->names[id];
}

int graph_find_vertex_by_name(const Graph *graph, const char *name) {
    if (!graph || !name) {
        return -1;
    }

    for (int i = 0; i < graph->numVertices; i++) {
        if (graph->names[i] &&
            strcmp(graph->names[i], name) == 0) {
            return i;
        }
    }

    return -1;
}

void printGraph(const Graph *graph) {
    if (!graph) return;

    for (int i = 0; i < graph->numVertices; i++) {
        printf("Vertex %d", i);
        const char *name = graphGetName(graph, i);
        if (name) {
            printf(" (%s)", name);
        }

        printf(": ");
        int first = 1;
        for (Edge *e = graph->array[i].head; e != NULL; e = e->next) {
            if (!first) {
                printf(" ");
            }
            printf("-> %d(w=%d)", e->to, e->weight);
            first = 0;
        }
        printf("\n");
    }
}

void graph_free(Graph *graph) {
    if (!graph) return;

    for (int i = 0; i < graph->numVertices; i++) {
        Edge *current = graph->array[i].head;
        while (current) {
            Edge *temp = current;
            current = current->next;
            free(temp);
        }
        free(graph->names[i]);
    }

    free(graph->array);
    free(graph->names);
    free(graph);
}