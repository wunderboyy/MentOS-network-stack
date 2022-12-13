#include "process/scheduler.h"
#include "process/process.h"
#include "fs/vfs.h"
#include "fcntl.h"
#include "sys/errno.h"
#include "stdio.h"
#include "io/debug.h"

/*
  falloc()
  Allocate a new file descriptor and file struct for the process
 */
int
falloc(task_struct* task, vfs_file_t** fp, int* fdd)
{
  int fd;
  vfs_file_t* file;

  for (fd = 0; fd < task->max_fd; ++fd) {
    if (!task->fd_list[fd].file_struct) {
      break;
    }
  }

  // Check if there is not fd available.
  if (fd >= MAX_OPEN_FD) {
    return -EMFILE;
  }

  // If fd limit is reached, try to allocate more
  if (fd == task->max_fd) {
    if (!vfs_extend_task_fd_list(task)) {
      pr_err("Failed to extend the file descriptor list.\n");
      return -EMFILE;
    }
  }

  file = kmalloc(sizeof(vfs_file_t));
  task->fd_list[fd].file_struct = file;

  *fp = file;
  *fdd = fd;

  return 0;
}
