//
//  analisador_escopo.hpp
//  TrabalhoCompiladores
//
//  Created by Arthur Fernandes on 10/30/15.
//  Copyright © 2015 Arthur Fernandes. All rights reserved.
//

#ifndef analisador_escopo_hpp
#define analisador_escopo_hpp

#include <stdio.h>
#include "analisador_lexico.h"
#include "analisador_sintatico.hpp"

#define MAX_NEST_LEVEL 50

/*object data structure*/
typedef struct object
{
    int nName;
    struct object *pNext;
} object, *pobject;

/*t_attrib data structure*/
typedef struct {
    t_nont nont;
    union {
        struct {
            pobject obj;
            int name;} IDD;
        struct {
            pobject obj;
            int name;} IDU;
        struct {
            pobject obj;
            int name;} ID;
    }_;
} t_attrib;

extern void semantics(int rule);

#endif /* analisador_escopo_hpp */
