#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/*
 * Create POSIX threads
 * and output its ids.
 * Exit.
 */

struct thread_info {
  pthread_t tid;
  int thread_num;
};

void* thread_start(void *arg)
{
  pid_t pid;
  struct thread_info *tinfo = arg;
  printf("thread num: %d\n", tinfo->thread_num);
  pid = getpid();
  printf("thread pid: %d\n", (int)pid);
}

int main(int argc, char **argv)
{
  int err;
  pid_t pid;
  struct thread_info tinfo;

  tinfo.thread_num = 1;

  err = pthread_create(&tinfo.tid, NULL, &thread_start, &tinfo);
  if (err != 0)
    printf("failed to create a pthread: %s", strerror(err));
  pid = getpid();
  printf("parent proc id: %d\n", (int)pid);
  err = pthread_join(tinfo.tid, NULL);
  if (err != 0)
    printf("failed to join a pthread: %s", strerror(err));
  exit(0);
};
