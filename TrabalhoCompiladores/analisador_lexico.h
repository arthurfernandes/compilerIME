#ifndef ANALISADOR_LEXICO
#define ANALISADOR_LEXICO

#include <stdio.h>
#include <iostream>
#include <string>
#include <unordered_map>

#define MAX_CONSTS 500
#define MAX_ID_LEN 100
#define MAX_NUM_LEN 15
#define MAX_STR_LEN 30

typedef enum{
// palavras reservadas
ARRAY, BOOLEAN, BREAK, CHAR, CONTINUE, DO, ELSE, FALSE, FUNCTION, IF, INTEGER, OF, RETURN,STRING, STRUCT, TRUE, TYPE, VAR, WHILE,
// simbolos
COLON, SEMI_COLON, COMMA, EQUALS, LEFT_SQUARE, RIGHT_SQUARE, LEFT_BRACES, RIGHT_BRACES, LEFT_PARENTHESIS, RIGHT_PARENTHESIS, AND, OR, LESS_THAN, GREATER_THAN, LESS_OR_EQUAL, GREATER_OR_EQUAL, NOT_EQUAL, EQUAL_EQUAL, PLUS, PLUS_PLUS, MINUS, MINUS_MINUS, TIMES, DIVIDE, DOT, NOT,
// tokens regulares
CHARACTER, NUMERAL, STRINGVAL, IDT,
// token deconhecido
UNKNOWN,
// final de arquivo
ENDFILE
} t_token;

typedef struct T_CONST{
public:
	int type; //0-char, 1-int,2-string
	union VAL{
		char cVal;
		int nVal;
        char* sVal;
	} _;
} t_const;

//Variáveis Globais
extern int currentLine;
extern t_const vConsts[MAX_CONSTS];
extern int nNumConsts;
extern char nextChar;
extern t_token token;
extern int tokenSecundario;
extern bool hasTokenSecundario;
extern std::string tokenString;
extern FILE* program_file;
extern std::unordered_map<std::string,int> tokenSecundarioIDMap;
extern std::unordered_map<std::string,t_token> t_tokenReservedWordsMap;
extern std::unordered_map<int,std::string> t_terminalNames;

//Métodos
int addCharConst(char c);
int addIntConst(int n);
int addStringConst(char* s);
char getCharConst(int n);
int getIntConst(int n);
char* getStringConst(int n);
char readChar();
t_token nextToken(void);
t_token searchKeyWord(std::string);
int searchName(std::string);

#endif
