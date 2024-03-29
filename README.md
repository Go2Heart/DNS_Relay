# DNS_Relay
#### 1.1 任务描述

课程设计要求设计一个DNS服务器程序，读入“IP地址-域名”对照表，当客户端查询域名对应的IP地址时，用域名检索该对照表，有三种可能检索结果：

* 检索结果：ip地址0.0.0.0，则向客户端返回“域名不存在”的报错消息（不良网站拦截功能）

* 检索结果：普通IP地址，则向客户端返回该地址（服务器功能）

* 表中未检到该域名，则向因特网DNS服务器发出查询，并将结果返给客户端（中继功能）

课程设计一个人独立完成。

#### 1.2 开发环境

* Mac OS Monterey 12.5
* Apple Clang Version 14.0.0
* Cmake Version 3.21
* CLion 2022.1
* Visual Studio Code 1.65.1

### 2 功能需求

**基本需求** 从**1.1**节的三个功能出发，本实验设计将包括如下四个基本需求：

1. 解析和构建DNS报文。
2. 监听所在计算机的53端口，获取DNS请求，并将查询到的DNS 回复发送回本地。
3. 加载本地的“IP 地址-域名”对照表，维护一个数据结构，用于增添、查询 DNS 记录。
4. 将 DNS 请求发送到远程服务器的 53 端口，并且监听来自远程服务器的回复。

**额外需求** 此外，基于性能和实用性的考虑，本实验设计还实现了如下数个额外需求：

1. 支持A、AAAA、TXT、SOA、NS、CNAME、PTR反向查询、MX等多种报文的查询。
2. Unix/Linux/Windows多平台兼容。
3. 支持一个域名对应多个IP地址。
4. 输出多级调试信息和DNS报文内容。
5. 在转发DNS给远程服务器时重新分配序号，防止多用户查询时可能的冲突。
6. 采用Select进行单线程非阻塞式IO的多路复用，可以实现非阻塞IO的功能以满足多用户查询时一定的运行效率和高并发需求。
7. 实现基于双向循环链表的LRU的高速缓存，对于经常访问的记录，加快查询效率，复杂度O(1)。
8. 实现字典树Trie的域名查询功能，并且同时使用在本地静态表查询和动态cache查询中，大大降低本地查询的时间，复杂度O(1)。
