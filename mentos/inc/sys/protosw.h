

struct domain;

struct protosw
{
  struct domain* dom;
  char type;
  char protocol;

  int (*init)();
  int (*ctlinput)();
};
