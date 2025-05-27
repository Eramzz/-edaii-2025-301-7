#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document2.h"
#include "query.h"
#include "reverse2.h"

#define MAX_QUERY_LENGTH 256

// Ordena los documentos por relevance_score (descendente)
void sortDocumentsByRelevance(DocumentsList* list) {
    if (!list || !list->head) return;

    int swapped;
    Document* ptr1;
    Document* lptr = NULL;

    do {
        swapped = 0;
        ptr1 = list->head;

        while (ptr1->next != lptr) {
            if (ptr1->relevance_score < ptr1->next->relevance_score) {
                // Intercambia datos entre documentos
                int temp_id = ptr1->id;
                char* temp_title = ptr1->title;
                char* temp_body = ptr1->body;
                float temp_score = ptr1->relevance_score;
                Link* temp_links = ptr1->links;

                ptr1->id = ptr1->next->id;
                ptr1->title = ptr1->next->title;
                ptr1->body = ptr1->next->body;
                ptr1->relevance_score = ptr1->next->relevance_score;
                ptr1->links = ptr1->next->links;

                ptr1->next->id = temp_id;
                ptr1->next->title = temp_title;
                ptr1->next->body = temp_body;
                ptr1->next->relevance_score = temp_score;
                ptr1->next->links = temp_links;

                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Uso: %s archivo1.txt archivo2.txt ...\n", argv[0]);
        return 1;
    }

    // Cargar documentos
    DocumentsList documents = { .head = NULL, .size = 0 };
    for (int i = 1; i < argc; i++) {
        Document* doc = documentDeserialize(argv[i]);
        if (doc) {
            documentsListAppend(&documents, doc);
        }
    }

    // Calcular relevance_score por enlaces
    Document* current = documents.head;
    while (current) {
        Link* link = current->links;
        while (link) {
            Document* target = documents.head;
            while (target) {
                if (target->id == link->destination_id) {
                    target->relevance_score++;
                    break;
                }
                target = target->next;
            }
            link = link->next;
        }
        current = current->next;
    }

    // Construir el índice inverso
    ReverseIndex* index = reverseIndexCreate(100);
    reverseIndexBuild(index, &documents);

    // Leer consulta del usuario
    char query_input[MAX_QUERY_LENGTH];
    printf("Search: ");
    if (!fgets(query_input, MAX_QUERY_LENGTH, stdin)) {
        fprintf(stderr, "Error leyendo la consulta\n");
        return 1;
    }
    query_input[strcspn(query_input, "\n")] = '\0'; // quitar \n

    Query* query = queryInit(query_input);

    // Obtener documentos que contienen todas las palabras
    DocumentNode* result_nodes = NULL;
    QueryItem* item = query->head;
    while (item) {
        DocumentNode* docs_for_word = reverseIndexGet(index, item->word);
        if (!docs_for_word) {
            result_nodes = NULL;
            break; // si una palabra no está, no hay resultados
        }
        if (!result_nodes) {
            result_nodes = docs_for_word;
        } else {
            // intersección entre result_nodes y docs_for_word
            DocumentNode* filtered = NULL;
            while (docs_for_word) {
                DocumentNode* temp = result_nodes;
                while (temp) {
                    if (temp->doc->id == docs_for_word->doc->id) {
                        DocumentNode* copy = malloc(sizeof(DocumentNode));
                        copy->doc = docs_for_word->doc;
                        copy->next = filtered;
                        filtered = copy;
                        break;
                    }
                    temp = temp->next;
                }
                docs_for_word = docs_for_word->next;
            }
            result_nodes = filtered;
        }
        item = item->next;
    }

    // Convertir resultados en lista y ordenarla
    DocumentsList results = { .head = NULL, .size = 0 };
    DocumentNode* node = result_nodes;
    while (node) {
        documentsListAppend(&results, node->doc);
        node = node->next;
    }
    sortDocumentsByRelevance(&results);

    // Mostrar resultados
    Document* res = results.head;
    int count = 0;
    while (res && count < 5) {
        printf("(%d) %s\nRelevance score: %.0f\n", count, res->title, res->relevance_score);
        printf("%.150s%s\n\n", res->body, strlen(res->body) > 150 ? "..." : "");
        count++;
        res = res->next;
    }
    if (count == 0) printf("No results found.\n");

    // Limpieza
    queryFree(query);
    documentsListFree(&results);
    documentsListFree(&documents);
    reverseIndexFree(index);

    return 0;
}
