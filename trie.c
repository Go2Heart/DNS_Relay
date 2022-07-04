//
// Created by Go2Heart on 2022/6/10.
//


#include "trie.h"


char *formatName(const char *name) {
    int len = strlen(name);
    char *newName = (char *) malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len; i++) {
        if (name[i] >= 'A' && name[i] <= 'Z') {
            newName[i] = name[i] + 32;
        } else {
            newName[i] = name[i];
        }
    }
    newName[len] = '\0';
    return newName;
}

Trie *initTrie() {
    Trie *trie = (Trie *)malloc(sizeof(Trie));
    if (trie == NULL) {
        return NULL;
    }
    memset(trie->children, 0, sizeof(trie->children));
    memset(trie->ip, 0, sizeof(trie->ip));
    trie->is_leaf_ip = false;
    trie->is_leaf_node = false;
    return trie;
}

Trie *insertTrie_table(Trie *trie, const char *name, unsigned char *ip) {
    if (trie == NULL) {
        return NULL;
    }
    if (name == NULL) {
        return NULL;
    }
    if (ip == NULL) {
        return NULL;
    }
    if (strlen(name) == 0) {
        return NULL;
    }
    char *newName = formatName(name);
    int name_len = strlen(newName);
    for (int i = 0; i < name_len; i++) {
        int index;
        if (newName[i] >= 'a' && newName[i] <= 'z') {
            index = newName[i] - 'a';
        } else if (newName[i] >= 0 && newName[i] <= 9) {
            index = newName[i] - '0' + 26;
        } else if (newName[i] == '-') {
            index = 36;
        } else {
            index = 37;
        }
        if (trie->children[index] == NULL) {
            trie->children[index] = initTrie();
        }
        trie = trie->children[index];
    }
    trie->is_leaf_ip = true;
    memcpy(trie->ip, ip, 4);
    return trie;
}

Trie *searchTrie(Trie *trie, const char*name) {
    if (trie == NULL) {
        return NULL;
    }
    if (name == NULL) {
        return NULL;
    }
    if (strlen(name) == 0) {
        return NULL;
    }
    char *newName = formatName(name);
    int name_len = strlen(newName);
    for (int i = 0; i < name_len; i++) {
        int index;
        if (newName[i] >= 'a' && newName[i] <= 'z') {
            index = newName[i] - 'a';
        } else if (newName[i] >= 0 && newName[i] <= 9) {
            index = newName[i] - '0' + 26;
        } else if (newName[i] == '-') {
            index = 36;
        } else {
            index = 37;
        }
        if (trie->children[index] == NULL) {
            return NULL;
        }
        trie = trie->children[index];
    }
    if (trie->is_leaf_ip || trie->is_leaf_node) {
        return trie;
    }
    return NULL;
}

void deleteTrie(Trie *trie) {
    if (trie == NULL) {
        return;
    }
    /*for (int i = 0; i < MAX_LENGTH; i++) {
        if (trie->children[i] != NULL) {
            deleteTrie(trie->children[i]); //recursive delete
        }
    }
    free(trie);*/
    trie->is_leaf_node = false;
}

Trie *insertTrie_linklist(Trie *trie, struct linklist_node *node) {
    if (trie == NULL) {
        return NULL;
    }
    if (node == NULL) {
        return NULL;
    }
    if (node->name == NULL) {
        return NULL;
    }
    if (node->ip == NULL) {
        return NULL;
    }
    if (strlen(node->name) == 0) {
        return NULL;
    }
    char *newName = formatName(node->name);
    int name_len = strlen(newName);
    for (int i = 0; i < name_len; i++) {
        int index;
        if (newName[i] >= 'a' && newName[i] <= 'z') {
            index = newName[i] - 'a';
        } else if (newName[i] >= 0 && newName[i] <= 9) {
            index = newName[i] - '0' + 26;
        } else if (newName[i] == '-') {
            index = 36;
        } else {
            index = 37;
        }
        if (trie->children[index] == NULL) {
            trie->children[index] = initTrie();
        }
        trie = trie->children[index];
    }
    trie->is_leaf_node = true;
    trie->node = node;
    return trie;

}

void printTrie(Trie *trie, int depth) {
    if (trie == NULL) {
        return;
    }
    for (int i = 0; i < MAX_LENGTH; i++) {
        if (trie->children[i] != NULL) {
            printTrie(trie->children[i], depth + 1);
        }
    }
    if (trie->is_leaf_ip) {
        for (int i = 0; i < depth; i++) {
            printf(" ");
        }
        printf("%d.%d.%d.%d\n", trie->ip[0], trie->ip[1], trie->ip[2], trie->ip[3]);
    }
    if (trie->is_leaf_node) {
        for (int i = 0; i < depth; i++) {
            printf(" ");
        }
        printf("%s\n", trie->node->name);
    }
}
