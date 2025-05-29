#ifndef DOCUMENT2_H
#define DOCUMENT2_H

#include "link.h"
#include <stdbool.h>

typedef struct Document {
    int id;
    char* title;
    char* body;
    Link* links;
    float relevance_score;
    struct Document* next;

} Document;

// Funciones
Document* documentDeserialize(char* path);
void documentFree(Document* doc, bool freeLinks);

#endif

