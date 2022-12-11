

struct mbuf;
struct sockaddr;

#define IFT_LOOP 0x1
#define IFT_ETHER 0x2
#define IFT_SLIP 0x3

struct ifnet
{
  struct ifnet* next;
  struct ifaddr* ifaddr_list;
  char* name;
  char index; // index in interface array
  char unit;  // eth0, unit = 0

  char type; // ether, loop etc
  char addrlen;
  char hdrlen;
  char flags;
  unsigned long mtu;

  int (*init)(int);
  int (*output)(struct ifnet*, struct mbuf*, struct sockaddr*, void*);

  struct ifqueue
  {
    struct mbuf* head;
    struct mbuf* tail;
    int len;
    int maxlen;
  } que;
};

#define IFF_LOOPBACK 0x1
#define IFF_BROADCAST 0x2
#define IFF_POINTOPOINT 0x3

#define QUEMAXLEN 50

struct ifaddr
{
  struct sockaddr* addr;
  struct sockaddr* broadaddr;
  struct sockaddr* netmask;
  struct ifnet* ifnet;
  struct ifaddr* next;
};

#define IF_QFULL(ifq) ((ifq)->len >= (ifq)->maxlen)
#define IF_ENQUEUE(ifq, m)                                                     \
  {                                                                            \
    (m)->m_nextpkt = NULL;                                                     \
    if ((ifq)->tail == NULL) {                                                 \
      (ifq)->head = m;                                                         \
    } else {                                                                   \
      (ifq)->tail->m_nextpkt = m;                                              \
    }                                                                          \
    (ifq)->tail = m;                                                           \
    (ifq)->len++;                                                              \
  }

#define IF_DEQUEUE(ifq, m)                                                     \
  {                                                                            \
    (m) = (ifq)->head;                                                         \
    if ((m)) {                                                                 \
      if (((ifq)->head = (m)->m_nextpkt) == 0) {                               \
        (ifq)->tail = NULL;                                                    \
      }                                                                        \
      (ifq)->len--;                                                            \
    }                                                                          \
  }

void
if_attach(struct ifnet*);

void
loopattach(void);
int
looutput(struct ifnet*, struct mbuf*, struct sockaddr*, void*);
