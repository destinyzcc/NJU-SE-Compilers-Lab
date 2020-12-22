# include <iostream>
# include <string>
# include <fstream>
# include <stdlib.h>
# include <list>

using namespace std;

extern "C"{
    #include "node.h"
    #include "semantic.h"
    void yyrestart(FILE *file);
    int yyparse();
}

Node* Root = NULL;
int errorNum = 0;

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
        initHashTable();
        //printTree(Root, 0);
        traverseTree(Root);
        //allSymbols();
    }
    return 0;
}
