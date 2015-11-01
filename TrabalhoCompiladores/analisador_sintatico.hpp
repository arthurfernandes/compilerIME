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

//Scope Rules

#define DF_RULE 7
#define IDD_RULE 82
#define IDU_RULE 83
#define ID_RULE 84
#define NB_RULE 85

//Type Rules

#define T_INTEGER_RULE 72
#define T_CHAR_RULE 73
#define T_BOOL_RULE 74
#define T_STRING_RULE 75
#define T_IDU_RULE 76

#define LI_COMMA_RULE 13
#define LI_IDD_RULE 14
#define DV_VAR_RULE 15

#define TRUE_RULE 77
#define FALSE_RULE 78
#define CHR_RULE 79
#define STR_RULE 80
#define NUM_RULE 81

#define DT_STRUCT_RULE 8
#define DT_ARRAY_RULE 9
#define DT_ALIAS_RULE 10
#define DC_DC_RULE 11
#define DC_LI_RULE 12



using namespace std;

extern void parse();

typedef enum{
    ACCEPT = 60,END,P,LDE,DE,DF,DT,DC,LI,DV,LP,B,LDV,LS,S,E,L,R,K,F,LE,LV,T,TRU,FALS,CHR,STR,NUM,IDD,IDU,ID,NB
} t_nont;




#endif /* analisador_sintatico_hpp */
