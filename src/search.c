#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "document2.h"
#include "link.h"
#include "reverse_index.h"
#include "query.h"
#include "directed_graph.h"
#include "last3_queries.h"
#include <stdbool.h>


//NO HACE FALTA, SE UTILIZA REVERSE_INDEX.C

int containsAllWords(Document* doc, Query* q) {
    QueryItem* item = q->head;
    while (item) {
        if (!strstr(doc->body, item->word) && !strstr(doc->title, item->word)) {
            return 0;
        }
        item = item->next;
    }
    return 1;
}

void searchDocuments(DocumentsList* docs, Query* q) {
    Document* curr = docs->head;
    int found = 0;
    while (curr && found < 5) {
        if (containsAllWords(curr, q)) {
            printf("(%d) %s\n---\n", found, curr->title);
            printf("%.150s%s\n---\n\n", curr->body, strlen(curr->body) > 150 ? "..." : "");
            found++;
        }
        curr = curr->next;
    }
    if (found == 0) printf("No results found.\n");
}