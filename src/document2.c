#include "document2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 1024

// Implementación segura de strndup (Windows no la tiene por defecto)
char* strndup(const char* s, size_t n) {
    size_t len = strlen(s);
    if (len > n) len = n;
    char* result = (char*)malloc(len + 1);
    if (!result) return NULL;
    strncpy(result, s, len);
    result[len] = '\0';
    return result;
}

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

    // Leer ID
    if (fscanf(file, "%d\n", &doc->id) != 1) {
        free(doc);
        fclose(file);
        return NULL;
    }

    // Leer título
    char buffer[MAX_LINE_LEN];
    if (!fgets(buffer, MAX_LINE_LEN, file)) {
        free(doc);
        fclose(file);
        return NULL;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    doc->title = strdup(buffer);

    // Inicializar valores
    doc->links = NULL;
    doc->relevance_score = 0.0f;
    char* body = NULL;

    // Leer cuerpo línea por línea
    while (fgets(buffer, MAX_LINE_LEN, file)) {
        buffer[strcspn(buffer, "\n")] = '\0';

        // Detectar enlaces
        char* link_start = strchr(buffer, '[');
        while (link_start) {
            char* link_end = strchr(link_start, ']');
            if (link_end) {
                char* dest_start = strchr(link_end, '(');
                char* dest_end = strchr(link_end, ')');
                if (dest_start && dest_end && dest_start < dest_end) {
                    char* link_text = strndup(link_start + 1, link_end - link_start - 1);
                    char* dest_str = strndup(dest_start + 1, dest_end - dest_start - 1);
                    int dest_id = atoi(dest_str);

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

        // Agregar línea al cuerpo
        if (!body) {
            body = strdup(buffer);
        } else {
            char* new_body = (char*)malloc(strlen(body) + strlen(buffer) + 2);
            strcpy(new_body, body);
            strcat(new_body, "\n");
            strcat(new_body, buffer);
            free(body);
            body = new_body;
        }
    }

    doc->body = body;
    fclose(file);
    return doc;
}
