#include "net/if.h"
#include "netinet/in.h"
#include "netinet/in_var.h"
#include "netinet/ip.h"
#include "sys/domain.h"
#include "sys/endian.h"
#include "sys/mbuf.h"
#include "sys/protosw.h"
#include "sys/socket.h"

static int ipforwarding = 0; // router?

struct ifqueue ipintrq = { .maxlen = 50 };
struct in_ifaddr* in_ifaddr;
extern struct protosw inetsw[];
extern struct domain inetdomain;

unsigned char ip_protox[IPPROTO_MAX];

// initialize ip_protox array
// default entry points to IPPROTO_RAW, so unknown protocols get routed to there
void
ipinit(void)
{
  struct protosw* p = pffindproto(AF_INET, IPPROTO_RAW, SOCK_RAW);
  int i;
  for (i = 0; i < IPPROTO_MAX; ++i) {
    ip_protox[i] = p - inetsw;
  }

  for (p = inetsw; p < inetdomain.protoswEND; p++) {
    if (p->protocol)
      ip_protox[p->protocol] = p - inetsw;
  }
}

// interface layer schedules a software interrupt for this routine
// take packets off the queue until its empty
void
ipintr(void)
{
  struct mbuf* m;
  struct ip* ip;
  struct in_ifaddr* ia;
  int hlen;

next:
  IF_DEQUEUE(&ipintrq, m);
  if (m == NULL)
    return;

  // TODO: check if m->len < sizeof(struct ip), m_pullup()
  ip = (struct ip*)mtod(m);

  if (ip->version != IPVERSION)
    goto fail;

  hlen = ip->hl * 4;
  if (hlen < sizeof(struct ip))
    goto fail;

  if (hlen > m->m_len)
    goto fail; // TODO: m_pullup()

  // TODO: checksum

  // TODO: NTOHS len, id and off

  ip->len = BIGTOLITTLE16(ip->len);
  ip->off = BIGTOLITTLE16(ip->off);
  ip->id = BIGTOLITTLE16(ip->id);

  if (ip->len > m->m_pkthdr.len)
    goto fail;

  // TODO: check ip->len < pkthdr.len

  // now packet is confirmed to be good

  // TODO: check ip options and maybe call ip_options

  /* 4 cases where packet might be for us:
     - exact match with ip address on an interface
     - a match with the broadcast address of the receiving interface
     - a match with a multicast group of the receiving interface
     - a match with 255.255.255.255
   */
  for (ia = in_ifaddr; ia; ia = ia->next) {

    if (ia->addr.addr == ip->dst)
      goto ours;

    if (ia->in_ifnet == m->m_pkthdr.ifnet &&
        ia->in_ifnet->flags & IFF_BROADCAST) {

      if (ia->netbroadcast == ip->dst || ia->in_broadaddr.addr == ip->dst)
        goto ours;
    }
  }
  if (ip->dst == INADDR_BROADCAST)
    goto ours;

  // TODO: check multicast groups for a match

  if (!ipforwarding) {
    // mfree_m(m);
  } else {
    // TODO: ip_forward()
  }
  goto next;
ours:

  // TODO: check for fragmentation
  ip->len -= hlen;

  inetsw[ip_protox[ip->proto]].input();
  goto next;
fail:
  //  mfree_m(m);
  goto next;
}
