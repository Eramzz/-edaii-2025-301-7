#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document2.h"
#include "query.h"
#include "reverse_index.h"
#include "directed_graph.h"
#include "last3_queries.h"

#define MAX_QUERY_LENGTH 256

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

    DocumentsList documents = { .head = NULL, .size = 0 };
    for (int i = 1; i < argc; i++) {
        Document* doc = documentDeserialize(argv[i]);
        if (doc) {
            documentsListAppend(&documents, doc);
        }
    }

    DocumentGraph* graph = documentGraphCreate(100);
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

    current = documents.head;
    while (current) {
        current->relevance_score = graphGetIndegree(graph, current->id);
        current = current->next;
    }

    ReverseIndex* index = reverseIndexCreate(100);
    reverseIndexBuild(index, &documents);

    QueryQueue history;
    initQueue(&history);

    while (1) {
        printf("\n******* Recent Searches ********\n");
        showLastQueries(&history);
        printf("********************************\n");
        printf("Search: ");

        char query_input[MAX_QUERY_LENGTH];
        if (!fgets(query_input, MAX_QUERY_LENGTH, stdin)) break;
        query_input[strcspn(query_input, "\n")] = '\0';
        if (strlen(query_input) == 0) break;

        enqueueQuery(&history, query_input);
        Query* query = queryInit(query_input);

        DocumentsList* result_lists[MAX_QUERY_LENGTH];
        int num_keywords = 0;
        QueryItem* item = query->head;
        while (item) {
            result_lists[num_keywords++] = reverseIndexGet(index, item->word);
            item = item->next;
        }

        DocumentsList results = { .head = NULL, .size = 0 };
        Document* doc = documents.head;
        while (doc) {
            int match = 1;
            for (int i = 0; i < num_keywords; i++) {
                Document* temp = result_lists[i] ? result_lists[i]->head : NULL;
                int found = 0;
                while (temp) {
                    if (temp->id == doc->id) {
                        found = 1;
                        break;
                    }
                    temp = temp->next;
                }
                if (!found) {
                    match = 0;
                    break;
                }
            }
            if (match) {
                documentsListAppend(&results, doc);
            }
            doc = doc->next;
        }

        sortDocumentsByRelevance(&results);

        Document* res = results.head;
        int count = 0;
        while (res && count < 5) {
            printf("(%d) %s\nRelevance score: %.0f\n", count, res->title, res->relevance_score);
            printf("%.150s%s\n\n", res->body, strlen(res->body) > 150 ? "..." : "");
            count++;
            res = res->next;
        }
        if (count == 0) printf("No results found.\n");

        queryFree(query);
        documentsListFree(&results);
    }

    reverseIndexFree(index);
    documentsListFree(&documents);
    documentGraphFree(graph);
    freeQueue(&history);
    return 0;
}