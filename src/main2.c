#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document2.c"
#include "document_list.c"
#include "directed_graph.c"
#include "reverse_index.c"
#include "query.c"
#include "last3_queries.c"
#include "link.c"

#define MAX_QUERY_LENGTH 256
#define SNIPPET_PADDING 75  // Caracteres alrededor de palabras clave

// Función para comparar documentos por relevancia (para qsort)
int compareDocuments(const void* a, const void* b) {
    const Document* docA = *(const Document**)a;
    const Document* docB = *(const Document**)b;
    return (docB->relevance_score > docA->relevance_score) ? 1 : -1;
}

// Genera un snippet del texto alrededor de la primera aparición de una palabra
char* generateSnippet(const char* text, const char* word) {
    const char* found = strstr(text, word);
    if (!found) return strdup("(No snippet available)");

    int start_pos = found - text;
    int start = (start_pos - SNIPPET_PADDING > 0) ? start_pos - SNIPPET_PADDING : 0;
    int end_pos = start_pos + strlen(word);
    int end = (end_pos + SNIPPET_PADDING < strlen(text)) ? end_pos + SNIPPET_PADDING : strlen(text);

    char* snippet = malloc(end - start + 4);  // +3 para "..." y null terminator
    if (!snippet) return NULL;

    if (start > 0) strcpy(snippet, "...");
    else snippet[0] = '\0';

    strncat(snippet, text + start, end - start);

    if (end < strlen(text)) strcat(snippet, "...");

    return snippet;
}

// Muestra los resultados de búsqueda con snippets
void displayResults(Document** results, int count, Query* query) {
    for (int i = 0; i < count && i < 5; i++) {
        printf("\n[%d] %s (Relevance: %.2f)\n", i, results[i]->title, results[i]->relevance_score);

        // Encontrar la primera palabra de la query en el documento
        QueryItem* item = query->head;
        char* snippet = NULL;
        while (item && !snippet) {
            if (item->type == INCLUDE) {
                snippet = generateSnippet(results[i]->body, item->word);
            }
            item = item->next;
        }

        printf("%s\n", snippet ? snippet : "(No snippet available)");
        free(snippet);
    }
    if (count == 0) printf("\nNo results found.\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s file1.txt file2.txt ...\n", argv[0]);
        return 1;
    }

    // 1. Cargar documentos
    DocumentsList documents = { .head = NULL, .size = 0 };
    for (int i = 1; i < argc; i++) {
        Document* doc = documentDeserialize(argv[i]);
        if (doc) documentsListAppend(&documents, doc);
    }

    // 2. Construir grafo de documentos
    DocumentGraph* graph = documentGraphCreate(argc * 2);  // Capacidad inicial generosa
    Document* current = documents.head;
    while (current) {
        documentGraphAddNode(graph, current->id);
        Link* link = current->links;
        while (link) {
            documentGraphAddEdge(graph, current->id, link->id);
            link = link->next;
        }
        current = current->next;
    }

    // 3. Calcular relevancia (usando PageRank)
    calculatePageRank(graph, 0.85, 100, 0.001);

    // 4. Construir índice invertido
    ReverseIndex* index = reverseIndexCreate(argc * 10);  // Tamaño hashmap grande
    reverseIndexBuild(index, &documents);

    // 5. Historial de búsquedas
    QueryQueue history;
    initQueue(&history);

    // 6. Bucle principal de búsqueda
    char query_input[MAX_QUERY_LENGTH];
    while (1) {
        printf("\n=== Search Engine ===\n");
        showLastQueries(&history);
        printf("\nEnter query (empty to exit): ");

        if (!fgets(query_input, MAX_QUERY_LENGTH, stdin)) break;
        query_input[strcspn(query_input, "\n")] = '\0';
        if (strlen(query_input) == 0) break;

        enqueueQuery(&history, query_input);
        Query* query = queryInit(query_input);

        // Buscar documentos que coincidan con TODOS los términos
        Document* results[documents.size];
        int result_count = 0;

        Document* doc = documents.head;
        while (doc) {
            int match = 1;
            QueryItem* item = query->head;

            while (item && match) {
                if (item->type == INCLUDE) {
                    DocumentsList* docs_with_word = reverseIndexGet(index, item->word);
                    if (!docs_with_word) {
                        match = 0;
                        break;
                    }

                    int found = 0;
                    Document* temp = docs_with_word->head;
                    while (temp) {
                        if (temp->id == doc->id) {
                            found = 1;
                            break;
                        }
                        temp = temp->next;
                    }
                    if (!found) match = 0;
                }
                item = item->next;
            }

            if (match) {
                results[result_count++] = doc;
            }
            doc = doc->next;
        }

        // Ordenar por relevancia
        qsort(results, result_count, sizeof(Document*), compareDocuments);

        // Mostrar resultados
        displayResults(results, result_count, query);

        queryFree(query);
    }

    // Liberar memoria
    reverseIndexFree(index);
    documentGraphFree(graph);
    documentsListFree(&documents);
    freeQueue(&history);

    return 0;
}