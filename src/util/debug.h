//debug.h
#pragma once
#include <stdio.h>
#include <string.h>
#ifdef DEBUG_LOG
#define dbg_log 1
#define debug_log(M, ...) fprintf(stdout, "DEBUG %s: " M "\n", __func__, ##__VA_ARGS__)
#else
#define debug_log(M, ...)
#define dbg_log 0
#endif

#ifdef DEBUG_CALLS
#define dbg 1
#define debug_calls(M, ...) fprintf(stdout, "DEBUG %s: " M "\n", __func__, ##__VA_ARGS__)
#else
#define debug_calls(M, ...)
#define dbg 0
#endif

#ifdef DEBUG_GETS
#define debug_gets(M, ...) fprintf(stdout, "DEBUG %s: " M "\n", __func__, ##__VA_ARGS__)
#define dbg_gets 1
#else
#define debug_gets(M, ...)
#define dbg_gets 0
#endif


