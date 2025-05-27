
#include "document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype
#include <stdbool.h>

#define _GNU_SOURCE


#define INITIAL_LINKS_CAPACITY 10
// funcion
Document* documentDeserialize(char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    Document* doc = (Document*)malloc(sizeof(Document));
    if (!doc) {
        fclose(file);
        return NULL;
    }

    // Read document ID
    if (fscanf(file, "%d\n", &doc->id) != 1) {
        free(doc);
        fclose(file);
        return NULL;
    }

    // Read title
    char* title = NULL;
    size_t title_len = 0;
    if (fgets(&title, &title_len, file) == -1) {
        free(doc);
        fclose(file);
        return NULL;
    }
    // Remove newline if present
    title[strcspn(title, "\n")] = '\0';
    doc->title = title;

    // Read body
    char* body = NULL;
    size_t body_len = 0;
    char* line = NULL;
    size_t line_len = 0;
    ssize_t read;

    doc->links = NULL;
    doc->relevance_score = 0.0f;

    while ((read = fgets(&line, &line_len, file)) != -1) {
        // Check for links in the line
        char* link_start = strchr(line, '[');
        while (link_start) {
            char* link_end = strchr(link_start, ']');
            if (link_end) {
                char* dest_start = strchr(link_end, '(');
                char* dest_end = strchr(link_end, ')');
                if (dest_start && dest_end && dest_start < dest_end) {
                    // Extract link text and destination
                    char* link_text = strndup(link_start + 1, link_end - link_start - 1);
                    char* dest_str = strndup(dest_start + 1, dest_end - dest_start - 1);
                    int dest_id = atoi(dest_str);

                    // Create new link
                    Link* new_link = (Link*)malloc(sizeof(Link));
                    new_link->text = link_text;
                    new_link->destination_id = dest_id;
                    new_link->next = doc->links;
                    doc->links = new_link;

                    free(dest_str);
                }
            }
            link_start = strchr(link_end ? link_end + 1 : link_start + 1, '[');
        }

        // Add line to body
        line[strcspn(line, "\n")] = '\0'; // Remove newline
        if (!body) {
            body = strdup(line);
        } else {
            char* new_body = (char*)malloc(strlen(body) + strlen(line) + 2);
            strcpy(new_body, body);
            strcat(new_body, "\n");
            strcat(new_body, line);
            free(body);
            body = new_body;
        }
    }

    doc->body = body;
    free(line);
    fclose(file);
    return doc;
}

void documentFree(Document* doc) {
    if (!doc) return;

    free(doc->title);
    free(doc->body);

    Link* current = doc->links;
    while (current) {
        Link* next = current->next;
        free(current->text);
        free(current);
        current = next;
    }

    free(doc);
}

void documentsListAppend(DocumentsList* list, Document* doc) {
    if (!list || !doc) return;

    if (!list->head) {
        list->head = doc;
    } else {
        Document* current = list->head;
        while (current->next) {
            current = current->next;
        }
        current->next = doc;
    }
    doc->next = NULL;
    list->size++;
}

Document* documentsListGet(DocumentsList* list, int index) {
    if (!list || index < 0 || index >= list->size) return NULL;

    Document* current = list->head;
    for (int i = 0; i < index && current; i++) {
        current = current->next;
    }
    return current;
}

void documentsListFree(DocumentsList* list) {
    if (!list) return;

    Document* current = list->head;
    while (current) {
        Document* next = current->next;
        documentFree(current);
        current = next;
    }
    list->head = NULL;
    list->size = 0;
}
//LAB 4
void sortDocumentsByRelevance(DocumentsList* list) {
    if (!list || !list->head) return;

    int swapped;
    Document* ptr1;
    Document* lptr = NULL;

    do {
        swapped = 0;
        ptr1 = list->head;

        while (ptr1->next != lptr) {
            if (ptr1->relevance_score < ptr1->next->relevance_score) {
                // Intercambia los contenidos de los documentos (no los punteros)
                int temp_id = ptr1->id;
                char* temp_title = ptr1->title;
                char* temp_body = ptr1->body;
                float temp_score = ptr1->relevance_score;
                Link* temp_links = ptr1->links;

                ptr1->id = ptr1->next->id;
                ptr1->title = ptr1->next->title;
                ptr1->body = ptr1->next->body;
                ptr1->relevance_score = ptr1->next->relevance_score;
                ptr1->links = ptr1->next->links;

                ptr1->next->id = temp_id;
                ptr1->next->title = temp_title;
                ptr1->next->body = temp_body;
                ptr1->next->relevance_score = temp_score;
                ptr1->next->links = temp_links;

                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;  // Optimización: ya no hace falta revisar hasta el final
    } while (swapped);
}
