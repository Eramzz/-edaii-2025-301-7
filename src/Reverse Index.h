//
// Created by Dell on 22/05/2025.
//
#include <stdbool.h>

#ifndef REVERSE_INDEX_H
#define REVERSE_INDEX_H

typedef struct ReverseIndexKey {
    char* word;
    DocumentsList* values;
    struct ReverseIndexKey* next;
} ReverseIndexKey;

typedef struct ReverseIndexSlot{
   ReverseIndexKey* keys;
   int KeysCount;
} ReverseIndexSlot;

typedef struct ReverseIndex {
    int SlotCount;
    int Keys
    ReverseIndexSlot** slots;
} ReverseIndex;

ReverseIndex* reverseIndexCreate(int slots);
void reverseIndePut(ReverseIndex* index, char* word, DocumentList* List);
DocumentsList* reverseIndexGet(ReverseIndex* index, char* word);
void reverseIndexFree(ReverseIndex* index), bool freeList, bool freeDocs;
void reverseIndexDocument(ReverseIndex* index, Document* document);

#endif //REVERSE_INDEX_H
