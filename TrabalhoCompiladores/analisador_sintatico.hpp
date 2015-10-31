#ifndef analisador_sintatico_hpp
#define analisador_sintatico_hpp

#include <stdio.h>
#include <stack>
#include <vector>
#include <unordered_map>
#include "analisador_lexico.h"


#define ACTION_TABLE_SIZE 173
#define IS_SHIFT(p) ((p)>0)
#define IS_REDUCTION(p) ((p)<0)
#define RULE(p) (-(p))

#define MAX_NEST_LEVEL 50

using namespace std;

extern void parse();



#endif /* analisador_sintatico_hpp */
