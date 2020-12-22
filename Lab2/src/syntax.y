%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "node.h"
    #include "lex.yy.c"
    extern int yylineno;
    void myerror(char *msg);
    extern Node* Root;
    extern int errorNum;
    extern int yyerror(char *msg);
%}

%union {
    struct Abstract_Tree* node;
}

//terminal token
%token <node> INT
%token <node> FLOAT
%token <node> ID
%token <node> SEMI
%token <node> COMMA
%token <node> ASSIGNOP 
%token <node> RELOP
%token <node> PLUS 
%token <node> MINUS 
%token <node> STAR 
%token <node> DIV
%token <node> AND 
%token <node> OR 
%token <node> NOT
%token <node> DOT
%token <node> TYPE
%token <node> LP 
%token <node> RP 
%token <node> LB 
%token <node> RB 
%token <node> LC 
%token <node> RC
%token <node> STRUCT
%token <node> RETURN
%token <node> IF 
%token <node> ELSE
%token <node> WHILE


//non-terminal type
%type <node> Program ExtDefList ExtDef ExtDecList
%type <node> Specifier StructSpecifier OptTag Tag
%type <node> VarDec FunDec VarList ParamDec
%type <node> CompSt StmtList Stmt
%type <node> DefList Def DecList Dec
%type <node> Exp Args

//association
%right ASSIGNOP  
%left OR 
%left AND 
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right UMINUS NOT
%left LP RP LB RB DOT

//noassociation
%nonassoc LOWER_THAN_ELSE 
%nonassoc ELSE



%%

//High-level Definitions

Program     : ExtDefList        {$$=createNode("Program","");addChild(1, $$, $1);Root=$$;}
            ;

ExtDefList  : ExtDef ExtDefList     {$$=createNode("ExtDefList","");addChild(2, $$, $1, $2);}
            |  /* empty*/           {$$=NULL;}
            ;
                
ExtDef      : Specifier ExtDecList SEMI     {$$=createNode("ExtDef","");addChild(3, $$, $1, $2, $3);}
            | Specifier SEMI                {$$=createNode("ExtDef","");addChild(2, $$, $1, $2);}    
            | Specifier FunDec CompSt       {$$=createNode("ExtDef","");addChild(3, $$, $1, $2, $3);}
            | error SEMI                    {errorNum++;}
            | error ExtDecList SEMI         {errorNum++;}
            | Specifier error SEMI          {errorNum++;}
            ;
ExtDecList  : VarDec                        {$$=createNode("ExtDecList","");addChild(1, $$, $1);} 
            | VarDec COMMA ExtDecList       {$$=createNode("ExtDecList","");addChild(3, $$, $1, $2, $3);} 
            ;

            

//Specifiers

Specifier   : TYPE                  {$$=createNode("Specifier","");addChild(1, $$, $1);}
            | StructSpecifier       {$$=createNode("Specifier","");addChild(1, $$, $1);}
            ;
            
StructSpecifier : STRUCT OptTag LC DefList RC   {$$=createNode("StructSpecifier","");addChild(5, $$, $1, $2, $3, $4, $5);}
                | STRUCT Tag                    {$$=createNode("StructSpecifier","");addChild(2, $$, $1, $2);}
                | STRUCT error LC DecList RC    {errorNum++;}
                | STRUCT OptTag LC error RC     {errorNum++;}
                ;
            
OptTag  : ID                {$$=createNode("OptTag","");addChild(1, $$, $1);}
        | /* empty*/        {$$=NULL;}
        ;
        
Tag     : ID                {$$=createNode("Tag","");addChild(1, $$, $1);}
        ;


//4.Declarators

VarDec      : ID                            {$$=createNode("VarDec","");addChild(1, $$, $1);}
            | VarDec LB INT RB              {$$=createNode("VarDec","");addChild(4, $$, $1, $2, $3, $4);}
            ;
            
FunDec      : ID LP VarList RP              {$$=createNode("FunDec","");addChild(4, $$, $1, $2, $3, $4);}
            | ID LP RP                      {$$=createNode("FunDec","");addChild(3, $$, $1, $2, $3);}
            | ID LP error RP                {errorNum++;}
            ;
            
VarList     : ParamDec COMMA VarList        {$$=createNode("VarList","");addChild(3, $$, $1, $2, $3);}
            | ParamDec                      {$$=createNode("VarList","");addChild(1, $$, $1);}
            ;
            
ParamDec    : Specifier VarDec              {$$=createNode("ParamDec","");addChild(2, $$, $1, $2);}
            ;

            
