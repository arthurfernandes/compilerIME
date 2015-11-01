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

#define DT_RULE 8
#define DF_RULE 7
#define IDD_RULE 82
#define IDU_RULE 83
#define ID_RULE 84
#define NB_RULE 85

using namespace std;

extern void parse();

typedef enum{
    ACCEPT = 60,END,P,LDE,DE,DF,DT,DC,LI,DV,LP,B,LDV,LS,S,E,L,R,K,F,LE,LV,T,TRU,FALS,CHR,STR,NUM,IDD,IDU,ID,NB
} t_nont;




#endif /* analisador_sintatico_hpp */
