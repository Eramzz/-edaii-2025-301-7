#ifndef QUERY_H
#define QUERY_H

#include <stdbool.h>

typedef enum { INCLUDE, EXCLUDE } QueryItemType;


typedef struct QueryItem {
    char* word;
    QueryItemType type;
    struct QueryItem* next;
} QueryItem;

typedef struct Query {
    QueryItem* head;
} Query;

QueryItem* queryItemCreate(const char* word, QueryItemType type);
void NormalizeWord(char* word);
Query* queryInit(const char* input);
void queryFree(Query* query);


#endif //QUERY_H
