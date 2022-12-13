
struct mbuf;

// used in udp checksum
struct ipovly
{
  char *n, n2;
  char x1;
  char proto;
  short len;
  unsigned long src;
  unsigned long dst;
};

int
ip_output(struct mbuf* m, int flags);
