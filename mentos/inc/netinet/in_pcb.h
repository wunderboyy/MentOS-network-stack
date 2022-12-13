

struct inpcb
{
  struct inpcb *next, *prev, *head;

  unsigned long faddr;  // foreign address
  unsigned short fport; // foreign port
  unsigned long laddr;  // local address
  unsigned long lport;  // local port

  struct socket* so;
  void* pcb;    // protocol specific
  struct ip ip; // prototype used when creating ip packets
};

#define IPLOOKUP_WILDCARD 1

int
in_pcbbind(struct inpcb* in, struct mbuf* m);

struct inpcb*
in_pcblookup(struct inpcb* head,
             unsigned long laddr,
             unsigned short lport,
             unsigned long faddr,
             unsigned short fport,
             int flags);
