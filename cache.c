//
// Created by Go2Heart on 2022/6/10.
//

#include "cache.h"


bool initCache(Cache *cache) {
    cache->size = 0;
    if((cache->list = initLinklist(cache->list))== NULL) {
        return false;
    }
    if((cache->trie = initTrie(cache->trie)) == NULL) {
        return false;
    }
    return true;
}

bool queryCache(Cache *cache, const char *name, unsigned char *ip) {
    Trie* re = searchTrie(cache->trie, name);
    if(re == NULL || re->is_leaf_node == false) {
        return false;
    }
    //if found is leaf node, return ip and update LRU
    Node* node = deleteNodeLinklist(cache->list, re->node);
    memcpy(ip, node->ip, 4);
    //todo update trie's node when deleting node from linklist
    free (node);
    Head* head = headInsertLinklist(cache->list, name, ip);
    re->node = head->head;
    return true;
}

bool insertCache(Cache *cache, char *name, unsigned char *ip) {
    Trie* re = searchTrie(cache->trie, name);
    if(re == NULL) {
        //if not found, insert into trie
        Node *node = (Node*)malloc(sizeof(Node));
        if(node == NULL) {
            return false;
        }
        strcpy(node->name, name);
        memcpy(node->ip, ip, 4);

        if(cache->size < MAX_CACHE_SIZE) {
            //if cache is not full, insert into cache
            Head *head = headInsertLinklist(cache->list, node->name, node->ip);
            insertTrie_linklist(cache->trie, head->head);
            cache->size++;
        } else {
            //if cache is full, delete tail node and insert into cache
            //change original tail node record to static table
            // todo  change head needed?
            Trie* trie = searchTrie(cache->trie, cache->list->head->prev->name);
            trie->is_leaf_node = false;
            deleteTailLinklist(cache->list);
            Head *head = headInsertLinklist(cache->list, node->name, node->ip);
            insertTrie_linklist(cache->trie, head->head);
        }
    } else {
        //if found, update ip and LRU
        Node* node = deleteNodeLinklist(cache->list, re->node);
        free (node);
        Head* head = headInsertLinklist(cache->list, name, ip);
        insertTrie_linklist(cache->trie, head->head);
    }
    return true;
}

void printCache(Cache *cache) {
    printf("cache size: %d\n", cache->size);
    printLinklist(cache->list);
}

unsigned char *getFirstCache(Cache *cache) {
    return cache->list->head->ip;
}
