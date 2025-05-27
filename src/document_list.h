#ifndef DOCUMENT_LIST_H
#define DOCUMENT_LIST_H

#include "document.h"
#include <stdbool.h>

void documentsListInit(DocumentsList* list);
void documentsListAppend(DocumentsList* list, Document* doc);
void documentsListFree(DocumentsList* list);
void documentsListPrint(DocumentsList* list);
void documentsListLoadFromDir(DocumentsList* list, const char* dirpath);

#endif // DOCUMENT_LIST_H

