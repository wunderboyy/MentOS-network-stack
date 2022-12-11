#include "net/if.h"
#include "net/if_dl.h"
#include "net/netisr.h"
#include "stddef.h"
#include "sys/errno.h"
#include "sys/mbuf.h"
#include "sys/socket.h"

#define LOMTU 1500

extern struct ifqueue ipintrq;
struct ifnet loif;

// allocate an ifnet struct and call if_attach
void
loopattach(void)
{
  struct ifnet* ifp = &loif;
  ifp->name = "lo";
  ifp->unit = 0;
  ifp->output = looutput;
  ifp->flags = IFF_LOOPBACK;
  ifp->mtu = LOMTU;
  ifp->addrlen = 0; // loopback doesnt have a header or an address
  ifp->hdrlen = 0;

  if_attach(ifp);
}

// loopback interface output
// -schedule a software interrupt for a protocol and queue the packet
int
looutput(struct ifnet* ifp, struct mbuf* m, struct sockaddr* dst, void* u)
{
  struct ifqueue* ifq;
  int isr;

  if ((m->m_flags & M_PKTHDR) == 0) {
    return -69;
  }
  m->m_pkthdr.ifnet = ifp;

  switch (dst->family) {
    case AF_INET: {
      ifq = &ipintrq;
      isr = NETISR_IP;
    }
  }

  if (IF_QFULL(ifq)) {
    mfree_m(m);
    return -ENOBUFS;
  }

  IF_ENQUEUE(ifq, m);
  schednetisr(isr);
  return 0;
}
