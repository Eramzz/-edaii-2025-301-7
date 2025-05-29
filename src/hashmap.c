#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hashmap.h"


//FUNCION QUE CONVIERTE PALABRA A NUMERO
unsigned int hash(const char* str, int size) { //entra palabra que es lo que cambiar a num, tamaño array
    unsigned int hash = 5381; //inicializa el valor hash
    int c;
    while ((c = *str++)) //recorre cadena str y va avanzando el puntero
        hash = ((hash << 5) + hash) + tolower(c); //funcion de hash se actualiza el hash
    return hash % size; //devuelve elñ hash en modulo
}

ReverseIndex* hashmapCreate(int size) {
    if (size <= 0) return NULL;

    ReverseIndex* index = malloc(sizeof(ReverseIndex));//Reserva memoria para la estructura ReverseIndex
    if (!index) return NULL;

    index->buckets = calloc(size, sizeof(WordEntry*));
    if (!index->buckets) {
        free(index);
        return NULL;
    }

    index->size = size;// Guarda el tamaño de la tabla hash
    return index;
}

void hashmapAdd(ReverseIndex* index, const char* word, Document* doc) {
    if (!index || !word || !doc) return;

    unsigned int bucket = hash(word, index->size); //Calcula el índice del bucket función hash
    WordEntry* entry = index->buckets[bucket];  //Saca el primer WordEntry del array buckets

    while (entry) {     //Recorre la lista de bucket
        if (strcasecmp(entry->word, word) == 0) {    //Si la palabra existe
            DocumentNode* node = entry->docs;  //Recorre lista de documentos con esa palabra
            while (node) {
                if (node->doc == doc) return;    //si ya lo tiene registrado no hace nada y sale de la función
                node = node->next; //sigue avanzando y continua buscandio en la lista
            }
            DocumentNode* new_node = malloc(sizeof(DocumentNode));    //Si no esta el documento, crea un nuevo nodo
            if (!new_node) return;

            new_node->doc = doc;                                       //Asigna el documento
            new_node->next = entry->docs;                              //Aade a la linked list
            entry->docs = new_node;
            return;
        }
        entry = entry->next;                                          //Pasa al siguiente WordEntry de la lista
    }
    // Crear nueva entrada
    WordEntry* new_entry = malloc(sizeof(WordEntry));
    if (!new_entry) return;

    new_entry->word = strdup(word);
    if (!new_entry->word) {
        free(new_entry);
        return;
    }

    new_entry->docs = malloc(sizeof(DocumentNode));
    if (!new_entry->docs) {
        free(new_entry->word);
        free(new_entry);
        return;
    }

    new_entry->docs->doc = doc;
    new_entry->docs->next = NULL;
    new_entry->next = index->buckets[bucket];
    index->buckets[bucket] = new_entry;


}


DocumentNode* hashmapGet(ReverseIndex* index, const char* word) {
    if (!index || !word) return NULL;
    ;  unsigned int bucket = hash(word, index->size);
    WordEntry* entry = index->buckets[bucket];

    while (entry) {
        if (strcasecmp(entry->word, word) == 0) {
            return entry->docs;
        }
        entry = entry->next;
    }
    return NULL;
}

void hashmapFree(ReverseIndex* index) {
    if (!index) return;

    for (int i = 0; i < index->size; i++) {         //Recorre todos los buckets del indice i
        WordEntry* entry = index->buckets[i];       //coge la lista de palabras del bucle actual
        //recorre todas cada palabra, WordEntry de esa lista
        while (entry) {
            WordEntry* nextEntry = entry->next;        //Guarda la siguiente antes de liberar el actual

            DocumentNode* node = entry->docs; //saca lista de documentos que contienen las palabras
            while (node) { //va recorrindo cada documento de la ista anterior
                DocumentNode* nextNode = node->next; //va guardando el doc antes de liberarlo
                free(node);                             //Libera cada nodo de documento
                node = nextNode;                         //pasa al siguiente documento
            }
            free(entry->word);                                // Libera el WordEntry (la palabra)
            free(entry);
            entry = nextEntry;                          // Pasa al siguiente en la lista (la siguiente Wordentry del bucketlist)
        }
    }
    free(index->buckets);                                            //Libera el array de buckets
    free(index);                                                     //Libera la estructura principal
}
