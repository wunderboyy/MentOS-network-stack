

struct socket
{
  char type;
  char state;
  char options;
  void* pcb;
  struct protosw* proto;

  struct
  {
    struct mbuf* data;
  } rcv, snd;
};
