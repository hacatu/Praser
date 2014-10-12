//lisp_env.c
#include <stdio.h>
#include <stdlib.h>
#include "lisp_interpreter.h"
#include "lisp_env.h"
#include "../util/debug.h"

struct Env{
	struct Env *parent;
	struct Env *left;
	struct Env *right;
	Attribute attr;
};

Env* createOneAttrEnv(Attribute attr){
	Env *temp = malloc(1*sizeof(Env));
	if(!temp){
		return NULL;
	}
	*temp = (Env){.attr = attr};
	return temp;
}

char addAttr(Env *e, Attribute attr){
	int o = strcmp(e->attr.name, attr.name);
	if(o < 0){
		if(!e->left){
			e->left = createOneAttrEnv(attr);
			return e->left?1:0;
		}
		return addAttr(e->left, attr);
	}
	if(o > 0){
		if(!e->right){
			e->right = createOneAttrEnv(attr);
			return e->right?1:0;
		}
		return addAttr(e->right, attr);
	}
	return 0;
}

char addName(Env *e, const char *name, LispVal val){
	return addAttr(e, (Attribute){.name = name, .value = val});
}

Env* getNode(Env *e, const char *name){
	int o = strcmp(e->attr.name, name);
	if(o < 0){
		if(!e->left){
			return NULL;
		}
		return getNode(e->left, name);
	}
	if(o > 0){
		if(!e->right){
			return NULL;
		}
		return getNode(e->right, name);
	}
	return e;
}

LispVal getName(Env *e, const char *name){
	Env *temp = getNode(e, name);
	if(!temp){
		return BASE_NYI;
	}
	return temp->attr.value;
}

char setName(Env *e, const char *name, LispVal val){
	Env *temp = getNode(e, name);
	if(!temp){
		return 0;
	}
	temp->attr.value = val;
	return 1;
}

Env* nextPreorderEnv(const Env *e){
	if(e->left){
		return e->left;
	}
	if(e->right){
		return e->right;
	}
	const Env *curr = e;
	Env *next = curr->parent;
	while(next){
		if(next->left == curr){
			return next->right;
		}
		curr = next;
		next = next->parent;
	}
	return NULL;
}

//TODO: Make iterative.
void deleteEnv(Env *e){
	if(e->left){
		deleteEnv(e->left);
		free(e->left);
		e->left = 0;
	}
	if(e->right){
		deleteEnv(e->right);
		free(e->right);
		e->right = 0;
	}
	deleteLispVal(e->attr.value);
}

Env* createEnvFrom(size_t attrc, Attribute attrv[]){
	if(!attrc){
		return NULL;
	}
	Env *e = createOneAttrEnv(attrv[0]);
	if(!e){
		return NULL;
	}
	for(size_t i = 1; i < attrc; ++i){
		if(!addAttr(e, attrv[i])){
			deleteEnv(e);
			return NULL;
		}
	}
	return e;
}

Env* firstPostorderEnv(Env *e){
	while(1){
		if(e->left){
			e = e->left;
			continue;
		}
		if(e->right){
			e = e->right;
			continue;
		}
	}
	return e;
}

Env* nextPostorderEnv(Env *e){
	Env *parent = e->parent;
	if(!parent){
		return NULL;
	}
	if(parent->right == e){
		return parent;
	}
	return firstPostorderEnv(parent->right);
}

//TODO: Build copy while iterating original.
Env* copyEnv(const Env *e){
	Env *cpy = createOneAttrEnv(e->attr);
	while((e = nextPreorderEnv(e))){
		if(!addName(cpy, e->attr.name, copyLispVal(e->attr.value))){
			deleteEnv(cpy);
			return NULL;
		}
	}
	return cpy;
}

