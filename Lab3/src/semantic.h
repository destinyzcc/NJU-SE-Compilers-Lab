#ifndef _SEMANTIC_H
#define _SEMANTIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"

#define HASH_SIZE 16384
#define INT_TYPE 1
#define FLOAT_TYPE 2
#define TRUE 1

//from project_2
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

typedef struct Type_
{
    enum{BASIC, ARRAY, STRUCTURE, FUNCTION} kind;
    union 
    {
        //基本类型
        int basic;

        //数组类型信息包括元素类型与数组大小构成
        struct 
        {
            Type elem;
            int size;
        }array;

        //结构体类型信息是一个链表
        FieldList structure;
        
        //函数类型的信息包括参数链表，函数返回值类型，参数个数
        struct
        {
            FieldList params;
            Type returnType;
            int paramNum;
        }function;
    }u;
}Type_;

typedef struct FieldList_
{
    //域的名字
    char* name;
    //域的类型
    Type type;
    //下一个域
    FieldList tail;
}FieldList_;

//遍历
void traverseTree(Node* root);
FieldList VarDec(Node* root, Type basicType);
Type Specifier(Node* root);
Type Exp(Node* root);
void ExtDefList(Node* root);
void DefList(Node* root);
void CompSt(Node* root, Type funcType);
void Stmt(Node* root, Type funcType);

unsigned int hash_pjw(char* name);
void initHashTable();
int insertSymbol(FieldList f);
int TypeEqual(Type type1, Type type2);
FieldList findSymbol(char* name, int function);//int: function 1 varible 0
void allSymbols();

#endif