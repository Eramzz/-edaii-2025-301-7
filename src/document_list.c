#include "document_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>

void documentsListInit(DocumentsList* list) {
    list->head = NULL;
    list->size = 0;
}

void documentsListAppend(DocumentsList* list, Document* doc) {
    if (!list || !doc) return;

    doc->next = list->head;
    list->head = doc;
    list->size++;
}

void documentsListFree(DocumentsList* list) {
    if (!list) return;

    Document* curr = list->head;
    while (curr) {
        Document* next = curr->next;
        documentFree(curr, true);
        curr = next;
    }
    list->head = NULL;
    list->size = 0;
}

void documentsListPrint(DocumentsList* list) {
    Document* curr = list->head;
    int index = 0;
    while (curr) {
        printf("(%d) %s\n", index++, curr->title);
        curr = curr->next;
    }
}

void documentsListLoadFromDir(DocumentsList* list, const char* dirpath) {
    DIR* dir = opendir(dirpath);
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir))) {
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);

        struct stat path_stat;
        stat(path, &path_stat);
        if (S_ISREG(path_stat.st_mode)) {
            Document* doc = documentDeserialize(path);
            if (doc) {
                documentsListAppend(list, doc);
            }
        }
    }
    closedir(dir);
}