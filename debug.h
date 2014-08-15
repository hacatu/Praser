//debug.h
#pragma once
#include <stdio.h>
#include <string.h>
#ifdef DEBUG
#define ifdebug
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%s:%d: " M "\n", __func__, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(M, ...)
#define ifdebug if(0)
#endif
