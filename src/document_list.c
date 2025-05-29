#include "document_list.h"
#include "document2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>


void documentsListInit(DocumentsList* list) {
    if (!list) return;
    list->head = NULL;
    list->size = 0;
}

void documentsListAppend(DocumentsList* list, Document* doc) {
    if (!list || !doc) return;
    doc->next = NULL;

    if (!list->head) {
        list->head = doc;
    } else {
        Document* curr = list->head;
        while (curr->next) curr = curr->next;
        curr->next = doc;
    }
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
    if (!list) return;

    Document* curr = list->head;
    int index = 0;
    while (curr) {
        printf("(%d) %s\n", index++, curr->title);
        curr = curr->next;
    }
}

bool documentsListLoadFromDir(DocumentsList* list, const char* dirpath) {
    DIR* dir = opendir(dirpath);
    if (!dir) return false;

    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);

        struct stat path_stat;

        if (stat(path, &path_stat) == 0 && S_ISREG(path_stat.st_mode)) {
            Document* doc = documentDeserialize(path);
            if (doc) {
                documentsListAppend(list, doc);
            }
        }
    }
    closedir(dir);
    return true;

}