//Statements

CompSt      : LC DefList StmtList RC        {$$=createNode("CompSt","");addChild(4, $$, $1, $2, $3, $4);}
            ;
            
StmtList    : Stmt StmtList                 {$$=createNode("StmtList","");addChild(2, $$, $1, $2);}
            | /*empty*/                     {$$=NULL;}
            ;
            
Stmt    : Exp SEMI                                      {$$=createNode("Stmt","");addChild(2, $$, $1, $2);}
        | CompSt                                        {$$=createNode("Stmt","");addChild(1, $$, $1);}
        | RETURN Exp SEMI                               {$$=createNode("Stmt","");addChild(3, $$, $1, $2, $3);}
        | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE       {$$=createNode("Stmt","");addChild(5, $$, $1, $2, $3, $4, $5);}
        | IF LP Exp RP Stmt ELSE Stmt                   {$$=createNode("Stmt","");addChild(7, $$, $1, $2, $3, $4, $5, $6, $7);}
        | WHILE LP Exp RP Stmt                          {$$=createNode("Stmt","");addChild(5, $$, $1, $2, $3, $4, $5);}
        | error SEMI                                    {errorNum++;}
        ;


//Local Definitions

DefList : Def DefList               {$$=createNode("DefList","");addChild(2, $$, $1, $2);}
        | /*empty*/                 {$$=NULL;}
        ;
        
Def     : Specifier DecList SEMI    {$$=createNode("Def","");addChild(3, $$, $1, $2, $3);}
        | error SEMI                {errorNum++;}
        ;

DecList : Dec                       {$$=createNode("DecList","");addChild(1, $$, $1);}
        | Dec COMMA DecList         {$$=createNode("DecList","");addChild(3, $$, $1, $2, $3);}
        ;
        
Dec     : VarDec                    {$$=createNode("Dec","");addChild(1, $$, $1);}
        | VarDec ASSIGNOP Exp       {$$=createNode("Dec","");addChild(3, $$, $1, $2, $3);}
        ;
            
            
//Expressions
Exp     : Exp ASSIGNOP Exp      {$$=createNode("Exp","");addChild(3, $$, $1, $2, $3);}
        | Exp AND Exp           {$$=createNode("Exp","");addChild(3, $$, $1, $2, $3);}
        | Exp OR Exp            {$$=createNode("Exp","");addChild(3, $$, $1, $2, $3);}
        | Exp RELOP Exp         {$$=createNode("Exp","");addChild(3, $$, $1, $2, $3);}
        | Exp PLUS Exp          {$$=createNode("Exp","");addChild(3, $$, $1, $2, $3);}
        | Exp MINUS Exp         {$$=createNode("Exp","");addChild(3, $$, $1, $2, $3);}
        | Exp STAR Exp          {$$=createNode("Exp","");addChild(3, $$, $1, $2, $3);}
        | Exp DIV Exp           {$$=createNode("Exp","");addChild(3, $$, $1, $2, $3);}
        | LP Exp RP             {$$=createNode("Exp","");addChild(3, $$, $1, $2, $3);}
        | MINUS Exp %prec UMINUS{$$=createNode("Exp","");addChild(2, $$, $1, $2);}
        | NOT Exp               {$$=createNode("Exp","");addChild(2, $$, $1, $2);}
        | ID LP Args RP         {$$=createNode("Exp","");addChild(4, $$, $1, $2, $3, $4);}
        | ID LP RP              {$$=createNode("Exp","");addChild(3, $$, $1, $2, $3);}
        | Exp LB Exp RB         {$$=createNode("Exp","");addChild(4, $$, $1, $2, $3, $4);}
        | Exp DOT ID            {$$=createNode("Exp","");addChild(3, $$, $1, $2, $3);}
        | ID                    {$$=createNode("Exp","");addChild(1, $$, $1);}
        | INT                   {$$=createNode("Exp","");addChild(1, $$, $1);}
        | FLOAT                 {$$=createNode("Exp","");addChild(1, $$, $1);}
        | Exp LB error RB       {errorNum++;}
        | ID LP error RP        {errorNum++;}
        | LP error RP           {errorNum++;}
        ;
    
Args        : Exp COMMA Args        {$$=createNode("Args","");addChild(3, $$, $1, $2, $3);}
            | Exp                   {$$=createNode("Args","");addChild(1, $$, $1);}
            ;

%%

int yyerror(char *msg){
    fprintf(stderr, "Error type B at Line %d: %s.\n", yylineno, msg);
}
