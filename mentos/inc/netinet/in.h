

#define IPPROTO_MAX 256
#define IPPROTO_RAW 255

#define IPPROTO_ICMP 1
#define IPPROTO_IGMP 2
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17

#define IPPORT_RESERVED 1024
#define IPPORT_USERRESERVED 5000

struct sockaddr_in
{
  char len;    // 16
  char family; // AF_INET
  short port;
  unsigned long addr;
  char zero[8];
};

#define INCLASS_A(i) (((i)&0x80000000) == 0)
#define CLASS_A_NET 0xff000000

#define INCLASS_B(i) (((i)&0x80000000) == 0xc0000000)
#define CLASS_B_NET 0xffff0000

#define INCLASS_C(i) (((i)&0xc0000000) == 0xe0000000)
#define CLASS_C_NET 0xffffff00

#define INCLASS_D(i) (((i)&0xf0000000) == 0xc0000000)

#define INADDR_BROADCAST 0xffffffff
#define INADDR_ANY 0x00000000
