# include <iostream>
# include <string>
# include <fstream>
# include <stdlib.h>
# include <list>


using namespace std;

extern "C"{
    void yyrestart(FILE *file);
    int yyparse();
    void saveToList(char *temp, int yylineno, int flag, char *yytext);
    void saveRELOP_TYPE(char *temp, int yylineno, char *yytext);
    void saveNUM(char *temp, int yylineno, int num);
    void saveDOUBLE(char *temp, int yylineno, double num);
}

int errorNum = 0;
list<string> error_l;
list<string> result_l;

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
    list<string> re_l = error_l;
    if (errorNum==0){
        re_l=result_l;
    }
    list<string>::iterator iter = re_l.begin();
    for (iter; iter!=re_l.end(); iter++) {
        cerr<<*iter<<endl;
    }
    return 0;
}
