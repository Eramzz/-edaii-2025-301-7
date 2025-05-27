#ifndef DOCUMENT2_H
#define DOCUMENT2_H

#include "link.h"

typedef struct Link {
    int destination_id;
    char* text;
    struct Link* next;
} Link;

typedef struct Document {
    int id;
    char* title;
    char* body;
    Link* links;
    float relevance_score;
    struct Document* next;
} Document;

// Carga un documento desde un archivo en disco
Document* documentDeserialize(char* path);

#endif // DOCUMENT_H
