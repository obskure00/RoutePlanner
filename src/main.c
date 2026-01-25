#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/graph.h"
#include "../include/io.h"
#include "../include/cli.h"

/**
 * @brief Entry point for the Route Planner application.
 *
 * Usage:
 *   ./route_planner [graph_file]
 *
 * If no graph file is provided, defaults to "data/sample_graph.txt".
 * Loads the graph, shows basic info, starts the interactive CLI,
 * and cleans up properly on exit.
 *
 * @param argc Number of command-line arguments
 * @param argv Argument array (argv[1] = optional graph file path)
 * @return 0 on success, non-zero on error
 */

int main(int argc, char *argv[]) {
    const char *filename = "data/sample_graph.txt";
    Graph *graph = NULL;

    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printf("Usage: %s [graph_file]\n\n", argv[0]);
            printf("Options:\n");
            printf("  graph_file     Path to graph file (default: data/sample_graph.txt)\n");
            printf("  --help, -h     Show this help message\n");
            printf("\n");
            printf("Example:\n");
            printf("  %s data/sample_graph.txt\n", argv[0]);
            printf("  %s my_cities.txt\n", argv[0]);
            return EXIT_SUCCESS;
        }
        filename = argv[1];
    }

    printf("Route Planner\n");
    printf("------------- \n");
    printf("Loading graph from: %s\n", filename);

    graph = load_graph_from_file(filename);
    if (!graph) {
        io_print_error("Failed to load graph file");
        fprintf(stderr, "  → Check file path, format, or content.\n");
        fprintf(stderr, "  → Expected header: num_vertices num_edges directed\n");
        fprintf(stderr, "  → Then vertex lines: id name\n");
        fprintf(stderr, "  → Then edge lines: from to weight\n");
        return EXIT_FAILURE;
    }

    printf("Graph loaded successfully!\n");
    printf("  → %d vertices (cities/places)\n", graph->numVertices);
    printf("  → Use Ctrl+C to exit at any time\n\n");

    printf("Quick graph summary (first few vertices):\n");
    printGraph(graph);

    printf("\nStarting interactive route planner...\n");
    printf("You can now select start and goal cities.\n\n");

    cli_main_loop(graph);

    printf("\nExiting. Freeing graph memory...\n");
    graph_free(graph);

    printf("Goodbye! Safe travels.\n");
    return EXIT_SUCCESS;
}