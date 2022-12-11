

struct in_ifaddr
{
  struct ifaddr ifa;

  // unsigned longs are stored in host byte order
  // sockaddr_in addresses are in network byte order

  // example ip address: 127.0.0.1
  unsigned long net;          // net = 127
  unsigned long netmask;      // netmask 255.0.0.0
  unsigned long subnet;       // subnet 127
  unsigned long subnetmask;   // subnetmask 255.0.0.0
  unsigned long netbroadcast; // netbroadcast 127.255.255.255

  struct in_ifaddr* next; // all ip addresses are in in_ifaddr list
  struct sockaddr_in addr;
  struct sockaddr_in dstaddr;  // 127.0.0.1
  struct sockaddr_in sockmask; // 255.0.0.0
};

int
in_control(void* s, unsigned long cmd, char* data, struct ifnet* ifnet);

int
in_ifinit(struct ifnet* ifnet, struct in_ifaddr* ia, struct sockaddr_in* si);
