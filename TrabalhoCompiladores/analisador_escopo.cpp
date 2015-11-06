
#include "analisador_escopo.hpp"
#include <string>
#include <stack>
#include <iostream>

using namespace std;

/*Global Variables*/

/*Output File*/
FILE *out;
/*Semantic Stack*/
stack<t_attrib> StackSem;
/*Functions*/
int nFuncs = 0;
pobject curFunction;
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

/*Label Generation*/

int newLabel(void){
    static int labelNo = 0;
    return labelNo++;
}

/*Scope Analysis*/

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

void Error(errorcode code){
    cout << "Linha: " << currentLine << " - ";
    switch(code){
        case ERR_NO_DECL:
            cout << "Variavel nao declarada";
            break;
        case ERR_REDCL:
            cout << "Variavel ja foi declarada";
            break;
        case ERR_TYPE_EXPECTED:
            cout << "Type Expected: Um tipo nao foi declarado anteriormente";
            break;
        case ERR_BOOL_TYPE_EXPECTED:
            cout << "Bool Expected: Um tipo booleano e esperado para expressao";
            break;
        case ERR_INVALID_TYPE:
            cout << "Invalid Type: O tipo e invalido para a operacao";
            break;
        case ERR_TYPE_MISMATCH:
            cout << "Type Mismatch: O tipo e invalido para a operacao";
            break;
        case ERR_KIND_NOT_STRUCT:
            cout << "Kind not Struct: A operacao so pode ser realizada em tipos Struct";
            break;
        case ERR_FIELD_NOT_DECL:
            cout << "Field not Declared: O campo nao foi declarado na estrutura";
            break;
        case ERR_KIND_NOT_ARRAY:
            cout << "Kind not Array: A operacao so pode ser realizada para um Array";
            break;
        case ERR_INVALID_INDEX_TYPE:
            cout << "Invalid Index: O Indice especificado para o Array e invalido";
            break;
        case ERR_KIND_NOT_VAR:
            cout << "Kind not Var: A operacao so e valida com tipos Var";
            break;
        case ERR_KIND_NOT_FUNCTION:
            cout << "Kind not Function: A operacao so e valida com tipos Function";
            break;
        case ERR_TOO_FEW_ARGS:
            cout << "Too Few Args: O numero de parametros especificado nao e suficiente";
            break;
        case ERR_TOO_MANY_ARGS:
            cout << "Too Many Args: O numero de parametros especificado e maior que o especificado";
            break;
        case ERR_PARAM_TYPE:
            cout << "Param Type: O tipo especificado para o parametro e invalido";
            break;
        case ERR_RETURN_TYPE_MISMATCH:
            cout << "Return Type Mismatch: O tipo de retorno não corresponde ao tipo especificado para a função" << endl;
            break;
        default:
            break;
    }
    cout << endl;
}

bool CheckTypes(pobject t1,pobject t2){
    if(t1 == t2){
        return true;
    }
    else if(t1 == pUniversal || t2 == pUniversal){
        return true;
    }
    else if(t1->eKind == UNIVERSAL_ || t2->eKind == UNIVERSAL_){
        return true;
    }
    else if(t1->eKind == ALIAS_TYPE_ && t2->eKind != ALIAS_TYPE_){
        return CheckTypes(t1->_.Alias.pBaseType,t2);
    }
    else if(t1->eKind != ALIAS_TYPE_ && t2->eKind == ALIAS_TYPE_){
        return CheckTypes(t1,t2->_.Alias.pBaseType);
    }
    else if(t1->eKind == t2->eKind){
        //alias
        if(t1->eKind == ALIAS_TYPE_){
            return CheckTypes(t1->_.Alias.pBaseType,t2->_.Alias.pBaseType);
        }
        //array
        else if(t1->eKind == ARRAY_TYPE_){
            if(t1->_.Array.nNumElems == t2->_.Array.nNumElems){
                return CheckTypes(t1->_.Array.pElemType,t2->_.Array.pElemType);
            }
        }
        //struct
        else if(t1->eKind == STRUCT_TYPE_){
            pobject f1 = t1->_.Struct.pFields;
            pobject f2 = t2->_.Struct.pFields;
            
            while( f1 != NULL && f2 != NULL){
                if( ! CheckTypes(f1->_.Field.pType,f2->_.Field.pType)){
                    return false;
                }
            }
            return (f1 == NULL && f2 == NULL);
        }
    }
    
    return false;
}

