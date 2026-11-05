/* No network on the Amiga build: enet.h asks for this header only. */
#ifndef AMIGA_SYS_IOCTL_H
#define AMIGA_SYS_IOCTL_H
#define FIONBIO 0x8004667e
#define FIONREAD 0x4004667f
#ifdef __cplusplus
extern "C" {
#endif
int ioctl(int, unsigned long, ...);
#ifdef __cplusplus
}
#endif
#endif
