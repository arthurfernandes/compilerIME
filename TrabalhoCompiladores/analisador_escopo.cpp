
#include "analisador_escopo.hpp"
#include <string>
#include <stack>
#include <iostream>

using namespace std;

stack<t_attrib> StackSem;

/*Symbol Tables*/
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
    cout << "Linha: " << currentLine << "," << msg << endl;
}

void semantics(int rule){
    int name;
    pobject p;
    switch(rule){
        case IDD_RULE:
            name = tokenSecundario;
            t_attrib IDD_;
            IDD_.nont = IDD;
            IDD_._.IDD.name = name;
            
            //cout << "TOKENSECUNDARIO" << name << endl;
            if( (p = search(name)) != NULL){
                scopeError("Ocorreu um erro de redeclaração");
            }
            else{
                p = define(name);
            }
            
            IDD_._.IDD.obj = p;
            StackSem.push(IDD_);
            //t_attrib attr2;
            //attr2 = StackSem.top();
            //cout << "LINE" << currentLine << "Stack Size:" << StackSem.size() << " Stack Top : " <<   << endl;
            break;
        case IDU_RULE:
            name = tokenSecundario;
            t_attrib IDU_;
            IDU_.nont = IDU;
            IDU_._.IDU.name = name;
            
            //cout << "TOKENSECUNDARIO" << name << endl;
            if((p = find(name)) == NULL){
                scopeError("Ocorreu um erro de não declaração");
                p = define(name);
            }
            
            IDU_._.IDU.obj = p;
            StackSem.push(IDU_);
            break;
        case ID_RULE:
            t_attrib ID_;
            ID_.nont = ID;
            name = tokenSecundario;
            ID_._.ID.name = name;
            ID_._.ID.obj = NULL;
            StackSem.push(ID_);
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
