//
// Created by Go2Heart on 2022/6/10.
//

#ifndef DNSRELAY_CACHE_H
#define DNSRELAY_CACHE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linklist.h"
#include "trie.h"

#define MAX_CACHE_SIZE 64
/*
 * @brief: LRU缓存的定义与实现, 基于双向循环链表和字典树实现了LRU缓存的插入、删除、查找等操作
 * 实现了时间复杂度为O(1)的插入、删除、查找操作
 *
 */
typedef struct cache {
    int size;
    Head *list;
    Trie *trie;
} Cache;

/*
 * @brief: 初始化缓存
 * @param: cache: 缓存指针
 */
bool initCache(Cache* cache);


/*
 * @brief: 查询缓存中是否存在某个域名
 * @param: cache: 缓存指针
 * @param: name: 域名
 * @param: ip: ip地址
 * @return: 存在返回true，并且更新ip地址，不存在返回false
 */
bool queryCache(Cache* cache , const char *name, unsigned char* ip);

/*
 * @brief: 向缓存中插入一个记录
 * @param: cache: 缓存指针
 * @param: name: 域名
 * @param: ip: ip地址
 * @return: 插入成功返回true，失败返回false
 */
bool insertCache(Cache* cache ,char* name, unsigned char* ip);
/*
 * @brief: 打印缓存中的所有记录
 * @param: cache: 缓存指针
 * @return: 无
 */
void printCache(Cache* cache);

/*
 * @brief: 获得缓存中第一个记录的ip地址
 * @param: cache: 缓存指针
 * @return: 第一个记录的ip地址
 */
unsigned char* getFirstCache(Cache* cache);






#endif //DNSRELAY_CACHE_H
