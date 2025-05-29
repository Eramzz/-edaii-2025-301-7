#include "query.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Función que crea un nuevo elemento para la linked ist
QueryItem* queryItemCreate(const char* word, QueryItemType type) {
    if (!word) return NULL; //si no recibe nada no hace nada

    QueryItem* item = (QueryItem*)malloc(sizeof(QueryItem)); //reserva memoria para una request
    if (!item) return NULL; //si no reserva memoria no hace nada

    item->word = strdup(word); //duplica la cadena utilizando strdup para tener la copia completa de la palabra en el nodo
    if (!item->word) { //si no copia la palabra no hace nada
        free(item); //libera memoria del nodo
        return NULL; //retorna NULL
    }
    item->type = type;
    item->next = NULL; //inicializa el puntero next como NULL
    return item; //retorna nuevo nodo creado
}

//convierte todas las letra a minúscula y elimina signos de puntuación, números o carácteres especiales
void NormalizeWord(char* word) {
    if (!word) return; //si no recibe nada no hace nada
    int i, j = 0; //inicializa variables i y j a 0. La i para recorrer la cadena original y la j para la nueva cadena

    for (i = 0; word[i]; i++) {
        //Si es una letra entra en el if, tanto mayúscula como minúscula
        if (isalpha((unsigned char)word[i])) {
            //Lo convierte a minúscula y lo guarda en la posición j
            word[j++] = tolower((unsigned char)word[i]);
        }
        //Si no es una letra lo ignora
    }
    word[j] = '\0';  //Añade al final de la cadena carácter nulo
}

//Función que inicializa la estructura Query cuando recibe texto
Query* queryInit(const char* input) {
    if (!input) return NULL;

    Query* q = (Query*)malloc(sizeof(Query)); //reserva memoria para la estructura Query q (lista enlazada)
    if (!q) return NULL;
    q->head = NULL; //establecemos el head a NULL pq inicialmente la lista esta vacía

    char* inputCopy = strdup(input); //hace copia del input recibido para no perder el valor después
    if (!inputCopy) {
        free(q); //libera memoria de la estructura Query
        return NULL; //retorna NULL pq no pudo copiar la cadena
    }

    char* token = strtok(inputCopy, " "); //primera palabra separada por espacios
    QueryItem** current = &q->head; //inicializa doble puntero que conecta los nuevos elementos al final de la lista

    while (token) {
        QueryItemType type = INCLUDE;
        if (token[0] == '-') {
            type = EXCLUDE;
            token++; //avanza el puntero para que no se tome en cuenta el -
        }
        NormalizeWord(token); //normaliza la palabra: quita simbolos y num solo quedan letras min

        if (strlen(token) > 2) {  //No acepta palabras <2
            QueryItem* nou_item = queryItemCreate(token, type); //crea el nuevo QueryItem y lo enlaza a la lista
            if (!nou_item) {
                token = strtok(NULL, " ");
                continue;
            }
            *current = nou_item; //enlaza el nuevo QueryItem al final de la lista
            current = &((*current)->next); //desplaza el puntero current al siguiente de next
        }
        token = strtok(NULL, " "); //siguiente palabra separada por espacios

    }
    free(inputCopy); //libera la copia del input de la memoria
    return q; //devuelve la lista enlazada q
}

//función que libera la memori
void queryFree(Query* query) {
    if (!query) return;
    QueryItem* curr = query->head; //apunta al 1r elem de la lista

    //va recorriendo la lista
    while (curr) {
        QueryItem* next = curr->next; //guarda el next antes de liberar la memoria
        free(curr->word); //libera memoria copia palabra
        free(curr); //libera memoria nodo
        curr = next; //pasa al siguiente nodo
    }
    free(query); //libera toda la estructura
}