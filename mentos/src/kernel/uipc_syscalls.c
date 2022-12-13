#include "fs/vfs_types.h"
#include "string.h"
#include "sys/errno.h"
#include "sys/mbuf.h"
#include "sys/socket.h"
#include "process/process.h"
#include "process/scheduler.h"
#include "sys/socketvar.h"

int
getsock(vfs_file_descriptor_t* fdp, int fd, vfs_file_t** file);

int
sockargs(struct mbuf** m, char* buf, int buflen, int type);

extern int
falloc(task_struct*, vfs_file_t**, int*);

// call socreate
int
sys_socket(int domain, int type, int protocol)
{
  task_struct* task = scheduler_get_current_process();
  int fd, error;
  vfs_file_t* file;
  struct socket* so;
  if ((error = falloc(task, &file, &fd)) < 0)
    return error;
  file->type = FTYPE_SOCKET;

  socreate(domain, &so, type, protocol);
  file->data = so;

  return fd;
}

int
sys_bind(int s, struct sockaddr* addr, int len)
{
  task_struct* task = scheduler_get_current_process();
  vfs_file_t* file;
  struct socket* so;
  struct mbuf* m;
  int error;

  if ((error = getsock(task->fd_list, s, &file)) < 0)
    return error;
  so = file->data;

  if ((error = sockargs(&m, (char*)addr, len, MT_SONAME)) < 0)
    return error;

  sobind(so, m);
  return 0;
}

int
sosend(struct socket* so,
       struct mbuf* to,
       char* buf,
       int blen,
       struct mbuf* top,
       int flags);

int
sys_sendto(int s, void* buf, int len, struct sockaddr* addr, int addrlen)
{
  task_struct* task = scheduler_get_current_process();
  vfs_file_t* file;
  struct socket* so;
  struct mbuf* m;
  int error;

  if ((error = getsock(task->fd_list, s, &file)) < 0)
    return error;
  so = file->data;

  if ((error = sockargs(&m, (char*)addr, addrlen, MT_SONAME)) < 0)
    return error;

  sosend(so, m, buf, len, NULL, 0);

  return 0;
}

int
sys_recvfrom(int s, void* buf, int len, struct sockaddr* addr, int addrlen)
{
  task_struct* task = scheduler_get_current_process();
  vfs_file_t* file;
  struct socket* so;
  struct mbuf* m;
  int error;

  if ((error = getsock(task->fd_list, s, &file)) < 0)
    return error;
  so = file->data;

  if ((error = sockargs(&m, (char*)addr, addrlen, MT_SONAME)) < 0)
    return error;

  int flags;
  soreceive(so, buf, len, &flags);

  return 0;
}

int
getsock(vfs_file_descriptor_t* fdp, int fd, vfs_file_t** file)
{
  if (fdp[fd].file_struct->type == FTYPE_SOCKET) {
    *file = fdp[fd].file_struct;
    return 0;
  }
  return -ENOTSOCK;
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
