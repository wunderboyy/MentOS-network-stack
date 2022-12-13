#include "mem/slab.h"
#include "sys/socket.h"
#include "net/if.h"
#include "sys/mbuf.h"
#include "sys/errno.h"
#include "netinet/ip.h"
#include "netinet/in_pcb.h"
#include "netinet/in.h"
#include "sys/socketvar.h"
#include "string.h"
#include "netinet/in_var.h"
#include "sys/protosw.h"

int
in_pcballoc(struct socket* so, struct inpcb* head)
{
  struct inpcb *in, *in2;

  in = kmalloc(sizeof(struct inpcb));
  if (in == NULL)
    return ENOBUFS;
  memset(in, 0, sizeof(struct inpcb));

  in2 = head;
  while (in2->next && (in2 = in2->next))
    ;
  in2->next = in;
  in->prev = in2;
  in->head = head;

  in->so = so;
  so->pcb = in;
  return 0;
}

int
in_pcbdealloc(struct inpcb* in)
{
  in->so->pcb = NULL;
  kfree(in);
  return 0;
}

/*
  in_pcbbind()
  Set the local address and port of the socket
  Can be called from bind(), or sendto() if client is not connected yet
 */
int
in_pcbbind(struct inpcb* in, struct mbuf* m)
{
  struct sockaddr_in* sin;
  struct inpcb* i;
  unsigned int lport, wild = 0, reuseport = (in->so->options & SO_REUSEPORT);

  if (in->laddr || in->lport)
    return EINVAL;
  if ((in->so->options & (SO_REUSEPORT | SO_REUSEADDR)) == 0)
    wild = IPLOOKUP_WILDCARD;

  if (m) {
    sin = (struct sockaddr_in*)mtod(m);
    if (sin->len != sizeof(struct sockaddr_in))
      return EINVAL;

    lport = sin->port;
    sin->port = 0; // fuck!
    if (if_getifaddr((struct sockaddr*)sin) == NULL)
      return EINVAL;

    in->laddr = sin->addr;
    if (lport) {
      i = in_pcblookup(in->head, sin->addr, lport, 0, 0, wild);
      if (i && (i->so->options && reuseport) == 0)
        return EINVAL;
    }
  }
  // TODO: random port
  if (lport == 0) {
  }
  in->lport = lport;
  return 0;
}

/*
  in_pcbconnect()
  Set the foreign address and port of a socket
  If the local address hasnt been set, it is set here aswell
  If the local port hasnt been set, it calls in_pcbbind()
 */
int
in_pcbconnect(struct inpcb* in, struct mbuf* m)
{
  struct sockaddr_in* sin = (struct sockaddr_in*)mtod(m);
  struct in_ifaddr* ia;
  int fport;

  if (sin->len != sizeof(struct sockaddr_in))
    return EINVAL;
  if (sin->family != AF_INET)
    return EAFNOSUPPORT;
  if (sin->port == 0)
    return EADDRNOTAVAIL;

  if (in_ifaddr) {
    if (sin->addr == INADDR_ANY) {
      sin->addr = in_ifaddr->addr.addr;
    } else if (sin->addr == INADDR_BROADCAST) {
      sin->addr = in_ifaddr->in_broadaddr.addr;
    }
  }
  // try to find a local address if not already set
  if (in->laddr == INADDR_ANY) {
    fport = sin->port;
    sin->port = 0; // needs to be disabled for if_getifaddr()
    ia = (struct in_ifaddr*)if_getifaddr((struct sockaddr*)sin);
    if (ia == NULL && (ia = in_ifaddr) == NULL)
      return EINVAL;
  }
  if (in_pcblookup(in->head,
                   in->laddr ? in->laddr : ia->addr.addr,
                   in->lport,
                   sin->addr,
                   sin->port,
                   0)) // no wildcard because looking for exact match
    return EADDRINUSE;
  if (in->laddr == INADDR_ANY) {
    if (in->lport == 0)
      in_pcbbind(in, NULL); // TODO: pcbbind support for random port
    in->laddr = ia->addr.addr;
  }
  in->faddr = sin->addr;
  in->fport = sin->port;
  return 0;
}

/*
  in_pcblookup()
  Lookup up a pcb that matches best with the arguments passed

  TCP and UDP use this to demultiplex received datagrams

  1. if local ports dont match, skip
  2. if either addresses is a wildcard, wildcard++
  3. if neither are wildcards, they have to be equal or else skip
  5. if both are wildcards, do nothing
*/
struct inpcb*
in_pcblookup(struct inpcb* head,
             unsigned long laddr,
             unsigned short lport,
             unsigned long faddr,
             unsigned short fport,
             int flags)
{
  struct inpcb *in, *match = NULL;
  int wildcard, matchwild = 3;

#define WILDCARD 1

  for (in = head; in; in = in->next) {
    if (in->lport != lport)
      continue;
    wildcard = 0;

    // socket listening on specific address
    if (in->laddr != INADDR_ANY) {
      // looking for socket with any local address
      if (laddr == INADDR_ANY)
        wildcard++;
      // looking for specific local address
      else if (laddr != in->laddr)
        continue;
    } else if (laddr != INADDR_ANY)
      wildcard++;

    // socket connected to specific address
    if (in->faddr != INADDR_ANY) {
      // looking for socket with any foreign address
      if (faddr == INADDR_ANY)
        wildcard++;
      // for foreign, ports have to be equal aswell
      else if (faddr != in->faddr || fport != in->fport)
        continue;
    } else if (faddr != INADDR_ANY)
      wildcard++;

    if (flags & WILDCARD && wildcard < matchwild) {
      match = in;
      if ((matchwild = wildcard) == 0)
        return match;
    }
  }
  return match;
}
