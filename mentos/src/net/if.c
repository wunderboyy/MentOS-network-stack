#include "net/if.h"
#include "mem/slab.h"
#include "net/if_dl.h"
#include "string.h"
#include "sys/socket.h"

struct ifnet* ifnet_list; // list of all interfaces
int if_index;             // index of last interface

// add interface to list of all interfaces
// allocates: 1 ifaddr and 2 sockaddr_dl
void
if_attach(struct ifnet* ifnet)
{
  struct ifaddr* ifa = NULL;
  struct sockaddr_dl* sdl;
  int ifasize, socksize, unitname, namelen, unitlen;
  char buf[12];

  ifnet->que.maxlen = QUEMAXLEN; // 50
  ifnet->next = ifnet_list;
  ifnet_list = ifnet;

  itoa(buf, ifnet->unit, 10);
  namelen = strlen(ifnet->name);
  unitlen = strlen(buf);

  socksize = namelen + unitlen;
#define _offsetof(s, m) ((int)&((s*)0)->s)
  socksize += offsetof(struct sockaddr_dl, data) + ifnet->addrlen;
  ifasize = sizeof(struct sockaddr_dl) + 2 * socksize;

  if ((ifa = kmalloc(ifasize)) == NULL)
    return;
  memset(ifa, 0x00, ifasize);
  ifa->ifnet = ifnet;
  ifa->next = ifnet->ifaddr_list;
  ifnet->ifaddr_list = ifa;

  sdl = (struct sockaddr_dl*)ifa + 1;
  ifa->addr = (struct sockaddr*)sdl;
  sdl->len = socksize;
  sdl->index = ifnet->index;
  sdl->type = ifnet->type;
  sdl->family = AF_LINK;
  memcpy(sdl->data, ifnet->name, namelen);
  memcpy(sdl->data + namelen, buf, unitlen);
  sdl->nlen = namelen + unitlen;

  sdl += socksize;
  ifa->netmask = (struct sockaddr*)sdl;
  sdl->len = socksize - ifnet->addrlen;
  memset(sdl->data, 0xff, namelen + unitlen);
}

// find an ifaddr with addr
struct ifaddr*
if_getifaddr(struct sockaddr* addr)
{
  struct ifnet* ifnet;
  struct ifaddr* ifa;

  for (ifnet = ifnet_list; ifnet; ifnet = ifnet->next)
    for (ifa = ifnet->ifaddr_list; ifa; ifa = ifa->next)
      if (ifa->addr->family == addr->family)
        if (memcmp(ifa->addr, addr, addr->len) == 0)
          return ifa;
  return NULL;
}
