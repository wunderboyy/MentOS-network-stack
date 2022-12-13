

#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3

#define AF_INET 2
#define AF_LINK 18

#define SO_REUSEPORT 1
#define SO_REUSEADDR 2

struct sockaddr
{
  char len;
  char family;

  char data[14];
};

int
sys_socket(int, int, int);

int
sys_bind(int, struct sockaddr*, int);

int
sys_sendto(int, void*, int, struct sockaddr*, int);

int
sys_recvfrom(int, void*, int, struct sockaddr*, int);
