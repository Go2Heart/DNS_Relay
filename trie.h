//
// Created by Go2Heart on 2022/6/10.
//

#ifndef DNSRELAY_TRIE_H
#define DNSRELAY_TRIE_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "linklist.h"

#define MAX_LENGTH 38
/*
 * @brief: 统一化域名的格式
 * @param: name: 域名
 * @return: 格式化后的域名
 */
char* formatName(const char *name);

typedef struct trie {
    struct trie *children[MAX_LENGTH];
    bool is_leaf_ip;
    bool is_leaf_node;
    unsigned char ip[4];
    struct linklist_node *node;

} Trie;

/*
 * @brief: 初始化trie
 * @return: 初始化成功返回trie指针，失败返回NULL
 */
Trie* initTrie();

/*
 * @brief: 插入trie
 * @param: trie: trie指针
 * @param: name: 域名
 * @param: ip: ip地址
 * @return: 插入成功返回trie指针，失败返回NULL
 */
Trie* insertTrie_table(Trie* trie, const char *name, unsigned char* ip);
/*
 * @brief: 插入trie
 * @param: trie: trie指针
 * @param: node: 链表节点
 * @return: 插入成功返回trie指针，失败返回NULL
 */
Trie* insertTrie_linklist(Trie* trie, struct linklist_node* node);

/*
 * @brief: 搜索trie
 * @param: trie: trie指针
 * @param: name: 域名
 * @return: 搜索成功返回trie指针，失败返回NULL
 */
Trie* searchTrie(Trie* trie, const char *name);

/*
 * @brief: 删除trie
 * @param: trie: trie指针
 */
void deleteTrie(Trie* trie);

/*
 * @brief: 打印trie
 * @param: trie: trie指针
 * @param: depth: 打印深度
 * @return: 无
 */
void printTrie(Trie* trie, int depth);



#endif //DNSRELAY_TRIE_H
