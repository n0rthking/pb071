/**
 * @file    main.c
 * \mainpage HW04 Documentation
 *
 *
 * On the 'Files' page, there is a list of documented files with brief descriptions.
 *
*/
#include "graph.h"
#include "heap.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long getline(char **line_ptr, size_t *n, FILE *stream)
{
    if (line_ptr == NULL || n == NULL) {
        return -1;
    }

    size_t read_size = 256;
    size_t counter = 0;

    do {
        if ((counter + 1) * read_size > *n) {
            char *old_ptr = *line_ptr;
            *line_ptr = realloc(*line_ptr, *n + read_size);

            if (*line_ptr == NULL) {
                free(old_ptr);
                return -1;
            }

            *n += read_size;
        }

        if (fgets(*line_ptr + (read_size * counter) - counter, read_size, stream) == NULL) {
            break;
        }

        counter++;
    } while (strchr(*line_ptr, '\n') == NULL);

    if (feof(stream) || ferror(stream)) {
        return -1;
    }

    return (long) strlen(*line_ptr);
}

bool int_from_str(char *string, int *number)
{
    char *end_ptr;
    long int converted_num = strtol(string, &end_ptr, 10);

    if (string == end_ptr || *end_ptr != '\0' || converted_num == LONG_MAX || converted_num == LONG_MIN) {
        return false;
    }

    *number = (int) converted_num;

    return true;
}

bool add_nodes_to_graph(FILE *node_file, Graph *graph)
{
    assert(graph != NULL);

    int id;
    bool status = false;
    char *line = NULL;
    size_t allocated = 0;

    while (getline(&line, &allocated, node_file) > 0) {
        char *token = strtok(line, ",");
        int counter = 0;

        while (token != NULL) {
            counter++;

            if (counter == 1) {
                if (!int_from_str(token, &id)) {
                    fprintf(stderr, "node id is not a number\n");
                    goto free_and_return;
                }

                if (id < 0) {
                    fprintf(stderr, "node id is not unsigned\n");
                    goto free_and_return;
                }
            }

            token = strtok(NULL, ",");
        }

        if (counter == 7) {
            if (!graph_insert_node(graph, id)) {
                fprintf(stderr, "could not add node to the graph");
                goto free_and_return;
            }
        } else {
            fprintf(stderr, "invalid file format\n");
            goto free_and_return;
        }
    }

    status = true;

free_and_return:
    free(line);
    return status;
}

bool add_edges_to_graph(FILE *edge_file, Graph *graph)
{
    assert(graph != NULL);

    int num;
    bool status = false;
    char *line = NULL;
    size_t allocated = 0;

    while (getline(&line, &allocated, edge_file) > 0) {
        char *token = strtok(line, ",");
        int counter = 0;
        int result[3];
        int index = 0;

        while (token != NULL) {
            counter++;

            if (counter == 1 || counter == 2 || counter == 4) {
                if (!int_from_str(token, &num)) {
                    fprintf(stderr, "input is not a number\n");
                    goto free_and_return;
                }

                if (num < 0) {
                    fprintf(stderr, "number is not unsigned\n");
                    goto free_and_return;
                }

                result[index] = num;
                index++;
            }

            token = strtok(NULL, ",");
        }

        if (counter == 7) {
            if (!graph_insert_edge(graph, result[0], result[1], result[2])) {
                fprintf(stderr, "could not add edge to the graph");
                goto free_and_return;
            }
        } else {
            fprintf(stderr, "invalid file format\n");
            goto free_and_return;
        }
    }

    status = true;

free_and_return:
    free(line);
    return status;
}

bool dijkstra(Graph *graph, unsigned int source, unsigned int dest)
{
    Heap *heap = heap_new_from_graph(graph);
    if (heap == NULL) {
        fprintf(stderr, "memory exhausted\n");
        return false;
    }

    Node *source_node = graph_get_node(graph, source);
    Node *dest_node = graph_get_node(graph, dest);

    heap_decrease_distance(heap, source_node, 0, NULL);

    while (!heap_is_empty(heap)) {
        Node *node_now = heap_extract_min(heap);

        int n_out = node_get_n_outgoing(node_now);

        struct edge *edges = node_get_edges(node_now);

        for (int i = 0; i < n_out; i++) {
            struct edge *edge_now = edges + i;
            int min_delay = edge_now->mindelay;
            unsigned int dest_dist = node_get_distance(edge_now->destination);
            unsigned int curr_dist = node_get_distance(node_now);

            if (curr_dist + min_delay < dest_dist) {
                heap_decrease_distance(heap, edge_now->destination, curr_dist + min_delay, node_now);
            }
        }

        if (node_now == dest_node) {
            break;
        }
    }

    heap_free(heap);
    return true;
}

