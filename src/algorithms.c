#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <math.h>

#include "../include/graph.h"
#include "../include/algorithms.h"

/* ============================================================================
   Shared helper: reconstruct path from parent array
   Returns number of vertices in path or 0 if invalid/no path
   ========================================================================== */

static int reconstruct_path(int start, int goal, const int *parent, int *out_path, int max_len, int num_vertices) {
    if (!parent || !out_path || max_len < 2) {
        return 0;
    }

    int *temp = malloc(num_vertices * sizeof(int));
    if (!temp) {
        return 0;
    }

    int length = 0;
    int at = goal;

    while (at != -1 && length < num_vertices) {
        temp[length++] = at;
        if (at == start) {
            break;
        }
        at = parent[at];
    }

    if (length == 0 || temp[length - 1] != start) {
        free(temp);
        return 0;
    }

    if (length > max_len) {
        free(temp);
        return 0;
    }

    for (int i = 0; i < length; i++) {
        out_path[i] = temp[length - 1 - i];
    }

    free(temp);
    return length;
}

/* ============================================================================
   BFS – fewest hops (unweighted / uniform cost 1)
   ========================================================================== */

int bfs_shortest_hops(Graph *graph, int start, int goal, int *out_path, int max_path_len, int *out_distance) {
    if (!graph || !out_path || !out_distance || start < 0 || goal < 0 || start >= graph->numVertices || goal >= graph->numVertices || max_path_len < 2) {
        if (out_distance) *out_distance = -1;
        return 0;
    }

    int n = graph->numVertices;

    bool *visited = calloc(n, sizeof(bool));
    int  *parent  = malloc(n * sizeof(int));
    int  *queue   = malloc(n * sizeof(int));
    if (!visited || !parent || !queue) {
        free(visited);
        free(parent);
        free(queue);
        if (out_distance) *out_distance = -1;
        return 0;
    }

    for (int i = 0; i < n; i++) parent[i] = -1;

    int front = 0, rear = 0;
    visited[start] = true;
    queue[rear++] = start;

    while (front < rear) {
        int v = queue[front++];

        if (v == goal) {
            break;
        }

        for (Edge *e = graph->array[v].head; e != NULL; e = e->next) {
            int u = e->to;
            if (!visited[u]) {
                visited[u] = true;
                parent[u] = v;
                queue[rear++] = u;
            }
        }
    }

    int path_len = reconstruct_path(start, goal, parent, out_path, max_path_len, n);

    free(queue);
    free(visited);
    free(parent);

    if (path_len == 0) {
        if (out_distance) *out_distance = -1;
        return 0;
    }

    if (out_distance) *out_distance = path_len - 1; /* hops */
    return path_len;
}

/* ============================================================================
   DFS – any path (not necessarily shortest)
   ========================================================================== */

static bool dfs_recursive(Graph *graph, int v, int goal, bool *visited, int *parent) {
    visited[v] = true;
    if (v == goal) return true;

    for (Edge *e = graph->array[v].head; e; e = e->next) {
        int u = e->to;
        if (!visited[u]) {
            parent[u] = v;
            if (dfs_recursive(graph, u, goal, visited, parent)) {
                return true;
            }
        }
    }
    return false;
}

int dfs_path(Graph *graph, int start, int goal, int *out_path, int max_path_len) {
    if (!graph || !out_path || start < 0 || goal < 0 || start >= graph->numVertices || goal >= graph->numVertices || max_path_len < 2) {
        return 0;
    }

    int n = graph->numVertices;

    bool *visited = calloc(n, sizeof(bool));
    int  *parent  = malloc(n * sizeof(int));
    if (!visited || !parent) {
        free(visited);
        free(parent);
        return 0;
    }

    for (int i = 0; i < n; i++) parent[i] = -1;

    bool found = dfs_recursive(graph, start, goal, visited, parent);
    int path_len = 0;

    if (found) {
        path_len = reconstruct_path(start, goal, parent, out_path, max_path_len, n);
    }

    free(visited);
    free(parent);
    return path_len;
}

/* ============================================================================
   Dijkstra – shortest weighted path (non-negative weights)
   ========================================================================== */

