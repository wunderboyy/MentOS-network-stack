

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
