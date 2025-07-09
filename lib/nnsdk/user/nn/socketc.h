#pragma once

#include <nn/primitives.h>
#include <sys/select.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;
typedef uint16_t sa_family_t;

typedef unsigned int socklen_t;

struct pollfd {
    s32 fd;
    s16 events;
    s16 revents;
};

struct in_addr {
    u32 s_addr;           // 0
};

struct sockaddr_in {
    sa_family_t sin_family; /* address family: AF_INET */
    in_port_t sin_port;   /* port in network byte order */
    struct in_addr sin_addr;   /* internet address */
};

struct sockaddr {
    u8 sa_len;              // 0
    u8 family;          // 1
    u16 port;           // 2
    struct in_addr address;    // 4
    u8 _8[8];           // 8
};

struct sockaddr_storage {
    u8 ss_len;
    u8 ss_family;
    char _ss_pad1[sizeof(int64_t) - 2 * sizeof(u8)];
    int64_t _ss_align;
    char _ss_pad2[128 - 4 * sizeof(u8) - 2 * sizeof(int64_t)];
};

struct hostent {
    char *h_name;
    char **h_aliases;
    int h_addrtype;
    int h_length;
    char **h_addr_list;
};

struct addrinfo {
    u32 ai_flags;
    u8 ai_family;
    u32 ai_socktype;
    s32 ai_protocol;
    u32 ai_addrlen;
    struct sockaddr* ai_addr;
    char* ai_canonname;
    struct addrinfo* ai_next;
};

struct iovec {
    void *iov_base; /* Starting address */
    size_t iov_len; /* Length in bytes */
};

/* Structure describing messages sent by
   `sendmsg' and received by `recvmsg'.  */
struct msghdr {
    void *msg_name;        /* Address to send to/receive from.  */
    socklen_t msg_namelen;    /* Length of address data.  */

    struct iovec *msg_iov;    /* Vector of data to send/receive into.  */
    size_t msg_iovlen;        /* Number of elements in the vector.  */

    void *msg_control;        /* Ancillary data (eg BSD filedesc passing). */
    size_t msg_controllen;    /* Ancillary data buffer length.
                              !! The type should be socklen_t but the
                              definition of the kernel is incompatible
                              with this.  */

    int msg_flags;        /* Flags on received message.  */
};

struct mmsghdr {
    struct msghdr msg_hdr;  /* Message header */
    unsigned int msg_len;  /* Number of received bytes for header */
};

/* Structure used for storage of ancillary data object information.  */
struct cmsghdr {
    size_t cmsg_len;        /* Length of data in cmsg_data plus length
                        of cmsghdr structure.
                        !! The type should be socklen_t but the
                        definition of the kernel is incompatible
                        with this.  */
    int cmsg_level;        /* Originating protocol.  */
    int cmsg_type;        /* Protocol specific type.  */
};

#define AF_UNSPEC 0
#define AF_INET 2
#define PF_INET AF_INET

#define SOCK_STREAM 1
#define SOCK_DGRAM 2

#define IPPROTO_IP 0
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17

#define AI_PASSIVE 1

#define SOL_SOCKET 0xffff

#define SO_REUSEADDR 4
#define SO_TYPE 0x1008
#define SO_KEEPALIVE 8

#define TCP_NODELAY 1

#define MSG_PEEK 2

int nnsocketRecv(int socket, void* out, ulong outLen, int flags);
int nnsocketRecvFrom(int,void *,ulong,int, struct sockaddr *,uint *);
int nnsocketSend(int socket, const void* data, ulong dataLen, int flags);
int nnsocketSendTo(int,void const*,ulong,int, struct sockaddr const*,uint);
int nnsocketAccept(int,struct sockaddr *,socklen_t *);
int nnsocketBind(int,struct sockaddr const*,uint);
u32 nnsocketConnect(int, struct sockaddr*, int); // returns nn::Result
int nnsocketGetPeerName(int,struct sockaddr *,uint *);
int nnsocketGetSockName(int,struct sockaddr *,uint *);
int nnsocketGetSockOpt(int,int,int,void *,uint *);
int nnsocketListen(int,int);
int nnsocketSetSockOpt(int socket, int socketLevel, int option, void const*, u32 len);
int nnsocketSockAtMark(int);
int nnsocketShutdown(int,int);
int nnsocketSocket(int,int,int);
int nnsocketWrite(int domain, int type, int protocol);
int nnsocketRead(int,int,int);
u32 nnsocketClose(int); // returns nn::Result
int nnsocketSelect(int,fd_set *,fd_set *,fd_set *,struct timeval *);
void nnsocketPoll(struct pollfd *,ulong,int);
int nnsocketFcntl(int,int,...);
void nnsocketInetPton(int,char const*,void *);
const char* nnsocketInetNtop(int af,void const* src,char* dst,uint size);
s32 nnsocketInetAton(const char* addressStr, struct in_addr* addressOut);
char* nnsocketInetNtoa(struct in_addr);
u16 nnsocketInetHtons(u16 val);
u32 nnsocketInetHtonl(uint);
u16 nnsocketInetNtohs(ushort);
u32 nnsocketInetNtohl(uint);
s32 nnsocketGetLastErrno();
void nnsocketSetLastErrno(int);
s32 nnsocketRecvMsg(int,struct msghdr *,int);
s32 nnsocketSendMsg(int,struct msghdr const*,int);
s32 nnsocketIoctl(int,uint,void *,ulong);
s32 nnsocketOpen(char const*,int);
u32 nnsocketInitialize(void* pool, ulong poolSize, ulong allocPoolSize, int concurLimit);
s32 nnsocketFinalize();
s32 nnsocketGetAddrInfo(char const*,char const*,struct addrinfo const*,struct addrinfo**);
s32 nnsocketGetAddrInfoCancel(char const*,char const*,struct addrinfo const*,struct addrinfo**,int);
s32 nnsocketGetAddrInfoWithoutNsdResolve(char const*,char const*,struct addrinfo const*,struct addrinfo**);
s32 nnsocketGetAddrInfoWithoutNsdResolveCancel(char const*,char const*,struct addrinfo const*,struct addrinfo**,int);
s32 nnsocketFreeAddrInfo(struct addrinfo *);
s32 nnsocketGetNameInfo(struct sockaddr const*,uint,char *,uint,char *,uint,int);
s32 nnsocketGetNameInfoCancel(struct sockaddr const*,uint,char *,uint,char *,uint,int,int);
struct hostent* nnsocketGetHostByName(const char* name);
struct hostent* nnsocketGetHostByNameCancel(char const*,int);
struct hostent* nnsocketGetHostByNameWithoutNsdResolve(char const*);;
struct hostent* nnsocketGetHostByNameWithoutNsdResolveCancel(char const*,int);;
struct hostent* nnsocketGetHostByAddr(void const*,uint,int);
struct hostent* nnsocketGetHostByAddrCancel(void const*,uint,int,int);
s32 nnsocketRequestCancelHandle();
s32 nnsocketCancel(int);
s32 nnsocketGetHErrno();
s32 nnsocketHStrError(int);
s32 nnsocketGAIStrError(int);
s32 nnsocketSysctl(int *,ulong,void *,ulong *,void *,ulong);
s32 nnsocketDuplicateSocket(int,ulong);

#ifdef __cplusplus
}
#endif
