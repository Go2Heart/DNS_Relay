cmake_minimum_required(VERSION 3.0.0)
project(dnsRelay VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(dnsRelay dnsStructure.h dnsResolver.h dnsResolver.c dnsRelay.c cache.c cache.h trie.c trie.h linklist.c linklist.h table.c table.h dnsServer.c dnsServer.h)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})


include(CPack)
