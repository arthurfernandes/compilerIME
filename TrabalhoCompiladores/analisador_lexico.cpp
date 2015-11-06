#include "analisador_lexico.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unordered_map>
#include <string>
#include <iostream>

FILE *program_file;
t_const vConsts[MAX_CONSTS];
int nNumConsts = 0;
char nextChar = '\x20';
t_token token = UNKNOWN;
int tokenSecundario = 0;
bool hasTokenSecundario = false;
int currentLine = 1;
std::string tokenString = "";

std::unordered_map<std::string,int> tokenSecundarioIDMap;
std::unordered_map<std::string,t_token> t_tokenMap;
std::unordered_map<int,std::string> t_terminalNames;

void initializeTerminalNames(void){
    t_terminalNames[ARRAY] = "ARRAY";
    t_terminalNames[BOOLEAN] = "BOOLEAN";
    t_terminalNames[BREAK] = "BREAK";
    t_terminalNames[CHAR] = "CHAR";
    t_terminalNames[CONTINUE] = "CONTINUE";
    t_terminalNames[DO] = "DO";
    t_terminalNames[ELSE] = "ELSE";
    t_terminalNames[FALSE] = "FALSE";
    t_terminalNames[FUNCTION] = "FUNCTION";
    t_terminalNames[IF] = "IF";
    t_terminalNames[INTEGER] = "INTEGER";
    t_terminalNames[OF] = "OF";
    t_terminalNames[RETURN] = "RETURN";
    t_terminalNames[STRING] = "STRING";
    t_terminalNames[STRUCT] = "STRUCT";
    t_terminalNames[TRUE] = "TRUE";
    t_terminalNames[TYPE] = "TYPE";
    t_terminalNames[VAR] = "VAR";
    t_terminalNames[WHILE] = "WHILE";
    t_terminalNames[COLON] = "COLON";
    t_terminalNames[SEMI_COLON] = "SEMI_COLON";
    t_terminalNames[COMMA] = "COMMA";
    t_terminalNames[EQUALS] = "EQUALS";
    t_terminalNames[LEFT_SQUARE] = "LEFT_SQUARE";
    t_terminalNames[RIGHT_SQUARE] = "RIGHT_SQUARE";
    t_terminalNames[LEFT_BRACES] = "LEFT_BRACES";
    t_terminalNames[RIGHT_BRACES] = "RIGHT_BRACES";
    t_terminalNames[LEFT_PARENTHESIS] = "LEFT_PARENTHESIS";
    t_terminalNames[RIGHT_PARENTHESIS] = "RIGHT_PARENTHESIS";
    t_terminalNames[AND] = "AND";
    t_terminalNames[OR] = "OR";
    t_terminalNames[LESS_THAN] = "LESS_THAN";
    t_terminalNames[GREATER_THAN] = "GREATER_THAN";
    t_terminalNames[LESS_OR_EQUAL] = "LESS_OR_EQUAL";
    t_terminalNames[GREATER_OR_EQUAL] = "GREATER_OR_EQUAL";
    t_terminalNames[NOT_EQUAL] = "NOT_EQUAL";
    t_terminalNames[EQUAL_EQUAL] = "EQUAL_EQUAL";
    t_terminalNames[PLUS] = "PLUS";
    t_terminalNames[PLUS_PLUS] = "PLUS_PLUS";
    t_terminalNames[MINUS] = "MINUS";
    t_terminalNames[MINUS_MINUS] = "MINUS_MINUS";
    t_terminalNames[TIMES] = "TIMES";
    t_terminalNames[DIVIDE] = "DIVIDE";
    t_terminalNames[DOT] = "DOT";
    t_terminalNames[NOT] = "NOT";
    t_terminalNames[CHARACTER] = "CHARACTER";
    t_terminalNames[NUMERAL] = "NUMERAL";
    t_terminalNames[STRINGVAL] = "STRINGVAL";
    t_terminalNames[IDT] = "IDT";
    t_terminalNames[UNKNOWN] = "UNKNOWN";
}

