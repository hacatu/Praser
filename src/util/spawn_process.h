//spawn_process.h
#ifdef _WIN32
#include <process.h>
#include <direct.h>

typedef intptr_t Pid;
#endif
#ifdef __unix__
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

typedef pid_t Pid;
#endif
Pid spawn_process(const char *path, char *const *args);

void waitPID(Pid childPID);

char checkFile(const char *path);

char *getpwd(char *buf, int len);

