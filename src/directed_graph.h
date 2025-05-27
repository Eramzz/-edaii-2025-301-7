#ifndef GRAPH_H
#define GRAPH_H

#include "document2.h"
#include <stdbool.h>


typedef struct GraphNode {
    int document_id;
    struct GraphNode** neighbors;
    int neighbors_count;
    int capacity;
    float pagerank;  //AÑADIDO SOFIA PARA LA FUNCIÓN DEL FINAL
} GraphNode;

typedef struct DocumentGraph {
    GraphNode** nodes;
    int size;
    int capacity;
} DocumentGraph;

DocumentGraph* documentGraphCreate(int capacity);
void documentGraphAddNode(DocumentGraph* graph, int document_id);
void documentGraphAddEdge(DocumentGraph* graph, int from_id, int to_id);
float graphGetIndegree(DocumentGraph* graph, int document_id);
void documentGraphFree(DocumentGraph* graph);
void calculatePageRank(DocumentGraph* graph, float damping_factor, int max_iterations, float tolerance);
#endif