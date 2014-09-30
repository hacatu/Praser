//test_get_line.c
#include <stdio.h>
#include <stdlib.h>
#include "get_line.h"

int main(){
	char *line = NULL;
	size_t size = 0;
	while(getLine(&line, &size, stdin) > 0){
		printf("read: %s\n", line);
	}
	free(line);
}

