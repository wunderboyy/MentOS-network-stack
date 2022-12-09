
#define IFT_LOOP 0x1
#define IFT_ETHER 0x2
#define IFT_SLIP 0x3


struct ifnet {
  struct ifnet *next;
  struct ifaddr *ifaddr_list;
  char *name;
  char index; // index in interface array
  char unit; // eth0, unit = 0

  char type; // ether, loop etc
  char addrlen;
  char hdrlen;
  unsigned long mtu;
  

  int (*init)(int);
  int (*output)(struct ifnet*, void*, void*, void*);

  struct ifqueue {
    void* head;
    int len;
    int maxlen;
  } que;
};

#define QUEMAXLEN 50



struct ifaddr {
  struct sockaddr *addr;
  struct sockaddr *broadaddr;
  struct sockaddr *netmask;
  struct ifnet *ifnet;
  struct ifaddr *next;
};


void if_attach(struct ifnet*);

void loopattach(void);
int looutput(struct ifnet*, void*, void*, void*);
