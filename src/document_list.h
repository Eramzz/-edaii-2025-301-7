#ifndef DOCUMENT_LIST_H
#define DOCUMENT_LIST_H

#include "document2.h"
#include <stdbool.h>

typedef struct DocumentsList {
    Document* head;
    int size;
} DocumentsList;

void documentsListInit(DocumentsList* list);
void documentsListAppend(DocumentsList* list, Document* doc);
void documentsListFree(DocumentsList* list);
void documentsListPrint(DocumentsList* list);
bool documentsListLoadFromDir(DocumentsList* list, const char* dirpath);

#endif // DOCUMENT_LIST_H