int dijkstra_shortest_path(Graph *graph, int start, int goal, int *out_path, int max_path_len, int *out_distance) {
    if (!graph || !out_path || !out_distance || start < 0 || goal < 0 || start >= graph->numVertices || goal >= graph->numVertices || max_path_len < 2) {
        if (out_distance) *out_distance = -1;
        return 0;
    }

    int n = graph->numVertices;

    int  *dist    = malloc(n * sizeof(int));
    int  *parent  = malloc(n * sizeof(int));
    bool *visited = calloc(n, sizeof(bool));
    if (!dist || !parent || !visited) {
        free(dist);
        free(parent);
        free(visited);
        if (out_distance) *out_distance = -1;
        return 0;
    }

    for (int i = 0; i < n; i++) {
        dist[i]   = INT_MAX;
        parent[i] = -1;
    }

    dist[start] = 0;

    for (int count = 0; count < n; count++) {
        int u = -1;
        int min_dist = INT_MAX;

        for (int v = 0; v < n; v++) {
            if (!visited[v] && dist[v] < min_dist) {
                min_dist = dist[v];
                u = v;
            }
        }

        if (u == -1) break;
        visited[u] = true;
        if (u == goal) break;

        for (Edge *e = graph->array[u].head; e != NULL; e = e->next) {
            int v = e->to;
            int w = e->weight;
            if (w < 0) {
                continue;
            }

            if (!visited[v] && dist[u] != INT_MAX) {
                long new_dist = (long)dist[u] + w;
                if (new_dist < dist[v]) {
                    dist[v] = (new_dist > INT_MAX) ? INT_MAX : (int)new_dist;
                    parent[v] = u;
                }
            }
        }
    }

    if (dist[goal] == INT_MAX) {
        if (out_distance) *out_distance = -1;
        free(dist);
        free(parent);
        free(visited);
        return 0;
    }

    int path_len = reconstruct_path(start, goal, parent, out_path, max_path_len, n);
    if (path_len == 0) {
        if (out_distance) *out_distance = -1;
        free(dist);
        free(parent);
        free(visited);
        return 0;
    }

    if (out_distance) *out_distance = dist[goal];
    free(dist);
    free(parent);
    free(visited);
    return path_len;
}

/* ============================================================================
   A* – shortest path with heuristic (falls back to Dijkstra if heuristic=NULL)
   ========================================================================== */

#ifndef GRID_WIDTH
#define GRID_WIDTH 5
#endif

int simple_manhattan_heuristic(int a, int b) {
    int ax = a % GRID_WIDTH;
    int ay = a / GRID_WIDTH;
    int bx = b % GRID_WIDTH;
    int by = b / GRID_WIDTH;
    return abs(ax - bx) + abs(ay - by);
}

int simple_euclidean_heuristic(int a, int b) {
    int ax = a % GRID_WIDTH, ay = a / GRID_WIDTH;
    int bx = b % GRID_WIDTH, by = b / GRID_WIDTH;
    int dx = ax - bx;
    int dy = ay - by;
    return (int)(sqrt(dx * dx + dy * dy) + 0.5);
}

/* --- Minimal binary min-heap for A* (priority = f_score) --- */

typedef struct {
    int node;
    int priority;
} PQNode;

typedef struct {
    PQNode *nodes;
    int    *heap_index; /* node -> index in heap, -1 if not in heap */
    int capacity;
    int size;
} PriorityQueue;

static PriorityQueue *pq_create(int capacity) {
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    if (!pq) return NULL;

    pq->nodes = malloc(capacity * sizeof(PQNode));
    pq->heap_index = malloc(capacity * sizeof(int));
    pq->capacity = capacity;
    pq->size = 0;

    if (!pq->nodes || !pq->heap_index) {
        free(pq->nodes);
        free(pq->heap_index);
        free(pq);
        return NULL;
    }

    for (int i = 0; i < capacity; i++) {
        pq->heap_index[i] = -1;
    }

    return pq;
}

static void pq_destroy(PriorityQueue *pq) {
    if (pq) {
        free(pq->nodes);
        free(pq->heap_index);
        free(pq);
    }
}

static void pq_swap(PriorityQueue *pq, int i, int j) {
    PQNode temp = pq->nodes[i];
    pq->nodes[i] = pq->nodes[j];
    pq->nodes[j] = temp;

    pq->heap_index[pq->nodes[i].node] = i;
    pq->heap_index[pq->nodes[j].node] = j;
}

static void pq_heapify_up(PriorityQueue *pq, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (pq->nodes[parent].priority <= pq->nodes[idx].priority) break;
        pq_swap(pq, parent, idx);
        idx = parent;
    }
}

