/** @file */
//
// Created by Go2Heart on 2022/6/10.
//

#ifndef DNSRELAY_LINKLIST_H
#define DNSRELAY_LINKLIST_H


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * @brief: 循环双向链表的定义与实现
 */
/**
 * @brief ip链表的节点结构体
 */
typedef struct linklist_ip{
    unsigned char ip[4];
    struct linklist_ip *next;
} Ip;
/**
 * @brief 复制一个Ip结构体
 * @param dest
 * @param src
 */
void copyIp(Ip *dest, Ip *src);
/**
 * @brief 链表节点的定义
 */
typedef struct linklist_node {
    char name[256];
    struct linklist_ip *ipHead;
    unsigned int ttl;
    struct linklist_node *next;
    struct linklist_node *prev;
} Node;

/**
 * @brief 链表头的定义，用于存放链表的长度和头节点
 */
typedef struct linklist_head {
    int length;
    Node* head;
} Head;
/**
 * @brief: 初始化链表
 * @param: head: 链表头指针
 */
Head* initLinklist(Head* head);

/**
 * @brief: 头插法插入链表
 * @param: list: 链表头指针
 * @param: name: 域名
 * @param: ip: ip地址
 * @return: 插入成功返回链表头指针，失败返回NULL
 */
Head* headInsertLinklist(Head* list, char* name, Ip *ip);
/**
 * @brief: 删除尾节点
 * @param: list: 链表头指针
 * @return: 删除成功返回链表头指针，失败返回NULL
 * @note: 删除成功后，链表长度减1
 */
Head* deleteTailLinklist(Head* list);

/**
 * @brief: 删除指定节点
 * @param: node: 指定节点
 * @return: 返回删除的节点, 该节点需要在外部free
 */
Node *deleteNodeLinklist(Head* list, Node* node);

/**
 * @brief: 打印链表
 * @param: list: 链表头指针
 * @return: 无
 */
void printLinklist(Head* list, FILE* fp);




#endif //DNSRELAY_LINKLIST_H