void semantics(int rule){
    static int name,n,l,l1,l2;
    static pobject p,t,f;
    static t_attrib IDD_,IDU_,ID_,T_,LI_,LI0_,LI1_,TRU_,FALS_,STR_,CHR_,NUM_,DC_,DC0_,DC1_,LP_,LP0_,LP1_,E_,E0_,E1_,L_,L0_,L1_,R_,R0_,R1_,K_,K0_,K1_,F_,F0_,F1_,LV_,LV0_,LV1_,MC_,LE_,LE0_,LE1_,MT_,ME_,MW_,MA_;
    
    switch(rule){
        case IDD_RULE:
            name = tokenSecundario;
            IDD_.nont = IDD;
            IDD_._.ID.name = name;
            if( (p = search(name)) != NULL){
                Error(ERR_REDCL);
            }
            else{
                p = define(name);
            }
            
            p->eKind = NO_KIND_DEF_;
            IDD_._.ID.obj = p;
            StackSem.push(IDD_);
            break;
        case IDU_RULE:
            name = tokenSecundario;
            IDU_.nont = IDU;
            IDU_._.ID.name = name;
            
            //cout << "TOKENSECUNDARIO" << name << endl;
            if((p = find(name)) == NULL){
                Error(ERR_NO_DECL);
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
                T_.nSize = p->_.Alias.nSize;
            }
            else{
                T_._.T.type = pUniversal;
                T_.nSize = 0;
                Error(ERR_TYPE_EXPECTED);
            }
            T_.nont = T;
            StackSem.push(T_);
            break;
        case T_INTEGER_RULE:
            T_._.T.type = pInt;
            T_.nont = T;
            T_.nSize = 1;
            StackSem.push(T_);
            break;
        case T_CHAR_RULE:
            T_._.T.type = pChar;
            T_.nont = T;
            T_.nSize = 1;
            StackSem.push(T_);
            break;
        case T_BOOL_RULE:
            T_._.T.type = pBool;
            T_.nont = T;
            T_.nSize = 1;
            StackSem.push(T_);
            break;
        case T_STRING_RULE:
            T_._.T.type = pString;
            T_.nont = T;
            T_.nSize = 1;
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
            StackSem.pop();
            p = LI_._.LI.list;
            n = curFunction->_.Function.nVars;
            
            while(p != NULL && p->eKind == NO_KIND_DEF_){
                p->eKind = VAR_;
                p->_.Var.pType = t;
                p->_.Var.nSize = T_.nSize;
                p->_.Var.nIndex = n;
                
                n += T_.nSize;
                p = p->pNext;
            }
            
            curFunction->_.Function.nVars = n;
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
            p->_.Array.nSize = n * T_.nSize;
            
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
            p->_.Alias.nSize = T_.nSize;
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
            n = 0;
            while( p != NULL && p->eKind == NO_KIND_DEF_){
                p->eKind = FIELD_;
                p->_.Field.pType = t;
                p->_.Field.nSize = T_.nSize;
                p->_.Field.nIndex = n;
                n = n + T_.nSize;
                p = p->pNext;
            }
            DC_._.DC.list = LI_._.LI.list;
            DC_.nSize = n;
            DC_.nont = DC;
            StackSem.push(DC_);
            break;
        case DC_DC_RULE:
            T_ = StackSem.top();
            StackSem.pop();
            LI_ = StackSem.top();
            StackSem.pop();
            DC1_ = StackSem.top();
            StackSem.pop();
            
            p = LI_._.LI.list;
            t = T_._.T.type;
            n = DC1_.nSize;
            
            while( p != NULL && p->eKind == NO_KIND_DEF_){
                p->eKind = FIELD_;
                p->_.Field.pType = t;
                p->_.Field.nIndex = n;
                p->_.Field.nSize = T_.nSize;
                n = n + T_.nSize;
                p = p->pNext;
            }
            
            DC0_._.DC.list = DC1_._.DC.list;
            DC0_.nSize = n;
            DC0_.nont = DC;
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
            p->_.Struct.nSize = DC_.nSize;
            endBlock();
            break;
        case LP_EPSILON_RULE:
            LP_._.LP.list = NULL;
            LP_.nont = LP;
            LP_.nSize = 0;
            StackSem.push(LP_);
            break;
        case LP_IDD_RULE:
            T_ = StackSem.top();
            StackSem.pop();
            IDD_ = StackSem.top();
            StackSem.pop();
            
            p = IDD_._.ID.obj;
            t = T_._.T.type;
            p->eKind = PARAM_;
            p->_.Param.pType = t;
            p->_.Param.nIndex = 0;
            p->_.Param.nSize = T_.nSize;
            LP_._.LP.list = p;
            LP_.nSize = T_.nSize;
            LP_.nont = LP;
            
            StackSem.push(LP_);
            break;
        case LP_LP_RULE:
            T_ = StackSem.top();
            StackSem.pop();
            IDD_ = StackSem.top();
            StackSem.pop();
            LP1_ = StackSem.top();
            StackSem.pop();
            
            p = IDD_._.ID.obj;
            t = T_._.T.type;
            n = LP1_.nSize;
            
            p->eKind = PARAM_;
            p->_.Param.pType = t;
            p->_.Param.nIndex = n;
            p->_.Param.nSize = T_.nSize;
            
            LP0_._.LP.list = LP1_._.LP.list;
            LP0_.nSize = n + T_.nSize;
            LP0_.nont = LP;
            StackSem.push(LP0_);
            break;
        case NF_RULE:
            IDD_ = StackSem.top();
            f = IDD_._.ID.obj;
            f->eKind = FUNCTION_;
            f->_.Function.nParams = 0;
            f->_.Function.nVars = 0;
            f->_.Function.nIndex = nFuncs++;
            newBlock();
            break;
        case MF_RULE:
            T_ = StackSem.top();
            StackSem.pop();
            LP_ = StackSem.top();
            StackSem.pop();
            IDD_ = StackSem.top();
            StackSem.pop();
            
            f = IDD_._.ID.obj;
            f->eKind = FUNCTION_;
            f->_.Function.pRetType = T_._.T.type;
            f->_.Function.pParams = LP_._.LP.list;
            f->_.Function.nParams = LP_.nSize;
            f->_.Function.nVars = LP_.nSize;
            curFunction = f;
            
            fprintf(out,"BEGIN_FUNC %d %d %d\n",f->_.Function.nIndex,
                    f->_.Function.nParams,
                    f->_.Function.nVars-f->_.Function.nParams);
            break;
        case DF_RULE:
            endBlock();
            fprintf(out,"END_FUNC\n");
            break;
        case S_BLOCK_RULE:
            endBlock();
            break;
        case S_E_SEMICOLON:
            E_ = StackSem.top();
            StackSem.pop();
            fprintf(out,"\tPOP\n");
            break;
        case MT_RULE:
            l = newLabel();
            MT_._.MT.label = l;
            MT_.nont = MT;
            fprintf(out,"\tTJMP_FW L%d\n",l);
            StackSem.push(MT_);
            break;
        case S_IF_RULE:
            StackSem.pop();
            MT_ = StackSem.top();
            StackSem.pop();
            E_ = StackSem.top();
            StackSem.pop();
            
            t = E_._.E.type;
            if( !CheckTypes(t,pBool)){
                Error(ERR_BOOL_TYPE_EXPECTED);
            }
            
            fprintf(out,"L%d\n",MT_._.MT.label);
            
            break;
        case ME_RULE:
            MT_ = StackSem.top();
            l1 = MT_._.MT.label;
            l2 = newLabel();
            ME_._.ME.label = l2;
            ME_.nont = ME;
            StackSem.push(ME_);
            
            fprintf(out,"\tTJMP_FW L%d\nL%d\n",l2,l1);
            break;
        case S_IF_ELSE_RULE:
            ME_ = StackSem.top();
            StackSem.pop();
            MT_ = StackSem.top();
            StackSem.pop();
            E_ = StackSem.top();
            StackSem.pop();
            
            l = ME_._.ME.label;
            
            t = E_._.E.type;
            if( !CheckTypes(t,pBool)){
                Error(ERR_BOOL_TYPE_EXPECTED);
            }
            fprintf(out,"\tL%d\n",l);
            break;
        case MW_RULE:
            l = newLabel();
            MW_._.MW.label = l;
            StackSem.push(MW_);
            fprintf(out,"\tL%d\n",l);
            break;
        case S_WHILE_RULE:
            MT_ = StackSem.top();
            StackSem.pop();
            E_ = StackSem.top();
            StackSem.pop();
            MW_ = StackSem.top();
            StackSem.pop();
            
            l1 = MW_._.MW.label;
            l2 = MT_._.MT.label;
            
            t = E_._.E.type;
            if( !CheckTypes(t,pBool)){
                Error(ERR_BOOL_TYPE_EXPECTED);
            }
            
            fprintf(out,"\tJMP_BW L%d\nL%d\n",l1,l2);
            break;
        case S_DO_WHILE_RULE:
            E_ = StackSem.top();
            StackSem.pop();
            
            MW_ = StackSem.top();
            StackSem.pop();
            l = MW_._.MW.label;
            
            t = E_._.E.type;
            if( !CheckTypes(t,pBool)){
                Error(ERR_BOOL_TYPE_EXPECTED);
            }
            
            fprintf(out,"\tNOT\n\tTJMP_BW L%d\n",l);
            break;
        case S_BREAK_RULE:
            MT_ = StackSem.top();
            break;
        case S_CONTINUE_RULE:
            break;
        case S_RETURN_RULE:
            E_ = StackSem.top();
            StackSem.pop();
            if(!CheckTypes(curFunction->_.Function.pRetType,E_._.E.type)){
                Error(ERR_RETURN_TYPE_MISMATCH);
            }
            fprintf(out,"\tRET\n");
            break;
        case E_AND_RULE:
            L_ = StackSem.top();
            StackSem.pop();
            E1_ = StackSem.top();
            StackSem.pop();
            if( !CheckTypes(E1_._.E.type,pBool)){
                Error(ERR_BOOL_TYPE_EXPECTED);
            }
            
            if( !CheckTypes(L_._.L.type,pBool)){
                Error(ERR_BOOL_TYPE_EXPECTED);
            }
            
            E0_._.E.type = pBool;
            E0_.nont = E;
            StackSem.push(E0_);
            fprintf(out,"\tAND\n");
            break;
        case E_OR_RULE:
            L_ = StackSem.top();
            StackSem.pop();
            E1_ = StackSem.top();
            StackSem.pop();
            if( !CheckTypes(E1_._.E.type,pBool)){
                Error(ERR_BOOL_TYPE_EXPECTED);
            }
            
            if( !CheckTypes(L_._.L.type,pBool)){
                Error(ERR_BOOL_TYPE_EXPECTED);
            }
            
            E0_._.E.type = pBool;
            E0_.nont = E;
            StackSem.push(E0_);
            fprintf(out,"\tOR\n");
            break;
        case E_L_RULE:
            L_ = StackSem.top();
            StackSem.pop();
            E_._.E.type = L_._.L.type;
            E_.nont = E;
            StackSem.push(E_);
            break;
        case L_LESS_THAN_RULE:
            R_ = StackSem.top();
            StackSem.pop();
            L1_ = StackSem.top();
            StackSem.pop();
            
            if( !CheckTypes(L1_._.L.type,R_._.R.type)){
                Error(ERR_TYPE_MISMATCH);
            }
            L0_._.L.type = pBool;
            L0_.nont = L;
            StackSem.push(L0_);
            
            fprintf(out,"\tLT\n");
            
            break;
        case L_GREATER_THAN_RULE:
            R_ = StackSem.top();
            StackSem.pop();
            L1_ = StackSem.top();
            StackSem.pop();
            
            if( !CheckTypes(L1_._.L.type,R_._.R.type)){
                Error(ERR_TYPE_MISMATCH);
            }
            L0_._.L.type = pBool;
            L0_.nont = L;
            StackSem.push(L0_);
            fprintf(out,"\tGT\n");
            break;
        case L_LESS_EQUAL_RULE:
            R_ = StackSem.top();
            StackSem.pop();
            L1_ = StackSem.top();
            StackSem.pop();
            
            if( !CheckTypes(L1_._.L.type,R_._.R.type)){
                Error(ERR_TYPE_MISMATCH);
            }
            L0_._.L.type = pBool;
            L0_.nont = L;
            StackSem.push(L0_);
            fprintf(out,"\tLE\n");
            break;
        case L_GREATER_EQUAL_RULE:
            R_ = StackSem.top();
            StackSem.pop();
            L1_ = StackSem.top();
            StackSem.pop();
            
            if( !CheckTypes(L1_._.L.type,R_._.R.type)){
                Error(ERR_TYPE_MISMATCH);
            }
            L0_._.L.type = pBool;
            L0_.nont = L;
            StackSem.push(L0_);
            fprintf(out,"\tGE\n");
            break;
        case L_EQUAL_EQUAL_RULE:
            R_ = StackSem.top();
            StackSem.pop();
            L1_ = StackSem.top();
            StackSem.pop();
            
            if( !CheckTypes(L1_._.L.type,R_._.R.type)){
                Error(ERR_TYPE_MISMATCH);
            }
            L0_._.L.type = pBool;
            L0_.nont = L;
            StackSem.push(L0_);
            fprintf(out,"\tEQ\n");
            break;
        case L_NOT_EQUAL_RULE:
            R_ = StackSem.top();
            StackSem.pop();
            L1_ = StackSem.top();
            StackSem.pop();
            
            if( !CheckTypes(L1_._.L.type,R_._.R.type)){
                Error(ERR_TYPE_MISMATCH);
            }
            L0_._.L.type = pBool;
            L0_.nont = L;
            StackSem.push(L0_);
            fprintf(out,"\tNE\n");
            break;
        case L_R_RULE:
            R_ = StackSem.top();
            StackSem.pop();
            L_._.L.type = R_._.R.type;
            L_.nont = L;
            StackSem.push(L_);
            break;
        case R_PLUS_RULE:
            K_ = StackSem.top();
            StackSem.pop();
            R1_ = StackSem.top();
            StackSem.pop();
            
            if( ! CheckTypes(R1_._.R.type,K_._.K.type)){
                Error(ERR_TYPE_MISMATCH);
            }
            
            if( !CheckTypes(R1_._.R.type,pInt) && !CheckTypes(R1_._.R.type,pString)){
                Error(ERR_INVALID_TYPE);
            }
            
            R0_._.R.type = R1_._.R.type;
            R0_.nont = R;
            StackSem.push(R0_);
            fprintf(out,"\tADD\n");
            break;
        case R_MINUS_RULE:
            K_ = StackSem.top();
            StackSem.pop();
            R1_ = StackSem.top();
            StackSem.pop();
            
            if( ! CheckTypes(R1_._.R.type,K_._.K.type)){
                Error(ERR_TYPE_MISMATCH);
            }
            
            if( !CheckTypes(R1_._.R.type,pInt)){
                Error(ERR_INVALID_TYPE);
            }
            
            R0_._.R.type = R1_._.R.type;
            R0_.nont = R;
            StackSem.push(R0_);
            fprintf(out,"\tSUB\n");
            break;
        case R_K_RULE:
            K_ = StackSem.top();
            StackSem.pop();
            R_._.R.type = K_._.K.type;
            R_.nont = R;
            StackSem.push(R_);
            break;
        case K_TIMES_RULE:
            F_ = StackSem.top();
            StackSem.pop();
            K1_ = StackSem.top();
            StackSem.pop();
            
            if( !CheckTypes(K1_._.K.type,F_._.F.type)){
                Error(ERR_TYPE_MISMATCH);
            }
            
            if( !CheckTypes(K1_._.K.type,pInt)){
                Error(ERR_INVALID_TYPE);
            }
            
            K0_._.K.type = K1_._.K.type;
            K0_.nont = K;
            StackSem.push(K0_);
            fprintf(out,"\tMUL\n");
            break;
        case K_DIVIDE_RULE:
            F_ = StackSem.top();
            StackSem.pop();
            K1_ = StackSem.top();
            StackSem.pop();
            
            if( !CheckTypes(K1_._.K.type,F_._.F.type)){
                Error(ERR_TYPE_MISMATCH);
            }
            
            if( !CheckTypes(K1_._.K.type,pInt)){
                Error(ERR_INVALID_TYPE);
            }
            
            K0_._.K.type = K1_._.K.type;
            K0_.nont = K;
            StackSem.push(K0_);
            fprintf(out,"\tDIV\n");
            break;
        case K_F_RULE:
            F_ = StackSem.top();
            StackSem.pop();
            K_._.K.type = F_._.F.type;
            K_.nont = K;
            StackSem.push(K_);
            break;
        case F_LV_RULE:
            LV_ = StackSem.top();
            StackSem.pop();
            
            n = LV_._.LV.type->_.Type.nSize;
            
            F_._.F.type = LV_._.LV.type;
            F_.nont = F;
            StackSem.push(F_);
            fprintf(out,"\tDE_REF %d\n",n);
            break;
        case F_LEFT_PLUS_PLUS_RULE:
            LV_ = StackSem.top();
            StackSem.pop();
            t = LV_._.LV.type;
            if( !CheckTypes(t,pInt)){
                Error(ERR_INVALID_TYPE);
            }
            
            F_._.F.type = pInt;
            F_.nont = F;
            fprintf(out,"\tDUP\n\tDUP\n\tDE_REF 1\n");
            fprintf(out,"\tINC\n\tSTORE_REF 1\n\tDE_REF 1\n");
            StackSem.push(F_);
            break;
        case F_LEFT_MINUS_MINUS_RULE:
            LV_ = StackSem.top();
            StackSem.pop();
            t = LV_._.LV.type;
            if( !CheckTypes(t,pInt)){
                Error(ERR_INVALID_TYPE);
            }
            
            F_._.F.type = LV_._.LV.type;
            F_.nont = F;
            StackSem.push(F_);
            fprintf(out,"\tDUP\n\tDUP\n\tDE_REF 1\n");
            fprintf(out,"\tDEC\n\tSTORE_REF 1\n\tDE_REF 1\n");
            break;
        case F_RIGHT_PLUS_PLUS_RULE:
            LV_ = StackSem.top();
            StackSem.pop();
            t = LV_._.LV.type;
            if( !CheckTypes(t,pInt)){
                Error(ERR_INVALID_TYPE);
            }
            
            F_._.F.type = LV_._.LV.type;
            F_.nont = F;
            StackSem.push(F_);
            fprintf(out,"\tDUP\n\tDUP\n\tDE_REF 1\n");
            fprintf(out,"\tINC\n\tSTORE_REF 1\n\tDE_REF 1\n");
            fprintf(out,"\tDEC\n");
            break;
        case F_RIGHT_MINUS_MINUS_RULE:
            LV_ = StackSem.top();
            StackSem.pop();
            t = LV_._.LV.type;
            if( !CheckTypes(t,pInt)){
                Error(ERR_INVALID_TYPE);
            }
            
            F_._.F.type = t;
            F_.nont = F;
            StackSem.push(F_);
            fprintf(out,"\tDUP\n\tDUP\n\tDE_REF 1\n");
            fprintf(out,"\tDEC\n\tSTORE_REF 1\n\tDE_REF 1\n");
            fprintf(out,"\tINC\n");
            break;
        case F_PARENTHESIS_E_RULE:
            E_ = StackSem.top();
            StackSem.pop();
            
            F_._.F.type = E_._.E.type;
            F_.nont = F;
            StackSem.push(F_);
            break;
        case F_MINUS_F_RULE:
            F1_ = StackSem.top();
            StackSem.pop();
            
            t = F1_._.F.type;
            if( !CheckTypes(t,pInt)){
                Error(ERR_INVALID_TYPE);
            }
            
            F0_._.F.type = t;
            F0_.nont = F;
            StackSem.push(F0_);
            fprintf(out,"\tNEG\n");
            break;
        case F_NOT_F_RULE:
            F1_ = StackSem.top();
            StackSem.pop();
            
            t = F1_._.F.type;
            if( !CheckTypes(t,pBool)){
                Error(ERR_INVALID_TYPE);
            }
            
            F0_._.F.type = t;
            F0_.nont = F;
            StackSem.push(F0_);
            fprintf(out,"\tNOT\n");
            break;
        case F_TRUE_RULE:
            TRU_ = StackSem.top();
            StackSem.pop();
            F_._.F.type = pBool;
            F_.nont = F;
            StackSem.push(F_);
            fprintf(out,"\tLOAD_TRUE\n");
            break;
        case F_FALSE_RULE:
            FALS_ = StackSem.top();
            StackSem.pop();
            F_._.F.type = pBool;
            F_.nont = F;
            StackSem.push(F_);
            fprintf(out,"\tLOAD_FALSE\n");
            break;
        case F_CHR_RULE:
            CHR_ = StackSem.top();
            StackSem.pop();
            F_._.F.type = pChar;
            F_.nont = F;
            StackSem.push(F_);
            n = tokenSecundario;
            fprintf(out,"\tLOAD_CONST %d\n",n);
            break;
        case F_STR_RULE:
            STR_ = StackSem.top();
            StackSem.pop();
            F_._.F.type = pString;
            F_.nont = F;
            StackSem.push(F_);
            n = tokenSecundario;
            fprintf(out,"\tLOAD_CONST %d\n",n);
            break;
        case F_NUM_RULE:
            STR_ = StackSem.top();
            StackSem.pop();
            F_._.F.type = pInt;
            F_.nont = F;
            StackSem.push(F_);
            n = tokenSecundario;
            fprintf(out,"\tLOAD_CONST %d\n",n);
            break;
        case LV_DOT_RULE:
            ID_ = StackSem.top();
            StackSem.pop();
            LV1_ = StackSem.top();
            StackSem.pop();
            
            t = LV1_._.LV.type;
            if( t-> eKind != STRUCT_TYPE_){
                if(t->eKind != UNIVERSAL_){
                    Error( ERR_KIND_NOT_STRUCT);
                }
                LV0_._.LV.type = pUniversal;
            }
            else{
                p = t->_.Struct.pFields;
                while(p!=NULL){
                    if(p->nName == ID_._.ID.name){
                        break;
                    }
                    p = p->pNext;
                }
                if(p == NULL){
                    Error(ERR_FIELD_NOT_DECL);
                    LV0_._.LV.type = pUniversal;
                }
                else{
                    LV0_._.LV.type = p->_.Field.pType;
                    LV0_._.LV.type->_.Type.nSize = p->_.Field.nSize;
                }
            }
            
            LV0_.nont = LV;
            StackSem.push(LV0_);
            fprintf(out,"\tADD %d\n",p->_.Field.nIndex);
            break;
        case LV_SQUARE_RULE:
            E_ = StackSem.top();
            StackSem.pop();
            LV1_ = StackSem.top();
            StackSem.pop();
            
            t = LV1_._.LV.type;
            if(t == pString){
                LV0_._.LV.type = pChar;
            }
            else if(t->eKind != ARRAY_TYPE_){
                if(t->eKind != UNIVERSAL_){
                    Error(ERR_KIND_NOT_ARRAY);
                }
                LV0_._.LV.type = pUniversal;
            }
            else{
                LV0_._.LV.type = t->_.Array.pElemType;
                n = t->_.Array.nSize/t->_.Array.nNumElems;
                fprintf(out,"\tMUL %d\n\tADD\n",n);
            }
            
            if( !CheckTypes(E_._.E.type,pInt)){
                Error(ERR_INVALID_INDEX_TYPE);
            }
            
            LV0_.nont = LV;
            StackSem.push(LV0_);
            break;
        case LV_IDU_RULE:
            IDU_ = StackSem.top();
            StackSem.pop();
            
            p = IDU_._.ID.obj;
            if(p->eKind != VAR_ && p->eKind != PARAM_){
                if(p->eKind != UNIVERSAL_){
                    Error(ERR_KIND_NOT_VAR);
                }
                LV_._.LV.type = pUniversal;
            }
            else{
                LV_._.LV.type = p->_.Var.pType;
                LV_._.LV.type->_.Type.nSize = p->_.Var.nSize;
            }
            LV_.nont = LV;
            StackSem.push(LV_);
            fprintf(out,"\tLOAD_REF %d\n",p->_.Var.nIndex);
            break;
        case MA_RULE:
            fprintf(out,"\tDUP\n");
            break;
        case E_LV_EQUAL_RULE:
            E1_ = StackSem.top();
            StackSem.pop();
            LV_ = StackSem.top();
            StackSem.pop();
            if(! CheckTypes(LV_._.LV.type,E1_._.E.type)){
                Error(ERR_TYPE_MISMATCH);
            }
            
            E0_._.F.type = E1_._.E.type;
            StackSem.push(E0_);
            fprintf(out,"\tSTORE_REF %d",t->_.Type.nSize);
            fprintf(out,"\tDE_REF %d",t->_.Type.nSize);
            break;
        case MC_RULE:
            IDU_ = StackSem.top();
            f = IDU_._.ID.obj;
            
            if(f->eKind != FUNCTION_){
                Error(ERR_KIND_NOT_FUNCTION);
                MC_._.MC.type = pUniversal;
                MC_._.MC.param = NULL;
                MC_._.MC.err = true;
            }
            else{
                MC_._.MC.type = f->_.Function.pRetType;
                MC_._.MC.param = f->_.Function.pParams;
                MC_._.MC.err = false;
            }
            MC_.nont = MC;
            StackSem.push(MC_);
            break;
        case LE_E_RULE:
            E_ = StackSem.top();
            StackSem.pop();
            MC_ = StackSem.top();
            
            LE_._.LE.param = NULL;
            LE_._.LE.err = MC_._.MC.err;
            n = 1;
            if( !MC_._.MC.err){
                p = MC_._.MC.param;
                if(p == NULL){
                    Error(ERR_TOO_MANY_ARGS);
                    LE_._.LE.err = true;
                }
                else{
                    if(!CheckTypes(p->_.Param.pType,E_._.E.type)){
                        Error(ERR_PARAM_TYPE);
                    }
                    LE_._.LE.param = p->pNext;
                    LE_._.LE.n = n+1;
                }
            }
            LE_.nont = LE;
            StackSem.push(LE_);
            break;
        case LE_LE_RULE:
            E_ = StackSem.top();
            StackSem.pop();
            LE1_ = StackSem.top();
            StackSem.pop();
            LE0_._.LE.param = NULL;
            LE0_._.LE.err = L1_._.LE.err;

            n = LE1_._.LE.n;
            if(!LE1_._.LE.err){
                p = LE1_._.LE.param;
                if(p == NULL){
                    Error(ERR_TOO_MANY_ARGS);
                    LE0_._.LE.err = true;
                }
                else{
                    if(!CheckTypes(p->_.Param.pType,E_._.E.type)){
                        Error(ERR_PARAM_TYPE);
                    }
                    LE0_._.LE.param = p->pNext;
                    LE0_._.LE.n = n + 1;
                }
            }
            LE0_.nont = LE;
            StackSem.push(LE0_);
            break;
        case LE_EPSILON_RULE:
            MC_ = StackSem.top();
            if(MC_._.MC.param != NULL){
                LE_._.LE.err = true;
            }
            else{
                LE_._.LE.err = false;
            }
            LE_._.LE.n = 0;
            LE_._.LE.param = MC_._.MC.param;
            LE_._.LE.type = pUniversal;
            LE_.nont = LE;
            StackSem.push(LE_);
            break;
        case F_IDU_MC_RULE:
            LE_ = StackSem.top();
            StackSem.pop();
            MC_ = StackSem.top();
            StackSem.pop();
            IDU_ = StackSem.top();
            StackSem.pop();
            f = IDU_._.ID.obj;
            F_._.F.type = MC_._.MC.type;
            if(!LE_._.LE.err){
                if(LE_._.LE.n-1 < f->_.Function.nParams){
                    Error(ERR_TOO_FEW_ARGS);
                }
               else if(LE_._.LE.n-1 > f->_.Function.nParams){
                    Error(ERR_TOO_MANY_ARGS);
                }
            }
            F_.nont = F;
            StackSem.push(F_);
            fprintf(out,"\tCALL %d\n",f->_.Function.nIndex);
            break;
        default:
            break;
    }
}
