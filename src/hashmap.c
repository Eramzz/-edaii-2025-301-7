#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hashmap.h"
#include "query.h"  //NormalizeWord
#include <stdbool.h>


//FUNCION QUE CONVIERTE PALABRA A NUMERO
unsigned int hash(const char* str, int size) { //entra palabra que es lo que cambiar a num, tamaño array
    unsigned int hash = 5381; //inicializa el valor hash
    int c;
    while ((c = *str++)) //recorre cadena str y va avanzando el puntero
        hash = ((hash << 5) + hash) + c; //funcion de hash se actualiza el hash
    return hash % size; //devuelve elñ hash en modulo
}

ReverseIndex* reverseIndexCreate(int size) {
    ReverseIndex* index = malloc(sizeof(ReverseIndex));//Reserva memoria para la estructura ReverseIndex
    index->size = size;// Guarda el tamaño de la tabla hash
    index->buckets = malloc(size * sizeof(WordEntry*));    // Reserva memoria para el array de buckets (punteros a WordEntry)
    for (int i = 0; i < size; i++) {     // Inicializa cada bucket manualmente a NULL
        index->buckets[i] = NULL;
    }
    return index;     // Devuelve el puntero a la estructura ReverseIndex ya inicializada
}

void reverseIndexAdd(ReverseIndex* index, const char* word, Document* doc) {
    unsigned int bucket = hash(word, index->size); //Calcula el índice del bucket función hash
    WordEntry* entry = index->buckets[bucket];  //Saca el primer WordEntry del array buckets

    while (entry) {     //Recorre la lista de bucket
        if (strcmp(entry->word, word) == 0) {    //Si la palabra existe
            DocumentNode* node = entry->docs;  //Recorre lista de documentos con esa palabra
            while (node) {
                if (node->doc == doc) return;    //si ya lo tiene registrado no hace nada y sale de la función
                node = node->next; //sigue avanzando y continua buscandio en la lista
            }
            DocumentNode* newNode = malloc(sizeof(DocumentNode));    //Si no esta el documento, crea un nuevo nodo
            newNode->doc = doc;                                       //Asigna el documento
            newNode->next = entry->docs;                              //Aade a la linked list
            entry->docs = newNode;
            return;
        }
        entry = entry->next;                                          //Pasa al siguiente WordEntry de la lista
    }

    WordEntry* newEntry = malloc(sizeof(WordEntry));                  //Si la palabra no existe aún, crea Crea un nuevo WordEntry
    newEntry->word = strdup(word);                                    //Duplica la palabra
    newEntry->docs = malloc(sizeof(DocumentNode));                    //Crea la lista de documentos y el primer nodo
    newEntry->docs->doc = doc;                                        //Asocia el documento
    newEntry->docs->next = NULL; //establece el final de la lista con un null
    newEntry->next = index->buckets[bucket];                          // Inserta nueva entrada al principio del array bucket
    index->buckets[bucket] = newEntry;
}


DocumentNode* reverseIndexGet(ReverseIndex* index, const char* word) {
    char normalized[256];                                             //Inicilizamos un buffer para palabra la normalizada (todos elem min sin numeros ni signos)
    strncpy(normalized, word, 255);                                   //Copia hasta 255 caracteres
    normalized[255] = '\0';                                           //Añade al final de la cadena un carácter nulo
    NormalizeWord(normalized);                                       //llama a la función para normalizar (minúsculas, sin símbolos)

    unsigned int bucket = hash(normalized, index->size);             //Calcula el bucket correspondiente con la función de hash
    WordEntry* entry = index->buckets[bucket];                       //Busca en la lista de palabras del bucket la palabra word

    while (entry) {
        if (strcmp(entry->word, normalized) == 0)                    //Si encuentra la palabra exacta
            return entry->docs;                                      //Devuelve lista de documentos encontrada
        entry = entry->next;                                         //Sigue con el siguiente elemento de lista
    }
    return NULL;                                                     //devuelve NULL si no lo ha encontrado
}


void reverseIndexFree(ReverseIndex* index) {
    for (int i = 0; i < index->size; i++) {         //Recorre todos los buckets del indice i
        WordEntry* entry = index->buckets[i];       //coge la lista de palabras del bucle actual
        //recorre todas cada palabra, WordEntry de esa lista
        while (entry) {
            WordEntry* nextEntry = entry->next;        //Guarda la siguiente antes de liberar el actual
            free(entry->word);                         // Libera la palabra
            DocumentNode* node = entry->docs; //saca lista de documentos que contienen las palabras
            while (node) { //va recorrindo cada documento de la ista anterior
                DocumentNode* nextNode = node->next; //va guardando el doc antes de liberarlo
                free(node);                             //Libera cada nodo de documento
                node = nextNode;                         //pasa al siguiente documento
            }
            free(entry);                                // Libera el WordEntry (la palabra)
            entry = nextEntry;                          // Pasa al siguiente en la lista (la siguiente Wordentry del bucketlist)
        }
    }
    free(index->buckets);                                            //Libera el array de buckets
    free(index);                                                     //Libera la estructura principal
}

void reverseIndexBuild(ReverseIndex* index, DocumentsList* list) {
    Document* doc = list->head;                                      //Comienza desde el primer documento

    while (doc) { //recorre todos documentos
        char* title_copy = strdup(doc->title);                       //Copia título documento
        char* token = strtok(title_copy, " ");                       //Separa el título en palabras

        while (token) { //recorre cada palabra
            NormalizeWord(token);                                    //Normaliza la palabra (todos elem min y sin numeros)
            if (strlen(token) > 2) reverseIndexAdd(index, token, doc); //Si la palabra es suficientemente larga
            token = strtok(NULL, " ");                               //Pasa siguiente palabra
        }
        free(title_copy);                                            //Libera copia del título

        char* body_copy = strdup(doc->body);                         //Copia el cuerpo del documento
        token = strtok(body_copy, " ");                              //Sepoara cuerpo en palabras con los espacios

        while (token) { //recorre cada palabra
            NormalizeWord(token);                                    //Normaliza cada palabra
            if (strlen(token) > 2) reverseIndexAdd(index, token, doc); //Si la palabra es suficientemente larga la añade al indice
            token = strtok(NULL, " ");                               //Siguiente palabra
        }
        free(body_copy);                                             //Libera la copia del cuerpo

        doc = doc->next;                                             //Avanza al siguiente documento
    }
}

void reverseIndexAddAll(ReverseIndex* index, DocumentsList* docs) {
    Document* doc = docs->head;                                      // Empieza por el primer documento

    while (doc) {
        //titulo
        char* title_copy = strdup(doc->title);                       // Copia el título
        char* token = strtok(title_copy, " ");                       // Divide el título en palabras

        while (token) {
            NormalizeWord(token);                                    // Convierte a minúsculas y quita símbolos
            if (strlen(token) > 2) reverseIndexAdd(index, token, doc); // Añade si la palabra tiene más de 2 letras
            token = strtok(NULL, " ");                               // Pasa a la siguiente palabra
        }
        free(title_copy);                                            // Libera la copia del título

        //cuerpo
        char* body_copy = strdup(doc->body);                         // Copia el cuerpo
        token = strtok(body_copy, " ");                              // Divide el cuerpo en palabras

        while (token) {
            NormalizeWord(token);                                    // Normaliza palabra
            if (strlen(token) > 2) reverseIndexAdd(index, token, doc); // Añade palabra al índice
            token = strtok(NULL, " ");
        }
        free(body_copy);                                             // Libera la copia del cuerpo

        doc = doc->next;                                             // Pasa al siguiente documento
    }
}
