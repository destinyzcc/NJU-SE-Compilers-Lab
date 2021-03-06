#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "node.h"

Node* createNode(char* name, char* text){
    Node *pnode = (Node*)malloc(sizeof(Node));
    strcpy(pnode->name, name);
    strcpy(pnode->text, text);

    pnode->lineno = yylineno;
    pnode->parent =NULL;
    for(int i=0;i<MAX_CHILD_NUM;i++){
        pnode->child[i] = NULL;
    }
    pnode->childsum = 0;
    return pnode;
}


void addChild(int childsum, Node* parent, ...){
    va_list ap; 
    va_start(ap,parent);
    
    for(int i=0; i<childsum; i++){
        parent->child[i] = va_arg(ap, Node*);

    }
    parent->lineno = parent->child[0]->lineno;
    parent->childsum = childsum;
    va_end(ap);
}

void printTree(Node *parent, int blank){
    if(parent == NULL){
        return;
    }
    for(int i=0;i<blank;i++){
        fprintf(stderr, " ");
    }
    if(parent->childsum != 0){
        fprintf(stderr, "%s (%d)\n", parent->name, parent->lineno);
        for(int i=0; i< parent->childsum; i++){
            printTree(parent->child[i],blank+2);
        }
    }else{
        if(strcmp(parent->name, "INT")==0){
            fprintf(stderr, "%s: %d\n", parent->name, atoi(parent->text));
        }
        else if(strcmp(parent->name, "FLOAT")==0){
            fprintf(stderr, "%s: %f\n", parent->name, atof(parent->text));
        }
        else if(strcmp(parent->name, "ID")==0 || strcmp(parent->name, "TYPE")==0){
            fprintf(stderr, "%s: %s\n", parent->name, parent->text);
        }
        else{
            fprintf(stderr, "%s\n", parent->name);
        }
    }

}
