
struct mbuf;
struct domain;
struct socket;

struct protosw
{
  struct domain* dom;
  char type;
  unsigned char protocol;

  void (*init)();
  int (*ctlinput)();
  int (*input)();
  int (
    *usrreq)(struct socket*, int req, struct mbuf*, struct mbuf*, struct mbuf*);
};

#define PRU_ATTACH 0
#define PRU_BIND 1

struct protosw*
pffindtype(unsigned char, unsigned char);
struct protosw*
pffindproto(unsigned char, unsigned char, unsigned char);
