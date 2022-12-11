#include "netinet/in.h"
#include "mem/slab.h"
#include "net/if.h"
#include "netinet/in_var.h"
#include "stddef.h"
#include "sys/errno.h"

#define SIOCSIFADDR 1
#define SIOCSIFDSTADDR 2
#define SIOCSIFNETMASK 3
#define SIOCGIFADDR 4
#define SIOCGIFNETMASK 5
#define SIOCGIFDSTADDR 6
#define SIOCGIFBRDADDR 7

int
in_control(void* s, unsigned long cmd, char* data, struct ifnet* ifnet)
{

  extern struct in_ifaddr* in_ifaddr;
  struct in_ifaddr *ia = NULL, *oia;
  struct ifaddr* ifa;

  if (ifnet)
    for (ia = in_ifaddr; ia; ia = ia->next)
      if (ia->ifa.ifnet == ifnet)
        break;

  switch (cmd) {
    case SIOCSIFDSTADDR:
    case SIOCSIFNETMASK:
    case SIOCSIFADDR: {
      if (ia == NULL) {
        ia = kmalloc(sizeof(struct in_ifaddr));
        if ((oia = in_ifaddr)) {
          for (; oia->next; oia = oia->next)
            ;
          oia->next = ia;
        } else {
          in_ifaddr = ia;
        }
        if ((ifa = ifnet->ifaddr_list)) {
          for (; ifa->next; oia = oia->next)
            ;
          ifa->next = (struct ifaddr*)ia;
        } else {
          ifnet->ifaddr_list = (struct ifaddr*)ia;
        }
        ia->ifa.addr = (struct sockaddr*)&ia->addr;
        ia->ifa.broadaddr = (struct sockaddr*)&ia->dstaddr;
        ia->ifa.netmask = (struct sockaddr*)&ia->sockmask;
        ia->ifa.ifnet = ifnet;
        break;
      }
      case SIOCGIFADDR:
      case SIOCGIFNETMASK:
      case SIOCGIFDSTADDR:
      case SIOCGIFBRDADDR:
        if (ia == NULL) {
          return -EADDRNOTAVAIL;
        }
        break;
    }
  }

  switch (cmd) {
    case SIOCSIFADDR:
      return in_ifinit(ifnet, ia, (struct sockaddr_in*)data);
    case SIOCSIFNETMASK:
      ia->subnetmask = ((struct sockaddr_in*)data)->addr;
      break;
    case SIOCSIFDSTADDR:
      if ((ifnet->flags & IFF_POINTOPOINT) == 0)
        return -EINVAL;
      ia->dstaddr = *(struct sockaddr_in*)data;
      break;
  }
  return 0;
}

// init an interface's ip address
int
in_ifinit(struct ifnet* ifnet, struct in_ifaddr* ia, struct sockaddr_in* si)
{
  unsigned long ip = si->addr; // XXX ntohl

  ia->addr = *si;

  if (INCLASS_A(ip))
    ia->netmask = CLASS_A_NET;
  else if (INCLASS_B(ip))
    ia->netmask = CLASS_B_NET;
  else
    ia->netmask = CLASS_C_NET;

  ia->subnetmask = ia->netmask;

  ia->net = ia->netmask & ip;
  ia->subnet = ia->subnetmask & ip;

  if (ifnet->flags & IFF_BROADCAST) {
    ia->netbroadcast = ia->net | ~ia->netmask;
    ia->dstaddr.addr = ia->subnet | ~ia->subnetmask;
  } else if (ifnet->flags & IFF_LOOPBACK) {
    ia->dstaddr.addr = ia->addr.addr;
  }
  return 0;
}
