#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document2.h"

#define MAX_LINE_LEN 1024

// Implementación segura de strndup (Windows no la tiene por defecto)
static char* _strndup(const char* s, size_t n) {
    char* result = (char*)malloc(n + 1);
    if (!result) return NULL;
    strncpy(result, s, n);
    result[n] = '\0';
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

    // Inicializar valores por defecto
    doc->id = 0;
    doc->title = NULL;
    doc->body = NULL;
    doc->links = NULL;
    doc->relevance_score = 0.0f;

    // Leer ID
    if (fscanf(file, "%d\n", &doc->id) != 1) {
        documentFree(doc, true);
        fclose(file);
        return NULL;
    }

    // Leer título
    char buffer[MAX_LINE_LEN];
    if (!fgets(buffer, MAX_LINE_LEN, file)) {
        documentFree(doc, true);
        fclose(file);
        return NULL;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    doc->title = strdup(buffer);
    if (!doc->title) {
        documentFree(doc, true);
        fclose(file);
        return NULL;
    }

    size_t body_size = 0;
    char* body = NULL;

    // Leer cuerpo línea por línea
    while (fgets(buffer, MAX_LINE_LEN, file)) {
        buffer[strcspn(buffer, "\n")] = '\0';

        // Detectar enlaces
        char* link_start = buffer;
        while ((link_start = strchr(link_start, '['))) {
            char* link_end = strchr(link_start, ']');
            if (!link_end) break;

            char* dest_start = strchr(link_end, '(');
            char* dest_end = strchr(link_end, ')');
            if (dest_start && dest_end && dest_start < dest_end) {
                int dest_id = atoi(dest_start + 1);
                Link* new_link = (Link*)malloc(sizeof(Link));
                if (!new_link) continue;
                new_link->id = dest_id;
                new_link->next = doc->links;
                doc->links = new_link;
            }
            link_start =  link_end + 1;
        }
        size_t line_len = strlen(buffer);
        char* new_body = (char*)realloc(body, body_size + line_len + 2);

        if (!new_body) {
            free(body);
            documentFree(doc, true);
            fclose(file);
            return NULL;
        }
        body = new_body;
        strcpy(body + body_size, buffer);
        body_size += line_len;
        strcat(body + body_size, "\n");
        body_size += 1;
    }
    doc->body = body;
    fclose(file);
    return doc;
}

void documentFree(Document* doc, bool freeLinks) {
    if (!doc) return;
    free(doc->title);
    free(doc->body);
    if (freeLinks) linkFree(doc->links);
	free(doc);
}