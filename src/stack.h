//stack.h
#pragma once

#define _MULTIPLIER 1.6

#define DEFINE_STACK(STACK_TYPE, DATA_TYPE)                          \
	typedef struct{                                                  \
		DATA_TYPE *data;                                             \
		size_t length;                                               \
		size_t size;                                                 \
	} STACK_TYPE;

#define CREATE_STACK(DATA_TYPE)                                      \
	{                                                                \
		.data = malloc(2*sizeof(DATA_TYPE)),                         \
		.length = 0,                                                 \
		.size = 2                                                    \
	}

#define DELETE_STACK(STACK)                                          \
	{                                                                \
		free(STACK.data);                                            \
		STACK.data = NULL;                                           \
		STACK.length = 0;                                            \
		STACK.size = 0;                                              \
	}

#define PUSH(DATA_TYPE, STACK, ELEMENT)                              \
	{                                                                \
		DATA_TYPE *temp;                                              \
		if(STACK.length == STACK.size){                              \
			temp = realloc(STACK.data, _MULTIPLIER*STACK.size*sizeof(DATA_TYPE));\
			if(temp){                                                \
				STACK.data = temp;                                   \
				STACK.size *= _MULTIPLIER;                           \
			}                                                        \
		}                                                            \
		if(STACK.length != STACK.size){                              \
			STACK.data[STACK.length] = ELEMENT;                      \
			++STACK.length;                                          \
		}                                                            \
	}

#define POP(STACK)                                                   \
	STACK.length?STACK.data[--STACK.length]:0

#define TOP(STACK)                                                   \
	STACK.length?STACK.data[STACK.length - 1]:0

#define LENGTH(STACK)                                                \
	STACK.length

#define PUSH_N(DATA_TYPE, STACK, N, ...)                             \
	{                                                                \
		DATA_TYPE elements[N] = {__VA_ARGS__};                       \
		for(size_t i = 0; i < N; ++i){                               \
			PUSH(DATA_TYPE, STACK, elements[i]);                     \
		}                                                            \
	}

