#include "last3_queries.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>





//Función que inicializa la cola
void initQueue(QueryQueue* q) {
    if (!q) return;

    q->head = 0; //inicio cola, indice 1r elem
    q->tail = -1; //final cola, inice último elem (en el ppio no hay)
    q->contador = 0; //contador a 0 pq todavía no hay elementos

    //inicializa punteros como NULL
    for (int i = 0; i < MAX_QUERIES; i++) {
        q->queries[i] = NULL;
    }
}

//Función que va añadiendo las requests a la cola
bool enqueueQuery(QueryQueue* q, const char* query_str) {
    if (!q || !query_str) return false;
    //si la cola está llena (contador=MAX), elimina el elemnto más antiguo
    if (q->contador == MAX_QUERIES) {
        free(q->queries[q->head]); //libera memoria último elem
        q->head = (q->head + 1) % MAX_QUERIES; //hace avanzar el head
        q->contador--; //disminuye el contador
    }

    q->tail = (q->tail + 1) % MAX_QUERIES; //avanza el tail para indicar el final de la cola
    q->queries[q->tail] = strdup(query_str);  //copia la cadena query_str y la guarda en el tail de la cola
    if (!q->queries[q->tail]) return false;  // Fallo en strdup

    q->contador++;  //aumenta contador
    return true;
}

//Función que muestra las 3 últimas request (print de la cola)
void showLastQueries(const QueryQueue* q) {
    if (!q || q->contador == 0) {
        printf("No hay consultas guardadas.\n");
        return;
    }

    printf("Las últimas %d consultas han sido:\n", q->contador);
    //recorre la cola desde el head hasta el tail para ir imprimiendo los elementos
    for (int i = 0; i < q->contador; i++) {
        int idx = (q->head + i) % MAX_QUERIES; //calcula el índice
        printf(" - %s\n", q->queries[idx]); //muestra la consulta del indice calculado anteriormente
    }
}

//Función que libera la memoria de las cadenas en la cola para usarla otra vez
void freeQueue(QueryQueue* q) {
    if (!q) return;

    for (int i = 0; i < q->contador; i++) {
        int idx = (q->head + i) % MAX_QUERIES; //indice
        free(q->queries[idx]); //Libera memoria de la cadena en la posición idx
    }

    //reinicia todos
    q->head = 0;
    q->tail = -1;
    q->contador = 0;
}
