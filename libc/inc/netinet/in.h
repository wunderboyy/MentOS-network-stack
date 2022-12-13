

struct sockaddr_in
{
  char len;    // 16
  char family; // AF_INET
  short port;
  unsigned long addr;
  char zero[8];
};

#define INADDR_ANY (long)0x00000000
