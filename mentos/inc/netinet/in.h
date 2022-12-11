

#define IPPROTO_UDP 1
#define IPPROTO_TCP 2
#define IPPROTO_IP 3
#define IPPROTO_RAW 4

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
