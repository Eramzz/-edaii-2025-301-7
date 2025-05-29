#ifndef LAST3_QUERIES_H
#define LAST3_QUERIES_H

#include <stdbool.h>

#define MAX_QUERIES 3 //num max request que se pueden guardar en la cola

//estructura cola circular para guardar requests
typedef struct {
    char* queries[MAX_QUERIES]; //array de medida 3 (queremos solo las 3 ultimas requests)
    int head; //indice 1r elemento, más antiguo
    int tail; //indice ultimo elemento, más reciente
    int contador; //num elementos en la cola (se va actualizando)
} QueryQueue;

void initQueue(QueryQueue* q);  //Inicializa cola
bool enqueueQuery(QueryQueue* q, const char* query_str);  //Añade la nueva request a la cola
void showLastQueries(const QueryQueue* q);  //Enseña las últimas 3 colas
void freeQueue(QueryQueue* q); //Libera memoria cola


#endif //LAST3_QUERIES_H