#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/graph.h"
#include "../include/algorithms.h"
#include "../include/io.h"

/* ============================================================================
   Internal helpers
   ========================================================================== */

static void parse_error(const char *msg, int line) {
    fprintf(stderr, "Parse error (line %d): %s\n", line, msg);
}

static int read_data_line(FILE *fp, char *buf, size_t size, int *line) {
    while (fgets(buf, size, fp)) {
        (*line)++;
        if (buf[0] == '\n' || buf[0] == '\r' || buf[0] == '\0') continue;
        if (buf[0] == '#') continue;
        char *p = strchr(buf, '\n');
        if (p) *p = '\0';
        return 1;
    }
    return 0;
}

/* ============================================================================
   Graph loading from file
   ========================================================================== */

Graph *load_graph_from_file(const char *filename) {
    if (!filename) {
        io_print_error("No filename provided");
        return NULL;
    }
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        io_print_error("Failed to open graph file");
        return NULL;
    }
    
    char buf[512];
    int line = 0;
    int num_vertices = 0, num_edges = 0, directed = 0;
    
    if (!read_data_line(fp, buf, sizeof(buf), &line)) {
        parse_error("Missing graph header", line);
        fclose(fp);
        return NULL;
    }
    
    int fields = sscanf(buf, "%d %d %d", &num_vertices, &num_edges, &directed);
    if (fields < 2 || num_vertices <= 0 || num_edges < 0) {
        parse_error("Invalid graph header", line);
        fclose(fp);
        return NULL;
    }
    
    Graph *g = createGraph(num_vertices);
    if (!g) {
        io_print_error("Graph allocation failed");
        fclose(fp);
        return NULL;
    }
    
    for (int i = 0; i < num_vertices; i++) {
        if (!read_data_line(fp, buf, sizeof(buf), &line)) {
            parse_error("Unexpected EOF while reading vertices", line);
            graph_free(g);
            fclose(fp);
            return NULL;
        }
        
        int id;
        char name[256];
        if (sscanf(buf, "%d %255s", &id, name) != 2 ||
            id < 0 || id >= num_vertices) {
            parse_error("Invalid vertex definition", line);
            graph_free(g);
            fclose(fp);
            return NULL;
        }
        graphSetName(g, id, name);
    }
    
    int edges_read = 0;
    while (edges_read < num_edges && read_data_line(fp, buf, sizeof(buf), &line)) {
        int from, to, weight;
        if (sscanf(buf, "%d %d %d", &from, &to, &weight) != 3 ||
            from < 0 || from >= num_vertices ||
            to < 0 || to >= num_vertices) {
            parse_error("Invalid edge definition", line);
            graph_free(g);
            fclose(fp);
            return NULL;
        }
        
        addWeightedEdge(g, from, to, weight, !directed);
        edges_read++;
    }
    
    if (edges_read != num_edges) {
        parse_error("Edge count mismatch", line);
        graph_free(g);
        fclose(fp);
        return NULL;
    }
    
    fclose(fp);
    return g;
}

/* ============================================================================
   PathResult wrappers for each algorithm
   ========================================================================== */

PathResult pathresult_from_bfs(Graph *g, int start, int goal) {
    PathResult res = {0};
    if (!g) return res;

    int bufsize = g->numVertices + 1;
    int *path = malloc(bufsize * sizeof(int));
    if (!path) return res;

    int distance = -1;
    int len = bfs_shortest_hops(g, start, goal, path, bufsize, &distance);

    if (len > 0 && len <= bufsize) {
        res.found     = 1;
        res.path      = realloc(path, len * sizeof(int));
        res.path_len  = len;
        res.distance  = distance;
    } else {
        free(path);
    }
    return res;
}

PathResult pathresult_from_dfs(Graph *g, int start, int goal) {
    PathResult res = {0};
    if (!g) return res;

    int bufsize = g->numVertices + 1;
    int *path = malloc(bufsize * sizeof(int));
    if (!path) return res;

    int len = dfs_path(g, start, goal, path, bufsize);

    if (len > 0 && len <= bufsize) {
        res.found     = 1;
        res.path      = realloc(path, len * sizeof(int));
        res.path_len  = len;
        res.distance  = len - 1;
    } else {
        free(path);
    }
    return res;
}

