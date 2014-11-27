//spawn_process.c
#ifdef _WIN32
#include <stdlib.h>
#include <windows.h>
#include "get_line.h"
#include <stdio.h>
#include "debug.h"
#include "spawn_process.h"

Pid spawn_process(const char *path, char *const *args){
	printf("sp: \"%s\", \"%s\"\n", path, args[0]);
	return spawnv(P_WAIT, path, args);
}

void waitPID(Pid childPID){
	return;
}

char checkFile(const char *path){
	return GetFileAttributes(path);
}

char *getpwd(char *buf, int len){
	return _getcwd(buf, len);
}
#endif
#ifdef __unix__
#include <sys/wait.h>
#include <stdlib.h>
#include "get_line.h"
#include "debug.h"
#include "spawn_process.h"

typedef pid_t Pid;

Pid spawn_process(const char *path, char *const *args){
	printf("sp: \"%s\", \"%s\"\n", path, args[0]);
	Pid childPID = fork();
	if(childPID == -1){
		return -1;
	}
	if(childPID != 0){
		return childPID;
	}
	execvp(path, args);
	perror("execv");
	_exit(-1);
}

void waitPID(Pid childPID){
	int status;
	(void)waitpid(childPID, &status, 0);
}

char checkFile(const char *path){
	return access(path, F_OK) != -1;
}

char *getpwd(char *buf, int len){
	return getcwd(buf, len);
}
#endif

