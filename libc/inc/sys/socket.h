

struct sockaddr
{
  char len;
  char family;
  char data[14];
};

#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3

#define AF_INET 2
#define AF_LINK 18

#define SO_REUSEPORT 1
#define SO_REUSEADDR 2

int
socket(int, int, int);

int
bind(int, struct sockaddr*, int);

int
sendto(int, void*, int, struct sockaddr*, int);

int
recvfrom(int, void*, int, struct sockaddr*, int);
