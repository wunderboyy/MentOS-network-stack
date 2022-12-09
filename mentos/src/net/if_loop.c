#include "net/if.h"
#include "net/netisr.h"
#include "stddef.h"

#define LOMTU 1500

struct ifnet loif;

// allocate an ifnet struct and call if_attach
void
loopattach(void)
{
  struct ifnet* ifp = &loif;
  ifp->name = "lo";
  ifp->unit = 0;
  ifp->output = looutput;
  ifp->mtu = LOMTU;
  ifp->addrlen = 0; // loopback doesnt have a header or an address
  ifp->hdrlen = 0;

  if_attach(ifp);
}

// loopback interface output
int
looutput(struct ifnet* ifp, void* x, void* y, void* u)
{
  schednetisr(NETISR_IP);
  return 0;
}
