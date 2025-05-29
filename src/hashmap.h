#ifndef HASHMAP_H
#define HASHMAP_H

#include "document2.h"


//estructura nodo de la lista quew tiene puntero a doc
typedef struct DocumentNode {
    Document* doc; //punetro doc
    struct DocumentNode* next; //lista enlazada de doc
} DocumentNode;

//entrada tabla hash
typedef struct WordEntry {
    char* word; //palabra indice
    DocumentNode* docs; //lista doc que tiene palabra
    struct WordEntry* next; //evitar colisiones
} WordEntry;

//tabla hash estructura
typedef struct ReverseIndex {
    WordEntry** buckets; //array de punteros
    int size;// tamaño tabla
} ReverseIndex;

//funciones Hashmap
ReverseIndex* hashmapCreate(int size); // Crea una nueva tabla hash.
void hashmapAdd(ReverseIndex* index, const char* word, Document* doc); // Añade un documento a la lista de una palabra.
DocumentNode* hashmapGet(ReverseIndex* index, const char* word); // Busca documentos asociados a una palabra.
void hashmapFree(ReverseIndex* index);


#endif // HASHMAP_H

