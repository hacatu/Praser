//parser_repl.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util/get_line.h"
#include "ptree/ptree.h"
#include "parser/parser.h"
#include "util/spawn_process.h"
#include "util/debug.h"

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
	size_t line_len = 0, parserPath_len = 0, read;
	char *line = NULL;
	int s_read = 0;
	Pid childPID;
	char *parserPath = NULL, *temp;
	char *const *parserArgs;
	char pwd[1024];
	getpwd(pwd, 1024);
	printf("pwd: %s\n", pwd);
	while((s_read = getLine(&line, &line_len, file)) > 0){
		read = (unsigned int)s_read;
		line[read - 1] = '\0';
		if(read + 11 > parserPath_len){//read + strlen(".parser") > parserPath_len
			temp = realloc(parserPath, (read + 11)*sizeof(char));
			if(!temp){
				puts("Out of memory");
				free(parserPath);
				free(line);
				fclose(file);
				return 1;
			}
			parserPath = temp;
			strcpy(parserPath, "bin/");
			strcat(parserPath, line);
			strcat(parserPath, ".parser");
			parserPath_len = read + 11;
		}
		if(!checkFile(parserPath)){
			printf("No parser \"%s\" found!\n", parserPath);
			continue;
		}
		parserArgs = (char *const[]){line, NULL};
		childPID = spawn_process(parserPath, parserArgs);
		printf("Created process (pid: %i)\n", childPID);
		if(childPID == -1){
			puts("Failed to create child process!");
			free(parserPath);
			free(line);
			fclose(file);
			return 1;
		}
		waitPID(childPID);
		puts("Child finished");
	}
	free(parserPath);
	free(line);
	fclose(file);
}

