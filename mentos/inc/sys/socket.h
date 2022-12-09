



#define AF_LINK 18

struct sockaddr {
  char len;
  char family;

  char data[14];
};
