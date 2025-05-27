#ifndef LINK_H
#define LINK_H
#include <stdbool.h>


typedef struct links {
    int size;
    Link* head;
    Link* tail;
} Links;

Link* linkInit();
void linksFree(Links* list);
void linkAppend(Link** head, int id);
bool LinksContains(Links* list, int id);
int LinksGet(Links* list, int index);
#endif