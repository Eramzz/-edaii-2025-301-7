#ifndef DIRECTED_GRAPH_H
#define DIRECTED_GRAPH_H

typedef struct GraphNode {
    int document_id;
    struct GraphNode** neighbors;
    int neighbors_count;
    int capacity;
    float relevancia;  //AÑADIDO SOFIA PARA LA FUNCIÓN DEL FINAL
} GraphNode;

typedef struct DocumentGraph {
    GraphNode** nodes;
    int size;
    int capacity;
} DocumentGraph;

DocumentGraph* documentGraphCreate(int ini_capacity); // Crea un grafo vacío. Retorna NULL si falla.
void documentGraphAddNode(DocumentGraph* graph, int document_id); // Añade un nodo al grafo (si no existe).
void documentGraphAddEdge(DocumentGraph* graph, int from_id, int to_id); // Añade una arista (from_id → to_id). Ignora si ya existe.
float graphGetIndegree(DocumentGraph* graph, int document_id);
void documentGraphFree(DocumentGraph* graph);
void calculatePageRank(DocumentGraph* graph, float damping_factor, int max_iterations, float tolerance);
#endif