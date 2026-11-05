/*
 * The Amiga build has no network (NETCODE_DISABLE): no network games.
 * The engine's old mmulti.cpp still builds, on the headers in
 * amiga/include/, and every socket call here fails as on a machine with
 * no TCP/IP stack.
 * The AmiCradle Team. GPL 2 or later, like EDuke32.
 */
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>

#ifndef ENOSYS
#define ENOSYS 88
#endif

static int no_net(void) { errno = ENOSYS; return -1; }

int socket(int d, int t, int p) { (void)d; (void)t; (void)p; return no_net(); }
int connect(int s, const struct sockaddr *n, socklen_t l) { (void)s; (void)n; (void)l; return no_net(); }
int bind(int s, const struct sockaddr *n, socklen_t l) { (void)s; (void)n; (void)l; return no_net(); }
int listen(int s, int b) { (void)s; (void)b; return no_net(); }
int accept(int s, struct sockaddr *a, socklen_t *l) { (void)s; (void)a; (void)l; return no_net(); }
ssize_t send(int s, const void *m, size_t l, int f) { (void)s; (void)m; (void)l; (void)f; return no_net(); }
ssize_t recv(int s, void *b, size_t l, int f) { (void)s; (void)b; (void)l; (void)f; return no_net(); }
ssize_t sendto(int s, const void *m, size_t l, int f, const struct sockaddr *t, socklen_t tl)
{ (void)s; (void)m; (void)l; (void)f; (void)t; (void)tl; return no_net(); }
ssize_t recvfrom(int s, void *b, size_t l, int f, struct sockaddr *fr, socklen_t *fl)
{ (void)s; (void)b; (void)l; (void)f; (void)fr; (void)fl; return no_net(); }
int setsockopt(int s, int lv, int o, const void *v, socklen_t l) { (void)s; (void)lv; (void)o; (void)v; (void)l; return no_net(); }
int getsockopt(int s, int lv, int o, void *v, socklen_t *l) { (void)s; (void)lv; (void)o; (void)v; (void)l; return no_net(); }
int getsockname(int s, struct sockaddr *n, socklen_t *l) { (void)s; (void)n; (void)l; return no_net(); }
int getpeername(int s, struct sockaddr *n, socklen_t *l) { (void)s; (void)n; (void)l; return no_net(); }
int shutdown(int s, int h) { (void)s; (void)h; return no_net(); }

/* Only the network code calls select(), and only on sockets. */
int select(int n, fd_set *r, fd_set *w, fd_set *e, struct timeval *t)
{ (void)n; (void)r; (void)w; (void)e; (void)t; return no_net(); }

in_addr_t inet_addr(const char *cp) { (void)cp; return INADDR_NONE; }

char *inet_ntoa(struct in_addr in)
{
	static char buf[16];
	unsigned long a = in.s_addr;
	char *p = buf;
	int i;
	for (i = 3; i >= 0; i--) {
		unsigned v = (unsigned)((a >> (i * 8)) & 255);
		if (v >= 100) *p++ = (char)('0' + v / 100);
		if (v >= 10) *p++ = (char)('0' + v / 10 % 10);
		*p++ = (char)('0' + v % 10);
		if (i) *p++ = '.';
	}
	*p = 0;
	return buf;
}

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
	struct in_addr in;
	const char *s;
	if (af != AF_INET || size < INET_ADDRSTRLEN) { errno = ENOSYS; return NULL; }
	memcpy(&in, src, sizeof(in));
	s = inet_ntoa(in);
	strcpy(dst, s);
	return dst;
}

int inet_pton(int af, const char *src, void *dst) { (void)af; (void)src; (void)dst; return 0; }

int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)
{
	(void)node; (void)service; (void)hints;
	if (res) *res = NULL;
	return EAI_FAIL;
}

void freeaddrinfo(struct addrinfo *res) { (void)res; }

const char *gai_strerror(int errcode) { (void)errcode; return "no network on this Amiga build"; }

int getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host, socklen_t hostlen,
                char *serv, socklen_t servlen, int flags)
{
	(void)sa; (void)salen; (void)host; (void)hostlen; (void)serv; (void)servlen; (void)flags;
	return EAI_FAIL;
}

struct hostent *gethostbyname(const char *name) { (void)name; return NULL; }

int h_errno;

int ioctl(int d, unsigned long r, ...) { (void)d; (void)r; return no_net(); }

int gethostname(char *name, size_t len)
{
	if (len) {
		strncpy(name, "amiga", len);
		name[len - 1] = 0;
	}
	return 0;
}
