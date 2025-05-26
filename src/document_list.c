#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document_list.h"
#include <stdbool.h>


void documentListInit(DocumentList* list) {
    list->head = NULL;
}

void documentListLoadFromDir(DocumentList* list, const char* dirpath) {
    DIR* dir = opendir(dirpath);
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_REG) {
            char path[512];
            snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);
            Document* doc = documentDeserialize(path);
            if (doc) {
                doc->next = list->head;
                list->head = doc;
            }
        }
    }
    closedir(dir);
}

void documentListPrint(DocumentList* list) {
    Document* curr = list->head;
    int index = 0;
    while (curr) {
        printf("(%d) %s\n", index++, curr->title);
        curr = curr->next;
    }
}

void documentListFree(DocumentList* list) {
    Document* curr = list->head;
    while (curr) {
        Document* next = curr->next;
        documentFree(curr);
        curr = next;
    }
    list->head = NULL;
}