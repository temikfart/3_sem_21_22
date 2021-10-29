#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

// Return ptr to sem_id or NULL if error occurred
int *MySemOpen(const char *path);     // Opening semaphore
// Return 0 if successful or -1 otherwise
int MySemClose(int *sem_id);          // Closing semaphore
int MySemRemove(const char *path);    // Removing semaphore
int MySemPost(const int *sem_id);     // Increasing semaphore value
int MySemWait(const int *sem_id);     // Decreasing semaphore value
