#include "stddef.h"
#include "sys/domain.h"
#include "sys/protosw.h"
#include "sys/socket.h"

void
domaininit(void)
{
  struct domain* d;
  struct protosw* p;
  extern struct domain inetdomain;
  inetdomain.next = domains;
  domains = &inetdomain;

  for (d = domains; d; d = d->next) {
    if (d->init)
      d->init();
    for (p = d->protosw; p < d->protoswEND; p++) {
      if (p->init)
        p->init();
    }
  }
}

struct protosw*
pffindtype(unsigned char family, unsigned char type)
{
  struct protosw* p;
  struct domain* d;

  for (d = domains; d; d = d->next)
    if (d->family == family)
      goto found;
  return NULL;
found:
  for (p = d->protosw; p < d->protoswEND; p++)
    if (p->type == type)
      return p;
  return NULL;
}

struct protosw*
pffindproto(unsigned char family, unsigned char proto, unsigned char type)
{

  struct protosw *p, *maybe;
  struct domain* d;

  for (d = domains; d; d = d->next)
    if (d->family == family)
      goto found;
  return NULL;
found:
  for (p = d->protosw; p < d->protoswEND; p++) {
    if (p->type == type && p->protocol == proto)
      return p;
    if (type == SOCK_RAW && p->type == SOCK_RAW && p->protocol == 0 &&
        maybe == NULL)
      maybe = p;
  }
  return maybe;
}

void
pfctlinput(int cmd, struct sockaddr* sa)
{
  struct domain* d;
  struct protosw* p;

  for (d = domains; d; d = d->next)
    for (p = d->protosw; p < d->protoswEND; p++)
      if (p->ctlinput)
        p->ctlinput();
}
