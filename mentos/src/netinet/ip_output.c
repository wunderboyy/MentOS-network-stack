#include "net/if.h"
#include "netinet/in.h"
#include "netinet/ip.h"
#include "sys/endian.h"
#include "sys/errno.h"
#include "sys/mbuf.h"
#include "sys/socket.h"

static unsigned short ip_id = 1;

struct sockaddr_in iproute;

// m mbuf already contains an ip packet, but not all fields are filled (eg
// src)
int
ip_output(struct mbuf* m, int flags)
{
  struct ip* ip;
  struct sockaddr_in* dst;
  struct ifaddr* ifa;
  struct ifnet* ifnet;
  int hlen = sizeof(struct ip);

  // TODO: check options

  ip = (struct ip*)mtod(m);

  if ((flags & (IP_FORWARDING | IP_RAWOUTPUT)) == 0) {
    ip->version = IPVERSION;
    ip->id = ip_id++; // TODO: check for overflow
    ip->hl = hlen / 4;
  } else {
  }

  // TODO: routing code

  dst = &iproute;

  dst->family = AF_INET;
  dst->len = sizeof *dst;
  dst->addr = ip->dst;

  if (flags & IP_ROUTETOIF) {
    ifa = if_getifaddr((struct sockaddr*)dst); // TODO: slip
    if (ifa == NULL) {
      return ENETUNREACH;
    }
    ifnet = ifa->ifnet;
    ip->ttl = 1;
  } else {
    // TODO: routing code
  }

  if (ip->src == INADDR_ANY)
    ip->src = ((struct sockaddr_in*)ifa->addr)->addr;

  // TODO: check broadcast

  if (ip->len < ifnet->mtu) {
    ip->id = LITTLETOBIG16(ip->id);
    ip->off = LITTLETOBIG16(ip->id);
    ip->len = LITTLETOBIG16(ip->len);
    // TODO: checksum
    return ifnet->output(ifnet, m, (struct sockaddr*)dst, 0);
  }
  // TODO: routing code
  return 0;
}
