
#include "directed_graph.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define INITIAL_NEIGHBORS_CAPACITY 10

DocumentGraph* documentGraphCreate(int capacity) {
    DocumentGraph* graph = (DocumentGraph*)malloc(sizeof(DocumentGraph));
    if (!graph) return NULL;

    graph->nodes = (GraphNode**)calloc(capacity, sizeof(GraphNode*));
    if (!graph->nodes) {
        free(graph);
        return NULL;
    }

    graph->size = 0;
    graph->capacity = capacity;
    return graph;
}

void documentGraphAddNode(DocumentGraph* graph, int document_id) {
    if (!graph || document_id < 0) return;

    // Check if node already exists
    for (int i = 0; i < graph->size; i++) {
        if (graph->nodes[i]->document_id == document_id) {
            return;
        }
    }

    // Create new node
    GraphNode* node = (GraphNode*)malloc(sizeof(GraphNode));
    if (!node) return;

    node->document_id = document_id;
    node->neighbors = (GraphNode**)malloc(INITIAL_NEIGHBORS_CAPACITY * sizeof(GraphNode*));
    if (!node->neighbors) {
        free(node);
        return;
    }

    node->neighbors_count = 0;
    node->capacity = INITIAL_NEIGHBORS_CAPACITY;

    // Add to graph
    if (graph->size >= graph->capacity) {
        // Resize graph (simplified, in real code would need proper realloc)

        //AÑADIDO SOFIA
        int new_capacity = graph->capacity * 2; //multiplica por 2 capacidad grafo
        //intenta reasignar memoria para el array de punteros realloc(array actual, nuevo tamaño)
        GraphNode** new_nodes = realloc(graph->nodes, new_capacity * sizeof(GraphNode*));

        //comprueba, si devuelve NULL, no hay memoria suf o error y entonces sale de la función sin modificar
        if (!new_nodes) return;
        //si ha podido hacer el realloc, actualiza los punteros y la capacidad (ahora apuntan a los nuevos valores)
        graph->nodes = new_nodes;
        graph->capacity = new_capacity;
        return;
    }

    graph->nodes[graph->size++] = node; //añade nuevo nodo a la nueva posición asignada
}

void documentGraphAddEdge(DocumentGraph* graph, int from_id, int to_id) {
    if (!graph) return;

    GraphNode* from_node = NULL;
    GraphNode* to_node = NULL;

    // Find nodes
    for (int i = 0; i < graph->size; i++) {
        if (graph->nodes[i]->document_id == from_id) {
            from_node = graph->nodes[i];
        }
        if (graph->nodes[i]->document_id == to_id) {
            to_node = graph->nodes[i];
        }
    }

    if (!from_node || !to_node) return;

    // Check if edge already exists
    for (int i = 0; i < from_node->neighbors_count; i++) {
        if (from_node->neighbors[i]->document_id == to_id) {
            return;
        }
    }

    // Add edge
    if (from_node->neighbors_count >= from_node->capacity) {
        // Resize neighbors array (simplified)

        //AÑADIDO SOFIA
        int new_capacity = from_node->capacity * 2; //calcula nueva capacidad, el doble de la anterior
        //intenta reasignar memoria para el array de punteros a los vecinos realloc(array actual, nuevo tamaño)
        GraphNode** new_neighbors = realloc(from_node->neighbors, new_capacity * sizeof(GraphNode*));
        //comprueba, si devuelve NULL, no hay memoria suf o error y entonces sale de la función sin modificar
        if (!new_neighbors) return;
        //si ha podido hacer el realloc, actualiza los punteros a los vecinos y la nueva capacidad (ahora apuntan a los nuevos valores)
        from_node->neighbors = new_neighbors;
        from_node->capacity = new_capacity;
        return;
    }

    //añade arista entre 2 nodos y actualiza el contador de vecinos (from node to node)
    from_node->neighbors[from_node->neighbors_count++] = to_node;
}

float graphGetIndegree(DocumentGraph* graph, int document_id) {
    if (!graph) return 0.0f;

    float count = 0.0f;
    for (int i = 0; i < graph->size; i++) {
        GraphNode* node = graph->nodes[i];
        for (int j = 0; j < node->neighbors_count; j++) {
            if (node->neighbors[j]->document_id == document_id) {
                count += 1.0f;
            }
        }
    }
    return count;
}

void documentGraphFree(DocumentGraph* graph) {
    if (!graph) return;

    for (int i = 0; i < graph->size; i++) {
        GraphNode* node = graph->nodes[i];
        free(node->neighbors);
        free(node);
    }

    free(graph->nodes);
    free(graph);
}

void calculatePageRank(DocumentGraph* graph, float damping_factor, int max_iterations, float tolerance) {
    if (!graph || graph->size == 0) return; //comprueba si el grafo es válido

    //crea 2 arrays
    float* ranks = (float*)malloc(graph->size * sizeof(float)); //para los valores actuales
    float* new_ranks = (float*)malloc(graph->size * sizeof(float)); //para calcular el siguiente

    if (!ranks || !new_ranks) { //si no ha podido asignar memoria
        free(ranks);
        free(new_ranks);
        return;
    }

    // Initialize ranks uniformly (con la formula, valores establecidos así todos comienzan igual)
    float initial_rank = 1.0f / graph->size;
    for (int i = 0; i < graph->size; i++) {
        ranks[i] = initial_rank;
    }

    //bucle ppal iteración
    for (int iter = 0; iter < max_iterations; iter++) {
        float diff = 0.0f; //variable para ir guardando como varian valores

        // Calculate new ranks
        for (int i = 0; i < graph->size; i++) {
            new_ranks[i] = (1.0f - damping_factor) / graph->size; //simula que pueda saltar a cualquier nodo aleatorio

            for (int j = 0; j < graph->size; j++) {
                GraphNode* node = graph->nodes[j]; //mira cada nodo j para ver si le da PageRank a i

                for (int k = 0; k < node->neighbors_count; k++) {
                    //mira si hay enlace de j a i
                    if (node->neighbors[k]->document_id == graph->nodes[i]->document_id) {
                        if (node->neighbors_count > 0) {
                            new_ranks[i] += damping_factor * ranks[j] / node->neighbors_count;
                        }
                    }
                }
            }
        }

        // calcula nous valors i actualitza
        for (int i = 0; i < graph->size; i++) {
            diff += fabs(new_ranks[i] - ranks[i]); //actualiza diff; fabs diferencia entre el anterior y el nuevo y
            ranks[i] = new_ranks[i]; //actualiza con nuevo
        }

        //si la diff es más peque que la tolerancia, ha convergido y ya podemos acabar
        if (diff < tolerance) {
            break;
        }
    }

    //AÑADIDO SOFIA
    //guarda el PageRank de cada nodo en la estructura GraphNode
    for (int i = 0; i < graph->size; i++) {
    graph->nodes[i]->pagerank = ranks[i];
    }


    //actualitza la importancia
    free(new_ranks);
    free(ranks);
}
