

#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3

#define AF_INET 2
#define AF_LINK 18

struct sockaddr
{
  char len;
  char family;

  char data[14];
};
