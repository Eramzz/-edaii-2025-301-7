#ifndef REVERSE_INDEX_H
#define REVERSE_INDEX_H

#include "document2.h"

typedef struct ReverseIndexEntry {
    char* word;
    DocumentsList* documents;
    struct ReverseIndexEntry* next;
} ReverseIndexEntry;

typedef struct {
    ReverseIndexEntry** entries;
    int capacity;
    int size;
} ReverseIndex;

ReverseIndex* reverseIndexCreate(int capacity);
void reverseIndexAdd(ReverseIndex* index, char* word, Document* doc);
DocumentsList* reverseIndexGet(ReverseIndex* index, char* word);
void reverseIndexFree(ReverseIndex* index);
void reverseIndexBuild(ReverseIndex* index, DocumentsList* documents);
void reverseIndexSerialize(ReverseIndex* index, char* filename);
ReverseIndex* reverseIndexDeserialize(char* filename, DocumentsList* all_docs);
Document* findDocumentById(DocumentsList* list, int id);

#endif // REVERSE_INDEX_H