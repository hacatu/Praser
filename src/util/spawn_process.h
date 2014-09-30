//spawn_process.h
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

pid_t spawn_process(const char *path, char *const *args);

void waitPID(pid_t childPID);