void initializeTokenMap(void){
    static bool isInitialized = false;
    if(!isInitialized){
        initializeTerminalNames();
        t_tokenMap["array"]=ARRAY;
        t_tokenMap["boolean"] = BOOLEAN;
        t_tokenMap["break"] = BREAK;
        t_tokenMap["char"] = CHAR;
        t_tokenMap["continue"] = CONTINUE;
        t_tokenMap["do"] = DO;
        t_tokenMap["else"] = ELSE;
        t_tokenMap["false"] = FALSE;
        t_tokenMap["function"] = FUNCTION;
        t_tokenMap["if"] = IF;
        t_tokenMap["integer"] = INTEGER;
        t_tokenMap["of"] = OF;
        t_tokenMap["return"] = RETURN;
        t_tokenMap["string"] = STRING;
        t_tokenMap["struct"] = STRUCT;
        t_tokenMap["true"] = TRUE;
        t_tokenMap["type"] = TYPE;
        t_tokenMap["var"] = VAR;
        t_tokenMap["while"] = WHILE;
        isInitialized = true;
    }
}

int addCharConst(char c){
	t_const _const;
	_const._.cVal = c;
	vConsts[nNumConsts] = _const;
	return nNumConsts++;
}

int addIntConst(int n){
	t_const _const;
	_const._.nVal = n;
	vConsts[nNumConsts] = _const;
	return nNumConsts++;
}

int addStringConst(char* s){
	t_const _const;
	_const._.sVal = s;
	vConsts[nNumConsts] = _const;
	return nNumConsts++;
}

char getCharConst(int n){
	return vConsts[n]._.cVal;
}

int getIntConst(int n){
	return vConsts[n]._.nVal;
}

char* getStringConst(int n){
	return vConsts[n]._.sVal;
}

char readChar(){
	char c =  fgetc(program_file);
    if(c == '\n'){
        currentLine++;
    }
    return c;
}

