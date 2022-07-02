//
// Created by Go2Heart on 2022/6/11.
//

#include "table.h"

Trie *loadTable(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    }
    Trie *trie = initTrie();
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    /*
    while ((read = getline(&line, &len, fp)) != -1) {
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
    }*/
    fclose(fp);
    return trie;
}
