//
// Created by Go2Heart on 2022/6/11.
//

#ifndef DNSRELAY_TABLE_H
#define DNSRELAY_TABLE_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dnsStructure.h"

#include "trie.h"

Trie* loadTable(char *filename);



#endif //DNSRELAY_TABLE_H
