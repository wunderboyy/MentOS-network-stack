

struct sockaddr_dl
{
  char len;
  char family;

  int type;
  int index; // index of interface
  int nlen;  // name length

  char data[12];
};

#define LLADDR(s) ((s)->data + (s)->nlen)
