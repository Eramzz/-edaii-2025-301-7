//
// Created by sofia on 22/05/2025.
//
#ifndef HASHMAP_H
#define HASHMAP_H

#include "document.h"
#include <stdbool.h>


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
ReverseIndex* reverseIndexCreate(int size); //crea tabla tamaño que queremos
void reverseIndexAdd(ReverseIndex* index, const char* word, Document* doc);//añade palabra y doc al indice
DocumentNode* reverseIndexGet(ReverseIndex* index, const char* word); //obten lista que contiene palabra
void reverseIndexFree(ReverseIndex* index); //liberar memoria indice
void reverseIndexBuild(ReverseIndex* index, DocumentsList* list);
void reverseIndexAddAll(ReverseIndex* index, DocumentsList* docs);


void reverseIndexBuild(ReverseIndex* index, DocumentsList* list); //conmstruye indice con doc

// FUNCION DE HASH PRINMCIPAL
unsigned int hash(const char* str, int size);

#endif // HASHMAP_H

