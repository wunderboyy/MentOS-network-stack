#include "netinet/ip_input.h"




#define NETISR_IP 0x1
#define NETISR_ARP 0x2

#define schednetisr(x) {			\
    netisr |= 1 << (x);				\
  }


int netisr;

void netintr(void) {
  if (netisr & (1 << NETISR_IP)) {
    netisr &= ~(1 << NETISR_IP);
    ipintr();
  }
}


