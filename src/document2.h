#ifndef DOCUMENT2_H
#define DOCUMENT2_H

#include "link.h"

typedef struct Document {
    int id;
    char* title;
    char* body;
    Link* links;
    float relevance_score;
    struct Document* next;
} Document;

typedef struct {
    Document* head;
    int size;
} DocumentsList;

// Funciones
Document* documentDeserialize(char* path);
void documentFree(Document* doc, bool freeLinks);
void documentsListAppend(DocumentsList* list, Document* doc);
void documentsListFree(DocumentsList* list);

#endif

