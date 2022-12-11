#include "netinet/in.h"
#include "stddef.h"
#include "sys/domain.h"
#include "sys/protosw.h"
#include "sys/socket.h"

extern struct domain inetdomain;

struct protosw inetsw[] = { { &inetdomain, 0, 0 },
                            {
                              &inetdomain,
                              SOCK_DGRAM,
                              IPPROTO_UDP,
                            },
                            { &inetdomain, SOCK_STREAM, IPPROTO_TCP },
                            { &inetdomain, SOCK_RAW, IPPROTO_RAW } };

struct domain inetdomain = { AF_INET,
                             "internet",
                             inetsw,
                             &inetsw[sizeof(inetsw) / sizeof(inetsw[0])],
                             NULL };
