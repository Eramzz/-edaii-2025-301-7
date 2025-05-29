#include <stdlib.h>
#include "link.h"

Link* linkCreate(int id) {
    Link* new_link = (Link*)malloc(sizeof(Link));
    if (!new_link) return NULL;

    new_link->id = id;
    new_link->next = NULL;
    return new_link;
}

void linkAppend(Link** head, int id) {
    if (!head) return;

    Link* new_link = linkCreate(id);
    if (!new_link) return;

    if (!*head) {
        *head = new_link;
    } else {
        Link* curr = *head;
        while (curr->next) curr = curr->next;
        curr->next = new_link;
    }
}

void linkFree(Link* head) {
    Link* tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}