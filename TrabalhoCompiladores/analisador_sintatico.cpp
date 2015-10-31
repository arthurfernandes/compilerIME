//
//  analisador_sintatico.cpp
//  TrabalhoCompiladores
//
//  Created by Arthur Fernandes on 10/24/15.
//  Copyright © 2015 Arthur Fernandes. All rights reserved.
//

#include "analisador_sintatico.hpp"
#include <stack>
#include <vector>
#include <unordered_map>

#define DT_RULE 8
#define DF_RULE 7
#define IDD_RULE 82
#define IDU_RULE 83
#define ID_RULE 84
#define NB_RULE 85

using namespace std;

stack<int> stateStack;
vector<unordered_map<int,int>> actionTable(ACTION_TABLE_SIZE);

typedef enum{
    ACCEPT = 60,END,P,LDE,DE,DF,DT,DC,LI,DV,LP,B,LDV,LS,S,E,L,R,K,F,LE,LV,T,TRU,FALS,CHR,STR,NUM,IDD,IDU,ID,NB
} symbols;

int ruleLen[] = {2,2,2,1,1,1,1,9,9,10,5,5,3,3,1,5,5,3,0,4,3,3,2,2,1,2,1,2,1,5,7,5,7,2,2,4,3,3,1,3,3,3,3,3,3,1,3,3,1,3,3,1,1,2,2,2,2,3,4,2,2,1,1,1,1,1,3,1,0,3,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0};

int ruleLeft[] = {ACCEPT,P,LDE,LDE,DE,DE,DE,DF,DT,DT,DT,DC,DC,LI,LI,DV,LP,LP,LP,B,B,B,B,LDV,LDV,LS,LS,S,S,S,S,S,S,S,S,S,E,E,E,L,L,L,L,L,L,L,R,R,R,K,K,K,F,F,F,F,F,F,F,F,F,F,F,F,F,F,LE,LE,LE,LV,LV,LV,T,T,T,T,T,TRU,FALS,CHR,STR,NUM,IDD,IDU,ID,NB};

typedef struct object
{
    int nName;
    struct object *pNext;
} object, *pobject;

pobject SymbolTable[MAX_NEST_LEVEL];
pobject SymbolTableLast[MAX_NEST_LEVEL];
int nCurrentLevel = 0;

/*METHODS*/

void syntaxError();

/*SCOPE*/

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

/*PARSER*/

void initializeActionTable();

void syntaxError(){
    cout << "Ocorreu um erro de sintaxe" << endl;
    exit(1);
}

void parse(){
    initializeActionTable();
    int final_state = 1;
    int q = 0;
    stateStack.push(q);
    int a = nextToken();
    do{
        cout << "PARSER: " << "STATE " << q << "token " << a << " name " << t_terminalNames[a] << endl;
        if(actionTable[q].count(a) < 1){
            syntaxError();
        }
        else{
            int p = actionTable[q][a];
            if(IS_SHIFT(p)){
                stateStack.push(p);
                a = nextToken();
            }
            else if(IS_REDUCTION(p)){
                int r = RULE(p);
                cout << "RULE " << r << endl;
                for(int i = 0;i<ruleLen[r];i++){
                    //cout << "POP " << stateStack.top() << endl;
                    stateStack.pop();
                }
                stateStack.push(actionTable[stateStack.top()][ruleLeft[r]]);
                semantics(r);
                
            }
            else{
                syntaxError();
            }
            q = stateStack.top();
        }
    }while(q != final_state);
}



