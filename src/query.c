#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "document2.h"
#include "link.h"
#include "reverse_index.h"
#include "query.h"
#include "directed_graph.h"
#include "last3_queries.h"
#include <stdbool.h>



//Función que crea un nuevo elemento para la linked ist
QueryItem* queryItemCreate(const char* word) {
    QueryItem* item = (QueryItem*)malloc(sizeof(QueryItem)); //reserva memoria para una request
    item->word = strdup(word); //duplica la cadena utilizando strdup para tener la copia completa de la palabra en el nodo
    item->next = NULL; //inicializa el puntero next como NULL
    return item; //retorna nuevo nodo creado
}

//convierte todas las letra a minúscula y elimina signos de puntuación, números o carácteres especiales
void NormalizeWord(char* word) {
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
    Query* q = (Query*)malloc(sizeof(Query)); //reserva memoria para la estructura Query q (lista enlazada)
    q->head = NULL; //establecemos el head a NULL pq inicialmente la lista esta vacía
    char* inputCopy = strdup(input); //hace copia del input recibido para no perder el valor después
    char* token = strtok(inputCopy, " "); //primera palabra separada por espacios
    QueryItem** current = &q->head; //inicializa doble puntero que conecta los nuevos elementos al final de la lista

    //mientras haya palabras
    while (token) {
        NormalizeWord(token); //normaliza la palabra: quita simbolos y num solo quedan letras min

        if (strlen(token) > 2) {  //No acepta palabras <2
            *current = queryItemCreate(token); //crea el nuevo QueryItem y lo enlaza a la lista
            current = &((*current)->next); //desplaza el puntero current al siguiente de next
        }

        //pasa a la siguiente palabra
        token = strtok(NULL, " ");
    }

    free(inputCopy); //libera la copia del input de la memoria

    return q; //devuelve la lista enlazada q
}

//función que libera la memori
void queryFree(Query* query) {
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