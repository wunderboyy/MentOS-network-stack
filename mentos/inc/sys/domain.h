

struct domain
{
  char family;
  char* name;

  struct protosw *protosw, *protoswEND;
  struct domain* next;

  int (*init)();
};

struct domain* domains;
