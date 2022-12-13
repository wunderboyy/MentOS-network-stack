

struct socket
{
  char type;
  char state;
  char options;
  void* pcb;
  struct protosw* proto;

  struct
  {
    struct mbuf* m;
    int len;
    int maxlen;
  } rcv, snd;
};

#define MSG_DONTROUTE 1

#define SO_DONTROUTE 1

#define sospace(so) ((so)->snd.maxlen - (so)->snd.len)

#define soatomic(so) ((so)->proto->flags & PR_ATOMIC)

int
socreate(unsigned char, struct socket**, unsigned char, unsigned char);

int
sobind(struct socket* so, struct mbuf* m);

int
soreceive(struct socket*, char*, int, int*);
