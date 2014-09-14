//debug.h
#pragma once
#include <stdio.h>
#include <string.h>
#ifdef DEBUG
#define dbg 1
#define debug(M, ...) fprintf(stdout, "DEBUG %s: " M "\n", __func__, ##__VA_ARGS__)
#else
#define debug(M, ...)
#define dbg 0
#endif

