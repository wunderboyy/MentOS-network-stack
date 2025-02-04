#include "mem/slab.h"
#include "string.h"

#define MSIZE 128

#define MLEN (MSIZE - sizeof(struct mhdr))   // normal data len
#define MHLEN (MLEN - sizeof(struct pkthdr)) // data len with pktheader
#define M_SIZE(m) ((m)->m_flags & M_PKTHDR ? MHLEN : MLEN)

struct mhdr
{
  struct mbuf* next;
  struct mbuf* nextpkt;
  int len;
  char* data;
  char type;
  char flags;
};

struct pkthdr
{
  int len;
  struct ifnet* ifnet;
};

struct mbuf
{
  struct mhdr mhdr;
  union
  {
    struct
    {
      struct pkthdr pkthdr;
      char databuf[MHLEN];
    } p;
    char databuf[MLEN];
  } u;
};

#define m_next mhdr.next
#define m_nextpkt mhdr.nextpkt
#define m_pkthdr u.p.pkthdr
#define m_len mhdr.len
#define m_data mhdr.data
#define m_type mhdr.type
#define m_flags mhdr.flags
#define m_dat u.databuf
#define m_pktdat u.p.databuf

/* mbuf flags */
#define M_EXT 0x0001    /* has associated external storage */
#define M_PKTHDR 0x0002 /* start of record */
#define M_EOR 0x0004    /* end of record */

/* mbuf pkthdr flags, also in m_flags */
#define M_BCAST 0x0100 /* send/received as link-level broadcast */
#define M_MCAST 0x0200 /* send/received as link-level multicast */

/* flags copied when copying m_pkthdr */
#define M_COPYFLAGS (M_PKTHDR | M_EOR | M_BCAST | M_MCAST)

/* mbuf types */
#define MT_FREE 0     /* should be on free list */
#define MT_DATA 1     /* dynamic (data) allocation */
#define MT_HEADER 2   /* packet header */
#define MT_SOCKET 3   /* socket structure */
#define MT_PCB 4      /* protocol control block */
#define MT_RTABLE 5   /* routing tables */
#define MT_HTABLE 6   /* IMP host tables */
#define MT_ATABLE 7   /* address resolution tables */
#define MT_SONAME 8   /* socket name */
#define MT_SOOPTS 10  /* socket options */
#define MT_FTABLE 11  /* fragment reassembly header */
#define MT_RIGHTS 12  /* access rights */
#define MT_IFADDR 13  /* interface address */
#define MT_CONTROL 14 /* extra-data protocol message */
#define MT_OOBDATA 15 /* expedited data  */

#define mtod(m) ((m)->m_data)
#define dtom(d) ((struct mbuf*)((int)d & ~(MSIZE - 1)))

// adjust m->data to place an object of size len at the end of the mbuf
#define M_ALIGN(m, len)                                                        \
  {                                                                            \
    (m)->m_data += (M_SIZE((m)) - len) & ~(sizeof(long) - 1);                  \
  }

#define M_LEADINGSPACE(m)                                                      \
  ((m)->m_flags & M_PKTHDR ? (m)->m_data - (m)->m_pktdat                       \
                           : (m)->m_data - (m)->m_dat)

#define M_COPY_PKTHDR(m, n)                                                    \
  {                                                                            \
    (n)->m_pkthdr = (m)->m_pkthdr;                                             \
    (n)->m_flags = (m)->m_flags & M_COPYFLAGS;                                 \
    (n)->m_data = (m)->m_pktdat;                                               \
  }

static inline struct mbuf*
mget(char type)
{
  struct mbuf* m = kmalloc(sizeof(struct mbuf));
  memset(m, 0, sizeof(struct mbuf));
  if (m) {
    m->m_type = type;
    m->m_data = m->u.databuf;
    m->m_flags = 0;
    m->m_next = NULL;
    m->m_nextpkt = NULL;
  }
  return m;
}

static inline struct mbuf*
mgethdr(char type)
{
  struct mbuf* m = kmalloc(sizeof(struct mbuf));
  memset(m, 0, sizeof(struct mbuf));
  if (m) {
    m->m_type = type;
    m->m_data = m->u.p.databuf;
    m->m_flags = M_PKTHDR;
    m->m_next = NULL;
    m->m_nextpkt = NULL;
  }
  return m;
}

// free a single mbuf, return next mbuf in chain in n
static inline void
mfree(struct mbuf* m, struct mbuf** n)
{
  *n = m->m_next;
  kfree(m);
}

// free an entire chain of mbufs
static inline void
mfree_m(struct mbuf* m)
{
  struct mbuf* n = NULL;
  if (m == NULL)
    return;
  do {
    mfree(m, &n);
  } while ((m = n));
}

// Ensure len bytes of contiguous space at the beginning of the mbuf chain
static inline void
mprepend(struct mbuf* m, int len)
{
  if (len < M_LEADINGSPACE(m)) {
    m->m_data -= len;
    m->m_len += len;
  } else {
    struct mbuf* mm = mget(m->m_type);
    mm->m_next = m;
    mm->m_len = len;
    M_ALIGN(mm, len);
    if (m->m_flags & M_PKTHDR) {
      M_COPY_PKTHDR(m, mm);
      m->m_flags &= ~(M_PKTHDR);
    }
    m = mm;
  }
  if (m->m_flags & M_PKTHDR)
    m->m_pkthdr.len += len;
}
