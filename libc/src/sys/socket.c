#include "system/syscall_types.h"
#include "sys/socket.h"
#include "sys/errno.h"
#include "sys/types.h"

_syscall3(int, socket, int, domain, int, type, int, protocol)

_syscall3(int, bind, int, fd, struct sockaddr*, addr, int, len)

_syscall5(int, sendto, int, fd, void*, buf, int, len, struct sockaddr*, addr, int, addrlen)
  
_syscall5(int, recvfrom, int, fd, void*, buf, int, len, struct sockaddr*, addr, int, addrlen)
