#include "mem/slab.h"
#include "string.h"
#include "sys/errno.h"
#include "sys/mbuf.h"
#include "sys/protosw.h"
#include "sys/socket.h"
#include "sys/socketvar.h"

int
socreate(unsigned char domain,
         struct socket** so,
         unsigned char type,
         unsigned char protocol)
{
  struct socket* sock;
  struct protosw* proto = NULL;

  if (proto)
    proto = pffindproto(domain, protocol, type);
  else
    proto = pffindtype(domain, type);
  if (proto == NULL || proto->usrreq == NULL)
    return EPROTONOSUPPORT;

  // TODO: set sock->state = SS_PRIV if sudo
  sock = kmalloc(sizeof(struct socket));
  memset(sock, 0x0, sizeof(struct socket));
  sock->type = type;
  sock->proto = proto;
  sock->snd.maxlen = 100;
  sock->rcv.maxlen = 100;
  proto->usrreq(sock, PRU_ATTACH, NULL, (struct mbuf*)proto, NULL);
  *so = sock;
  return 0;
}

int
sobind(struct socket* so, struct mbuf* m)
{
  // TODO: splnet()
  return so->proto->usrreq(so, PRU_BIND, NULL, m, NULL);
}

// collect buf to a new mbuf chain and send it to the protocol
// to holds the destination sockaddr
int
sosend(struct socket* so,
       struct mbuf* to,
       char* buf,
       int blen,
       struct mbuf* top,
       int flags)
{
  struct mbuf **mp, *m, *n;
  int dontroute = 0, mlen, len, space, resid = blen, atomic = soatomic(so);

  // enable routing for this send
  if (flags & MSG_DONTROUTE && (so->options & SO_DONTROUTE) == 0)
    dontroute = 1;

  // TODO: lock snd

  if (resid > so->snd.maxlen)
    return EINVAL;

#define min(x, y) ((x) < (y) ? (x) : (y))

  space = sospace(so);
  mp = &top;
  do {
    if (top == NULL) {
      m = mgethdr(MT_DATA);
      mlen = MHLEN;
    } else {
      m = mget(MT_DATA);
      mlen = MLEN;
    }

    len = min(min(mlen, resid), space);

    // leave space for protocol headers for first mbuf in chain
    if (top == NULL)
      M_ALIGN(m, len);

    space -= len;

    // TODO: uiomove

    memcpy(m->m_data, buf, len);
    buf += len;

    m->m_len = len;
    *mp = m;
    top->m_pkthdr.len += len;
    mp = &m->m_next;
  } while (space && (resid -= len));

  if (dontroute)
    so->options = SO_DONTROUTE;
  // TODO: splnet()
  so->proto->usrreq(so, PRU_SEND, top, to, NULL);
  top = NULL;

  // TODO: unlock snd

  if (top)
    mfree_m(top);
  return 0;
}

// collect data from the receive mbuf chain to buf for blen amount
int
soreceive(struct socket* so, char* buf, int blen, int* flags)
{
  struct mbuf *m, *m2;
  int len, off = 0, resid = blen;

  if (so->rcv.m == NULL || so->rcv.len < blen) {
    // TODO: wait()
    return 0;
  }

  m = so->rcv.m;
loop:
  len = min(resid, m->m_len);
  memcpy(buf, m->m_data, len);
  resid -= len;
  buf += len;

  mfree(m, &m2);
  if (resid && (m = m2))
    goto loop;

  return 0;
}
