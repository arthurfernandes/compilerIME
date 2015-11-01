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

#define IS_TYPE_KIND(k) ((k)==ARRAY_TYPE_ || \
(k)==STRUCT_TYPE_|| \
(k)==ALIAS_TYPE_ || \
(k)==SCALAR_TYPE_)

/*List of Available Types*/

typedef enum { NO_KIND_DEF_=-1, VAR_, PARAM_, FUNCTION_, FIELD_, ARRAY_TYPE_, STRUCT_TYPE_, ALIAS_TYPE_, SCALAR_TYPE_ , UNIVERSAL_} t_kind;

/*object data structure*/

typedef struct object
{
    int nName;
    struct object *pNext;
    t_kind eKind;
    
    union {
        struct {
            struct object *pType;
        } Var, Param, Field;
        struct {
            struct object *pRetType;
            struct object *pParams;
        } Function;
        struct {
            struct object *pElemType; int nNumElems;
        } Array;
        struct {
            struct object *pFields;
        } Struct;
        struct {
            struct object *pBaseType;
        } Alias;
    }_;
    
} object, *pobject;

/*t_attrib data structure*/
typedef struct {
    t_nont nont;
    union {
        struct {
            pobject obj;
            int name;}
        ID;
        struct {
            pobject type;
        } T;
        struct {
            pobject list;
        } LI,DC;
        struct {
            pobject type;
            bool val;
        } TRU,FALS;
        struct {
            pobject type;
            int pos;
            char val;
        } CHR;
        struct {
            pobject type;
            char* val;
            int pos;
        } STR;
        struct {
            pobject type;
            int val;
            int pos;
        } NUM;  
    }_;
} t_attrib;

extern void semantics(int rule);

#endif /* analisador_escopo_hpp */