bool path_from_source_to_dest(Graph *graph, unsigned int source, unsigned int dest)
{
    Node *node_source = graph_get_node(graph, source);
    Node *node_dest = graph_get_node(graph, dest);
    Node *node_now = node_dest;

    while (node_get_previous(node_now) != NULL) {
        node_now = node_get_previous(node_now);
    }

    return node_now == node_source;
}

bool source_dest_in_graph(Graph *graph, unsigned int source, unsigned int dest)
{
    return graph_get_node(graph, source) != NULL && graph_get_node(graph, dest) != NULL;
}

bool create_path(Graph *graph, unsigned int dest, FILE *out_file)
{
    unsigned int source_id;
    unsigned int dest_id = node_get_id(graph_get_node(graph, dest));
    unsigned int len, len_prev;
    Node *node_now = graph_get_node(graph, dest);

    fprintf(out_file, "digraph {\n");

    while (node_get_previous(node_now) != NULL) {
        len_prev = node_get_distance(node_now);
        node_now = node_get_previous(node_now);
        source_id = node_get_id(node_now);
        len = len_prev - node_get_distance(node_now);

        fprintf(out_file, "\t%u -> %u [label=%u];\n", source_id, dest_id, len);

        dest_id = node_get_id(node_now);
    }

    fprintf(out_file, "}\n");
    return true;
}

bool create_output(FILE *node_file, FILE *edge_file, FILE *out_file, unsigned int source, unsigned int dest)
{
    bool status = false;
    Graph *graph = graph_new();

    if (graph == NULL) {
        fprintf(stderr, "memory exhausted\n");
        return status;
    }

    if (!add_nodes_to_graph(node_file, graph)) {
        goto free_and_return;
    }

    if (!add_edges_to_graph(edge_file, graph)) {
        goto free_and_return;
    }

    if (!dijkstra(graph, source, dest)) {
        goto free_and_return;
    }

    if (!source_dest_in_graph(graph, source, dest)) {
        fprintf(stderr, "node does not exist in graph\n");
        goto free_and_return;
    }

    if (!path_from_source_to_dest(graph, source, dest)) {
        fprintf(stderr, "no path between source and dest\n");
        goto free_and_return;
    }

    status = create_path(graph, dest, out_file);

free_and_return:
    graph_free(graph);
    return status;
}

int main(int argc, char *argv[])
{
    int status = EXIT_FAILURE;

    if (argc != 5 && argc != 6) {
        fprintf(stderr, "usage: %s NODE_FILE EDGE_FILE FROM_NODE TO_NODE [OUTFILE]\n", argv[0]);
        return status;
    }

    FILE *node_file, *edge_file, *out_file;

    // open all files
    if ((node_file = fopen(argv[1], "r")) == NULL) {
        perror(argv[1]);
        goto error_open_node;
    }
    if ((edge_file = fopen(argv[2], "r")) == NULL) {
        perror(argv[2]);
        goto error_open_edge;
    }
    if (argc == 6 && (out_file = fopen(argv[5], "w")) == NULL) {
        perror(argv[5]);
        goto error_open_out;
    }
    if (argc == 5) {
        out_file = stdout;
    }

    int source, dest;
    if (int_from_str(argv[3], &source) && int_from_str(argv[4], &dest)) {
        if (create_output(node_file, edge_file, out_file, source, dest)) {
            status = EXIT_SUCCESS;
        }
    } else {
        fprintf(stderr, "input is not valid number\n");
    }

    // close files in reverse
    if (argc == 6) {
        if (fclose(out_file) == EOF) {
            perror(argv[5]);
        }
    }

error_open_out:
    if (fclose(edge_file) == EOF) {
        perror(argv[2]);
    }

error_open_edge:
    if (fclose(node_file) == EOF) {
        perror(argv[1]);
    }

error_open_node:
    return status;
}
