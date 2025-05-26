//
// Created by sofia on 08/05/2025.
//

//NO HACE FALTA, SE UTILIZA REVERSE_INDEX.C
#include <stdbool.h>

#ifndef SEARCH_H
#define SEARCH_H

int containsAllWords(Document* doc, Query* q);
void searchDocuments(DocumentList* docs, Query* q);

#endif //SEARCH_H