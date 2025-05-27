#ifndef REVERSE2_H
#define REVERSE2_H

#include "document.h"   // Para usar Document y DocumentsList

// Estructura de entrada del índice invertido
typedef struct ReverseIndexEntry {
    char* word;                      // Palabra clave
    DocumentsList* documents;       // Lista de documentos donde aparece
    struct ReverseIndexEntry* next; // Para colisiones (chaining)
} ReverseIndexEntry;

// Estructura principal del índice invertido (hashmap)
typedef struct ReverseIndex {
    ReverseIndexEntry** entries;  // Array de listas enlazadas
    int size;                     // Número total de entradas
    int capacity;                 // Capacidad del array
} ReverseIndex;

// Funciones públicas
unsigned long hash(const char* str, int capacity);

ReverseIndex* reverseIndexCreate(int capacity);
void reverseIndexAdd(ReverseIndex* index, char* word, Document* doc);
DocumentsList* reverseIndexGet(ReverseIndex* index, char* word);
void reverseIndexBuild(ReverseIndex* index, DocumentsList* documents);
void reverseIndexFree(ReverseIndex* index);

void reverseIndexSerialize(ReverseIndex* index, char* filename);
ReverseIndex* reverseIndexDeserialize(char* filename);

#endif // REVERSE_INDEX_H
