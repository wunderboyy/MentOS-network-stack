#include "fs/vfs_types.h"
#include "string.h"
#include "sys/errno.h"
#include "sys/mbuf.h"
#include "sys/socket.h"

int
getsock(vfs_file_descriptor_t* fdp, int fd, vfs_file_t** file)
{
  if (fdp[fd].type == FTYPE_SOCKET) {
    *file = fdp[fd].file_struct;
    return 0;
  }
  return ENOTSOCK;
}

int
sockargs(struct mbuf** m, char* buf, int buflen, int type)
{
  struct mbuf* mm;
  struct sockaddr* so;

  if (buflen > MLEN)
    return EINVAL;
  mm = mget(type);
  if (mm == NULL)
    return ENOBUFS;
  memcpy(mm->m_data, buf, buflen);
  mm->m_len = buflen;
  *m = mm;
  if (type == MT_SONAME) {
    so = (struct sockaddr*)mtod(mm);
    so->len = buflen;
  }
  return 0;
}
