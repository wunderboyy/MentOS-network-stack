#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int
main()
{
  int fd;
  struct sockaddr_in sin;

  fd = socket(AF_INET, SOCK_DGRAM, 0);
  sin.addr = 0x7f000001;
  sin.port = 443;
  sin.family = AF_INET;
  sin.len = 16;
  bind(fd, (struct sockaddr*)&sin, 16);
  printf("Server bound to port 443!\n");
  char buf[100];

  sin.addr = INADDR_ANY;
  sin.port = 0;
  while (1) {
    recvfrom(fd, buf, 55, (struct sockaddr*)&sin, 16);
    if (strlen(buf) > 20) {
      printf("Data: %s", buf);
      return 1;
    }
  }
}
