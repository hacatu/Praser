//spawn_process.c
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include "get_line.h"
#include "debug.h"

pid_t spawn_process(const char *path, char *const *args){
	pid_t childPID = fork();
	if(childPID == -1){
		return -1;
	}
	if(childPID != 0){
		return childPID;
	}
	execv(path, args);
	puts("execv failed!");
	_exit(-1);
}

void waitPID(pid_t childPID){
	int status;
	(void)waitpid(childPID, &status, 0);
}

