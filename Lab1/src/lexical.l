%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    extern int errorNum;
    extern void saveToList(char *temp, int yylineno, int flag, char *yytext);
    extern void saveRELOP_TYPE(char *temp, int yylineno, char *yytext);
    extern void saveNUM(char *temp, int yylineno, int num);
    extern void saveDOUBLE(char *temp, int yylineno, double num);
%}
%option yylineno


SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS "+"
MINUS "-"
COMMENT \/\/.*
COMMENTT \/\*(?:[^\*]|\*+[^\/\*])*\*+\/
STAR "*"
DIV "/"
AND "&&"
OR "||"
NOT "!"
DOT "."
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
SPACE [ \r\n\t]+
DEC ([1-9][0-9]*)|0
OCT 0[0-7]+
HEX 0[Xx][0-9a-fA-F]+
INT {DEC}|{OCT}|{HEX}
FLOAT [0-9]*[.][0-9]*[eE]?[+|-]*[0-9]+
ID [A-Za-z_][A-Za-z_0-9]*
OTHER .

%%
{SEMI}	{
    char *temp = "SEMI ; at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{COMMA} {
    char *temp = "COMMA , at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{ASSIGNOP} {
    char *temp = "ASSIGNOP = at Line ";
    saveToList(temp, yylineno, 0, yytext);
}


{RELOP}     {
	char *temp = "RELOP ";
    saveRELOP_TYPE(temp, yylineno, yytext);
}

{PLUS}      {
    char *temp = "PLUS + at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{MINUS}     {
    char *temp = "MINUS - at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{COMMENT}	{
}
{COMMENTT}  {
}
{STAR}      {
    char *temp = "STAR * at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{DIV}       {
    char *temp = "DIV / at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{AND}       {
    char *temp = "AND && at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{OR}        {
    char *temp = "OR || at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{DOT}       {
    char *temp = "DOT . at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{NOT}       {
    char *temp = "NOT ! at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{TYPE}      {
    char *temp = "TYPE ";
    saveRELOP_TYPE(temp, yylineno, yytext);
}
{LP}        {
    char *temp = "LP ( at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{RP}        {
    char *temp = "RP ) at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{LB}        {
    char *temp = "LB [ at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{RB}        {
    char *temp = "RB ] at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{LC}        {

    char *temp = "LC { at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{RC}        {
    char *temp = "RC } at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{STRUCT}    {
    char *temp = "STRUCT struct at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{RETURN}    {
    char *temp = "RETURN return at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{IF}        {
    char *temp = "IF if at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{ELSE}      {
    char *temp = "ELSE else at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{WHILE}     {
    char *temp = "WHILE while at Line ";
    saveToList(temp, yylineno, 0, yytext);
}
{SPACE}     {}


{OCT}       {
    char *end = NULL;
    int l = (int)strtol(yytext, &end, 8);
    char *temp = "INT ";
    saveNUM(temp, yylineno, l);
}
{HEX}       {
    char *end = NULL;
    int l = (int)strtol(yytext, &end, 16);
    char *temp = "INT ";
    saveNUM(temp, yylineno, l);
}
{INT}       {
    char *temp = "INT ";
    saveRELOP_TYPE(temp, yylineno, yytext);
}
{FLOAT}     {
    char *temp = "FLOAT ";
    saveDOUBLE(temp, yylineno, atof(yytext));
}

{ID}        {
    char *temp = "ID ";
    saveRELOP_TYPE(temp, yylineno, yytext);
}
{OTHER} {
    errorNum++;
    char *temp = "Error type A at Line ";
    saveToList(temp, yylineno, 3, yytext);
}

%%

int yywrap() {
    return 1;
}
