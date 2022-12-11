#include "net/if.h"
#include "netinet/in.h"
#include "netinet/in_var.h"

struct ifqueue ipintrq = { .maxlen = 50 };

struct in_ifaddr* in_ifaddr;

void
ipintr(void)
{}
