
#ifndef REVERSE_INDEX_H
#define REVERSE_INDEX_H

#include "document.h"
#include <stdbool.h>


typedef struct ReverseIndexEntry {
    char* word;
    DocumentsList* documents;
    struct ReverseIndexEntry* next;
} ReverseIndexEntry;

typedef struct ReverseIndex {
    ReverseIndexEntry** entries;
    int size;
    int capacity;
} ReverseIndex;

ReverseIndex* reverseIndexCreate(int capacity);
void reverseIndexAdd(ReverseIndex* index, char* word, Document* doc);
DocumentsList* reverseIndexGet(ReverseIndex* index, char* word);
void reverseIndexFree(ReverseIndex* index);
void reverseIndexBuild(ReverseIndex* index, DocumentsList* documents);
void reverseIndexSerialize(ReverseIndex* index, char* filename);
ReverseIndex* reverseIndexDeserialize(char* filename);

#endif