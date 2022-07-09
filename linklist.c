//
// Created by Go2Heart on 2022/6/10.
//

#include "linklist.h"
Head *initLinklist(Head *head) {
    head = (Head *)malloc(sizeof(Head));
    head->length = 0;
    head->head = NULL;
    return head;
}

Head* headInsertLinklist(Head *list,char *name, unsigned char *ip) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL) {
        return NULL;
    }
    strcpy(node->name, name);
    memcpy(node->ip, ip, 4);
    if (list->head != NULL) {
        list->head->prev->next = node; /* 将原尾节点的next指针指向新节�? */
        node->prev = list->head->prev; /* 将新节点的prev指针指向尾节�? */
        list->head->prev = node; /* 将原头节点的prev指针指向新节�? */
        node->next = list->head; /* 将新节点的next指针指向原头节点 */
        list->head = node; /* 将头结点更新为插入的节点*/
    } else {
        list->head = node;
        node->next = node;
        node->prev = node;
    }


    list->length++;
    return list;
}

Head *deleteTailLinklist(Head *list) {
    if (list->head == NULL) {
        return NULL;
    }
    if (list->head == list->head->next) {
        list->head = NULL;
        list->length = 0;
        free (list->head);
    } else {
        Node *node = list->head->prev;
        list->head->prev = node->prev;
        node->prev->next = list->head;
        free(node);
    }
    list->length--;
    return list;
}

Node *deleteNodeLinklist(Head *list, Node *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    if (list != NULL) {
        list->length--;
    }
    if(node == list->head) {
        list->head = node->next;
    }
    if(list->length == 0){
        list->head = NULL;
    }
    return node;
}

void printLinklist(Head *list, FILE *fp) {
    Node *node = list->head;
    int i = 0;
    while (i < list->length) {

        printf("No.%d node name: %s ", i, node->name);
        printf("node ip: %d.%d.%d.%d\n", node->ip[0], node->ip[1], node->ip[2], node->ip[3]);
        fprintf(fp, "No.%d node name: %s ", i, node->name);
        fprintf(fp, "%s %d.%d.%d.%d\n", node->name, node->ip[0], node->ip[1], node->ip[2], node->ip[3]);
        node = node->next;
        i++;
    }

}





