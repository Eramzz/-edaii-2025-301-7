#include <stdio.h>
#include <string.h>
#include "search.h"

// Búsqueda lineal solo para debugging (ineficiente)
static int containsAllWords(Document* doc, Query* q) {
    QueryItem* item = q->head;
    while (item) {
        if (item->type == INCLUDE &&
            !strstr(doc->body, item->word) &&
            !strstr(doc->title, item->word)) {
            return 0;
            }
        item = item->next;
    }
    return 1;
}

void searchDocumentsLinear(DocumentsList* docs, Query* q) {
    Document* curr = docs->head;
    int found = 0;
    while (curr && found < 5) {
        if (containsAllWords(curr, q)) {
            printf("[%d] %s\n%.150s%s\n\n",
                  found, curr->title, curr->body,
                  strlen(curr->body) > 150 ? "..." : "");
            found++;
        }
        curr = curr->next;
    }
    if (found == 0) printf("No results found.\n");
}