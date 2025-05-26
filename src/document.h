#ifndef DOCUMENT_H
#define DOCUMENT_H
#include <stdbool.h>


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

typedef struct DocumentsList {
    Document* head;
    int size;
} DocumentsList;

Document* documentDeserialize(char* path);
void documentFree(Document* doc);
void documentsListAppend(DocumentsList* list, Document* doc);
Document* documentsListGet(DocumentsList* list, int index);
void documentsListFree(DocumentsList* list);
void sortDocumentsByRelevance(DocumentsList* list);

#endif


