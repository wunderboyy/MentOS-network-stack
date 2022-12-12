

#define IP_FORWARDING 1     // this packet is being forwarded
#define IP_RAWOUTPUT 2      // packet contains a preconstructed ip header
#define IP_ROUTETOIF 0x0010 // route packet straight to an interface

#define IPVERSION 4

struct ip
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  unsigned char hl : 4, version : 4;
#endif
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  unsigned char version : 4, hl : 4;
#endif
  unsigned char tos;
  unsigned short len;
  unsigned short id;
  unsigned short off;
  unsigned char ttl;
  unsigned char proto;
  unsigned short sum;

  unsigned long src;
  unsigned long dst;
};
