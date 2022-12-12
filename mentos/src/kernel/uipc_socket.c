#include "mem/slab.h"
#include "string.h"
#include "sys/errno.h"
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
  proto->usrreq(sock, PRU_ATTACH, NULL, (struct mbuf*)proto, NULL);
  *so = sock;
  return 0;
}

int
sobind(struct socket* so, struct mbuf* m)
{
  return so->proto->usrreq(so, PRU_BIND, NULL, m, NULL);
}
