#include "netinet/ip_var.h"
#include "sys/endian.h"
#include "sys/protosw.h"
#include "netinet/udp.h"
#include "netinet/udp_var.h"
#include "sys/socketvar.h"
#include "sys/errno.h"
#include "sys/socket.h"
#include "sys/mbuf.h"
#include "netinet/in.h"
#include "netinet/ip.h"
#include "netinet/in_pcb.h"

struct inpcb udp;

/*
  udp_output()
  Called from sosend() -> usrreq(PRU_SEND)
  Sets these ip fields: proto, src, dst, tos, len, ttl
  UDP checksum includes ip pseudo header
  Socket has to be connected or destination addr defined
 */
int
udp_output(struct inpcb* in, struct mbuf* m, struct mbuf* addr)
{
  struct ip* ip;
  struct udp* up;
  unsigned long laddr;
  int len = m->m_pkthdr.len;

  mprepend(m, sizeof(struct ip) + sizeof(struct udp));

  if (!in->faddr && !addr)
    return EINVAL;

  if (addr) {
    // if socket is already connected, cant specify destination
    if (in->faddr)
      return EINVAL;
    laddr = in->laddr; // if this is wildcard, it will get changed
    in_pcbconnect(in, addr);
  }

  // TODO: tos, len and ttl get set after checksum

  ip = (struct ip*)mtod(m);
  ip->src = in->laddr;
  ip->dst = in->faddr;
  ip->tos = 0;
  ip->len = len + sizeof(struct ip) + sizeof(struct udp);
  ip->ttl = 50; // todo
  ip->proto = IPPROTO_UDP;

  up = (struct udp*)(ip + 1);
  up->sport = in->lport;
  up->dport = in->fport; // already in network byte order
  up->len = LITTLETOBIG16(len + sizeof(struct udp));
  up->sum = 0; // TODO

  ip_output(m, IP_ROUTETOIF);

  if (addr) {
    // TODO: disconnect()
    in->laddr = laddr;
  }

  return 0;
}

/*
  udp_input()
  Mbuf m holds the ip and udp headers (ip->len doesnt include ip header length)
  Find a socket to receive this datagram
  Add datagram to the socket's receive queue
 */
int
udp_input(struct mbuf* m, int iphlen)
{
  struct ip* ip;
  struct udp* up;
  struct inpcb* in;
  struct mbuf *m0, *m1;

  ip = (struct ip*)mtod(m);
  up = (struct udp*)((char*)ip + iphlen);

  in = in_pcblookup(
    &udp, ip->dst, up->dport, ip->src, up->sport, IPLOOKUP_WILDCARD);
  if (in == NULL) {
    return -EHOSTUNREACH;
  }
  iphlen += sizeof(struct udp);
  m->m_len -= iphlen;
  m->m_pkthdr.len -= iphlen;
  m->m_data += iphlen;
  if (in->so->rcv.m == NULL) {
    in->so->rcv.m = m;
    in->so->rcv.len += m->m_pkthdr.len;
  } else {
    m0 = in->so->rcv.m;
    while (m0->m_nextpkt && (m0 = m0->m_nextpkt))
      ;
    m0->m_nextpkt = m;
  }

  return 0;
}

int
udp_usrreq(struct socket* so, int req, struct mbuf* m, struct mbuf* addr)
{
  struct inpcb* in = (struct inpcb*)so->pcb;
  int error;

  if (in == NULL && req != PRU_ATTACH)
    return EINVAL;

  switch (req) {
    case PRU_ATTACH:
      if (in) {
        return EINVAL;
      }
      error = in_pcballoc(so, &udp);
      break;
    case PRU_BIND:
      error = in_pcbbind(in, addr);
      break;
    case PRU_CONNECT:
      error = in_pcbconnect(in, addr);
      break;
    case PRU_SEND:
      return udp_output(in, m, addr);
      break;
  }
  if (m)
    mfree_m(m); // should this even be here
  return error;
}
