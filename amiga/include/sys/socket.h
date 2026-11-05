/* No network on the Amiga build: just enough of the BSD socket interface
 * for enet.h and the engine's mmulti.cpp to compile. Every call is in
 * amiga/net_stubs.c and fails (no network games).
 * The AmiCradle Team. GPL 2 or later, like EDuke32. */
#ifndef AMIGA_SYS_SOCKET_H
#define AMIGA_SYS_SOCKET_H
#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef unsigned int socklen_t;
typedef unsigned char sa_family_t;
struct sockaddr { unsigned char sa_len; sa_family_t sa_family; char sa_data[14]; };
struct sockaddr_storage { unsigned char ss_len; sa_family_t ss_family; char ss_data[126]; };
#define AF_UNSPEC 0
#define AF_INET   2
#define AF_INET6  28
#define PF_UNSPEC AF_UNSPEC
#define PF_INET   AF_INET
#define PF_INET6  AF_INET6
#define SOCK_STREAM 1
#define SOCK_DGRAM  2
#define SOL_SOCKET  0xffff
#define SO_REUSEADDR 0x0004
#define SO_KEEPALIVE 0x0008
#define SO_SNDBUF   0x1001
#define SO_RCVBUF   0x1002
#define SOMAXCONN   5
#define MSG_PEEK    0x2
#define MSG_NOSIGNAL 0
#define SHUT_RD   0
#define SHUT_WR   1
#define SHUT_RDWR 2
int socket(int domain, int type, int protocol);
int connect(int s, const struct sockaddr *name, socklen_t namelen);
int bind(int s, const struct sockaddr *name, socklen_t namelen);
int listen(int s, int backlog);
int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
ssize_t send(int s, const void *msg, size_t len, int flags);
ssize_t recv(int s, void *buf, size_t len, int flags);
ssize_t sendto(int s, const void *msg, size_t len, int flags, const struct sockaddr *to, socklen_t tolen);
ssize_t recvfrom(int s, void *buf, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);
int getsockname(int s, struct sockaddr *name, socklen_t *namelen);
int getpeername(int s, struct sockaddr *name, socklen_t *namelen);
int shutdown(int s, int how);
#ifdef __cplusplus
}
#endif
#endif
