/* See sys/socket.h: no network on the Amiga build. */
#ifndef AMIGA_ARPA_INET_H
#define AMIGA_ARPA_INET_H
#include <netinet/in.h>
#ifdef __cplusplus
extern "C" {
#endif
in_addr_t inet_addr(const char *cp);
char *inet_ntoa(struct in_addr in);
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
int inet_pton(int af, const char *src, void *dst);
#ifdef __cplusplus
}
#endif
#endif
