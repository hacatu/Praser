//run_parser.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include "get_line.h"
#include "ptree.h"
#include "parser_llk.h"
#include "spawn_process.h"
#include "debug.h"

int main(int argc, char **argv){
	FILE *file = stdin;
	if(argc < 2){
		puts("No file specified, reading from stdin:");
	}else{
		file = fopen(argv[1], "r");
		if(!file){
			printf("Could not open \"%s\"!  Reading from stdin:\n", argv[1]);
			file = stdin;
		}
	}
	size_t length = 0;
	char *line = NULL;
	int read = 0;
	pid_t childPID;
	char *parserPath;
	char *const *parserArgs;
	while((read = getLine(&line, &length, file)) > 0){
		line[read - 1] = '\0';
		if(!strcmp(line, "calculator")){
			parserPath = "bin/calculator.parser";
			parserArgs = (char *const[]){"calculator.parser", NULL};
		}else if(!strcmp(line, "tree")){
			parserPath = "bin/tree.parser";
			parserArgs = (char *const[]){"tree.parser", NULL};
		}else{
			puts("No such command found!");
			continue;
		}
		childPID = spawn_process(parserPath, parserArgs);
		printf("Created process (pid: %i)\n", childPID);
		if(childPID == -1){
			puts("Failed to create child process!");
			free(line);
			fclose(file);
			return 1;
		}
		waitPID(childPID);
		puts("Child finished");
	}
	free(line);
	fclose(file);
}