t_token nextToken(void){
    tokenString = "";
    hasTokenSecundario = false;
    // loop do estado inicial para pular os separadores
    while(isspace(nextChar)){
        nextChar = readChar();
    }
    
    if(nextChar == EOF){
        token = ENDFILE;
        return token;
    }
    
    if( isalpha(nextChar) ){
        do {
            tokenString = tokenString + nextChar;
            nextChar = readChar();
        } while( isalnum(nextChar) || nextChar == '_' );
        token = searchKeyWord(tokenString);
        //std::cout << "Read:" << text << ", Token: " << token;
        if(token == IDT){
            tokenSecundario = searchName(tokenString);
            hasTokenSecundario = true;
            //std::cout << ", Token Secundario IDT " << tokenSecundario << std::endl;
        }
        else{
            //std::cout << std::endl;
        }
    }
    else if( isdigit(nextChar) ){
        char numeral[MAX_NUM_LEN+1];
        int i = 0;
        //std::cout << "Read: ";
        do{
            tokenString = tokenString + nextChar;
            numeral[i++] = nextChar;
            //std::cout << nextChar;
            nextChar = readChar();
        }while( isdigit(nextChar) );
        numeral[i] = '\0';
        token = NUMERAL;
        //std::cout << ", Token: " << token;
        tokenSecundario = addIntConst(atoi(numeral));
        hasTokenSecundario = true;
        //std::cout << ", Token Secundario Const: " << tokenSecundario << std::endl;
    } else if( nextChar == '"' ){
        tokenString = "";
        nextChar = readChar();
        while(nextChar != '"'){
            tokenString += nextChar;
            nextChar = readChar();
        }
        token = STRINGVAL;
        char* cstr = new char[tokenString.size()+1];
        memcpy(cstr, tokenString.c_str(), tokenString.size()+1);
        tokenSecundario = addStringConst(cstr);
        hasTokenSecundario = true;
        nextChar = readChar();
        //std::cout << "Read: " << tokenString << ", Token: " << token << ", Token Secundario Const: " << tokenSecundario << std::endl;
    }else if( nextChar == '\''){
        nextChar = readChar();
        tokenString = nextChar;
        token = CHARACTER;
        tokenSecundario = addCharConst(nextChar);
        hasTokenSecundario = true;
        //std::cout << "Read: " << '\'' << nextChar << '\'' << ", Token: " << token << ", Token Secundario Const: " << tokenSecundario << std::endl;
        nextChar = readChar(); //pular o '
        nextChar = readChar();
    }
    else{ //SIMBOLOS
        //std::cout << "Read: " << nextChar;
        tokenString = nextChar;
        switch(nextChar){
            case ':':
                nextChar = readChar();
                token = COLON;
                break;
            case ';':
                nextChar = readChar();
                token = SEMI_COLON;
                break;
            case ',':
                nextChar = readChar();
                token = COMMA;
                break;
            case '[':
                nextChar = readChar();
                token = LEFT_SQUARE;
                break;
            case ']':
                nextChar = readChar();
                token = RIGHT_SQUARE;
                break;
            case '{':
                nextChar = readChar();
                token = LEFT_BRACES;
                break;
            case '}':
                nextChar = readChar();
                token = RIGHT_BRACES;
                break;
            case '(':
                nextChar = readChar();
                token = LEFT_PARENTHESIS;
                break;
            case ')':
                nextChar = readChar();
                token = RIGHT_PARENTHESIS;
                break;
            case '&':
                nextChar = readChar();
                if(nextChar == '&'){
                    tokenString += nextChar;
                    token = AND;
                    nextChar = readChar();
                }
                else{
                    token = UNKNOWN;
                }
                break;
            case '|':
                nextChar = readChar();
                if(nextChar == '|'){
                    tokenString += nextChar;
                    token = OR;
                    nextChar = readChar();
                }
                else{
                    token = UNKNOWN;
                }
                break;
            case '=':
                nextChar = readChar();
                if( nextChar == '=' )
                {
                    tokenString += nextChar;
                    token = EQUAL_EQUAL;
                    nextChar = readChar();
                } else {
                    token = EQUALS;
                } break;
            case '<':
                nextChar = readChar();
                if( nextChar == '=' )
                {
                    tokenString += nextChar;
                    token = LESS_OR_EQUAL;
                    nextChar = readChar();
                } else {
                    token = LESS_THAN;
                } break;
            case '>':
                nextChar = readChar();
                if( nextChar == '=' )
                {
                    tokenString += nextChar;
                    token = GREATER_OR_EQUAL;
                    nextChar = readChar();
                } else {
                    token = GREATER_THAN;
                } break;
            case '!':
                nextChar = readChar();
                if( nextChar == '=' )
                {
                    tokenString += nextChar;
                    token = NOT_EQUAL;
                    nextChar = readChar();
                } else {
                    token = NOT;
                } break;
            case '+':
                nextChar = readChar();
                if( nextChar == '+' )
                {
                    tokenString += nextChar;
                    token = PLUS_PLUS;
                    nextChar = readChar();
                } else {
                    token = PLUS;
                } break;
            case '-':
                nextChar = readChar();
                if( nextChar == '-' )
                {
                    tokenString += nextChar;
                    token = MINUS_MINUS;
                    nextChar = readChar();
                } else {
                    token = MINUS;
                } break;
            case '*':
                nextChar = readChar();
                token = TIMES;
                break;
            case '/':
                nextChar = readChar();
                token = DIVIDE;
                break;
            case '.':
                nextChar = readChar();
                token = DOT;
                break;
            default:
                token = UNKNOWN;
        }
        //std::cout << ", Token: " << token << std::endl;
    }
    
    return token;
}

t_token searchKeyWord(std::string key){
    initializeTokenMap();
    if(t_tokenMap.count(key) == 1){
        return t_tokenMap[key];
    }
    else{
        return IDT;
    }
    
}

int searchName(std::string key){
    static int counter = 0;
    if(tokenSecundarioIDMap.count(key) == 1){
        return tokenSecundarioIDMap[key];
    }
    else{
        tokenSecundarioIDMap[key] = counter;
        return counter++;
    }
}