//spawn_process.c
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include "get_line.h"
#include <stdio.h>
#include "debug.h"

pid_t spawn_process(const char *path, char *const *args){
	printf("sp: \"%s\", \"%s\"\n", path, args[0]);
	pid_t childPID = fork();
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

void waitPID(pid_t childPID){
	int status;
	(void)waitpid(childPID, &status, 0);
}

