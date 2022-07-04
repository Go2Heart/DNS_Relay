//
// Created by Go2Heart on 2022/6/11.
//

#include "table.h"
unsigned int readLine(char *line, unsigned int size, FILE *fp) {
    unsigned int i = 0;
    char c;
    while ((c = fgetc(fp)) != '\n' && c != EOF) {
        if (i < size) {
            line[i++] = c;
        }
    }
    line[i] = '\0';
    return i;
}
Trie *loadTable(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    }
    Trie *trie = initTrie();
    char *line = NULL;
    int len = 1024;
    ssize_t read;
    char buf[1024];
    while (fgets(buf, len, fp) != NULL) {
        line = (char *)malloc(sizeof(buf));
        memcpy(line, buf, sizeof(buf));
        if (line == "\r\n" || line == "\n") {
            continue;
        }
        char *ip = strtok(line, " ");
        char *name = strtok(NULL, "\r\n");
        unsigned char ip_bytes[4] = {0};
        char *ip_str = strtok(ip, ".");
        for (int i = 0; i < 4; i++) {
            int ip_int = atoi(ip_str);
            ip_bytes[i] = ip_int;
            ip_str = strtok(NULL, ".");
        }
        insertTrie_table(trie, name, ip_bytes);
    }
    fclose(fp);
    return trie;
}
