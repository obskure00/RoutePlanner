#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/cli.h"
#include "../include/graph.h"
#include "../include/io.h"
#include "../include/algorithms.h"

Algorithm cli_select_algorithm(void) {
    int choice = -1;

    while (1) {
        printf("\nSelect routing mode:\n");
        printf("  1) Fewest hops (BFS)\n");
        printf("  2) Any path (DFS)\n");
        printf("  3) Shortest travel time (Dijkstra – non-negative weights)\n");
        printf("  4) Shortest travel time with heuristic (A*)\n");
        printf("     4.1) No heuristic (same as Dijkstra)\n");
        printf("     4.2) Manhattan heuristic\n");
        printf("     4.3) Euclidean heuristic\n");
        printf("  5) Shortest time with negative weights (Bellman-Ford)\n");
        printf("  0) Exit\n");
        printf("Enter choice (0–5 or 4.1–4.3): ");

        char input[16];
        if (!fgets(input, sizeof(input), stdin)) {
            printf("Input error.\n");
            continue;
        }

        if (strncmp(input, "4.1", 3) == 0) {
            choice = ALGO_ASTAR_NONE;
        } else if (strncmp(input, "4.2", 3) == 0) {
            choice = ALGO_ASTAR_MANHATTAN;
        } else if (strncmp(input, "4.3", 3) == 0) {
            choice = ALGO_ASTAR_EUCLIDEAN;
        } else if (sscanf(input, "%d", &choice) == 1) {
            // normal 0-5
        } else {
            printf("Invalid input.\n");
            continue;
        }

        if (choice >= 0 && choice <= 5) break;
        if (choice >= ALGO_ASTAR_NONE && choice <= ALGO_ASTAR_EUCLIDEAN) break;

        printf("Please enter a valid choice.\n");
    }

    return (Algorithm)choice;
}

static int cli_prompt_vertex_number(const Graph *g, const char *prompt) {
    int id;

    while (1) {
        printf("%s (numeric ID 0–%d): ", prompt, g->numVertices - 1);
        if (scanf("%d", &id) != 1) {
            while (getchar() != '\n' && getchar() != EOF);
            printf("Invalid number. Try again.\n");
            continue;
        }

        if (id >= 0 && id < g->numVertices) {
            while (getchar() != '\n' && getchar() != EOF);
            return id;
        }

        printf("ID must be between 0 and %d. Try again.\n", g->numVertices - 1);
    }
}

static int cli_prompt_vertex_name(const Graph *g, const char *prompt) {
    char buf[256];

    while (1) {
        printf("%s (exact name): ", prompt);

        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("Input error. Try again.\n");
            continue;
        }

        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }

        if (buf[0] == '\0') {
            printf("Name cannot be empty. Try again.\n");
            continue;
        }

        int id = graph_find_vertex_by_name(g, buf);
        if (id >= 0) {
            printf("Found: \"%s\" → ID %d\n", buf, id);
            return id;
        }

        printf("No vertex named \"%s\". Try again.\n", buf);
    }
}

int cli_prompt_vertex(const Graph *g, const char *prompt) {
    if (!g || g->numVertices <= 0) {
        io_print_error("No vertices available in graph");
        return -1;
    }

    while (1) {
        printf("\nAvailable vertices:\n");
        for (int i = 0; i < g->numVertices; i++) {
            const char *name = graphGetName(g, i);
            printf("  %3d : %s\n", i, name ? name : "(unnamed)");
        }

        printf("\nSelect %s by:\n", prompt);
        printf("  1 = numeric ID\n");
        printf("  2 = name (exact match)\n");
        printf("Choice (1 or 2): ");

        int mode;
        if (scanf("%d", &mode) != 1) {
            while (getchar() != '\n' && getchar() != EOF);
            printf("Invalid input.\n");
            continue;
        }

        while (getchar() != '\n' && getchar() != EOF);

        if (mode == 1) {
            return cli_prompt_vertex_number(g, prompt);
        }
        if (mode == 2) {
            return cli_prompt_vertex_name(g, prompt);
        }

        printf("Please enter 1 or 2.\n");
    }
}

void cli_main_loop(Graph *g) {
    if (!g) {
        io_print_error("No graph loaded");
        return;
    }

    while (1) {
        Algorithm algo = cli_select_algorithm();

        if (algo == ALGO_NONE) {
            printf("Exiting route planner. Goodbye!\n");
            break;
        }

        int start = cli_prompt_vertex(g, "Start vertex");
        if (start < 0) continue;

        int goal = cli_prompt_vertex(g, "Goal vertex");
        if (goal < 0) continue;

        if (start == goal) {
            const char *name = graphGetName(g, start);
            printf("Start and goal are the same vertex.\n");
            printf("Trivial path: %s (ID %d)\n", name ? name : "(unnamed)", start);
            continue;
        }

        PathResult res = {0};

        switch (algo) {
            case ALGO_BFS:
                res = pathresult_from_bfs(g, start, goal);
                break;

            case ALGO_DFS:
                res = pathresult_from_dfs(g, start, goal);
                break;

            case ALGO_DIJKSTRA:
                res = pathresult_from_dijkstra(g, start, goal);
                break;

            case ALGO_ASTAR_NONE:
                res = pathresult_from_astar(g, start, goal, NULL);
                break;

            case ALGO_ASTAR_MANHATTAN:
                res = pathresult_from_astar(g, start, goal, simple_manhattan_heuristic);
                break;

            case ALGO_ASTAR_EUCLIDEAN:
                res = pathresult_from_astar(g, start, goal, simple_euclidean_heuristic);
                break;

            case ALGO_BELLMAN_FORD:
                res = pathresult_from_bellman_ford(g, start, goal);
                break;

            default:
                io_print_error("Algorithm not implemented in CLI");
                continue;
        }

        print_path_result(g, &res, algo);
        pathresult_free(&res);
    }
}