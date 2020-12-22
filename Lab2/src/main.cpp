# include <iostream>
# include <string>
# include <fstream>
# include <stdlib.h>
# include <list>

using namespace std;

extern "C"{
    #include "node.h"
    void yyrestart(FILE *file);
    int yyparse();
    void saveToList(char *temp, int yylineno, int flag, char *yytext);
    void saveRELOP_TYPE(char *temp, int yylineno, char *yytext);
    void saveNUM(char *temp, int yylineno, int num);
    void saveDOUBLE(char *temp, int yylineno, double num);
    void myerror(char *msg);
}

Node* Root = NULL;
int errorNum = 0;
list<string> error_l;
list<string> result_l;
int lineno = 0;

void saveToList(char *temp,int yylineno, int flag, char *yytext){
    if (flag==3){
        string re = temp + to_string(yylineno) + ": Mysterious character '" + yytext + "'.";
        error_l.push_back(re);
        return;
    } else if (flag==2){
        string re = temp + to_string(yylineno) + ": Illegal hexadecimal number '" + yytext + "'.";
        error_l.push_back(re);
        return;
    } else if (flag==1){
        string re = temp + to_string(yylineno) + ": Illegal octal number '" + yytext + "'.";
        error_l.push_back(re);
        return;
    }
    string re = temp + to_string(yylineno) + ".";
    result_l.push_back(re);
    return;
}

void saveRELOP_TYPE(char *temp, int yylineno, char *yytext){
    string t = yytext;
    string re = temp + t + " at Line " + to_string(yylineno) + ".";
    result_l.push_back(re);
    return;
}

void saveNUM(char *temp, int yylineno, int num){
    string re = temp + to_string(num) + " at Line " + to_string(yylineno) + ".";
    result_l.push_back(re);
    return;
}
void saveDOUBLE(char *temp, int yylineno, double num){
    string re = temp + to_string(num) + " at Line " + to_string(yylineno) + ".";
    result_l.push_back(re);
    return;
}

void myerror(char *msg){
    if(lineno != yylineno){
        fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, msg);
        lineno = yylineno;
    }
}

int main(int argc, char const **argv){
    if (argc<=1)
    {
        return 1;
    }
    FILE * fp = fopen(argv[1], "r");
    if (!fp){
        perror(argv[1]);
        return 1;
    }
    yyrestart(fp);
    yyparse();
    if (errorNum==0)
    {
        printTree(Root, 0);
    }
    //printTree(Root, 0);
    return 0;
}
