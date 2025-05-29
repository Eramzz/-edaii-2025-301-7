#ifndef LINK_H
#define LINK_H

typedef struct Link {
    int id; // solo tiene el ID del documento destino
    struct Link* next;
} Link;

Link* linkCreate(int id); // Crea un nuevo enlace. Retorna NULL si falla malloc.
void linkAppend(Link** head, int id); // Añade un enlace al final de la lista.
void linkFree(Link* head); // Libera toda la memoria de la lista de enlaces.

#endif

