//
// Created by Dell on 24/04/2025.
//

#ifndef QUERY_H
#define QUERY_H

typedef struct QueryItem {
    char* word;
    struct QueryItem* next;
} QueryItem;

typedef struct Query {
    QueryItem* head;
} Query;

QueryItem* queryItemCreate(const char* word);
void NormalizeWord(char* word);
Query* queryInit(const char* input);
void queryFree(Query* query);


#endif //QUERY_H
