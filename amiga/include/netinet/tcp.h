/* See sys/socket.h: no network on the Amiga build. */
#ifndef AMIGA_NETINET_TCP_H
#define AMIGA_NETINET_TCP_H
#define TCP_NODELAY 0x01
#endif
