
#include "analisador_escopo.hpp"
#include "analisador_lexico.h"
#include "analisador_sintatico.hpp"
#include <string>
#include <iostream>

using namespace std;

typedef struct object
{
    int nName;
    struct object *pNext;
} object, *pobject;

pobject SymbolTable[MAX_NEST_LEVEL];
pobject SymbolTableLast[MAX_NEST_LEVEL];
int nCurrentLevel = 0;

int newBlock(void){
    SymbolTable[++nCurrentLevel] = NULL;
    SymbolTableLast[nCurrentLevel] = NULL;
    return nCurrentLevel;
}

int endBlock(void){
    return --nCurrentLevel;
}

pobject define(int aName){
    pobject obj = new object();
    obj->nName = aName;
    obj->pNext = NULL;
    
    if( SymbolTable[nCurrentLevel] == NULL){
        SymbolTable[nCurrentLevel] = obj;
        SymbolTableLast[nCurrentLevel] = obj;
    }
    else{
        SymbolTableLast[nCurrentLevel]->pNext = obj;
        SymbolTableLast[nCurrentLevel] = obj;
    }
    return obj;
}

pobject search (int aName){
    pobject obj = SymbolTable[nCurrentLevel];
    while(obj != NULL){
        if(obj-> nName == aName)
            break;
        else
            obj = obj->pNext;
    }
    return obj;
}

pobject find (int aName)
{
    int i;
    pobject obj = NULL;
    for( i = nCurrentLevel; i >= 0; --i ) {
        obj = SymbolTable[i];
        while( obj != NULL )
        {
            if( obj->nName == aName )
                break;
            else
                obj = obj->pNext;
        }
        if( obj != NULL ) break;
    }
    return obj;
}

void scopeError(string msg){
    cout << msg << endl;;
}

void semantics(int rule){
    int name;
    pobject p;
    switch(rule){
        case IDD_RULE:
            name = tokenSecundario;
            cout << "TOKENSECUNDARIO" << name << endl;
            if( (p = search(name)) != NULL){
                scopeError("Ocorreu um erro de redeclaração: ");
                exit(2);
            }
            else{
                p = define(name);
            }
            break;
        case IDU_RULE:
            name = tokenSecundario;
            cout << "TOKENSECUNDARIO" << name << endl;
            if((p = find(name)) == NULL){
                scopeError("Ocorreu um erro de não declaração: ");
                exit(2);
                p = define(name);
            }
            break;
        case ID_RULE:
            name = tokenSecundario;
            break;
        case NB_RULE:
            newBlock();
            break;
        case DT_RULE:
            endBlock();
            break;
        case DF_RULE:
            endBlock();
            break;
        default:
            break;
    }
}
