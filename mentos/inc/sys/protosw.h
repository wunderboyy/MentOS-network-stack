
struct mbuf;
struct domain;
struct socket;

struct protosw
{
  struct domain* dom;
  unsigned char type;
  unsigned char protocol;
  char flags;

  void (*init)();
  int (*ctlinput)();
  int (*input)();
  int (
    *usrreq)(struct socket*, int req, struct mbuf*, struct mbuf*, struct mbuf*);
};

#define PR_ATOMIC 1

#define PRU_ATTACH 0
#define PRU_BIND 1
#define PRU_SEND 2
#define PRU_CONNECT 3

struct protosw*
pffindtype(unsigned char, unsigned char);
struct protosw*
pffindproto(unsigned char, unsigned char, unsigned char);