PathResult pathresult_from_dijkstra(Graph *g, int start, int goal) {
    PathResult res = {0};
    if (!g) return res;

    int bufsize = g->numVertices + 1;
    int *path = malloc(bufsize * sizeof(int));
    if (!path) return res;

    int distance = -1;
    int len = dijkstra_shortest_path(g, start, goal, path, bufsize, &distance);

    if (len > 0 && len <= bufsize) {
        res.found     = 1;
        res.path      = realloc(path, len * sizeof(int));
        res.path_len  = len;
        res.distance  = distance;
    } else {
        free(path);
    }
    return res;
}

PathResult pathresult_from_astar(Graph *g, int start, int goal, int (*heuristic)(int, int)) {
    PathResult res = {0};
    if (!g) return res;

    int bufsize = g->numVertices + 1;
    int *path = malloc(bufsize * sizeof(int));
    if (!path) return res;

    int distance = -1;
    int len = astar_shortest_path(g, start, goal, path, bufsize, &distance, heuristic);

    if (len > 0 && len <= bufsize) {
        res.found     = 1;
        res.path      = realloc(path, len * sizeof(int));
        res.path_len  = len;
        res.distance  = distance;
    } else {
        free(path);
    }
    return res;
}

PathResult pathresult_from_bellman_ford(Graph *g, int start, int goal) {
    PathResult res = {0};
    if (!g) return res;

    int bufsize = g->numVertices + 1;
    int *path = malloc(bufsize * sizeof(int));
    if (!path) return res;

    int distance = -1;
    int len = bellman_ford_shortest_path(g, start, goal, path, bufsize, &distance);

    if (len > 0 && len <= bufsize) {
        res.found     = 1;
        res.path      = realloc(path, len * sizeof(int));
        res.path_len  = len;
        res.distance  = distance;
    } else {
        free(path);
    }
    return res;
}

/* ============================================================================
   Output & cleanup
   ========================================================================== */

void pathresult_free(PathResult *res) {
    if (!res) return;
    free(res->path);
    res->path = NULL;
    res->path_len = 0;
    res->distance = -1;
    res->found = 0;
}

void print_path_result(Graph *g, const PathResult *res, int algo_type) {
    if (!g || !res) {
        io_print_error("Invalid graph or result");
        return;
    }

    if (!res->found || res->path_len <= 0) {
        printf("No path found from start to goal.\n");
        return;
    }

    printf("Path found (%d vertices): ", res->path_len);
    for (int i = 0; i < res->path_len; i++) {
        int v = res->path[i];
        const char *name = graphGetName(g, v);
        printf("%s", name ? name : "(unnamed)");
        if (i < res->path_len - 1) printf(" → ");
    }
    printf("\n");

    if (algo_type == 1 || algo_type == 2) {
        int hops = res->distance;

        int real_time = 0;
        bool all_edges_found = true;

        for (int i = 0; i < res->path_len - 1; i++) {
            int u = res->path[i];
            int v = res->path[i + 1];
            bool edge_found = false;

            for (Edge *e = g->array[u].head; e != NULL; e = e->next) {
                if (e->to == v) {
                    real_time += e->weight;
                    edge_found = true;
                    break;
                }
            }

            if (!edge_found) {
                printf("Warning: no direct edge found between %d (%s) and %d (%s)\n",
                       u, graphGetName(g, u) ?: "?", v, graphGetName(g, v) ?: "?");
                all_edges_found = false;
            }
        }

        printf("Hops: %d → ", hops);
        if (all_edges_found) {
            printf("actual travel time along this path: %d minutes\n", real_time);
        } else {
            printf("could not calculate exact time (missing some edges)\n");
        }

        printf("Tip: For shortest travel time use Dijkstra or A* (option 3 or 4)\n");
    } 
    else {
        printf("Travel time: %d minutes (optimal)\n", res->distance);
    }
}

void io_print_error(const char *msg) {
    if (msg && msg[0]) {
        fprintf(stderr, "Error: %s\n", msg);
    }
}