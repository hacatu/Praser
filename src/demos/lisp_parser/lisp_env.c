//lisp_env.c
#include <stdio.h>
#include <stdlib.h>
#include "lisp_env.h"
#include "lisp_value.h"
#include <debug.h>

//will -Wincompatible-pointer-types -Wint-converison -Wint-to-pointer-cast warn about assigning *int = int;?

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
	char *name = malloc((strlen(attr.name) + 1)*sizeof(char));
	if(!name){
		free(temp);
		return NULL;
	}
	strcpy(name, attr.name);
	attr.name = name;
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
	//replace existing variable (shadow external variable):
	e->attr.value = attr.value;
	return 1;
}

char addName(Env *e, const char *name, LispVal value){
	return addAttr(e, (Attribute){.name = (char*)name, .value = value});
}

Env* getNode(Env *e, const char *name){
	if(!e){
		return NULL;
	}
	int o = strcmp(e->attr.name, name);
	if(o < 0){
		return getNode(e->left, name);
	}
	if(o > 0){
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

char setName(Env *e, const char *name, LispVal value){
	Env *temp = getNode(e, name);
	if(!temp){
		return 0;
	}
	temp->attr.value = value;
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
		e->left = 0;
	}
	if(e->right){
		deleteEnv(e->right);
		e->right = 0;
	}
	free(e->attr.name);
	free(e);
}

void deleteEnvData(Env *e){
	if(e->left){
		deleteEnv(e->left);
		e->left = 0;
	}
	if(e->right){
		deleteEnv(e->right);
		e->right = 0;
	}
	deleteLispVal(e->attr.value);
	free(e->attr.name);
	free(e);
}

Env* createEnvFrom(Attribute attrv[]){
	if(!attrv || !attrv[0].name){
		return NULL;
	}
	Env *e = createOneAttrEnv(attrv[0]);
	if(!e){
		return NULL;
	}
	for(size_t i = 1; attrv[i].name; ++i){
		if(!addAttr(e, attrv[i])){
			deleteEnv(e);
			return NULL;
		}
	}
	return e;
}

Env* PRA_firstPostorderEnv(Env *e){
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

Env* PRA_nextPostorderEnv(Env *e){
	Env *parent = e->parent;
	if(!parent){
		return NULL;
	}
	if(parent->right == e){
		return parent;
	}
	return PRA_firstPostorderEnv(parent->right);
}

//TODO: Build copy while iterating original.
Env* copyEnv(const Env *e){
	Env *cpy = createOneAttrEnv(e->attr);
	while((e = nextPreorderEnv(e))){
		if(!addAttr(cpy, e->attr)){
			deleteEnv(cpy);
			return NULL;
		}
	}
	return cpy;
}

