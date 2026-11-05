/* See sys/socket.h: no network on the Amiga build. */
#ifndef AMIGA_NETINET_IN_H
#define AMIGA_NETINET_IN_H
#include <sys/socket.h>
#include <stdint.h>
typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;
struct in_addr { in_addr_t s_addr; };
struct sockaddr_in { unsigned char sin_len; sa_family_t sin_family; in_port_t sin_port; struct in_addr sin_addr; char sin_zero[8]; };
struct in6_addr { uint8_t s6_addr[16]; };
struct sockaddr_in6 { unsigned char sin6_len; sa_family_t sin6_family; in_port_t sin6_port; uint32_t sin6_flowinfo; struct in6_addr sin6_addr; uint32_t sin6_scope_id; };
#define INADDR_ANY       ((in_addr_t)0x00000000)
#define INADDR_LOOPBACK  ((in_addr_t)0x7f000001)
#define INADDR_NONE      ((in_addr_t)0xffffffff)
#define IPPROTO_IP   0
#define IPPROTO_TCP  6
#define IPPROTO_UDP  17
#define IPPROTO_IPV6 41
#define IPV6_V6ONLY  27
#define INET_ADDRSTRLEN  16
#define INET6_ADDRSTRLEN 46
/* The 68k is big-endian: network order is its own. */
#define htons(x) ((uint16_t)(x))
#define ntohs(x) ((uint16_t)(x))
#define htonl(x) ((uint32_t)(x))
#define ntohl(x) ((uint32_t)(x))
#endif
