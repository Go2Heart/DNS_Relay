/** @file */
//
// Created by Go2Heart on 2022/6/11.
//

#ifndef DNSRELAY_TABLE_H
#define DNSRELAY_TABLE_H

#include "dnsStructure.h"
#include "trie.h"
/**
 * @details 读取本地静态表格文件，并将其加载到trie树中
 * @param filename
 * @return Trie*
 */
Trie* loadTable(char *filename);



#endif //DNSRELAY_TABLE_H
