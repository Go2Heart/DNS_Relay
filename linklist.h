//
// Created by Go2Heart on 2022/6/10.
//

#ifndef DNSRELAY_LINKLIST_H
#define DNSRELAY_LINKLIST_H


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * @brief: 循环双向链表的定义与实现
 */

typedef struct linklist_node {
    char name[256];
    unsigned char ip[4];
    struct linklist_node *next;
    struct linklist_node *prev;
} Node;
typedef struct linklist_head {
    int length;
    Node* head;
} Head;
/*
 * @brief: 初始化链表
 * @param: head: 链表头指针
 */
Head* initLinklist(Head* head);

/*
 * @brief: 头插法插入链表
 * @param: list: 链表头指针
 * @param: name: 域名
 * @param: ip: ip地址
 * @return: 插入成功返回链表头指针，失败返回NULL
 */
Head* headInsertLinklist(Head* list, char* name, unsigned char* ip);

/*
 * @brief: 尾插法插入链表
 * @param: list: 链表头指针
 * @param: name: 域名
 * @param: ip: ip地址
 * @return: 插入成功返回链表头指针，失败返回NULL
 */
Head* tailInsertLinklist(Head* list, char* name, unsigned char* ip);

/*
 * @brief: 删除头节点
 * @param: list: 链表头指针
 * @return: 删除成功返回链表头指针，失败返回NULL
 */
Head* deleteHeadLinklist(Head* list);

/*
 * @brief: 删除尾节点
 * @param: list: 链表头指针
 * @return: 删除成功返回链表头指针，失败返回NULL
 * @note: 删除成功后，链表长度减1
 */
Head* deleteTailLinklist(Head* list);

/*
 * @brief: 删除指定节点
 * @param: node: 指定节点
 * @return: 返回删除的节点
 */
Node *deleteNodeLinklist(Head *list, Node *node);

/*
 * @brief: 打印链表
 * @param: list: 链表头指针
 * @return: 无
 */
void printLinklist(Head* list);




#endif //DNSRELAY_LINKLIST_H
