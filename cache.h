/** @file */
//
// Created by Go2Heart on 2022/6/10.
//

#ifndef DNSRELAY_CACHE_H
#define DNSRELAY_CACHE_H

#include "trie.h"

#define MAX_CACHE_SIZE 64
/**
 * @brief: LRU缓存的定义与实现, 基于双向循环链表和字典树实现了LRU缓存的插入、删除、查找等操作
 * 实现了时间复杂度为O(1)的插入、删除、查找操作
 *
 */
typedef struct cache {
    int size; /* 缓存的大小 */
    Head *list; /* 链表头指针 */
    Trie *trie; /* 字典树头指针 */
} Cache;

/**
 * @callgraph
 * @brief: 初始化缓存
 * @param: cache: 缓存指针
 */
bool initCache(Cache* cache);


/**
 * @callgraph
 * @details 插入缓存
 * @brief: 查询缓存中是否存在某个域名
 * @param: cache: 缓存指针
 * @param: name: 域名
 * @param: ip: ip地址链表
 * @return: 存在返回true，并且更新ip地址，不存在返回false
 */
bool queryCache(Cache* cache , const char *name, Ip *ip);

/**
 * @brief: 向缓存中插入一个记录
 * @param: cache: 缓存指针
 * @param: name: 域名
 * @param: ip: ip地址链表
 * @return: 插入成功返回true，失败返回false
 */
bool insertCache(Cache* cache , char* name, Ip *ip);
/**
 * @brief: 打印缓存中的所有记录
 * @param: cache: 缓存指针
 * @param: fp: 文件指针
 * @return: 无
 */
void printCache(Cache* cache, FILE* fp);

#endif //DNSRELAY_CACHE_H