static void pq_heapify_down(PriorityQueue *pq, int idx) {
    while (1) {
        int min_idx = idx;
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;

        if (left < pq->size &&
            pq->nodes[left].priority < pq->nodes[min_idx].priority) {
            min_idx = left;
        }
        if (right < pq->size &&
            pq->nodes[right].priority < pq->nodes[min_idx].priority) {
            min_idx = right;
        }

        if (min_idx == idx) break;
        pq_swap(pq, idx, min_idx);
        idx = min_idx;
    }
}

static int pq_insert(PriorityQueue *pq, int node, int priority) {
    if (pq->size >= pq->capacity) return -1;
    pq->nodes[pq->size].node = node;
    pq->nodes[pq->size].priority = priority;
    pq->heap_index[node] = pq->size;
    pq_heapify_up(pq, pq->size);
    pq->size++;
    return 0;
}

static int pq_decrease_key(PriorityQueue *pq, int node, int new_priority) {
    int idx = pq->heap_index[node];
    if (idx < 0 || idx >= pq->size) return -1;
    if (new_priority >= pq->nodes[idx].priority) return 0;
    pq->nodes[idx].priority = new_priority;
    pq_heapify_up(pq, idx);
    return 0;
}

static int pq_pop_min(PriorityQueue *pq, int *node_out, int *priority_out) {
    if (pq->size == 0) return -1;

    *node_out = pq->nodes[0].node;
    *priority_out = pq->nodes[0].priority;

    pq->heap_index[*node_out] = -1;

    pq->size--;
    if (pq->size > 0) {
        pq->nodes[0] = pq->nodes[pq->size];
        pq->heap_index[pq->nodes[0].node] = 0;
        pq_heapify_down(pq, 0);
    }

    return 0;
}

static bool pq_contains(PriorityQueue *pq, int node) {
    int idx = pq->heap_index[node];
    return (idx >= 0 && idx < pq->size);
}

/* ============================================================================
   A* – shortest path with heuristic
   ========================================================================== */

int astar_shortest_path(Graph *graph, int start, int goal, int *out_path, int max_path_len, int *out_distance, int (*heuristic)(int, int)) {
    if (!graph || !out_path || !out_distance || start < 0 || goal < 0 || start >= graph->numVertices || goal >= graph->numVertices || max_path_len < 2) {
        if (out_distance) *out_distance = -1;
        return 0;
    }

    int n = graph->numVertices;

    int *dist    = malloc(n * sizeof(int));
    int *parent  = malloc(n * sizeof(int));
    int *f_score = malloc(n * sizeof(int));
    PriorityQueue *pq = pq_create(n);

    if (!dist || !parent || !f_score || !pq) {
        free(dist);
        free(parent);
        free(f_score);
        pq_destroy(pq);
        if (out_distance) *out_distance = -1;
        return 0;
    }

    for (int i = 0; i < n; i++) {
        dist[i] = INT_MAX;
        f_score[i] = INT_MAX;
        parent[i] = -1;
    }

    dist[start] = 0;
    f_score[start] = heuristic ? heuristic(start, goal) : 0;
    pq_insert(pq, start, f_score[start]);

    while (pq->size > 0) {
        int current, current_f;
        if (pq_pop_min(pq, &current, &current_f) != 0) {
            break;
        }

        if (current == goal) {
            int path_len = reconstruct_path(start, goal, parent,
                                            out_path, max_path_len, n);
            if (path_len == 0) {
                if (out_distance) *out_distance = -1;
            } else {
                if (out_distance) *out_distance = dist[goal];
            }
            free(dist);
            free(parent);
            free(f_score);
            pq_destroy(pq);
            return path_len;
        }

        for (Edge *e = graph->array[current].head; e != NULL; e = e->next) {
            int nei = e->to;
            int w   = e->weight;
            if (w < 0) {
                continue;
            }
            if (dist[current] == INT_MAX) continue;

            long tentative = (long)dist[current] + w;
            if (tentative < dist[nei]) {
                dist[nei] = (tentative > INT_MAX) ? INT_MAX : (int)tentative;
                parent[nei] = current;
                int h = heuristic ? heuristic(nei, goal) : 0;
                f_score[nei] = dist[nei] + h;

                if (pq_contains(pq, nei)) {
                    pq_decrease_key(pq, nei, f_score[nei]);
                } else {
                    pq_insert(pq, nei, f_score[nei]);
                }
            }
        }
    }

    if (out_distance) *out_distance = -1;
    free(dist);
    free(parent);
    free(f_score);
    pq_destroy(pq);
    return 0;
}

