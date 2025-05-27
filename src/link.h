#ifndef LINK_H
#define LINK_H

typedef struct Link {
    int id;               // ✅ solo tiene el ID del documento destino
    struct Link* next;
} Link;

Link* linkCreate(int id);
void linkAppend(Link** head, int id);
void linkFree(Link* head);

#endif