void initializeActionTable(){
    //STATE0
    actionTable[0][P] = 1;
    actionTable[0][LDE] = 2;
    actionTable[0][DE] = 3;
    actionTable[0][DF] = 4;
    actionTable[0][DT] = 5;
    actionTable[0][DV] = 6;
    actionTable[0][FUNCTION] = 7;
    actionTable[0][TYPE] = 8;
    actionTable[0][VAR] = 9;
    //STATE1
    actionTable[1][ENDFILE] = 1;
    //STATE2
    actionTable[2][ENDFILE] = 10;
    actionTable[2][DE] = 11;
    actionTable[2][DF] = 4;
    actionTable[2][DT] = 5;
    actionTable[2][DV] = 6;
    actionTable[2][FUNCTION] = 7;
    actionTable[2][TYPE] = 8;
    actionTable[2][VAR] = 9;
    //STATE3
    actionTable[3][ENDFILE] = -3;
    actionTable[3][FUNCTION] = -3;
    actionTable[3][TYPE] = -3;
    actionTable[3][VAR] = -3;
    //STATE4
    actionTable[4][ENDFILE] = -4;
    actionTable[4][FUNCTION] = -4;
    actionTable[4][TYPE] = -4;
    actionTable[4][VAR] = -4;
    //STATE5
    actionTable[5][ENDFILE] = -5;
    actionTable[5][FUNCTION] = -5;
    actionTable[5][TYPE] = -5;
    actionTable[5][VAR] = -5;
    //STATE6
    actionTable[6][ENDFILE] = -6;
    actionTable[6][FUNCTION] = -6;
    actionTable[6][TYPE] = -6;
    actionTable[6][VAR] = -6;
    //STATE7
    actionTable[7][IDD] = 12;
    actionTable[7][IDT] = 13;
    //STATE8
    actionTable[8][IDD] = 14;
    actionTable[8][IDT] = 13;
    //STATE9
    actionTable[9][IDD] = 16;
    actionTable[9][LI] = 15;
    actionTable[9][IDT] = 13;
    //STATE10
    actionTable[10][ENDFILE] = -1;
    //STATE11
    actionTable[11][ENDFILE] = -2;
    actionTable[11][FUNCTION] = -2;
    actionTable[11][TYPE] = -2;
    actionTable[11][VAR] = -2;
    //STATE12
    actionTable[12][NB] = 17;
    actionTable[12][LEFT_PARENTHESIS] = -85;
    //STATE13
    actionTable[13][LEFT_PARENTHESIS] = -82;
    actionTable[13][COLON] = -82;
    actionTable[13][EQUALS] = -82;
    actionTable[13][COMMA] = -82;
    //STATE14
    actionTable[14][EQUALS] = 18;
    //STATE15
    actionTable[15][COLON] = 19;
    actionTable[15][COMMA] = 20;
    //STATE16
    actionTable[16][COLON] = -14;
    actionTable[16][COMMA] = -14;
    //STATE17
    actionTable[17][LEFT_PARENTHESIS] = 21;
    //STATE18
    actionTable[18][T] = 24;
    actionTable[18][STRUCT] = 22;
    actionTable[18][ARRAY] = 23;
    actionTable[18][IDU] = 29;
    actionTable[18][INTEGER] = 25;
    actionTable[18][CHAR] = 26;
    actionTable[18][BOOLEAN] = 27;
    actionTable[18][STRING] = 28;
    actionTable[18][IDT] = 30;
    //STATE19
    actionTable[19][T] = 31;
    actionTable[19][IDU] = 29;
    actionTable[19][INTEGER] = 25;
    actionTable[19][CHAR] = 26;
    actionTable[19][BOOLEAN] = 27;
    actionTable[19][STRING] = 28;
    actionTable[19][IDT] = 30;
    //STATE20
    actionTable[20][IDD] = 32;
    actionTable[20][IDT] = 13;
    //STATE21
    actionTable[21][IDD] = 34;
    actionTable[21][LP] = 33;
    actionTable[21][RIGHT_PARENTHESIS] = -18;
    actionTable[21][COMMA] = -18;
    actionTable[21][IDT] = 13;
    //STATE22
    actionTable[22][NB] = 35;
    actionTable[22][LEFT_BRACES] = -85;
    //STATE23
    actionTable[23][LEFT_SQUARE] = 36;
    //STATE24
    actionTable[24][SEMI_COLON] = 37;
    //STATE25
    actionTable[25][RIGHT_PARENTHESIS] = -72;
    actionTable[25][LEFT_BRACES] = -72;
    actionTable[25][RIGHT_BRACES] = -72;
    actionTable[25][SEMI_COLON] = -72;
    actionTable[25][COMMA] = -72;
    //STATE26
    actionTable[26][RIGHT_PARENTHESIS] = -73;
    actionTable[26][LEFT_BRACES] = -73;
    actionTable[26][RIGHT_BRACES] = -73;
    actionTable[26][SEMI_COLON] = -73;
    actionTable[26][COMMA] = -73;
    //STATE27
    actionTable[27][RIGHT_PARENTHESIS] = -74;
    actionTable[27][LEFT_BRACES] = -74;
    actionTable[27][RIGHT_BRACES] = -74;
    actionTable[27][SEMI_COLON] = -74;
    actionTable[27][COMMA] = -74;
    //STATE28
    actionTable[28][RIGHT_PARENTHESIS] = -75;
    actionTable[28][LEFT_BRACES] = -75;
    actionTable[28][RIGHT_BRACES] = -75;
    actionTable[28][SEMI_COLON] = -75;
    actionTable[28][COMMA] = -75;
    //STATE29
    actionTable[29][RIGHT_PARENTHESIS] = -76;
    actionTable[29][LEFT_BRACES] = -76;
    actionTable[29][RIGHT_BRACES] = -76;
    actionTable[29][SEMI_COLON] = -76;
    actionTable[29][COMMA] = -76;
    //STATE30
    actionTable[30][LEFT_PARENTHESIS] = -83;
    actionTable[30][RIGHT_PARENTHESIS] = -83;
    actionTable[30][EQUALS] = -83;
    actionTable[30][LEFT_BRACES] = -83;
    actionTable[30][RIGHT_BRACES] = -83;
    actionTable[30][SEMI_COLON] = -83;
    actionTable[30][LEFT_SQUARE] = -83;
    actionTable[30][RIGHT_SQUARE] = -83;
    actionTable[30][COMMA] = -83;
    actionTable[30][AND] = -83;
    actionTable[30][OR] = -83;
    actionTable[30][LESS_THAN] = -83;
    actionTable[30][GREATER_THAN] = -83;
    actionTable[30][LESS_OR_EQUAL] = -83;
    actionTable[30][GREATER_OR_EQUAL] = -83;
    actionTable[30][EQUAL_EQUAL] = -83;
    actionTable[30][NOT_EQUAL] = -83;
    actionTable[30][PLUS] = -83;
    actionTable[30][MINUS] = -83;
    actionTable[30][TIMES] = -83;
    actionTable[30][DIVIDE] = -83;
    actionTable[30][PLUS_PLUS] = -83;
    actionTable[30][MINUS_MINUS] = -83;
    actionTable[30][DOT] = -83;
    //STATE31
    actionTable[31][SEMI_COLON] = 38;
    //STATE32
    actionTable[32][COLON] = -13;
    actionTable[32][COMMA] = -13;
    //STATE33
    actionTable[33][RIGHT_PARENTHESIS] = 39;
    actionTable[33][COMMA] = 40;
    //STATE34
    actionTable[34][COLON] = 41;
    //STATE35
    actionTable[35][LEFT_BRACES] = 42;
    //STATE36
    actionTable[36][NUM] = 43;
    actionTable[36][NUMERAL] = 44;
    //STATE37
    actionTable[37][ENDFILE] = -10;
    actionTable[37][FUNCTION] = -10;
    actionTable[37][TYPE] = -10;
    actionTable[37][VAR] = -10;
    //STATE38
    actionTable[38][ENDFILE] = -15;
    actionTable[38][FUNCTION] = -15;
    actionTable[38][LEFT_PARENTHESIS] = -15;
    actionTable[38][TYPE] = -15;
    actionTable[38][LEFT_BRACES] = -15;
    actionTable[38][RIGHT_BRACES] = -15;
    actionTable[38][VAR] = -15;
    actionTable[38][WHILE] = -15;
    actionTable[38][DO] = -15;
    actionTable[38][IF] = -15;
    actionTable[38][BREAK] = -15;
    actionTable[38][CONTINUE] = -15;
    actionTable[38][MINUS] = -15;
    actionTable[38][PLUS_PLUS] = -15;
    actionTable[38][MINUS_MINUS] = -15;
    actionTable[38][NOT] = -15;
    actionTable[38][TRUE] = -15;
    actionTable[38][FALSE] = -15;
    actionTable[38][CHARACTER] = -15;
    actionTable[38][STRINGVAL] = -15;
    actionTable[38][NUMERAL] = -15;
    actionTable[38][IDT] = -15;
    //STATE39
    actionTable[39][COLON] = 45;
    //STATE40
    actionTable[40][IDD] = 46;
    actionTable[40][IDT] = 13;
    //STATE41
    actionTable[41][T] = 47;
    actionTable[41][IDU] = 29;
    actionTable[41][INTEGER] = 25;
    actionTable[41][CHAR] = 26;
    actionTable[41][BOOLEAN] = 27;
    actionTable[41][STRING] = 28;
    actionTable[41][IDT] = 30;
    //STATE42
    actionTable[42][IDD] = 16;
    actionTable[42][DC] = 48;
    actionTable[42][LI] = 49;
    actionTable[42][IDT] = 13;
    //STATE43
    actionTable[43][RIGHT_SQUARE] = 50;
    //STATE44
    actionTable[44][RIGHT_PARENTHESIS] = -81;
    actionTable[44][SEMI_COLON] = -81;
    actionTable[44][RIGHT_SQUARE] = -81;
    actionTable[44][COMMA] = -81;
    actionTable[44][AND] = -81;
    actionTable[44][OR] = -81;
    actionTable[44][LESS_THAN] = -81;
    actionTable[44][GREATER_THAN] = -81;
    actionTable[44][LESS_OR_EQUAL] = -81;
    actionTable[44][GREATER_OR_EQUAL] = -81;
    actionTable[44][EQUAL_EQUAL] = -81;
    actionTable[44][NOT_EQUAL] = -81;
    actionTable[44][PLUS] = -81;
    actionTable[44][MINUS] = -81;
    actionTable[44][TIMES] = -81;
    actionTable[44][DIVIDE] = -81;
    //STATE45
    actionTable[45][T] = 51;
    actionTable[45][IDU] = 29;
    actionTable[45][INTEGER] = 25;
    actionTable[45][CHAR] = 26;
    actionTable[45][BOOLEAN] = 27;
    actionTable[45][STRING] = 28;
    actionTable[45][IDT] = 30;
    //STATE46
    actionTable[46][COLON] = 52;
    //STATE47
    actionTable[47][RIGHT_PARENTHESIS] = -17;
    actionTable[47][COMMA] = -17;
    //STATE48
    actionTable[48][RIGHT_BRACES] = 53;
    actionTable[48][SEMI_COLON] = 54;
    //STATE49
    actionTable[49][COLON] = 55;
    actionTable[49][COMMA] = 20;
    //STATE50
    actionTable[50][OF] = 56;
    //STATE51
    actionTable[51][B] = 57;
    actionTable[51][LEFT_BRACES] = 58;
    //STATE52
    actionTable[52][T] = 59;
    actionTable[52][IDU] = 29;
    actionTable[52][INTEGER] = 25;
    actionTable[52][CHAR] = 26;
    actionTable[52][BOOLEAN] = 27;
    actionTable[52][STRING] = 28;
    actionTable[52][IDT] = 30;
    //STATE53
    actionTable[53][SEMI_COLON] = 60;
    //STATE54
    actionTable[54][IDD] = 16;
    actionTable[54][LI] = 61;
    actionTable[54][IDT] = 13;
    //STATE55
    actionTable[55][T] = 62;
    actionTable[55][IDU] = 29;
    actionTable[55][INTEGER] = 25;
    actionTable[55][CHAR] = 26;
    actionTable[55][BOOLEAN] = 27;
    actionTable[55][STRING] = 28;
    actionTable[55][IDT] = 30;
    //STATE56
    actionTable[56][T] = 63;
    actionTable[56][IDU] = 29;
    actionTable[56][INTEGER] = 25;
    actionTable[56][CHAR] = 26;
    actionTable[56][BOOLEAN] = 27;
    actionTable[56][STRING] = 28;
    actionTable[56][IDT] = 30;
    //STATE57
    actionTable[57][ENDFILE] = -7;
    actionTable[57][FUNCTION] = -7;
    actionTable[57][TYPE] = -7;
    actionTable[57][VAR] = -7;
    //STATE58
    actionTable[58][DV] = 67;
    actionTable[58][LEFT_PARENTHESIS] = 84;
    actionTable[58][B] = 70;
    actionTable[58][LEFT_BRACES] = 58;
    actionTable[58][RIGHT_BRACES] = 66;
    actionTable[58][NUM] = 91;
    actionTable[58][VAR] = 9;
    actionTable[58][LDV] = 64;
    actionTable[58][LS] = 65;
    actionTable[58][S] = 68;
    actionTable[58][E] = 69;
    actionTable[58][WHILE] = 71;
    actionTable[58][DO] = 72;
    actionTable[58][IF] = 73;
    actionTable[58][BREAK] = 74;
    actionTable[58][CONTINUE] = 75;
    actionTable[58][LV] = 76;
    actionTable[58][L] = 77;
    actionTable[58][R] = 79;
    actionTable[58][K] = 80;
    actionTable[58][MINUS] = 85;
    actionTable[58][F] = 81;
    actionTable[58][PLUS_PLUS] = 82;
    actionTable[58][MINUS_MINUS] = 83;
    actionTable[58][IDU] = 78;
    actionTable[58][NOT] = 86;
    actionTable[58][TRU] = 87;
    actionTable[58][FALS] = 88;
    actionTable[58][CHR] = 89;
    actionTable[58][STR] = 90;
    actionTable[58][TRUE] = 92;
    actionTable[58][FALSE] = 93;
    actionTable[58][CHARACTER] = 94;
    actionTable[58][STRINGVAL] = 95;
    actionTable[58][NUMERAL] = 44;
    actionTable[58][IDT] = 30;
    //STATE59
    actionTable[59][RIGHT_PARENTHESIS] = -16;
    actionTable[59][COMMA] = -16;
    //STATE60
    actionTable[60][ENDFILE] = -8;
    actionTable[60][FUNCTION] = -8;
    actionTable[60][TYPE] = -8;
    actionTable[60][VAR] = -8;
    //STATE61
    actionTable[61][COLON] = 96;
    actionTable[61][COMMA] = 20;
    //STATE62
    actionTable[62][RIGHT_BRACES] = -12;
    actionTable[62][SEMI_COLON] = -12;
    //STATE63
    actionTable[63][SEMI_COLON] = 97;
    //STATE64
    actionTable[64][DV] = 100;
    actionTable[64][LEFT_PARENTHESIS] = 84;
    actionTable[64][B] = 70;
    actionTable[64][LEFT_BRACES] = 58;
    actionTable[64][RIGHT_BRACES] = 99;
    actionTable[64][NUM] = 91;
    actionTable[64][VAR] = 9;
    actionTable[64][LS] = 98;
    actionTable[64][S] = 68;
    actionTable[64][E] = 69;
    actionTable[64][WHILE] = 71;
    actionTable[64][DO] = 72;
    actionTable[64][IF] = 73;
    actionTable[64][BREAK] = 74;
    actionTable[64][CONTINUE] = 75;
    actionTable[64][LV] = 76;
    actionTable[64][L] = 77;
    actionTable[64][R] = 79;
    actionTable[64][K] = 80;
    actionTable[64][MINUS] = 85;
    actionTable[64][F] = 81;
    actionTable[64][PLUS_PLUS] = 82;
    actionTable[64][MINUS_MINUS] = 83;
    actionTable[64][IDU] = 78;
    actionTable[64][NOT] = 86;
    actionTable[64][TRU] = 87;
    actionTable[64][FALS] = 88;
    actionTable[64][CHR] = 89;
    actionTable[64][STR] = 90;
    actionTable[64][TRUE] = 92;
    actionTable[64][FALSE] = 93;
    actionTable[64][CHARACTER] = 94;
    actionTable[64][STRINGVAL] = 95;
    actionTable[64][NUMERAL] = 44;
    actionTable[64][IDT] = 30;
    //STATE65
    actionTable[65][LEFT_PARENTHESIS] = 84;
    actionTable[65][B] = 70;
    actionTable[65][LEFT_BRACES] = 58;
    actionTable[65][RIGHT_BRACES] = 101;
    actionTable[65][NUM] = 91;
    actionTable[65][S] = 102;
    actionTable[65][E] = 69;
    actionTable[65][WHILE] = 71;
    actionTable[65][DO] = 72;
    actionTable[65][IF] = 73;
    actionTable[65][BREAK] = 74;
    actionTable[65][CONTINUE] = 75;
    actionTable[65][LV] = 76;
    actionTable[65][L] = 77;
    actionTable[65][R] = 79;
    actionTable[65][K] = 80;
    actionTable[65][MINUS] = 85;
    actionTable[65][F] = 81;
    actionTable[65][PLUS_PLUS] = 82;
    actionTable[65][MINUS_MINUS] = 83;
    actionTable[65][IDU] = 78;
    actionTable[65][NOT] = 86;
    actionTable[65][TRU] = 87;
    actionTable[65][FALS] = 88;
    actionTable[65][CHR] = 89;
    actionTable[65][STR] = 90;
    actionTable[65][TRUE] = 92;
    actionTable[65][FALSE] = 93;
    actionTable[65][CHARACTER] = 94;
    actionTable[65][STRINGVAL] = 95;
    actionTable[65][NUMERAL] = 44;
    actionTable[65][IDT] = 30;
    //STATE66
    actionTable[66][ENDFILE] = -22;
    actionTable[66][FUNCTION] = -22;
    actionTable[66][LEFT_PARENTHESIS] = -22;
    actionTable[66][TYPE] = -22;
    actionTable[66][LEFT_BRACES] = -22;
    actionTable[66][RIGHT_BRACES] = -22;
    actionTable[66][VAR] = -22;
    actionTable[66][WHILE] = -22;
    actionTable[66][DO] = -22;
    actionTable[66][IF] = -22;
    actionTable[66][ELSE] = -22;
    actionTable[66][BREAK] = -22;
    actionTable[66][CONTINUE] = -22;
    actionTable[66][MINUS] = -22;
    actionTable[66][PLUS_PLUS] = -22;
    actionTable[66][MINUS_MINUS] = -22;
    actionTable[66][NOT] = -22;
    actionTable[66][TRUE] = -22;
    actionTable[66][FALSE] = -22;
    actionTable[66][CHARACTER] = -22;
    actionTable[66][STRINGVAL] = -22;
    actionTable[66][NUMERAL] = -22;
    actionTable[66][IDT] = -22;
    //STATE67
    actionTable[67][LEFT_PARENTHESIS] = -24;
    actionTable[67][LEFT_BRACES] = -24;
    actionTable[67][RIGHT_BRACES] = -24;
    actionTable[67][VAR] = -24;
    actionTable[67][WHILE] = -24;
    actionTable[67][DO] = -24;
    actionTable[67][IF] = -24;
    actionTable[67][BREAK] = -24;
    actionTable[67][CONTINUE] = -24;
    actionTable[67][MINUS] = -24;
    actionTable[67][PLUS_PLUS] = -24;
    actionTable[67][MINUS_MINUS] = -24;
    actionTable[67][NOT] = -24;
    actionTable[67][TRUE] = -24;
    actionTable[67][FALSE] = -24;
    actionTable[67][CHARACTER] = -24;
    actionTable[67][STRINGVAL] = -24;
    actionTable[67][NUMERAL] = -24;
    actionTable[67][IDT] = -24;
    //STATE68
    actionTable[68][LEFT_PARENTHESIS] = -26;
    actionTable[68][LEFT_BRACES] = -26;
    actionTable[68][RIGHT_BRACES] = -26;
    actionTable[68][WHILE] = -26;
    actionTable[68][DO] = -26;
    actionTable[68][IF] = -26;
    actionTable[68][BREAK] = -26;
    actionTable[68][CONTINUE] = -26;
    actionTable[68][MINUS] = -26;
    actionTable[68][PLUS_PLUS] = -26;
    actionTable[68][MINUS_MINUS] = -26;
    actionTable[68][NOT] = -26;
    actionTable[68][TRUE] = -26;
    actionTable[68][FALSE] = -26;
    actionTable[68][CHARACTER] = -26;
    actionTable[68][STRINGVAL] = -26;
    actionTable[68][NUMERAL] = -26;
    actionTable[68][IDT] = -26;
    //STATE69
    actionTable[69][SEMI_COLON] = 103;
    actionTable[69][AND] = 104;
    actionTable[69][OR] = 105;
    //STATE70
    actionTable[70][LEFT_PARENTHESIS] = -28;
    actionTable[70][LEFT_BRACES] = -28;
    actionTable[70][RIGHT_BRACES] = -28;
    actionTable[70][WHILE] = -28;
    actionTable[70][DO] = -28;
    actionTable[70][IF] = -28;
    actionTable[70][ELSE] = -28;
    actionTable[70][BREAK] = -28;
    actionTable[70][CONTINUE] = -28;
    actionTable[70][MINUS] = -28;
    actionTable[70][PLUS_PLUS] = -28;
    actionTable[70][MINUS_MINUS] = -28;
    actionTable[70][NOT] = -28;
    actionTable[70][TRUE] = -28;
    actionTable[70][FALSE] = -28;
    actionTable[70][CHARACTER] = -28;
    actionTable[70][STRINGVAL] = -28;
    actionTable[70][NUMERAL] = -28;
    actionTable[70][IDT] = -28;
    //STATE71
    actionTable[71][LEFT_PARENTHESIS] = 106;
    //STATE72
    actionTable[72][LEFT_PARENTHESIS] = 84;
    actionTable[72][B] = 70;
    actionTable[72][LEFT_BRACES] = 58;
    actionTable[72][NUM] = 91;
    actionTable[72][S] = 107;
    actionTable[72][E] = 69;
    actionTable[72][WHILE] = 71;
    actionTable[72][DO] = 72;
    actionTable[72][IF] = 73;
    actionTable[72][BREAK] = 74;
    actionTable[72][CONTINUE] = 75;
    actionTable[72][LV] = 76;
    actionTable[72][L] = 77;
    actionTable[72][R] = 79;
    actionTable[72][K] = 80;
    actionTable[72][MINUS] = 85;
    actionTable[72][F] = 81;
    actionTable[72][PLUS_PLUS] = 82;
    actionTable[72][MINUS_MINUS] = 83;
    actionTable[72][IDU] = 78;
    actionTable[72][NOT] = 86;
    actionTable[72][TRU] = 87;
    actionTable[72][FALS] = 88;
    actionTable[72][CHR] = 89;
    actionTable[72][STR] = 90;
    actionTable[72][TRUE] = 92;
    actionTable[72][FALSE] = 93;
    actionTable[72][CHARACTER] = 94;
    actionTable[72][STRINGVAL] = 95;
    actionTable[72][NUMERAL] = 44;
    actionTable[72][IDT] = 30;
    //STATE73
    actionTable[73][LEFT_PARENTHESIS] = 108;
    //STATE74
    actionTable[74][SEMI_COLON] = 109;
    //STATE75
    actionTable[75][SEMI_COLON] = 110;
    //STATE76
    actionTable[76][EQUALS] = 111;
    actionTable[76][SEMI_COLON] = -52;
    actionTable[76][LEFT_SQUARE] = 113;
    actionTable[76][AND] = -52;
    actionTable[76][OR] = -52;
    actionTable[76][LESS_THAN] = -52;
    actionTable[76][GREATER_THAN] = -52;
    actionTable[76][LESS_OR_EQUAL] = -52;
    actionTable[76][GREATER_OR_EQUAL] = -52;
    actionTable[76][EQUAL_EQUAL] = -52;
    actionTable[76][NOT_EQUAL] = -52;
    actionTable[76][PLUS] = -52;
    actionTable[76][MINUS] = -52;
    actionTable[76][TIMES] = -52;
    actionTable[76][DIVIDE] = -52;
    actionTable[76][PLUS_PLUS] = 114;
    actionTable[76][MINUS_MINUS] = 115;
    actionTable[76][DOT] = 112;
    //STATE77
    actionTable[77][RIGHT_PARENTHESIS] = -38;
    actionTable[77][SEMI_COLON] = -38;
    actionTable[77][RIGHT_SQUARE] = -38;
    actionTable[77][COMMA] = -38;
    actionTable[77][AND] = -38;
    actionTable[77][OR] = -38;
    actionTable[77][LESS_THAN] = 116;
    actionTable[77][GREATER_THAN] = 117;
    actionTable[77][LESS_OR_EQUAL] = 118;
    actionTable[77][GREATER_OR_EQUAL] = 119;
    actionTable[77][EQUAL_EQUAL] = 120;
    actionTable[77][NOT_EQUAL] = 121;
    //STATE78
    actionTable[78][LEFT_PARENTHESIS] = 122;
    actionTable[78][RIGHT_PARENTHESIS] = -71;
    actionTable[78][EQUALS] = -71;
    actionTable[78][SEMI_COLON] = -71;
    actionTable[78][LEFT_SQUARE] = -71;
    actionTable[78][RIGHT_SQUARE] = -71;
    actionTable[78][COMMA] = -71;
    actionTable[78][AND] = -71;
    actionTable[78][OR] = -71;
    actionTable[78][LESS_THAN] = -71;
    actionTable[78][GREATER_THAN] = -71;
    actionTable[78][LESS_OR_EQUAL] = -71;
    actionTable[78][GREATER_OR_EQUAL] = -71;
    actionTable[78][EQUAL_EQUAL] = -71;
    actionTable[78][NOT_EQUAL] = -71;
    actionTable[78][PLUS] = -71;
    actionTable[78][MINUS] = -71;
    actionTable[78][TIMES] = -71;
    actionTable[78][DIVIDE] = -71;
    actionTable[78][PLUS_PLUS] = -71;
    actionTable[78][MINUS_MINUS] = -71;
    actionTable[78][DOT] = -71;
    //STATE79
    actionTable[79][RIGHT_PARENTHESIS] = -45;
    actionTable[79][SEMI_COLON] = -45;
    actionTable[79][RIGHT_SQUARE] = -45;
    actionTable[79][COMMA] = -45;
    actionTable[79][AND] = -45;
    actionTable[79][OR] = -45;
    actionTable[79][LESS_THAN] = -45;
    actionTable[79][GREATER_THAN] = -45;
    actionTable[79][LESS_OR_EQUAL] = -45;
    actionTable[79][GREATER_OR_EQUAL] = -45;
    actionTable[79][EQUAL_EQUAL] = -45;
    actionTable[79][NOT_EQUAL] = -45;
    actionTable[79][PLUS] = 123;
    actionTable[79][MINUS] = 124;
    //STATE80
    actionTable[80][RIGHT_PARENTHESIS] = -48;
    actionTable[80][SEMI_COLON] = -48;
    actionTable[80][RIGHT_SQUARE] = -48;
    actionTable[80][COMMA] = -48;
    actionTable[80][AND] = -48;
    actionTable[80][OR] = -48;
    actionTable[80][LESS_THAN] = -48;
    actionTable[80][GREATER_THAN] = -48;
    actionTable[80][LESS_OR_EQUAL] = -48;
    actionTable[80][GREATER_OR_EQUAL] = -48;
    actionTable[80][EQUAL_EQUAL] = -48;
    actionTable[80][NOT_EQUAL] = -48;
    actionTable[80][PLUS] = -48;
    actionTable[80][MINUS] = -48;
    actionTable[80][TIMES] = 125;
    actionTable[80][DIVIDE] = 126;
    //STATE81
    actionTable[81][RIGHT_PARENTHESIS] = -51;
    actionTable[81][SEMI_COLON] = -51;
    actionTable[81][RIGHT_SQUARE] = -51;
    actionTable[81][COMMA] = -51;
    actionTable[81][AND] = -51;
    actionTable[81][OR] = -51;
    actionTable[81][LESS_THAN] = -51;
    actionTable[81][GREATER_THAN] = -51;
    actionTable[81][LESS_OR_EQUAL] = -51;
    actionTable[81][GREATER_OR_EQUAL] = -51;
    actionTable[81][EQUAL_EQUAL] = -51;
    actionTable[81][NOT_EQUAL] = -51;
    actionTable[81][PLUS] = -51;
    actionTable[81][MINUS] = -51;
    actionTable[81][TIMES] = -51;
    actionTable[81][DIVIDE] = -51;
    //STATE82
    actionTable[82][LV] = 127;
    actionTable[82][IDU] = 128;
    actionTable[82][IDT] = 30;
    //STATE83
    actionTable[83][LV] = 129;
    actionTable[83][IDU] = 128;
    actionTable[83][IDT] = 30;
    //STATE84
    actionTable[84][LEFT_PARENTHESIS] = 84;
    actionTable[84][NUM] = 91;
    actionTable[84][E] = 130;
    actionTable[84][LV] = 131;
    actionTable[84][L] = 77;
    actionTable[84][R] = 79;
    actionTable[84][K] = 80;
    actionTable[84][MINUS] = 85;
    actionTable[84][F] = 81;
    actionTable[84][PLUS_PLUS] = 82;
    actionTable[84][MINUS_MINUS] = 83;
    actionTable[84][IDU] = 78;
    actionTable[84][NOT] = 86;
    actionTable[84][TRU] = 87;
    actionTable[84][FALS] = 88;
    actionTable[84][CHR] = 89;
    actionTable[84][STR] = 90;
    actionTable[84][TRUE] = 92;
    actionTable[84][FALSE] = 93;
    actionTable[84][CHARACTER] = 94;
    actionTable[84][STRINGVAL] = 95;
    actionTable[84][NUMERAL] = 44;
    actionTable[84][IDT] = 30;
    //STATE85
    actionTable[85][LEFT_PARENTHESIS] = 84;
    actionTable[85][NUM] = 91;
    actionTable[85][LV] = 131;
    actionTable[85][MINUS] = 85;
    actionTable[85][F] = 132;
    actionTable[85][PLUS_PLUS] = 82;
    actionTable[85][MINUS_MINUS] = 83;
    actionTable[85][IDU] = 78;
    actionTable[85][NOT] = 86;
    actionTable[85][TRU] = 87;
    actionTable[85][FALS] = 88;
    actionTable[85][CHR] = 89;
    actionTable[85][STR] = 90;
    actionTable[85][TRUE] = 92;
    actionTable[85][FALSE] = 93;
    actionTable[85][CHARACTER] = 94;
    actionTable[85][STRINGVAL] = 95;
    actionTable[85][NUMERAL] = 44;
    actionTable[85][IDT] = 30;
    //STATE86
    actionTable[86][LEFT_PARENTHESIS] = 84;
    actionTable[86][NUM] = 91;
    actionTable[86][LV] = 131;
    actionTable[86][MINUS] = 85;
    actionTable[86][F] = 133;
    actionTable[86][PLUS_PLUS] = 82;
    actionTable[86][MINUS_MINUS] = 83;
    actionTable[86][IDU] = 78;
    actionTable[86][NOT] = 86;
    actionTable[86][TRU] = 87;
    actionTable[86][FALS] = 88;
    actionTable[86][CHR] = 89;
    actionTable[86][STR] = 90;
    actionTable[86][TRUE] = 92;
    actionTable[86][FALSE] = 93;
    actionTable[86][CHARACTER] = 94;
    actionTable[86][STRINGVAL] = 95;
    actionTable[86][NUMERAL] = 44;
    actionTable[86][IDT] = 30;
    //STATE87
    actionTable[87][RIGHT_PARENTHESIS] = -61;
    actionTable[87][SEMI_COLON] = -61;
    actionTable[87][RIGHT_SQUARE] = -61;
    actionTable[87][COMMA] = -61;
    actionTable[87][AND] = -61;
    actionTable[87][OR] = -61;
    actionTable[87][LESS_THAN] = -61;
    actionTable[87][GREATER_THAN] = -61;
    actionTable[87][LESS_OR_EQUAL] = -61;
    actionTable[87][GREATER_OR_EQUAL] = -61;
    actionTable[87][EQUAL_EQUAL] = -61;
    actionTable[87][NOT_EQUAL] = -61;
    actionTable[87][PLUS] = -61;
    actionTable[87][MINUS] = -61;
    actionTable[87][TIMES] = -61;
    actionTable[87][DIVIDE] = -61;
    //STATE88
    actionTable[88][RIGHT_PARENTHESIS] = -62;
    actionTable[88][SEMI_COLON] = -62;
    actionTable[88][RIGHT_SQUARE] = -62;
    actionTable[88][COMMA] = -62;
    actionTable[88][AND] = -62;
    actionTable[88][OR] = -62;
    actionTable[88][LESS_THAN] = -62;
    actionTable[88][GREATER_THAN] = -62;
    actionTable[88][LESS_OR_EQUAL] = -62;
    actionTable[88][GREATER_OR_EQUAL] = -62;
    actionTable[88][EQUAL_EQUAL] = -62;
    actionTable[88][NOT_EQUAL] = -62;
    actionTable[88][PLUS] = -62;
    actionTable[88][MINUS] = -62;
    actionTable[88][TIMES] = -62;
    actionTable[88][DIVIDE] = -62;
    //STATE89
    actionTable[89][RIGHT_PARENTHESIS] = -63;
    actionTable[89][SEMI_COLON] = -63;
    actionTable[89][RIGHT_SQUARE] = -63;
    actionTable[89][COMMA] = -63;
    actionTable[89][AND] = -63;
    actionTable[89][OR] = -63;
    actionTable[89][LESS_THAN] = -63;
    actionTable[89][GREATER_THAN] = -63;
    actionTable[89][LESS_OR_EQUAL] = -63;
    actionTable[89][GREATER_OR_EQUAL] = -63;
    actionTable[89][EQUAL_EQUAL] = -63;
    actionTable[89][NOT_EQUAL] = -63;
    actionTable[89][PLUS] = -63;
    actionTable[89][MINUS] = -63;
    actionTable[89][TIMES] = -63;
    actionTable[89][DIVIDE] = -63;
    //STATE90
    actionTable[90][RIGHT_PARENTHESIS] = -64;
    actionTable[90][SEMI_COLON] = -64;
    actionTable[90][RIGHT_SQUARE] = -64;
    actionTable[90][COMMA] = -64;
    actionTable[90][AND] = -64;
    actionTable[90][OR] = -64;
    actionTable[90][LESS_THAN] = -64;
    actionTable[90][GREATER_THAN] = -64;
    actionTable[90][LESS_OR_EQUAL] = -64;
    actionTable[90][GREATER_OR_EQUAL] = -64;
    actionTable[90][EQUAL_EQUAL] = -64;
    actionTable[90][NOT_EQUAL] = -64;
    actionTable[90][PLUS] = -64;
    actionTable[90][MINUS] = -64;
    actionTable[90][TIMES] = -64;
    actionTable[90][DIVIDE] = -64;
    //STATE91
    actionTable[91][RIGHT_PARENTHESIS] = -65;
    actionTable[91][SEMI_COLON] = -65;
    actionTable[91][RIGHT_SQUARE] = -65;
    actionTable[91][COMMA] = -65;
    actionTable[91][AND] = -65;
    actionTable[91][OR] = -65;
    actionTable[91][LESS_THAN] = -65;
    actionTable[91][GREATER_THAN] = -65;
    actionTable[91][LESS_OR_EQUAL] = -65;
    actionTable[91][GREATER_OR_EQUAL] = -65;
    actionTable[91][EQUAL_EQUAL] = -65;
    actionTable[91][NOT_EQUAL] = -65;
    actionTable[91][PLUS] = -65;
    actionTable[91][MINUS] = -65;
    actionTable[91][TIMES] = -65;
    actionTable[91][DIVIDE] = -65;
    //STATE92
    actionTable[92][RIGHT_PARENTHESIS] = -77;
    actionTable[92][SEMI_COLON] = -77;
    actionTable[92][RIGHT_SQUARE] = -77;
    actionTable[92][COMMA] = -77;
    actionTable[92][AND] = -77;
    actionTable[92][OR] = -77;
    actionTable[92][LESS_THAN] = -77;
    actionTable[92][GREATER_THAN] = -77;
    actionTable[92][LESS_OR_EQUAL] = -77;
    actionTable[92][GREATER_OR_EQUAL] = -77;
    actionTable[92][EQUAL_EQUAL] = -77;
    actionTable[92][NOT_EQUAL] = -77;
    actionTable[92][PLUS] = -77;
    actionTable[92][MINUS] = -77;
    actionTable[92][TIMES] = -77;
    actionTable[92][DIVIDE] = -77;
    //STATE93
    actionTable[93][RIGHT_PARENTHESIS] = -78;
    actionTable[93][SEMI_COLON] = -78;
    actionTable[93][RIGHT_SQUARE] = -78;
    actionTable[93][COMMA] = -78;
    actionTable[93][AND] = -78;
    actionTable[93][OR] = -78;
    actionTable[93][LESS_THAN] = -78;
    actionTable[93][GREATER_THAN] = -78;
    actionTable[93][LESS_OR_EQUAL] = -78;
    actionTable[93][GREATER_OR_EQUAL] = -78;
    actionTable[93][EQUAL_EQUAL] = -78;
    actionTable[93][NOT_EQUAL] = -78;
    actionTable[93][PLUS] = -78;
    actionTable[93][MINUS] = -78;
    actionTable[93][TIMES] = -78;
    actionTable[93][DIVIDE] = -78;
    //STATE94
    actionTable[94][RIGHT_PARENTHESIS] = -79;
    actionTable[94][SEMI_COLON] = -79;
    actionTable[94][RIGHT_SQUARE] = -79;
    actionTable[94][COMMA] = -79;
    actionTable[94][AND] = -79;
    actionTable[94][OR] = -79;
    actionTable[94][LESS_THAN] = -79;
    actionTable[94][GREATER_THAN] = -79;
    actionTable[94][LESS_OR_EQUAL] = -79;
    actionTable[94][GREATER_OR_EQUAL] = -79;
    actionTable[94][EQUAL_EQUAL] = -79;
    actionTable[94][NOT_EQUAL] = -79;
    actionTable[94][PLUS] = -79;
    actionTable[94][MINUS] = -79;
    actionTable[94][TIMES] = -79;
    actionTable[94][DIVIDE] = -79;
    //STATE95
    actionTable[95][RIGHT_PARENTHESIS] = -80;
    actionTable[95][SEMI_COLON] = -80;
    actionTable[95][RIGHT_SQUARE] = -80;
    actionTable[95][COMMA] = -80;
    actionTable[95][AND] = -80;
    actionTable[95][OR] = -80;
    actionTable[95][LESS_THAN] = -80;
    actionTable[95][GREATER_THAN] = -80;
    actionTable[95][LESS_OR_EQUAL] = -80;
    actionTable[95][GREATER_OR_EQUAL] = -80;
    actionTable[95][EQUAL_EQUAL] = -80;
    actionTable[95][NOT_EQUAL] = -80;
    actionTable[95][PLUS] = -80;
    actionTable[95][MINUS] = -80;
    actionTable[95][TIMES] = -80;
    actionTable[95][DIVIDE] = -80;
    //STATE96
    actionTable[96][T] = 134;
    actionTable[96][IDU] = 29;
    actionTable[96][INTEGER] = 25;
    actionTable[96][CHAR] = 26;
    actionTable[96][BOOLEAN] = 27;
    actionTable[96][STRING] = 28;
    actionTable[96][IDT] = 30;
    //STATE97
    actionTable[97][ENDFILE] = -9;
    actionTable[97][FUNCTION] = -9;
    actionTable[97][TYPE] = -9;
    actionTable[97][VAR] = -9;
    //STATE98
    actionTable[98][LEFT_PARENTHESIS] = 84;
    actionTable[98][B] = 70;
    actionTable[98][LEFT_BRACES] = 58;
    actionTable[98][RIGHT_BRACES] = 135;
    actionTable[98][NUM] = 91;
    actionTable[98][S] = 102;
    actionTable[98][E] = 69;
    actionTable[98][WHILE] = 71;
    actionTable[98][DO] = 72;
    actionTable[98][IF] = 73;
    actionTable[98][BREAK] = 74;
    actionTable[98][CONTINUE] = 75;
    actionTable[98][LV] = 76;
    actionTable[98][L] = 77;
    actionTable[98][R] = 79;
    actionTable[98][K] = 80;
    actionTable[98][MINUS] = 85;
    actionTable[98][F] = 81;
    actionTable[98][PLUS_PLUS] = 82;
    actionTable[98][MINUS_MINUS] = 83;
    actionTable[98][IDU] = 78;
    actionTable[98][NOT] = 86;
    actionTable[98][TRU] = 87;
    actionTable[98][FALS] = 88;
    actionTable[98][CHR] = 89;
    actionTable[98][STR] = 90;
    actionTable[98][TRUE] = 92;
    actionTable[98][FALSE] = 93;
    actionTable[98][CHARACTER] = 94;
    actionTable[98][STRINGVAL] = 95;
    actionTable[98][NUMERAL] = 44;
    actionTable[98][IDT] = 30;
    //STATE99
    actionTable[99][ENDFILE] = -20;
    actionTable[99][FUNCTION] = -20;
    actionTable[99][LEFT_PARENTHESIS] = -20;
    actionTable[99][TYPE] = -20;
    actionTable[99][LEFT_BRACES] = -20;
    actionTable[99][RIGHT_BRACES] = -20;
    actionTable[99][VAR] = -20;
    actionTable[99][WHILE] = -20;
    actionTable[99][DO] = -20;
    actionTable[99][IF] = -20;
    actionTable[99][ELSE] = -20;
    actionTable[99][BREAK] = -20;
    actionTable[99][CONTINUE] = -20;
    actionTable[99][MINUS] = -20;
    actionTable[99][PLUS_PLUS] = -20;
    actionTable[99][MINUS_MINUS] = -20;
    actionTable[99][NOT] = -20;
    actionTable[99][TRUE] = -20;
    actionTable[99][FALSE] = -20;
    actionTable[99][CHARACTER] = -20;
    actionTable[99][STRINGVAL] = -20;
    actionTable[99][NUMERAL] = -20;
    actionTable[99][IDT] = -20;
    //STATE100
    actionTable[100][LEFT_PARENTHESIS] = -23;
    actionTable[100][LEFT_BRACES] = -23;
    actionTable[100][RIGHT_BRACES] = -23;
    actionTable[100][VAR] = -23;
    actionTable[100][WHILE] = -23;
    actionTable[100][DO] = -23;
    actionTable[100][IF] = -23;
    actionTable[100][BREAK] = -23;
    actionTable[100][CONTINUE] = -23;
    actionTable[100][MINUS] = -23;
    actionTable[100][PLUS_PLUS] = -23;
    actionTable[100][MINUS_MINUS] = -23;
    actionTable[100][NOT] = -23;
    actionTable[100][TRUE] = -23;
    actionTable[100][FALSE] = -23;
    actionTable[100][CHARACTER] = -23;
    actionTable[100][STRINGVAL] = -23;
    actionTable[100][NUMERAL] = -23;
    actionTable[100][IDT] = -23;
    //STATE101
    actionTable[101][ENDFILE] = -21;
    actionTable[101][FUNCTION] = -21;
    actionTable[101][LEFT_PARENTHESIS] = -21;
    actionTable[101][TYPE] = -21;
    actionTable[101][LEFT_BRACES] = -21;
    actionTable[101][RIGHT_BRACES] = -21;
    actionTable[101][VAR] = -21;
    actionTable[101][WHILE] = -21;
    actionTable[101][DO] = -21;
    actionTable[101][IF] = -21;
    actionTable[101][ELSE] = -21;
    actionTable[101][BREAK] = -21;
    actionTable[101][CONTINUE] = -21;
    actionTable[101][MINUS] = -21;
    actionTable[101][PLUS_PLUS] = -21;
    actionTable[101][MINUS_MINUS] = -21;
    actionTable[101][NOT] = -21;
    actionTable[101][TRUE] = -21;
    actionTable[101][FALSE] = -21;
    actionTable[101][CHARACTER] = -21;
    actionTable[101][STRINGVAL] = -21;
    actionTable[101][NUMERAL] = -21;
    actionTable[101][IDT] = -21;
    //STATE102
    actionTable[102][LEFT_PARENTHESIS] = -25;
    actionTable[102][LEFT_BRACES] = -25;
    actionTable[102][RIGHT_BRACES] = -25;
    actionTable[102][WHILE] = -25;
    actionTable[102][DO] = -25;
    actionTable[102][IF] = -25;
    actionTable[102][BREAK] = -25;
    actionTable[102][CONTINUE] = -25;
    actionTable[102][MINUS] = -25;
    actionTable[102][PLUS_PLUS] = -25;
    actionTable[102][MINUS_MINUS] = -25;
    actionTable[102][NOT] = -25;
    actionTable[102][TRUE] = -25;
    actionTable[102][FALSE] = -25;
    actionTable[102][CHARACTER] = -25;
    actionTable[102][STRINGVAL] = -25;
    actionTable[102][NUMERAL] = -25;
    actionTable[102][IDT] = -25;
    //STATE103
    actionTable[103][LEFT_PARENTHESIS] = -27;
    actionTable[103][LEFT_BRACES] = -27;
    actionTable[103][RIGHT_BRACES] = -27;
    actionTable[103][WHILE] = -27;
    actionTable[103][DO] = -27;
    actionTable[103][IF] = -27;
    actionTable[103][ELSE] = -27;
    actionTable[103][BREAK] = -27;
    actionTable[103][CONTINUE] = -27;
    actionTable[103][MINUS] = -27;
    actionTable[103][PLUS_PLUS] = -27;
    actionTable[103][MINUS_MINUS] = -27;
    actionTable[103][NOT] = -27;
    actionTable[103][TRUE] = -27;
    actionTable[103][FALSE] = -27;
    actionTable[103][CHARACTER] = -27;
    actionTable[103][STRINGVAL] = -27;
    actionTable[103][NUMERAL] = -27;
    actionTable[103][IDT] = -27;
    //STATE104
    actionTable[104][LEFT_PARENTHESIS] = 84;
    actionTable[104][NUM] = 91;
    actionTable[104][LV] = 131;
    actionTable[104][L] = 136;
    actionTable[104][R] = 79;
    actionTable[104][K] = 80;
    actionTable[104][MINUS] = 85;
    actionTable[104][F] = 81;
    actionTable[104][PLUS_PLUS] = 82;
    actionTable[104][MINUS_MINUS] = 83;
    actionTable[104][IDU] = 78;
    actionTable[104][NOT] = 86;
    actionTable[104][TRU] = 87;
    actionTable[104][FALS] = 88;
    actionTable[104][CHR] = 89;
    actionTable[104][STR] = 90;
    actionTable[104][TRUE] = 92;
    actionTable[104][FALSE] = 93;
    actionTable[104][CHARACTER] = 94;
    actionTable[104][STRINGVAL] = 95;
    actionTable[104][NUMERAL] = 44;
    actionTable[104][IDT] = 30;
    //STATE105
    actionTable[105][LEFT_PARENTHESIS] = 84;
    actionTable[105][NUM] = 91;
    actionTable[105][LV] = 131;
    actionTable[105][L] = 137;
    actionTable[105][R] = 79;
    actionTable[105][K] = 80;
    actionTable[105][MINUS] = 85;
    actionTable[105][F] = 81;
    actionTable[105][PLUS_PLUS] = 82;
    actionTable[105][MINUS_MINUS] = 83;
    actionTable[105][IDU] = 78;
    actionTable[105][NOT] = 86;
    actionTable[105][TRU] = 87;
    actionTable[105][FALS] = 88;
    actionTable[105][CHR] = 89;
    actionTable[105][STR] = 90;
    actionTable[105][TRUE] = 92;
    actionTable[105][FALSE] = 93;
    actionTable[105][CHARACTER] = 94;
    actionTable[105][STRINGVAL] = 95;
    actionTable[105][NUMERAL] = 44;
    actionTable[105][IDT] = 30;
    //STATE106
    actionTable[106][LEFT_PARENTHESIS] = 84;
    actionTable[106][NUM] = 91;
    actionTable[106][E] = 138;
    actionTable[106][LV] = 131;
    actionTable[106][L] = 77;
    actionTable[106][R] = 79;
    actionTable[106][K] = 80;
    actionTable[106][MINUS] = 85;
    actionTable[106][F] = 81;
    actionTable[106][PLUS_PLUS] = 82;
    actionTable[106][MINUS_MINUS] = 83;
    actionTable[106][IDU] = 78;
    actionTable[106][NOT] = 86;
    actionTable[106][TRU] = 87;
    actionTable[106][FALS] = 88;
    actionTable[106][CHR] = 89;
    actionTable[106][STR] = 90;
    actionTable[106][TRUE] = 92;
    actionTable[106][FALSE] = 93;
    actionTable[106][CHARACTER] = 94;
    actionTable[106][STRINGVAL] = 95;
    actionTable[106][NUMERAL] = 44;
    actionTable[106][IDT] = 30;
    //STATE107
    actionTable[107][WHILE] = 139;
    //STATE108
    actionTable[108][LEFT_PARENTHESIS] = 84;
    actionTable[108][NUM] = 91;
    actionTable[108][E] = 140;
    actionTable[108][LV] = 131;
    actionTable[108][L] = 77;
    actionTable[108][R] = 79;
    actionTable[108][K] = 80;
    actionTable[108][MINUS] = 85;
    actionTable[108][F] = 81;
    actionTable[108][PLUS_PLUS] = 82;
    actionTable[108][MINUS_MINUS] = 83;
    actionTable[108][IDU] = 78;
    actionTable[108][NOT] = 86;
    actionTable[108][TRU] = 87;
    actionTable[108][FALS] = 88;
    actionTable[108][CHR] = 89;
    actionTable[108][STR] = 90;
    actionTable[108][TRUE] = 92;
    actionTable[108][FALSE] = 93;
    actionTable[108][CHARACTER] = 94;
    actionTable[108][STRINGVAL] = 95;
    actionTable[108][NUMERAL] = 44;
    actionTable[108][IDT] = 30;
    //STATE109
    actionTable[109][LEFT_PARENTHESIS] = -33;
    actionTable[109][LEFT_BRACES] = -33;
    actionTable[109][RIGHT_BRACES] = -33;
    actionTable[109][WHILE] = -33;
    actionTable[109][DO] = -33;
    actionTable[109][IF] = -33;
    actionTable[109][ELSE] = -33;
    actionTable[109][BREAK] = -33;
    actionTable[109][CONTINUE] = -33;
    actionTable[109][MINUS] = -33;
    actionTable[109][PLUS_PLUS] = -33;
    actionTable[109][MINUS_MINUS] = -33;
    actionTable[109][NOT] = -33;
    actionTable[109][TRUE] = -33;
    actionTable[109][FALSE] = -33;
    actionTable[109][CHARACTER] = -33;
    actionTable[109][STRINGVAL] = -33;
    actionTable[109][NUMERAL] = -33;
    actionTable[109][IDT] = -33;
    //STATE110
    actionTable[110][LEFT_PARENTHESIS] = -34;
    actionTable[110][LEFT_BRACES] = -34;
    actionTable[110][RIGHT_BRACES] = -34;
    actionTable[110][WHILE] = -34;
    actionTable[110][DO] = -34;
    actionTable[110][IF] = -34;
    actionTable[110][ELSE] = -34;
    actionTable[110][BREAK] = -34;
    actionTable[110][CONTINUE] = -34;
    actionTable[110][MINUS] = -34;
    actionTable[110][PLUS_PLUS] = -34;
    actionTable[110][MINUS_MINUS] = -34;
    actionTable[110][NOT] = -34;
    actionTable[110][TRUE] = -34;
    actionTable[110][FALSE] = -34;
    actionTable[110][CHARACTER] = -34;
    actionTable[110][STRINGVAL] = -34;
    actionTable[110][NUMERAL] = -34;
    actionTable[110][IDT] = -34;
    //STATE111
    actionTable[111][LEFT_PARENTHESIS] = 84;
    actionTable[111][NUM] = 91;
    actionTable[111][E] = 141;
    actionTable[111][LV] = 131;
    actionTable[111][L] = 77;
    actionTable[111][R] = 79;
    actionTable[111][K] = 80;
    actionTable[111][MINUS] = 85;
    actionTable[111][F] = 81;
    actionTable[111][PLUS_PLUS] = 82;
    actionTable[111][MINUS_MINUS] = 83;
    actionTable[111][IDU] = 78;
    actionTable[111][NOT] = 86;
    actionTable[111][TRU] = 87;
    actionTable[111][FALS] = 88;
    actionTable[111][CHR] = 89;
    actionTable[111][STR] = 90;
    actionTable[111][TRUE] = 92;
    actionTable[111][FALSE] = 93;
    actionTable[111][CHARACTER] = 94;
    actionTable[111][STRINGVAL] = 95;
    actionTable[111][NUMERAL] = 44;
    actionTable[111][IDT] = 30;
    //STATE112
    actionTable[112][ID] = 142;
    actionTable[112][IDT] = 143;
    //STATE113
    actionTable[113][LEFT_PARENTHESIS] = 84;
    actionTable[113][NUM] = 91;
    actionTable[113][E] = 144;
    actionTable[113][LV] = 131;
    actionTable[113][L] = 77;
    actionTable[113][R] = 79;
    actionTable[113][K] = 80;
    actionTable[113][MINUS] = 85;
    actionTable[113][F] = 81;
    actionTable[113][PLUS_PLUS] = 82;
    actionTable[113][MINUS_MINUS] = 83;
    actionTable[113][IDU] = 78;
    actionTable[113][NOT] = 86;
    actionTable[113][TRU] = 87;
    actionTable[113][FALS] = 88;
    actionTable[113][CHR] = 89;
    actionTable[113][STR] = 90;
    actionTable[113][TRUE] = 92;
    actionTable[113][FALSE] = 93;
    actionTable[113][CHARACTER] = 94;
    actionTable[113][STRINGVAL] = 95;
    actionTable[113][NUMERAL] = 44;
    actionTable[113][IDT] = 30;
    //STATE114
    actionTable[114][RIGHT_PARENTHESIS] = -55;
    actionTable[114][SEMI_COLON] = -55;
    actionTable[114][RIGHT_SQUARE] = -55;
    actionTable[114][COMMA] = -55;
    actionTable[114][AND] = -55;
    actionTable[114][OR] = -55;
    actionTable[114][LESS_THAN] = -55;
    actionTable[114][GREATER_THAN] = -55;
    actionTable[114][LESS_OR_EQUAL] = -55;
    actionTable[114][GREATER_OR_EQUAL] = -55;
    actionTable[114][EQUAL_EQUAL] = -55;
    actionTable[114][NOT_EQUAL] = -55;
    actionTable[114][PLUS] = -55;
    actionTable[114][MINUS] = -55;
    actionTable[114][TIMES] = -55;
    actionTable[114][DIVIDE] = -55;
    //STATE115
    actionTable[115][RIGHT_PARENTHESIS] = -56;
    actionTable[115][SEMI_COLON] = -56;
    actionTable[115][RIGHT_SQUARE] = -56;
    actionTable[115][COMMA] = -56;
    actionTable[115][AND] = -56;
    actionTable[115][OR] = -56;
    actionTable[115][LESS_THAN] = -56;
    actionTable[115][GREATER_THAN] = -56;
    actionTable[115][LESS_OR_EQUAL] = -56;
    actionTable[115][GREATER_OR_EQUAL] = -56;
    actionTable[115][EQUAL_EQUAL] = -56;
    actionTable[115][NOT_EQUAL] = -56;
    actionTable[115][PLUS] = -56;
    actionTable[115][MINUS] = -56;
    actionTable[115][TIMES] = -56;
    actionTable[115][DIVIDE] = -56;
    //STATE116
    actionTable[116][LEFT_PARENTHESIS] = 84;
    actionTable[116][NUM] = 91;
    actionTable[116][LV] = 131;
    actionTable[116][R] = 145;
    actionTable[116][K] = 80;
    actionTable[116][MINUS] = 85;
    actionTable[116][F] = 81;
    actionTable[116][PLUS_PLUS] = 82;
    actionTable[116][MINUS_MINUS] = 83;
    actionTable[116][IDU] = 78;
    actionTable[116][NOT] = 86;
    actionTable[116][TRU] = 87;
    actionTable[116][FALS] = 88;
    actionTable[116][CHR] = 89;
    actionTable[116][STR] = 90;
    actionTable[116][TRUE] = 92;
    actionTable[116][FALSE] = 93;
    actionTable[116][CHARACTER] = 94;
    actionTable[116][STRINGVAL] = 95;
    actionTable[116][NUMERAL] = 44;
    actionTable[116][IDT] = 30;
    //STATE117
    actionTable[117][LEFT_PARENTHESIS] = 84;
    actionTable[117][NUM] = 91;
    actionTable[117][LV] = 131;
    actionTable[117][R] = 146;
    actionTable[117][K] = 80;
    actionTable[117][MINUS] = 85;
    actionTable[117][F] = 81;
    actionTable[117][PLUS_PLUS] = 82;
    actionTable[117][MINUS_MINUS] = 83;
    actionTable[117][IDU] = 78;
    actionTable[117][NOT] = 86;
    actionTable[117][TRU] = 87;
    actionTable[117][FALS] = 88;
    actionTable[117][CHR] = 89;
    actionTable[117][STR] = 90;
    actionTable[117][TRUE] = 92;
    actionTable[117][FALSE] = 93;
    actionTable[117][CHARACTER] = 94;
    actionTable[117][STRINGVAL] = 95;
    actionTable[117][NUMERAL] = 44;
    actionTable[117][IDT] = 30;
    //STATE118
    actionTable[118][LEFT_PARENTHESIS] = 84;
    actionTable[118][NUM] = 91;
    actionTable[118][LV] = 131;
    actionTable[118][R] = 147;
    actionTable[118][K] = 80;
    actionTable[118][MINUS] = 85;
    actionTable[118][F] = 81;
    actionTable[118][PLUS_PLUS] = 82;
    actionTable[118][MINUS_MINUS] = 83;
    actionTable[118][IDU] = 78;
    actionTable[118][NOT] = 86;
    actionTable[118][TRU] = 87;
    actionTable[118][FALS] = 88;
    actionTable[118][CHR] = 89;
    actionTable[118][STR] = 90;
    actionTable[118][TRUE] = 92;
    actionTable[118][FALSE] = 93;
    actionTable[118][CHARACTER] = 94;
    actionTable[118][STRINGVAL] = 95;
    actionTable[118][NUMERAL] = 44;
    actionTable[118][IDT] = 30;
    //STATE119
    actionTable[119][LEFT_PARENTHESIS] = 84;
    actionTable[119][NUM] = 91;
    actionTable[119][LV] = 131;
    actionTable[119][R] = 148;
    actionTable[119][K] = 80;
    actionTable[119][MINUS] = 85;
    actionTable[119][F] = 81;
    actionTable[119][PLUS_PLUS] = 82;
    actionTable[119][MINUS_MINUS] = 83;
    actionTable[119][IDU] = 78;
    actionTable[119][NOT] = 86;
    actionTable[119][TRU] = 87;
    actionTable[119][FALS] = 88;
    actionTable[119][CHR] = 89;
    actionTable[119][STR] = 90;
    actionTable[119][TRUE] = 92;
    actionTable[119][FALSE] = 93;
    actionTable[119][CHARACTER] = 94;
    actionTable[119][STRINGVAL] = 95;
    actionTable[119][NUMERAL] = 44;
    actionTable[119][IDT] = 30;
    //STATE120
    actionTable[120][LEFT_PARENTHESIS] = 84;
    actionTable[120][NUM] = 91;
    actionTable[120][LV] = 131;
    actionTable[120][R] = 149;
    actionTable[120][K] = 80;
    actionTable[120][MINUS] = 85;
    actionTable[120][F] = 81;
    actionTable[120][PLUS_PLUS] = 82;
    actionTable[120][MINUS_MINUS] = 83;
    actionTable[120][IDU] = 78;
    actionTable[120][NOT] = 86;
    actionTable[120][TRU] = 87;
    actionTable[120][FALS] = 88;
    actionTable[120][CHR] = 89;
    actionTable[120][STR] = 90;
    actionTable[120][TRUE] = 92;
    actionTable[120][FALSE] = 93;
    actionTable[120][CHARACTER] = 94;
    actionTable[120][STRINGVAL] = 95;
    actionTable[120][NUMERAL] = 44;
    actionTable[120][IDT] = 30;
    //STATE121
    actionTable[121][LEFT_PARENTHESIS] = 84;
    actionTable[121][NUM] = 91;
    actionTable[121][LV] = 131;
    actionTable[121][R] = 150;
    actionTable[121][K] = 80;
    actionTable[121][MINUS] = 85;
    actionTable[121][F] = 81;
    actionTable[121][PLUS_PLUS] = 82;
    actionTable[121][MINUS_MINUS] = 83;
    actionTable[121][IDU] = 78;
    actionTable[121][NOT] = 86;
    actionTable[121][TRU] = 87;
    actionTable[121][FALS] = 88;
    actionTable[121][CHR] = 89;
    actionTable[121][STR] = 90;
    actionTable[121][TRUE] = 92;
    actionTable[121][FALSE] = 93;
    actionTable[121][CHARACTER] = 94;
    actionTable[121][STRINGVAL] = 95;
    actionTable[121][NUMERAL] = 44;
    actionTable[121][IDT] = 30;
    //STATE122
    actionTable[122][LEFT_PARENTHESIS] = 84;
    actionTable[122][RIGHT_PARENTHESIS] = -68;
    actionTable[122][NUM] = 91;
    actionTable[122][COMMA] = -68;
    actionTable[122][E] = 152;
    actionTable[122][LV] = 131;
    actionTable[122][L] = 77;
    actionTable[122][R] = 79;
    actionTable[122][K] = 80;
    actionTable[122][MINUS] = 85;
    actionTable[122][F] = 81;
    actionTable[122][PLUS_PLUS] = 82;
    actionTable[122][MINUS_MINUS] = 83;
    actionTable[122][IDU] = 78;
    actionTable[122][LE] = 151;
    actionTable[122][NOT] = 86;
    actionTable[122][TRU] = 87;
    actionTable[122][FALS] = 88;
    actionTable[122][CHR] = 89;
    actionTable[122][STR] = 90;
    actionTable[122][TRUE] = 92;
    actionTable[122][FALSE] = 93;
    actionTable[122][CHARACTER] = 94;
    actionTable[122][STRINGVAL] = 95;
    actionTable[122][NUMERAL] = 44;
    actionTable[122][IDT] = 30;
    //STATE123
    actionTable[123][LEFT_PARENTHESIS] = 84;
    actionTable[123][NUM] = 91;
    actionTable[123][LV] = 131;
    actionTable[123][K] = 153;
    actionTable[123][MINUS] = 85;
    actionTable[123][F] = 81;
    actionTable[123][PLUS_PLUS] = 82;
    actionTable[123][MINUS_MINUS] = 83;
    actionTable[123][IDU] = 78;
    actionTable[123][NOT] = 86;
    actionTable[123][TRU] = 87;
    actionTable[123][FALS] = 88;
    actionTable[123][CHR] = 89;
    actionTable[123][STR] = 90;
    actionTable[123][TRUE] = 92;
    actionTable[123][FALSE] = 93;
    actionTable[123][CHARACTER] = 94;
    actionTable[123][STRINGVAL] = 95;
    actionTable[123][NUMERAL] = 44;
    actionTable[123][IDT] = 30;
    //STATE124
    actionTable[124][LEFT_PARENTHESIS] = 84;
    actionTable[124][NUM] = 91;
    actionTable[124][LV] = 131;
    actionTable[124][K] = 154;
    actionTable[124][MINUS] = 85;
    actionTable[124][F] = 81;
    actionTable[124][PLUS_PLUS] = 82;
    actionTable[124][MINUS_MINUS] = 83;
    actionTable[124][IDU] = 78;
    actionTable[124][NOT] = 86;
    actionTable[124][TRU] = 87;
    actionTable[124][FALS] = 88;
    actionTable[124][CHR] = 89;
    actionTable[124][STR] = 90;
    actionTable[124][TRUE] = 92;
    actionTable[124][FALSE] = 93;
    actionTable[124][CHARACTER] = 94;
    actionTable[124][STRINGVAL] = 95;
    actionTable[124][NUMERAL] = 44;
    actionTable[124][IDT] = 30;
    //STATE125
    actionTable[125][LEFT_PARENTHESIS] = 84;
    actionTable[125][NUM] = 91;
    actionTable[125][LV] = 131;
    actionTable[125][MINUS] = 85;
    actionTable[125][F] = 155;
    actionTable[125][PLUS_PLUS] = 82;
    actionTable[125][MINUS_MINUS] = 83;
    actionTable[125][IDU] = 78;
    actionTable[125][NOT] = 86;
    actionTable[125][TRU] = 87;
    actionTable[125][FALS] = 88;
    actionTable[125][CHR] = 89;
    actionTable[125][STR] = 90;
    actionTable[125][TRUE] = 92;
    actionTable[125][FALSE] = 93;
    actionTable[125][CHARACTER] = 94;
    actionTable[125][STRINGVAL] = 95;
    actionTable[125][NUMERAL] = 44;
    actionTable[125][IDT] = 30;
    //STATE126
    actionTable[126][LEFT_PARENTHESIS] = 84;
    actionTable[126][NUM] = 91;
    actionTable[126][LV] = 131;
    actionTable[126][MINUS] = 85;
    actionTable[126][F] = 156;
    actionTable[126][PLUS_PLUS] = 82;
    actionTable[126][MINUS_MINUS] = 83;
    actionTable[126][IDU] = 78;
    actionTable[126][NOT] = 86;
    actionTable[126][TRU] = 87;
    actionTable[126][FALS] = 88;
    actionTable[126][CHR] = 89;
    actionTable[126][STR] = 90;
    actionTable[126][TRUE] = 92;
    actionTable[126][FALSE] = 93;
    actionTable[126][CHARACTER] = 94;
    actionTable[126][STRINGVAL] = 95;
    actionTable[126][NUMERAL] = 44;
    actionTable[126][IDT] = 30;
    //STATE127
    actionTable[127][RIGHT_PARENTHESIS] = -53;
    actionTable[127][SEMI_COLON] = -53;
    actionTable[127][LEFT_SQUARE] = 113;
    actionTable[127][RIGHT_SQUARE] = -53;
    actionTable[127][COMMA] = -53;
    actionTable[127][AND] = -53;
    actionTable[127][OR] = -53;
    actionTable[127][LESS_THAN] = -53;
    actionTable[127][GREATER_THAN] = -53;
    actionTable[127][LESS_OR_EQUAL] = -53;
    actionTable[127][GREATER_OR_EQUAL] = -53;
    actionTable[127][EQUAL_EQUAL] = -53;
    actionTable[127][NOT_EQUAL] = -53;
    actionTable[127][PLUS] = -53;
    actionTable[127][MINUS] = -53;
    actionTable[127][TIMES] = -53;
    actionTable[127][DIVIDE] = -53;
    actionTable[127][DOT] = 112;
    //STATE128
    actionTable[128][RIGHT_PARENTHESIS] = -71;
    actionTable[128][SEMI_COLON] = -71;
    actionTable[128][LEFT_SQUARE] = -71;
    actionTable[128][RIGHT_SQUARE] = -71;
    actionTable[128][COMMA] = -71;
    actionTable[128][AND] = -71;
    actionTable[128][OR] = -71;
    actionTable[128][LESS_THAN] = -71;
    actionTable[128][GREATER_THAN] = -71;
    actionTable[128][LESS_OR_EQUAL] = -71;
    actionTable[128][GREATER_OR_EQUAL] = -71;
    actionTable[128][EQUAL_EQUAL] = -71;
    actionTable[128][NOT_EQUAL] = -71;
    actionTable[128][PLUS] = -71;
    actionTable[128][MINUS] = -71;
    actionTable[128][TIMES] = -71;
    actionTable[128][DIVIDE] = -71;
    actionTable[128][DOT] = -71;
    //STATE129
    actionTable[129][RIGHT_PARENTHESIS] = -54;
    actionTable[129][SEMI_COLON] = -54;
    actionTable[129][LEFT_SQUARE] = 113;
    actionTable[129][RIGHT_SQUARE] = -54;
    actionTable[129][COMMA] = -54;
    actionTable[129][AND] = -54;
    actionTable[129][OR] = -54;
    actionTable[129][LESS_THAN] = -54;
    actionTable[129][GREATER_THAN] = -54;
    actionTable[129][LESS_OR_EQUAL] = -54;
    actionTable[129][GREATER_OR_EQUAL] = -54;
    actionTable[129][EQUAL_EQUAL] = -54;
    actionTable[129][NOT_EQUAL] = -54;
    actionTable[129][PLUS] = -54;
    actionTable[129][MINUS] = -54;
    actionTable[129][TIMES] = -54;
    actionTable[129][DIVIDE] = -54;
    actionTable[129][DOT] = 112;
    //STATE130
    actionTable[130][RIGHT_PARENTHESIS] = 157;
    actionTable[130][AND] = 104;
    actionTable[130][OR] = 105;
    //STATE131
    actionTable[131][RIGHT_PARENTHESIS] = -52;
    actionTable[131][SEMI_COLON] = -52;
    actionTable[131][LEFT_SQUARE] = 113;
    actionTable[131][RIGHT_SQUARE] = -52;
    actionTable[131][COMMA] = -52;
    actionTable[131][AND] = -52;
    actionTable[131][OR] = -52;
    actionTable[131][LESS_THAN] = -52;
    actionTable[131][GREATER_THAN] = -52;
    actionTable[131][LESS_OR_EQUAL] = -52;
    actionTable[131][GREATER_OR_EQUAL] = -52;
    actionTable[131][EQUAL_EQUAL] = -52;
    actionTable[131][NOT_EQUAL] = -52;
    actionTable[131][PLUS] = -52;
    actionTable[131][MINUS] = -52;
    actionTable[131][TIMES] = -52;
    actionTable[131][DIVIDE] = -52;
    actionTable[131][PLUS_PLUS] = 114;
    actionTable[131][MINUS_MINUS] = 115;
    actionTable[131][DOT] = 112;
    //STATE132
    actionTable[132][RIGHT_PARENTHESIS] = -59;
    actionTable[132][SEMI_COLON] = -59;
    actionTable[132][RIGHT_SQUARE] = -59;
    actionTable[132][COMMA] = -59;
    actionTable[132][AND] = -59;
    actionTable[132][OR] = -59;
    actionTable[132][LESS_THAN] = -59;
    actionTable[132][GREATER_THAN] = -59;
    actionTable[132][LESS_OR_EQUAL] = -59;
    actionTable[132][GREATER_OR_EQUAL] = -59;
    actionTable[132][EQUAL_EQUAL] = -59;
    actionTable[132][NOT_EQUAL] = -59;
    actionTable[132][PLUS] = -59;
    actionTable[132][MINUS] = -59;
    actionTable[132][TIMES] = -59;
    actionTable[132][DIVIDE] = -59;
    //STATE133
    actionTable[133][RIGHT_PARENTHESIS] = -60;
    actionTable[133][SEMI_COLON] = -60;
    actionTable[133][RIGHT_SQUARE] = -60;
    actionTable[133][COMMA] = -60;
    actionTable[133][AND] = -60;
    actionTable[133][OR] = -60;
    actionTable[133][LESS_THAN] = -60;
    actionTable[133][GREATER_THAN] = -60;
    actionTable[133][LESS_OR_EQUAL] = -60;
    actionTable[133][GREATER_OR_EQUAL] = -60;
    actionTable[133][EQUAL_EQUAL] = -60;
    actionTable[133][NOT_EQUAL] = -60;
    actionTable[133][PLUS] = -60;
    actionTable[133][MINUS] = -60;
    actionTable[133][TIMES] = -60;
    actionTable[133][DIVIDE] = -60;
    //STATE134
    actionTable[134][RIGHT_BRACES] = -11;
    actionTable[134][SEMI_COLON] = -11;
    //STATE135
    actionTable[135][ENDFILE] = -19;
    actionTable[135][FUNCTION] = -19;
    actionTable[135][LEFT_PARENTHESIS] = -19;
    actionTable[135][TYPE] = -19;
    actionTable[135][LEFT_BRACES] = -19;
    actionTable[135][RIGHT_BRACES] = -19;
    actionTable[135][VAR] = -19;
    actionTable[135][WHILE] = -19;
    actionTable[135][DO] = -19;
    actionTable[135][IF] = -19;
    actionTable[135][ELSE] = -19;
    actionTable[135][BREAK] = -19;
    actionTable[135][CONTINUE] = -19;
    actionTable[135][MINUS] = -19;
    actionTable[135][PLUS_PLUS] = -19;
    actionTable[135][MINUS_MINUS] = -19;
    actionTable[135][NOT] = -19;
    actionTable[135][TRUE] = -19;
    actionTable[135][FALSE] = -19;
    actionTable[135][CHARACTER] = -19;
    actionTable[135][STRINGVAL] = -19;
    actionTable[135][NUMERAL] = -19;
    actionTable[135][IDT] = -19;
    //STATE136
    actionTable[136][RIGHT_PARENTHESIS] = -36;
    actionTable[136][SEMI_COLON] = -36;
    actionTable[136][RIGHT_SQUARE] = -36;
    actionTable[136][COMMA] = -36;
    actionTable[136][AND] = -36;
    actionTable[136][OR] = -36;
    actionTable[136][LESS_THAN] = 116;
    actionTable[136][GREATER_THAN] = 117;
    actionTable[136][LESS_OR_EQUAL] = 118;
    actionTable[136][GREATER_OR_EQUAL] = 119;
    actionTable[136][EQUAL_EQUAL] = 120;
    actionTable[136][NOT_EQUAL] = 121;
    //STATE137
    actionTable[137][RIGHT_PARENTHESIS] = -37;
    actionTable[137][SEMI_COLON] = -37;
    actionTable[137][RIGHT_SQUARE] = -37;
    actionTable[137][COMMA] = -37;
    actionTable[137][AND] = -37;
    actionTable[137][OR] = -37;
    actionTable[137][LESS_THAN] = 116;
    actionTable[137][GREATER_THAN] = 117;
    actionTable[137][LESS_OR_EQUAL] = 118;
    actionTable[137][GREATER_OR_EQUAL] = 119;
    actionTable[137][EQUAL_EQUAL] = 120;
    actionTable[137][NOT_EQUAL] = 121;
    //STATE138
    actionTable[138][RIGHT_PARENTHESIS] = 158;
    actionTable[138][AND] = 104;
    actionTable[138][OR] = 105;
    //STATE139
    actionTable[139][LEFT_PARENTHESIS] = 159;
    //STATE140
    actionTable[140][RIGHT_PARENTHESIS] = 160;
    actionTable[140][AND] = 104;
    actionTable[140][OR] = 105;
    //STATE141
    actionTable[141][SEMI_COLON] = 161;
    actionTable[141][AND] = 104;
    actionTable[141][OR] = 105;
    //STATE142
    actionTable[142][RIGHT_PARENTHESIS] = -69;
    actionTable[142][EQUALS] = -69;
    actionTable[142][SEMI_COLON] = -69;
    actionTable[142][LEFT_SQUARE] = -69;
    actionTable[142][RIGHT_SQUARE] = -69;
    actionTable[142][COMMA] = -69;
    actionTable[142][AND] = -69;
    actionTable[142][OR] = -69;
    actionTable[142][LESS_THAN] = -69;
    actionTable[142][GREATER_THAN] = -69;
    actionTable[142][LESS_OR_EQUAL] = -69;
    actionTable[142][GREATER_OR_EQUAL] = -69;
    actionTable[142][EQUAL_EQUAL] = -69;
    actionTable[142][NOT_EQUAL] = -69;
    actionTable[142][PLUS] = -69;
    actionTable[142][MINUS] = -69;
    actionTable[142][TIMES] = -69;
    actionTable[142][DIVIDE] = -69;
    actionTable[142][PLUS_PLUS] = -69;
    actionTable[142][MINUS_MINUS] = -69;
    actionTable[142][DOT] = -69;
    //STATE143
    actionTable[143][RIGHT_PARENTHESIS] = -84;
    actionTable[143][EQUALS] = -84;
    actionTable[143][SEMI_COLON] = -84;
    actionTable[143][LEFT_SQUARE] = -84;
    actionTable[143][RIGHT_SQUARE] = -84;
    actionTable[143][COMMA] = -84;
    actionTable[143][AND] = -84;
    actionTable[143][OR] = -84;
    actionTable[143][LESS_THAN] = -84;
    actionTable[143][GREATER_THAN] = -84;
    actionTable[143][LESS_OR_EQUAL] = -84;
    actionTable[143][GREATER_OR_EQUAL] = -84;
    actionTable[143][EQUAL_EQUAL] = -84;
    actionTable[143][NOT_EQUAL] = -84;
    actionTable[143][PLUS] = -84;
    actionTable[143][MINUS] = -84;
    actionTable[143][TIMES] = -84;
    actionTable[143][DIVIDE] = -84;
    actionTable[143][PLUS_PLUS] = -84;
    actionTable[143][MINUS_MINUS] = -84;
    actionTable[143][DOT] = -84;
    //STATE144
    actionTable[144][RIGHT_SQUARE] = 162;
    actionTable[144][AND] = 104;
    actionTable[144][OR] = 105;
    //STATE145
    actionTable[145][RIGHT_PARENTHESIS] = -39;
    actionTable[145][SEMI_COLON] = -39;
    actionTable[145][RIGHT_SQUARE] = -39;
    actionTable[145][COMMA] = -39;
    actionTable[145][AND] = -39;
    actionTable[145][OR] = -39;
    actionTable[145][LESS_THAN] = -39;
    actionTable[145][GREATER_THAN] = -39;
    actionTable[145][LESS_OR_EQUAL] = -39;
    actionTable[145][GREATER_OR_EQUAL] = -39;
    actionTable[145][EQUAL_EQUAL] = -39;
    actionTable[145][NOT_EQUAL] = -39;
    actionTable[145][PLUS] = 123;
    actionTable[145][MINUS] = 124;
    //STATE146
    actionTable[146][RIGHT_PARENTHESIS] = -40;
    actionTable[146][SEMI_COLON] = -40;
    actionTable[146][RIGHT_SQUARE] = -40;
    actionTable[146][COMMA] = -40;
    actionTable[146][AND] = -40;
    actionTable[146][OR] = -40;
    actionTable[146][LESS_THAN] = -40;
    actionTable[146][GREATER_THAN] = -40;
    actionTable[146][LESS_OR_EQUAL] = -40;
    actionTable[146][GREATER_OR_EQUAL] = -40;
    actionTable[146][EQUAL_EQUAL] = -40;
    actionTable[146][NOT_EQUAL] = -40;
    actionTable[146][PLUS] = 123;
    actionTable[146][MINUS] = 124;
    //STATE147
    actionTable[147][RIGHT_PARENTHESIS] = -41;
    actionTable[147][SEMI_COLON] = -41;
    actionTable[147][RIGHT_SQUARE] = -41;
    actionTable[147][COMMA] = -41;
    actionTable[147][AND] = -41;
    actionTable[147][OR] = -41;
    actionTable[147][LESS_THAN] = -41;
    actionTable[147][GREATER_THAN] = -41;
    actionTable[147][LESS_OR_EQUAL] = -41;
    actionTable[147][GREATER_OR_EQUAL] = -41;
    actionTable[147][EQUAL_EQUAL] = -41;
    actionTable[147][NOT_EQUAL] = -41;
    actionTable[147][PLUS] = 123;
    actionTable[147][MINUS] = 124;
    //STATE148
    actionTable[148][RIGHT_PARENTHESIS] = -42;
    actionTable[148][SEMI_COLON] = -42;
    actionTable[148][RIGHT_SQUARE] = -42;
    actionTable[148][COMMA] = -42;
    actionTable[148][AND] = -42;
    actionTable[148][OR] = -42;
    actionTable[148][LESS_THAN] = -42;
    actionTable[148][GREATER_THAN] = -42;
    actionTable[148][LESS_OR_EQUAL] = -42;
    actionTable[148][GREATER_OR_EQUAL] = -42;
    actionTable[148][EQUAL_EQUAL] = -42;
    actionTable[148][NOT_EQUAL] = -42;
    actionTable[148][PLUS] = 123;
    actionTable[148][MINUS] = 124;
    //STATE149
    actionTable[149][RIGHT_PARENTHESIS] = -43;
    actionTable[149][SEMI_COLON] = -43;
    actionTable[149][RIGHT_SQUARE] = -43;
    actionTable[149][COMMA] = -43;
    actionTable[149][AND] = -43;
    actionTable[149][OR] = -43;
    actionTable[149][LESS_THAN] = -43;
    actionTable[149][GREATER_THAN] = -43;
    actionTable[149][LESS_OR_EQUAL] = -43;
    actionTable[149][GREATER_OR_EQUAL] = -43;
    actionTable[149][EQUAL_EQUAL] = -43;
    actionTable[149][NOT_EQUAL] = -43;
    actionTable[149][PLUS] = 123;
    actionTable[149][MINUS] = 124;
    //STATE150
    actionTable[150][RIGHT_PARENTHESIS] = -44;
    actionTable[150][SEMI_COLON] = -44;
    actionTable[150][RIGHT_SQUARE] = -44;
    actionTable[150][COMMA] = -44;
    actionTable[150][AND] = -44;
    actionTable[150][OR] = -44;
    actionTable[150][LESS_THAN] = -44;
    actionTable[150][GREATER_THAN] = -44;
    actionTable[150][LESS_OR_EQUAL] = -44;
    actionTable[150][GREATER_OR_EQUAL] = -44;
    actionTable[150][EQUAL_EQUAL] = -44;
    actionTable[150][NOT_EQUAL] = -44;
    actionTable[150][PLUS] = 123;
    actionTable[150][MINUS] = 124;
    //STATE151
    actionTable[151][RIGHT_PARENTHESIS] = 163;
    actionTable[151][COMMA] = 164;
    //STATE152
    actionTable[152][RIGHT_PARENTHESIS] = -67;
    actionTable[152][COMMA] = -67;
    actionTable[152][AND] = 104;
    actionTable[152][OR] = 105;
    //STATE153
    actionTable[153][RIGHT_PARENTHESIS] = -46;
    actionTable[153][SEMI_COLON] = -46;
    actionTable[153][RIGHT_SQUARE] = -46;
    actionTable[153][COMMA] = -46;
    actionTable[153][AND] = -46;
    actionTable[153][OR] = -46;
    actionTable[153][LESS_THAN] = -46;
    actionTable[153][GREATER_THAN] = -46;
    actionTable[153][LESS_OR_EQUAL] = -46;
    actionTable[153][GREATER_OR_EQUAL] = -46;
    actionTable[153][EQUAL_EQUAL] = -46;
    actionTable[153][NOT_EQUAL] = -46;
    actionTable[153][PLUS] = -46;
    actionTable[153][MINUS] = -46;
    actionTable[153][TIMES] = 125;
    actionTable[153][DIVIDE] = 126;
    //STATE154
    actionTable[154][RIGHT_PARENTHESIS] = -47;
    actionTable[154][SEMI_COLON] = -47;
    actionTable[154][RIGHT_SQUARE] = -47;
    actionTable[154][COMMA] = -47;
    actionTable[154][AND] = -47;
    actionTable[154][OR] = -47;
    actionTable[154][LESS_THAN] = -47;
    actionTable[154][GREATER_THAN] = -47;
    actionTable[154][LESS_OR_EQUAL] = -47;
    actionTable[154][GREATER_OR_EQUAL] = -47;
    actionTable[154][EQUAL_EQUAL] = -47;
    actionTable[154][NOT_EQUAL] = -47;
    actionTable[154][PLUS] = -47;
    actionTable[154][MINUS] = -47;
    actionTable[154][TIMES] = 125;
    actionTable[154][DIVIDE] = 126;
    //STATE155
    actionTable[155][RIGHT_PARENTHESIS] = -49;
    actionTable[155][SEMI_COLON] = -49;
    actionTable[155][RIGHT_SQUARE] = -49;
    actionTable[155][COMMA] = -49;
    actionTable[155][AND] = -49;
    actionTable[155][OR] = -49;
    actionTable[155][LESS_THAN] = -49;
    actionTable[155][GREATER_THAN] = -49;
    actionTable[155][LESS_OR_EQUAL] = -49;
    actionTable[155][GREATER_OR_EQUAL] = -49;
    actionTable[155][EQUAL_EQUAL] = -49;
    actionTable[155][NOT_EQUAL] = -49;
    actionTable[155][PLUS] = -49;
    actionTable[155][MINUS] = -49;
    actionTable[155][TIMES] = -49;
    actionTable[155][DIVIDE] = -49;
    //STATE156
    actionTable[156][RIGHT_PARENTHESIS] = -50;
    actionTable[156][SEMI_COLON] = -50;
    actionTable[156][RIGHT_SQUARE] = -50;
    actionTable[156][COMMA] = -50;
    actionTable[156][AND] = -50;
    actionTable[156][OR] = -50;
    actionTable[156][LESS_THAN] = -50;
    actionTable[156][GREATER_THAN] = -50;
    actionTable[156][LESS_OR_EQUAL] = -50;
    actionTable[156][GREATER_OR_EQUAL] = -50;
    actionTable[156][EQUAL_EQUAL] = -50;
    actionTable[156][NOT_EQUAL] = -50;
    actionTable[156][PLUS] = -50;
    actionTable[156][MINUS] = -50;
    actionTable[156][TIMES] = -50;
    actionTable[156][DIVIDE] = -50;
    //STATE157
    actionTable[157][RIGHT_PARENTHESIS] = -57;
    actionTable[157][SEMI_COLON] = -57;
    actionTable[157][RIGHT_SQUARE] = -57;
    actionTable[157][COMMA] = -57;
    actionTable[157][AND] = -57;
    actionTable[157][OR] = -57;
    actionTable[157][LESS_THAN] = -57;
    actionTable[157][GREATER_THAN] = -57;
    actionTable[157][LESS_OR_EQUAL] = -57;
    actionTable[157][GREATER_OR_EQUAL] = -57;
    actionTable[157][EQUAL_EQUAL] = -57;
    actionTable[157][NOT_EQUAL] = -57;
    actionTable[157][PLUS] = -57;
    actionTable[157][MINUS] = -57;
    actionTable[157][TIMES] = -57;
    actionTable[157][DIVIDE] = -57;
    //STATE158
    actionTable[158][LEFT_PARENTHESIS] = 84;
    actionTable[158][B] = 70;
    actionTable[158][LEFT_BRACES] = 58;
    actionTable[158][NUM] = 91;
    actionTable[158][S] = 165;
    actionTable[158][E] = 69;
    actionTable[158][WHILE] = 71;
    actionTable[158][DO] = 72;
    actionTable[158][IF] = 73;
    actionTable[158][BREAK] = 74;
    actionTable[158][CONTINUE] = 75;
    actionTable[158][LV] = 76;
    actionTable[158][L] = 77;
    actionTable[158][R] = 79;
    actionTable[158][K] = 80;
    actionTable[158][MINUS] = 85;
    actionTable[158][F] = 81;
    actionTable[158][PLUS_PLUS] = 82;
    actionTable[158][MINUS_MINUS] = 83;
    actionTable[158][IDU] = 78;
    actionTable[158][NOT] = 86;
    actionTable[158][TRU] = 87;
    actionTable[158][FALS] = 88;
    actionTable[158][CHR] = 89;
    actionTable[158][STR] = 90;
    actionTable[158][TRUE] = 92;
    actionTable[158][FALSE] = 93;
    actionTable[158][CHARACTER] = 94;
    actionTable[158][STRINGVAL] = 95;
    actionTable[158][NUMERAL] = 44;
    actionTable[158][IDT] = 30;
    //STATE159
    actionTable[159][LEFT_PARENTHESIS] = 84;
    actionTable[159][NUM] = 91;
    actionTable[159][E] = 166;
    actionTable[159][LV] = 131;
    actionTable[159][L] = 77;
    actionTable[159][R] = 79;
    actionTable[159][K] = 80;
    actionTable[159][MINUS] = 85;
    actionTable[159][F] = 81;
    actionTable[159][PLUS_PLUS] = 82;
    actionTable[159][MINUS_MINUS] = 83;
    actionTable[159][IDU] = 78;
    actionTable[159][NOT] = 86;
    actionTable[159][TRU] = 87;
    actionTable[159][FALS] = 88;
    actionTable[159][CHR] = 89;
    actionTable[159][STR] = 90;
    actionTable[159][TRUE] = 92;
    actionTable[159][FALSE] = 93;
    actionTable[159][CHARACTER] = 94;
    actionTable[159][STRINGVAL] = 95;
    actionTable[159][NUMERAL] = 44;
    actionTable[159][IDT] = 30;
    //STATE160
    actionTable[160][LEFT_PARENTHESIS] = 84;
    actionTable[160][B] = 70;
    actionTable[160][LEFT_BRACES] = 58;
    actionTable[160][NUM] = 91;
    actionTable[160][S] = 167;
    actionTable[160][E] = 69;
    actionTable[160][WHILE] = 71;
    actionTable[160][DO] = 72;
    actionTable[160][IF] = 73;
    actionTable[160][BREAK] = 74;
    actionTable[160][CONTINUE] = 75;
    actionTable[160][LV] = 76;
    actionTable[160][L] = 77;
    actionTable[160][R] = 79;
    actionTable[160][K] = 80;
    actionTable[160][MINUS] = 85;
    actionTable[160][F] = 81;
    actionTable[160][PLUS_PLUS] = 82;
    actionTable[160][MINUS_MINUS] = 83;
    actionTable[160][IDU] = 78;
    actionTable[160][NOT] = 86;
    actionTable[160][TRU] = 87;
    actionTable[160][FALS] = 88;
    actionTable[160][CHR] = 89;
    actionTable[160][STR] = 90;
    actionTable[160][TRUE] = 92;
    actionTable[160][FALSE] = 93;
    actionTable[160][CHARACTER] = 94;
    actionTable[160][STRINGVAL] = 95;
    actionTable[160][NUMERAL] = 44;
    actionTable[160][IDT] = 30;
    //STATE161
    actionTable[161][LEFT_PARENTHESIS] = -35;
    actionTable[161][LEFT_BRACES] = -35;
    actionTable[161][RIGHT_BRACES] = -35;
    actionTable[161][WHILE] = -35;
    actionTable[161][DO] = -35;
    actionTable[161][IF] = -35;
    actionTable[161][ELSE] = -35;
    actionTable[161][BREAK] = -35;
    actionTable[161][CONTINUE] = -35;
    actionTable[161][MINUS] = -35;
    actionTable[161][PLUS_PLUS] = -35;
    actionTable[161][MINUS_MINUS] = -35;
    actionTable[161][NOT] = -35;
    actionTable[161][TRUE] = -35;
    actionTable[161][FALSE] = -35;
    actionTable[161][CHARACTER] = -35;
    actionTable[161][STRINGVAL] = -35;
    actionTable[161][NUMERAL] = -35;
    actionTable[161][IDT] = -35;
    //STATE162
    actionTable[162][RIGHT_PARENTHESIS] = -70;
    actionTable[162][EQUALS] = -70;
    actionTable[162][SEMI_COLON] = -70;
    actionTable[162][LEFT_SQUARE] = -70;
    actionTable[162][RIGHT_SQUARE] = -70;
    actionTable[162][COMMA] = -70;
    actionTable[162][AND] = -70;
    actionTable[162][OR] = -70;
    actionTable[162][LESS_THAN] = -70;
    actionTable[162][GREATER_THAN] = -70;
    actionTable[162][LESS_OR_EQUAL] = -70;
    actionTable[162][GREATER_OR_EQUAL] = -70;
    actionTable[162][EQUAL_EQUAL] = -70;
    actionTable[162][NOT_EQUAL] = -70;
    actionTable[162][PLUS] = -70;
    actionTable[162][MINUS] = -70;
    actionTable[162][TIMES] = -70;
    actionTable[162][DIVIDE] = -70;
    actionTable[162][PLUS_PLUS] = -70;
    actionTable[162][MINUS_MINUS] = -70;
    actionTable[162][DOT] = -70;
    //STATE163
    actionTable[163][RIGHT_PARENTHESIS] = -58;
    actionTable[163][SEMI_COLON] = -58;
    actionTable[163][RIGHT_SQUARE] = -58;
    actionTable[163][COMMA] = -58;
    actionTable[163][AND] = -58;
    actionTable[163][OR] = -58;
    actionTable[163][LESS_THAN] = -58;
    actionTable[163][GREATER_THAN] = -58;
    actionTable[163][LESS_OR_EQUAL] = -58;
    actionTable[163][GREATER_OR_EQUAL] = -58;
    actionTable[163][EQUAL_EQUAL] = -58;
    actionTable[163][NOT_EQUAL] = -58;
    actionTable[163][PLUS] = -58;
    actionTable[163][MINUS] = -58;
    actionTable[163][TIMES] = -58;
    actionTable[163][DIVIDE] = -58;
    //STATE164
    actionTable[164][LEFT_PARENTHESIS] = 84;
    actionTable[164][NUM] = 91;
    actionTable[164][E] = 168;
    actionTable[164][LV] = 131;
    actionTable[164][L] = 77;
    actionTable[164][R] = 79;
    actionTable[164][K] = 80;
    actionTable[164][MINUS] = 85;
    actionTable[164][F] = 81;
    actionTable[164][PLUS_PLUS] = 82;
    actionTable[164][MINUS_MINUS] = 83;
    actionTable[164][IDU] = 78;
    actionTable[164][NOT] = 86;
    actionTable[164][TRU] = 87;
    actionTable[164][FALS] = 88;
    actionTable[164][CHR] = 89;
    actionTable[164][STR] = 90;
    actionTable[164][TRUE] = 92;
    actionTable[164][FALSE] = 93;
    actionTable[164][CHARACTER] = 94;
    actionTable[164][STRINGVAL] = 95;
    actionTable[164][NUMERAL] = 44;
    actionTable[164][IDT] = 30;
    //STATE165
    actionTable[165][LEFT_PARENTHESIS] = -29;
    actionTable[165][LEFT_BRACES] = -29;
    actionTable[165][RIGHT_BRACES] = -29;
    actionTable[165][WHILE] = -29;
    actionTable[165][DO] = -29;
    actionTable[165][IF] = -29;
    actionTable[165][ELSE] = -29;
    actionTable[165][BREAK] = -29;
    actionTable[165][CONTINUE] = -29;
    actionTable[165][MINUS] = -29;
    actionTable[165][PLUS_PLUS] = -29;
    actionTable[165][MINUS_MINUS] = -29;
    actionTable[165][NOT] = -29;
    actionTable[165][TRUE] = -29;
    actionTable[165][FALSE] = -29;
    actionTable[165][CHARACTER] = -29;
    actionTable[165][STRINGVAL] = -29;
    actionTable[165][NUMERAL] = -29;
    actionTable[165][IDT] = -29;
    //STATE166
    actionTable[166][RIGHT_PARENTHESIS] = 169;
    actionTable[166][AND] = 104;
    actionTable[166][OR] = 105;
    //STATE167
    actionTable[167][LEFT_PARENTHESIS] = -31;
    actionTable[167][LEFT_BRACES] = -31;
    actionTable[167][RIGHT_BRACES] = -31;
    actionTable[167][WHILE] = -31;
    actionTable[167][DO] = -31;
    actionTable[167][IF] = -31;
    actionTable[167][ELSE] = 170;
    actionTable[167][BREAK] = -31;
    actionTable[167][CONTINUE] = -31;
    actionTable[167][MINUS] = -31;
    actionTable[167][PLUS_PLUS] = -31;
    actionTable[167][MINUS_MINUS] = -31;
    actionTable[167][NOT] = -31;
    actionTable[167][TRUE] = -31;
    actionTable[167][FALSE] = -31;
    actionTable[167][CHARACTER] = -31;
    actionTable[167][STRINGVAL] = -31;
    actionTable[167][NUMERAL] = -31;
    actionTable[167][IDT] = -31;
    //STATE168
    actionTable[168][RIGHT_PARENTHESIS] = -66;
    actionTable[168][COMMA] = -66;
    actionTable[168][AND] = 104;
    actionTable[168][OR] = 105;
    //STATE169
    actionTable[169][SEMI_COLON] = 171;
    //STATE170
    actionTable[170][LEFT_PARENTHESIS] = 84;
    actionTable[170][B] = 70;
    actionTable[170][LEFT_BRACES] = 58;
    actionTable[170][NUM] = 91;
    actionTable[170][S] = 172;
    actionTable[170][E] = 69;
    actionTable[170][WHILE] = 71;
    actionTable[170][DO] = 72;
    actionTable[170][IF] = 73;
    actionTable[170][BREAK] = 74;
    actionTable[170][CONTINUE] = 75;
    actionTable[170][LV] = 76;
    actionTable[170][L] = 77;
    actionTable[170][R] = 79;
    actionTable[170][K] = 80;
    actionTable[170][MINUS] = 85;
    actionTable[170][F] = 81;
    actionTable[170][PLUS_PLUS] = 82;
    actionTable[170][MINUS_MINUS] = 83;
    actionTable[170][IDU] = 78;
    actionTable[170][NOT] = 86;
    actionTable[170][TRU] = 87;
    actionTable[170][FALS] = 88;
    actionTable[170][CHR] = 89;
    actionTable[170][STR] = 90;
    actionTable[170][TRUE] = 92;
    actionTable[170][FALSE] = 93;
    actionTable[170][CHARACTER] = 94;
    actionTable[170][STRINGVAL] = 95;
    actionTable[170][NUMERAL] = 44;
    actionTable[170][IDT] = 30;
    //STATE171
    actionTable[171][LEFT_PARENTHESIS] = -30;
    actionTable[171][LEFT_BRACES] = -30;
    actionTable[171][RIGHT_BRACES] = -30;
    actionTable[171][WHILE] = -30;
    actionTable[171][DO] = -30;
    actionTable[171][IF] = -30;
    actionTable[171][ELSE] = -30;
    actionTable[171][BREAK] = -30;
    actionTable[171][CONTINUE] = -30;
    actionTable[171][MINUS] = -30;
    actionTable[171][PLUS_PLUS] = -30;
    actionTable[171][MINUS_MINUS] = -30;
    actionTable[171][NOT] = -30;
    actionTable[171][TRUE] = -30;
    actionTable[171][FALSE] = -30;
    actionTable[171][CHARACTER] = -30;
    actionTable[171][STRINGVAL] = -30;
    actionTable[171][NUMERAL] = -30;
    actionTable[171][IDT] = -30;
    //STATE172
    actionTable[172][LEFT_PARENTHESIS] = -32;
    actionTable[172][LEFT_BRACES] = -32;
    actionTable[172][RIGHT_BRACES] = -32;
    actionTable[172][WHILE] = -32;
    actionTable[172][DO] = -32;
    actionTable[172][IF] = -32;
    actionTable[172][ELSE] = -32;
    actionTable[172][BREAK] = -32;
    actionTable[172][CONTINUE] = -32;
    actionTable[172][MINUS] = -32;
    actionTable[172][PLUS_PLUS] = -32;
    actionTable[172][MINUS_MINUS] = -32;
    actionTable[172][NOT] = -32;
    actionTable[172][TRUE] = -32;
    actionTable[172][FALSE] = -32;
    actionTable[172][CHARACTER] = -32;
    actionTable[172][STRINGVAL] = -32;
    actionTable[172][NUMERAL] = -32;
    actionTable[172][IDT] = -32;
}

