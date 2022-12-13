#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int
main()
{
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in sin;
  sin.addr = 0x7f000001;
  sin.port = 6900;
  sin.len = sizeof(struct sockaddr_in);
  sin.family = AF_INET;
  char buf[100] = "you know like in the movies where it ends with a scream";

  bind(fd, (struct sockaddr*)&sin, sizeof(struct sockaddr_in));

  sin.port = 443;

  sendto(fd, buf, strlen(buf), (struct sockaddr*)&sin, 16);

  printf("test");
  printf("test");
}
