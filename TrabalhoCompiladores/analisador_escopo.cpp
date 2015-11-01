
#include "analisador_escopo.hpp"
#include <string>
#include <stack>
#include <iostream>

using namespace std;

/*Global Variables*/

stack<t_attrib> StackSem;

/*Symbol Tables*/
pobject SymbolTable[MAX_NEST_LEVEL];
pobject SymbolTableLast[MAX_NEST_LEVEL];
int nCurrentLevel = 0;

/*Scalar Types*/

object int_ = {-1, NULL, SCALAR_TYPE_};
pobject pInt = &int_;

object char_ = {-1, NULL, SCALAR_TYPE_};
pobject pChar = &char_;

object bool_ = {-1, NULL, SCALAR_TYPE_};
pobject pBool = &bool_;

object string_ = {-1, NULL, SCALAR_TYPE_};
pobject pString = &string_;

object universal_ = {-1, NULL, SCALAR_TYPE_};
pobject pUniversal = &universal_;


/*METHODS*/

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
    int name,n;
    pobject p,t;
    t_attrib IDD_,IDU_,ID_,T_,LI_,LI0_,LI1_,TRU_,FALS_,STR_,CHR_,NUM_,DC_,DC0_,DC1_;
    switch(rule){
        case IDD_RULE:
            name = tokenSecundario;
            IDD_.nont = IDD;
            IDD_._.ID.name = name;
            
            //cout << "TOKENSECUNDARIO" << name << endl;
            if( (p = search(name)) != NULL){
                scopeError("Ocorreu um erro de redeclaração");
            }
            else{
                p = define(name);
            }
            
            p->eKind = NO_KIND_DEF_;
            IDD_._.ID.obj = p;
            StackSem.push(IDD_);
            //t_attrib attr2;
            //attr2 = StackSem.top();
            //cout << "LINE" << currentLine << "Stack Size:" << StackSem.size() << " Stack Top : " <<   << endl;
            break;
        case IDU_RULE:
            name = tokenSecundario;
            IDU_.nont = IDU;
            IDU_._.ID.name = name;
            
            //cout << "TOKENSECUNDARIO" << name << endl;
            if((p = find(name)) == NULL){
                scopeError("Ocorreu um erro de não declaração");
                p = define(name);
            }
            IDU_._.ID.obj = p;
            StackSem.push(IDU_);
            break;
        case ID_RULE:
            ID_.nont = ID;
            name = tokenSecundario;
            ID_._.ID.name = name;
            ID_._.ID.obj = NULL;
            StackSem.push(ID_);
            break;
            
        case T_IDU_RULE:
            IDU_ = StackSem.top();
            p = IDU_._.ID.obj;
            StackSem.pop();
            
            if(IS_TYPE_KIND(p->eKind) || p->eKind == UNIVERSAL_){
                T_._.T.type = p;
            }
            else{
                T_._.T.type = pUniversal;
                scopeError("Erro de tipos: Tipo esperado não foi definido");
            }
            T_.nont = T;
            StackSem.push(T_);
            break;
        case T_INTEGER_RULE:
            T_._.T.type = pInt;
            T_.nont = T;
            StackSem.push(T_);
            break;
        case T_CHAR_RULE:
            T_._.T.type = pChar;
            T_.nont = T;
            StackSem.push(T_);
            break;
        case T_BOOL_RULE:
            T_._.T.type = pBool;
            T_.nont = T;
            StackSem.push(T_);
            break;
        case T_STRING_RULE:
            T_._.T.type = pString;
            T_.nont = T;
            StackSem.push(T_);
            break;
        case LI_IDD_RULE:
            IDD_ = StackSem.top();
            LI_._.LI.list = IDD_._.ID.obj;
            LI_.nont = LI;
            StackSem.pop();
            StackSem.push(LI_);
            break;
        case LI_COMMA_RULE:
            IDD_ = StackSem.top();
            StackSem.pop();
            LI1_ = StackSem.top();
            StackSem.pop();
            LI0_._.LI.list = LI1_._.LI.list;
            LI0_.nont = LI;
            StackSem.push(LI0_);
            break;
        case DV_VAR_RULE:
            T_ = StackSem.top();
            t = T_._.T.type;
            StackSem.pop();
            LI_ = StackSem.top();
            p = LI_._.LI.list;
            StackSem.pop();
            
            while(p != NULL && p->eKind == NO_KIND_DEF_){
                p->eKind = VAR_;
                p->_.Var.pType = t;
                p = p->pNext;
            }
            break;
        case TRUE_RULE:
            TRU_.nont = TRU;
            TRU_._.TRU.val = true;
            TRU_._.TRU.type = pBool;
            StackSem.push(TRU_);
            break;
        case FALSE_RULE:
            FALS_.nont = FALS;
            FALS_._.FALS.val = false;
            FALS_._.FALS.type = pBool;
            StackSem.push(FALS_);
            break;
        case CHR_RULE:
            CHR_.nont = CHR;
            CHR_._.CHR.type = pChar;
            CHR_._.CHR.pos = tokenSecundario;
            CHR_._.CHR.val = getCharConst(tokenSecundario);
            StackSem.push(CHR_);
            break;
        case STR_RULE:
            STR_.nont = STR;
            STR_._.STR.type = pString;
            STR_._.STR.pos = tokenSecundario;
            STR_._.STR.val = getStringConst(tokenSecundario);
            StackSem.push(STR_);
            break;
        case NUM_RULE:
            NUM_.nont = NUM;
            NUM_._.NUM.type = pInt;
            NUM_._.NUM.pos = tokenSecundario;
            NUM_._.NUM.val = getIntConst(tokenSecundario);
            StackSem.push(NUM_);
            break;
        case DT_ARRAY_RULE:
            T_ = StackSem.top();
            StackSem.pop();
            NUM_ = StackSem.top();
            StackSem.pop();
            IDD_ = StackSem.top();
            StackSem.pop();
            
            p = IDD_._.ID.obj;
            n = NUM_._.NUM.val;
            t = T_._.T.type;
            
            p->eKind = ARRAY_TYPE_;
            p->_.Array.nNumElems = n;
            p->_.Array.pElemType = t;
            break;
        case DT_ALIAS_RULE:
            T_ = StackSem.top();
            StackSem.pop();
            IDD_ = StackSem.top();
            StackSem.pop();
            
            p = IDD_._.ID.obj;
            t = T_._.T.type;
            
            p->eKind = ALIAS_TYPE_;
            p->_.Alias.pBaseType = t;
            break;
        case NB_RULE:
            newBlock();
            break;
        case DC_LI_RULE:
            T_ = StackSem.top();
            StackSem.pop();
            LI_ = StackSem.top();
            StackSem.pop();
            
            p = LI_._.LI.list;
            t = T_._.T.type;
            while( p != NULL && p->eKind == NO_KIND_DEF_){
                p->eKind = FIELD_;
                p->_.Field.pType = t;
                p = p->pNext;
            }
            DC_._.DC.list = LI_._.LI.list;
            StackSem.push(DC_);
            break;
        case DC_DC_RULE:
            T_ = StackSem.top();
            StackSem.pop();
            LI_ = StackSem.top();
            StackSem.pop();
            
            p = LI_._.LI.list;
            t = T_._.T.type;
            while( p != NULL && p->eKind == NO_KIND_DEF_){
                p->eKind = FIELD_;
                p->_.Field.pType = t;
                p = p->pNext;
            }
            
            DC1_ = StackSem.top();
            StackSem.pop();
            DC0_._.DC.list = DC1_._.DC.list;
            StackSem.push(DC0_);
            break;
        case DT_STRUCT_RULE:
            DC_ = StackSem.top();
            StackSem.pop();
            IDD_ = StackSem.top();
            StackSem.pop();
            
            p = IDD_._.ID.obj;
            p->eKind = STRUCT_TYPE_;
            p->_.Struct.pFields = DC_._.DC.list;
            
            endBlock();
            break;
        case DF_RULE:
            endBlock();
            break;
        default:
            break;
    }
}