/* ============================================================================
   Bellman-Ford – handles negative weights + detects negative cycles reachable
   from start
   ========================================================================== */

int bellman_ford_shortest_path(Graph *graph, int start, int goal, int *out_path, int max_path_len, int *out_distance) {
    if (!graph || !out_path || !out_distance) {
        if (out_distance) *out_distance = -1;
        return 0;
    }

    if (start < 0 || goal < 0 ||
        start >= graph->numVertices || goal >= graph->numVertices ||
        max_path_len <= 0) {
        *out_distance = -1;
        return 0;
    }

    if (start == goal) {
        out_path[0] = start;
        *out_distance = 0;
        return 1;
    }

    int n = graph->numVertices;

    int *dist   = malloc(n * sizeof(int));
    int *parent = malloc(n * sizeof(int));
    if (!dist || !parent) {
        free(dist);
        free(parent);
        *out_distance = -1;
        return 0;
    }

    for (int i = 0; i < n; i++) {
        dist[i] = INT_MAX;
        parent[i] = -1;
    }
    dist[start] = 0;

    for (int iteration = 0; iteration < n - 1; iteration++) {
        bool changed = false;

        for (int u = 0; u < n; u++) {
            if (dist[u] == INT_MAX) continue;

            Edge *edge = graph->array[u].head;
            while (edge != NULL) {
                int v = edge->to;
                int weight = edge->weight;

                long long new_distance =
                    (long long)dist[u] + (long long)weight;

                if (new_distance < (long long)dist[v]) {
                    if (new_distance < INT_MIN) {
                        dist[v] = INT_MIN;
                    } else if (new_distance > INT_MAX) {
                        dist[v] = INT_MAX;
                    } else {
                        dist[v] = (int)new_distance;
                    }
                    parent[v] = u;
                    changed = true;
                }

                edge = edge->next;
            }
        }

        if (!changed) {
            break;
        }
    }

    bool *affected_by_neg_cycle = calloc(n, sizeof(bool));
    if (!affected_by_neg_cycle) {
        free(dist);
        free(parent);
        *out_distance = -1;
        return 0;
    }

    for (int u = 0; u < n; u++) {
        if (dist[u] == INT_MAX) continue;
        Edge *edge = graph->array[u].head;
        while (edge != NULL) {
            int v = edge->to;
            int weight = edge->weight;

            long long new_distance =
                (long long)dist[u] + (long long)weight;

            if (new_distance < (long long)dist[v]) {
                affected_by_neg_cycle[v] = true;
            }

            edge = edge->next;
        }
    }

    for (int iter = 0; iter < n; iter++) {
        for (int u = 0; u < n; u++) {
            if (!affected_by_neg_cycle[u]) continue;
            Edge *edge = graph->array[u].head;
            while (edge != NULL) {
                affected_by_neg_cycle[edge->to] = true;
                edge = edge->next;
            }
        }
    }

    if (affected_by_neg_cycle[goal]) {
        free(dist);
        free(parent);
        free(affected_by_neg_cycle);
        *out_distance = -1;
        return 0;
    }

    free(affected_by_neg_cycle);

    if (dist[goal] == INT_MAX) {
        free(dist);
        free(parent);
        *out_distance = -1;
        return 0;
    }

    int *temp_path = malloc(n * sizeof(int));
    if (!temp_path) {
        free(dist);
        free(parent);
        *out_distance = -1;
        return 0;
    }

    bool *visited = calloc(n, sizeof(bool));
    if (!visited) {
        free(temp_path);
        free(dist);
        free(parent);
        *out_distance = -1;
        return 0;
    }

    int path_length = 0;
    int current = goal;

    while (current != -1 && path_length < n) {
        if (visited[current]) {
            free(visited);
            free(temp_path);
            free(dist);
            free(parent);
            *out_distance = -1;
            return 0;
        }
        visited[current] = true;

        temp_path[path_length++] = current;
        if (current == start) {
            break;
        }
        current = parent[current];
    }

    free(visited);

    if (path_length == 0 || temp_path[path_length - 1] != start) {
        free(temp_path);
        free(dist);
        free(parent);
        *out_distance = -1;
        return 0;
    }

    if (path_length > max_path_len) {
        free(temp_path);
        free(dist);
        free(parent);
        *out_distance = -1;
        return 0;
    }

    for (int i = 0; i < path_length; i++) {
        out_path[i] = temp_path[path_length - 1 - i];
    }

    *out_distance = dist[goal];

    free(temp_path);
    free(dist);
    free(parent);

    return path_length;
}