#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "reverse_index.h"
#include "document2.h"
#include "document_list.h"

#define HASH_MULTIPLIER 31

unsigned long hash(const char* str, int capacity) {
    unsigned long hash = 5381;
    int c;
    while ((c = tolower(*str++))) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }
    return hash % capacity;
}

ReverseIndex* reverseIndexCreate(int capacity) {
    if (capacity <= 0) return NULL;

    ReverseIndex* index = (ReverseIndex*)malloc(sizeof(ReverseIndex));
    if (!index) return NULL;

    index->entries = (ReverseIndexEntry**)calloc(capacity, sizeof(ReverseIndexEntry*));
    if (!index->entries) {
        free(index);
        return NULL;
    }
    index->capacity = capacity;
    index->size = 0;
    return index;
}

void reverseIndexAdd(ReverseIndex* index, const char* word, Document* doc) {
    if (!index || !word || !doc) return;

    char* normalized = strdup(word);
    if (!normalized) return;

    for (char* p = normalized; *p; p++) *p = tolower(*p);

    unsigned long h = hash(normalized, index->capacity);
    ReverseIndexEntry* entry = index->entries[h];

    while (entry) {
        if (strcmp(entry->word, normalized) == 0) {
            Document* current = entry->documents->head;
            while (current) {
                if (current->id == doc->id) {
                    free(normalized);
                    return;
                }
                current = current->next;
            }
            documentsListAppend(entry->documents, doc);
            free(normalized);
            return;
        }
        entry = entry->next;
    }

    ReverseIndexEntry* new_entry = malloc(sizeof(ReverseIndexEntry));
    if (!new_entry) {
        free(normalized);
        return;
    }
    new_entry->word = normalized;
    new_entry->documents = malloc(sizeof(DocumentsList));
    if (!new_entry->documents) {
        free(new_entry);
        free(normalized);
        return;
    }
    new_entry->documents->head = NULL;
    new_entry->documents->size = 0;
    documentsListAppend(new_entry->documents, doc);

    new_entry->next = index->entries[h];
    index->entries[h] = new_entry;
    index->size++;
}

DocumentsList* reverseIndexGet(ReverseIndex* index, const char* word) {
    if (!index || !word) return NULL;

    char* normalized = strdup(word);
    if (!normalized) return NULL;

    for (char* p = normalized; *p; p++) *p = tolower(*p);

    unsigned long h = hash(normalized, index->capacity);
    ReverseIndexEntry* entry = index->entries[h];
    DocumentsList* result = NULL;

    while (entry) {
        if (strcmp(entry->word, normalized) == 0) {
            result = entry->documents;
            break;
        }
        entry = entry->next;
    }
    free(normalized);
    return result;
}

void reverseIndexFree(ReverseIndex* index) {
    if (!index) return;

    for (int i = 0; i < index->capacity; i++) {
        ReverseIndexEntry* entry = index->entries[i];
        while (entry) {
            ReverseIndexEntry* next = entry->next;
            free(entry->word);
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
            word = strtok(body, " \t\n\r.,;:!?()[]{}");
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

Document* findDocumentById(DocumentsList* list, int id) {
    Document* current = list->head;
    while (current) {
        if (current->id == id) return current;
        current = current->next;
    }
    return NULL;
}

ReverseIndex* reverseIndexDeserialize(char* filename, DocumentsList* all_docs) {
    if (!filename || !all_docs) return NULL;

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for reading");
        return NULL;
    }

    ReverseIndex* index = reverseIndexCreate(100);
    if (!index) {
        fclose(file);
        return NULL;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        char* word = strtok(line, " ");
        if (!word) continue;

        char* id_str;
        while ((id_str = strtok(NULL, " "))) {
            int id = atoi(id_str);
            Document* doc = findDocumentById(all_docs, id);
            if (doc) {
                reverseIndexAdd(index, word, doc);
            }
        }
    }

    fclose(file);
    return index;
}