//
//  main.c
//  TrabalhoCompiladores
//

#include <iostream>
#include <stdio.h>
#include "analisador_lexico.h"
#include "analisador_sintatico.hpp"

extern FILE* program_file;

using namespace std;

int main(int argc, const char * argv[]) {
    if(argc > 1){
        program_file = fopen(argv[0],"r");
    }
    else{
        program_file = fopen("programa","r");
    }
    
    parse();
    
    cout << "SUCESSO! O programa funcionou corretamente =]" << endl;
}