#include "reverse2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define HASH_MULTIPLIER 31

unsigned long hash(const char* str, int capacity) {
    unsigned long hash = 0;
    while (*str) {
        hash = hash * HASH_MULTIPLIER + tolower(*str);
        str++;
    }
    return hash % capacity;
}

ReverseIndex* reverseIndexCreate(int capacity) {
    ReverseIndex* index = (ReverseIndex*)malloc(sizeof(ReverseIndex));
    if (!index) return NULL;

    index->entries = (ReverseIndexEntry**)calloc(capacity, sizeof(ReverseIndexEntry*));
    if (!index->entries) {
        free(index);
        return NULL;
    }

    index->size = 0;
    index->capacity = capacity;
    return index;
}

void reverseIndexAdd(ReverseIndex* index, char* word, Document* doc) {
    if (!index || !word || !doc) return;

    for (char* p = word; *p; p++) *p = tolower(*p);

    unsigned long h = hash(word, index->capacity);
    ReverseIndexEntry* entry = index->entries[h];

    while (entry) {
        if (strcmp(entry->word, word) == 0) {
            Document* current = entry->documents->head;
            while (current) {
                if (current->id == doc->id) return;
                current = current->next;
            }
            Document* doc_copy = (Document*)malloc(sizeof(Document));
            doc_copy->id = doc->id;
            doc_copy->title = strdup(doc->title);
            doc_copy->body = NULL;
            doc_copy->links = NULL;
            doc_copy->relevance_score = 0.0f;
            doc_copy->next = NULL;
            documentsListAppend(entry->documents, doc_copy);
            return;
        }
        entry = entry->next;
    }

    ReverseIndexEntry* new_entry = (ReverseIndexEntry*)malloc(sizeof(ReverseIndexEntry));
    new_entry->word = strdup(word);
    new_entry->documents = (DocumentsList*)malloc(sizeof(DocumentsList));
    new_entry->documents->head = NULL;
    new_entry->documents->size = 0;

    Document* doc_copy = (Document*)malloc(sizeof(Document));
    doc_copy->id = doc->id;
    doc_copy->title = strdup(doc->title);
    doc_copy->body = NULL;
    doc_copy->links = NULL;
    doc_copy->relevance_score = 0.0f;
    doc_copy->next = NULL;
    documentsListAppend(new_entry->documents, doc_copy);

    new_entry->next = index->entries[h];
    index->entries[h] = new_entry;
    index->size++;
}

DocumentsList* reverseIndexGet(ReverseIndex* index, char* word) {
    if (!index || !word) return NULL;

    char* normalized = strdup(word);
    for (char* p = normalized; *p; p++) *p = tolower(*p);

    unsigned long h = hash(normalized, index->capacity);
    ReverseIndexEntry* entry = index->entries[h];

    while (entry) {
        if (strcmp(entry->word, normalized) == 0) {
            free(normalized);
            return entry->documents;
        }
        entry = entry->next;
    }

    free(normalized);
    return NULL;
}

void reverseIndexFree(ReverseIndex* index) {
    if (!index) return;

    for (int i = 0; i < index->capacity; i++) {
        ReverseIndexEntry* entry = index->entries[i];
        while (entry) {
            ReverseIndexEntry* next = entry->next;
            free(entry->word);
            documentsListFree(entry->documents);
            free(entry->documents);
            free(entry);
            entry = next;
        }
    }

    free(index->entries);
    free(index);
}

void reverseIndexBuild(ReverseIndex* index, DocumentsList* documents) {
    if (!index || !documents) return;

    Document* current = documents->head;
    while (current) {
        char* title = strdup(current->title);
        char* word = strtok(title, " \t\n\r.,;:!?()[]{}");
        while (word) {
            reverseIndexAdd(index, word, current);
            word = strtok(NULL, " \t\n\r.,;:!?()[]{}");
        }
        free(title);

        if (current->body) {
            char* body = strdup(current->body);
            char* word = strtok(body, " \t\n\r.,;:!?()[]{}");
            while (word) {
                reverseIndexAdd(index, word, current);
                word = strtok(NULL, " \t\n\r.,;:!?()[]{}");
            }
            free(body);
        }

        current = current->next;
    }
}

void reverseIndexSerialize(ReverseIndex* index, char* filename) {
    if (!index || !filename) return;

    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    for (int i = 0; i < index->capacity; i++) {
        ReverseIndexEntry* entry = index->entries[i];
        while (entry) {
            fprintf(file, "%s", entry->word);
            Document* doc = entry->documents->head;
            while (doc) {
                fprintf(file, " %d", doc->id);
                doc = doc->next;
            }
            fprintf(file, "\n");
            entry = entry->next;
        }
    }

    fclose(file);
}

ReverseIndex* reverseIndexDeserialize(char* filename) {
    if (!filename) return NULL;

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for reading");
        return NULL;
    }

    char line[1024];
    ReverseIndex* temp = reverseIndexCreate(100);
    if (!temp) {
        fclose(file);
        return NULL;
    }

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';

        char* word = strtok(line, " ");
        if (!word) continue;

        char* id_str;
        while ((id_str = strtok(NULL, " "))) {
            int id = atoi(id_str);
            temp->size++;
        }
    }

    fclose(file);
    reverseIndexFree(temp);

    ReverseIndex* index = reverseIndexCreate(temp->size * 2);
    return index;
}
