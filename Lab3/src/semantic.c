#include "node.h"
#include "semantic.h"

FieldList HashTable[HASH_SIZE];

//from project_2
unsigned int hash_pjw(char* name){
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff)
        {
            val = (val ^ (i >> 12)) & 0x3fff;
        }
        return val;
    }
}

//初始化散列表
void initHashTable(){

    for (int i = 0; i < HASH_SIZE; i++)
    {
        HashTable[i] = NULL;
    }
}

//插入符号
int insertSymbol(FieldList f){
    if (f == NULL || f->name == NULL)
    {
        return 0;
    }
    unsigned int key;
    key = hash_pjw(f->name);
    if (HashTable[key] == NULL)
    {
        HashTable[key] = f;
        return 1;
    }
    while (TRUE)
    {
        key = (++key);
        if (HashTable[key] == NULL)
        {
            HashTable[key] = f;
            return 1;
        }
    }
    return 0;
}

//寻找符号
FieldList findSymbol(char* name, int function){
    if (name == NULL)
    {
        return NULL;
    }
    unsigned int key;
    key = hash_pjw(name);
    FieldList flp = HashTable[key];
    while (flp != NULL)
    {
        if (strcmp(name, flp->name) == 0)
        {
            if ((function == 1) && (flp->type->kind==3))
            {//FUNCTION
                return flp;
            }
            if ((function == 0) && (flp->type->kind!=3))
            {//STRUCTURE/ARRAY/BASIC
                return flp;
            }
        }
        key = (++key);
        flp = HashTable[key];
    }
    return NULL;
}

//打印所有符号
void allSymbols(){
    for (int i = 0; i < HASH_SIZE; i++)
    {
        if (HashTable[i]!=NULL)
        {
            printf("name: %s, kind: %d ", HashTable[i]->name, HashTable[i]->type->kind);
            Type temp = HashTable[i]->type;
            while (temp->kind == 1)
            {
                printf("sise:[%d]", temp->u.array.size);
                temp = temp->u.array.elem;
            }
            printf("\n");
        }
    }
}

//比较类型是否等价
int TypeEqual(Type type1, Type type2){
    if ((type1==NULL) || (type2==NULL) || (type1->kind != type2->kind))
    {
        return 0;
    }
    switch (type1->kind)
    {
    case BASIC: //对于basic，直接比较basic
        {
            if (type1->u.basic == type2->u.basic)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        break;
    case ARRAY: //对于array，递归比较元素类型
        {
            if (TypeEqual(type1->u.array.elem, type2->u.array.elem)==1)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        break;
    case STRUCTURE: //对于structure，比较其每个域的类型是否一样
        {
            FieldList f1 = type1->u.structure;
            FieldList f2 = type2->u.structure;
            if (f1!=NULL && f2!=NULL)
            {
                while ((f1!=NULL) && (f2!=NULL))
                {
                    if (TypeEqual(f1->type, f2->type)==0)
                    {
                        return 0;
                    }
                    f1 = f1->tail;
                    f2 = f2->tail;
                }
                if ((f1==NULL) && (f2==NULL))
                {
                    return 1;
                }
            }
            return 0;
        }
        break;
    case FUNCTION:  //对于函数，先比较参数个数，相同的情况下逐个比较参数域的类型
        {
            if (type1->u.function.paramNum!=type2->u.function.paramNum)
            {
                return 0;
            }
            FieldList param1 = type1->u.function.params;
            FieldList param2 = type2->u.function.params;
            int paramN = type1->u.function.paramNum;
            for (int i = 0; i < paramN; i++)
            {
                if (TypeEqual(param1->type, param2->type) == 0)
                {
                    return 0;
                }
                param1 = param1->tail;
                param2 = param2->tail;
            }
            return 1;
        }
        break;
    default:
        {
            return 0;
        }
        break;
    }    
}

//VarDec返回数字或数组的FieldList
FieldList VarDec(Node* root, Type basicType){
    Node* temp = root;
    int i = 0;
    while (strcmp(temp->child[0]->name, "ID")!=0)
    {
        temp = temp->child[0];
        i++;
    }
    char* arrayName = temp->child[0]->text;

    FieldList fl = (FieldList) malloc(sizeof(FieldList_));
    fl->name = arrayName;
    if (i==0)   //i=0说明就是单个数字
    {
        fl->type = basicType;
        return fl;
    }
    else if (i==1)  //i>=1表面是数组 =1：一维   =2：二维···
    {//ID LB INT RB
        //printf("1\n");
        //printf("node: %s; line: %d\n", root->name, root->lineno);
        Type t = (Type) malloc(sizeof(Type_));
        t->kind = ARRAY;
        t->u.array.elem = basicType;
        t->u.array.size = atoi(root->child[2]->text);
        fl->type = t;
        return fl;
    }
    else
    {//i>1 二维及以上数组
        //printf("2\n");
        //printf("node: %s; line: %d\n", root->name, root->lineno);
        Node* tempp = root;
        Type t[HASH_SIZE];
        for (int j = 0; j < i; j++)
        {
            //printf("3\n");
            t[j] = (Type)malloc(sizeof(Type_));
            if (j==0)
            {
                //printf("3\n");
                t[j]->kind = ARRAY;
                t[j]->u.array.elem = basicType;
                t[j]->u.array.size = atoi(tempp->child[2]->text);
            }
            else
            {
                t[j]->kind = ARRAY;
                t[j]->u.array.elem = t[j-1];
                t[j]->u.array.size = atoi(tempp->child[2]->text);
            }
            tempp = tempp->child[0];
        }
        fl->type = t[i-1];
        return fl;
    }
}

//specifier识别int、float、struct,并同时将这些符号插入到符号表中（如果没有错误）
Type Specifier(Node* root){
    Type spe = (Type)malloc(sizeof(Type_));
    if (strcmp(root->child[0]->name, "TYPE")==0)
    {
        //Specifier->TYPE
        spe->kind = BASIC;
        if (strcmp(root->child[0]->text, "int")==0)
        {
            spe->u.basic = INT_TYPE;
        }
        else
        {
            spe->u.basic = FLOAT_TYPE;
        }
        return spe;
    }
    else    //Specifier->StructSpecifier
    {
        spe->kind = STRUCTURE;
        Node* structspecifier = root->child[0];
        if (structspecifier->childsum==2)
        {//StructSpecifier->STRUCT Tag
         //Tag->ID
            char* structName = structspecifier->child[1]->child[0]->text;
            FieldList fl = findSymbol(structName, 0);
            if (fl == NULL)
            {
                fprintf(stderr, "Error type 17 at Line %d: Undefined structure \"%s\".\n", root->lineno, structName);
                spe->u.structure = NULL;
                return spe;
            }
            else if (fl->type != NULL)
            {
                return fl->type;
            }
            spe->u.structure = NULL;
            return spe;
        }
        else
        {//StructSpecifier->STRUCT OptTag LC DefList RC
            Node* DefList = structspecifier->child[3];
            spe->u.structure = NULL;
            // FieldList fl = (FieldList)malloc(sizeof(FieldList_));
            // fl = NULL;
            FieldList* fl = &spe->u.structure;
            while(DefList!=NULL)
            {//DefList->Def DefList
                Node *Def=DefList->child[0];
                //Def->Specifier DecList SEMI
                Type basictype=Specifier(Def->child[0]);
                if (basictype==NULL)
                {
                    return NULL;
                }
                Node* DecList=Def->child[1];
                while(DecList->childsum==3)
                {//DecList->Dec COMMA DecList
                 //Dec->VarDec
                 //Dec->VarDec ASSIGNOP Exp
                    FieldList field=VarDec(DecList->child[0]->child[0],basictype);
                    if(DecList->child[0]->childsum==3)
                    {//在结构体中定义时对域进行初始化
                        fprintf(stderr, "Error type 15 at Line %d: Initialized field \"%s\".\n",DecList->child[0]->lineno,field->name);
                    }
                    FieldList temp=spe->u.structure;
                    //FieldList temp=fl;
                    while(temp!=NULL)
                    {
                        if(strcmp(temp->name,field->name)==0)
                        {//结构体中域名重复定义
                            fprintf(stderr, "Error type 15 at Line %d: Duplicated field name \"%s\".\n",DecList->child[0]->lineno,field->name);
                            break;
                        }
                        temp=temp->tail;
                    }
                    if(temp==NULL)
                    {
                        if(findSymbol(field->name,0)!=NULL)
                        {
                            fprintf(stderr, "Error type 3 at Line %d: Redefined variable \"%s\".\n",DecList->child[0]->lineno,field->name);
                        }
                        else
                        {
                            //printf("%s  %d\n", field->name, DecList->child[0]->child[0]->lineno);
                            //这里的tail是反过来的，想要实现正向tail，但是一直出现空指针，分配内存也不行，不知道为什么。
                            //已解决
                            // field->tail=spe->u.structure;
                            // spe->u.structure=field;
                            field->tail = NULL;
                            *fl = field;
                            fl = &(*fl)->tail;
                            insertSymbol(field);
                        }
                    }
                    DecList=DecList->child[2];//DecList->Dec COMMA DecList
                }
                FieldList field=VarDec(DecList->child[0]->child[0],basictype);
                if(DecList->child[0]->childsum==3)
                {
                    fprintf(stderr, "Error type 15 at Line %d: Initialized field \"%s\".\n",DecList->child[0]->lineno,field->name);
                }
                FieldList temp=spe->u.structure;
                while(temp!=NULL)
                {
                    //printf("node: %s; line: %d\n", root->name, root->lineno);
                    if(strcmp(temp->name,field->name)==0)
                    {
                        fprintf(stderr, "Error type 15 at Line %d: Redefined field \"%s\".\n",DecList->child[0]->lineno,field->name);
                        break;
                    }
                    temp=temp->tail;
                }
                if(temp==NULL)
                {
                    if(findSymbol(field->name,0)!=NULL)
                    {
                        fprintf(stderr, "Error type 3 at Line %d: Redefined variable \"%s\".\n",DecList->child[0]->lineno,field->name);
                    }
                    else
                    {
                        //printf("%s  %d\n", field->name, DecList->child[0]->child[0]->lineno);
                        //这里的tail是反过来的，想要实现正向tail，但是一直出现空指针，分配内存也不行，不知道为什么。
                        //已解决
                        // field->tail=spe->u.structure;
                        // spe->u.structure = field;
                        field->tail = NULL;
                        *fl = field;
                        fl = &(*fl)->tail;
                        //spe->u.structure->tail = field;
                        insertSymbol(field);
		            }
                }
                DefList=DefList->child[1];
            }
            // FieldList flt = spe->u.structure;
            // while (flt!=NULL)
            // {
            //     printf("%s\n",flt->name);
            //     flt = flt->tail;
            // }

            if(structspecifier->child[1]!=NULL)
            {//OptTag exist
                FieldList field=(FieldList)malloc(sizeof(FieldList_));
                field->type=spe;
                //StructSpecifier->STRUCT OptTag LC DefList RC
                //OptTag->ID
                char* OptTagName = structspecifier->child[1]->child[0]->text;
                field->name = OptTagName;
                if(findSymbol(field->name,0) != NULL)
                {
                    fprintf(stderr, "Error type 16 at Line %d: Duplicated name \"%s\".\n",structspecifier->lineno,field->name);
                }
                else 
                {
                    insertSymbol(field);
                }
            }
            return spe;
        }
    }
}

//入口
void ExtDefList(Node *root){
    Node* ExtDefList=root;
    while(ExtDefList->childsum!=0)
    {//ExtDefList->ExtDef ExtDefList
        Node* ExtDef=ExtDefList->child[0];
        //printf("node: %s; line: %d\n", ExtDef->name, ExtDef->lineno);
        Type basictype=Specifier(ExtDef->child[0]);
        
        if(strcmp(ExtDef->child[1]->name,"ExtDecList")==0)
        {//ExtDef->Specifier ExtDecList SEMI
            Node* temp=ExtDef->child[1];//ExtDecList
            FieldList field;
            while(temp->childsum==3)
            {//ExtDecList->VarDec COMMA ExtDecList
                field=VarDec(temp->child[0],basictype);
                if(findSymbol(field->name,0)!=NULL)
                {
                    fprintf(stderr, "Error type 3 at Line %d: Redefined variable \"%s\".\n",ExtDef->lineno,field->name);
                }
                else 
                {
                    insertSymbol(field);
                }
                temp=temp->child[2];
            }
            field=VarDec(temp->child[0],basictype);
            if(findSymbol(field->name,0)!=NULL)
            {
                fprintf(stderr, "Error type 3 at Line %d: Redefined variable \"%s\".\n",ExtDef->lineno,field->name);
            }
            else
            {
                insertSymbol(field);
            }
        }
        else if(strcmp(ExtDef->child[1]->name,"FunDec")==0)
        {//ExtDef->Specifier FunDec CompSt
            FieldList field=(FieldList)malloc(sizeof(FieldList_));
            //FunDec->ID LP VarList RP 
            //FunDec->ID LP RP 
            field->name=ExtDef->child[1]->child[0]->text;
            Type typ=(Type)malloc(sizeof(Type_));
            typ->kind=FUNCTION;
            typ->u.function.returnType=basictype;
            typ->u.function.paramNum=0;
            typ->u.function.params=NULL;

            if(strcmp(ExtDef->child[1]->child[2]->name,"VarList")==0)
            {//FunDec->ID LP VarList RP
                Node *VarList=ExtDef->child[1]->child[2];
                FieldList *fl = &typ->u.function.params;
                while(VarList->childsum==3)
                {//VarList->ParamDec COMMA VarList
                    Type tempType=Specifier(VarList->child[0]->child[0]);
                    FieldList tempField=VarDec(VarList->child[0]->child[1],tempType);
                    if(findSymbol(tempField->name,0)!=NULL)
                    {
                        fprintf(stderr, "Error type 3 at Line %d: Redefined variable \"%s\".\n",VarList->child[0]->child[1]->lineno,tempField->name);
                    }
                    else
                    {
                        //printf("%s %d\n", tempField->name, VarList->lineno);
                        insertSymbol(tempField);
                    }
                    //这里的tail是反过来的，想要实现正向tail，但是一直出现空指针，分配内存也不行，不知道为什么。
                    //已解决
                    typ->u.function.paramNum++;
                    // tempField->tail=typ->u.function.params;
                    // typ->u.function.params=tempField;
                    tempField->tail = NULL;
                    *fl = tempField;
                    fl = &((*fl)->tail);
                    VarList=VarList->child[2];
                }//VarList->ParamDec
                Type tempType=Specifier(VarList->child[0]->child[0]);
                FieldList tempField=VarDec(VarList->child[0]->child[1],tempType);
                if(findSymbol(tempField->name,0)!=NULL)
                {
                    fprintf(stderr, "Error type 3 at Line %d: Redefined variable \"%s\".\n",VarList->child[0]->child[1]->lineno,tempField->name);
                }
                else
                {
                    //printf("%s %d\n", tempField->name, VarList->lineno);
                    insertSymbol(tempField);
                }
                //这里的tail是反过来的，想要实现正向tail，但是一直出现空指针，分配内存也不行，不知道为什么。
                //已解决
                typ->u.function.paramNum++;
                // tempField->tail=typ->u.function.params;
                // typ->u.function.params=tempField;
                tempField->tail = NULL;
                *fl = tempField;
                fl = &((*fl)->tail);
            }
            // FieldList fl = typ->u.function.params;
            // while (fl!=NULL)
            // {
            //     printf("%s\n", fl->name);
            //     fl = fl->tail;
            // }
            
            field->type=typ;
            if(findSymbol(field->name,1)!=NULL)
            {
                fprintf(stderr, "Error type 4 at Line %d: Redefined function \"%s\".\n",ExtDef->child[1]->child[0]->lineno,field->name);
            }
            else 
            {
                insertSymbol(field);
            }
            //CompSt->LC DefList StmtList RC
            CompSt(ExtDef->child[2],basictype);
        }
        else
        {//ExtDef->Specifier SIMI
            //do nothing
        }

        if(ExtDefList->child[1]==NULL)//ExtDef
        {
            return;
        }
        ExtDefList=ExtDefList->child[1];
    }
}

//{}
void CompSt(Node *root,Type returnType){
    //CompSt->LC DefList StmtList RC
    Node *CompSt=root;
    DefList(CompSt->child[1]);
    Node *StmtList=CompSt->child[2];
    while(StmtList!=NULL)
    {
        //StmtList->Stmt StmtList
        Node *Stmt_=StmtList->child[0];
        Stmt(Stmt_,returnType);
        StmtList=StmtList->child[1];
    }
}

//处理DefList
void DefList(Node *root){
    Node* DefList=root;
    while(DefList!=NULL)
    {//DefList->Def DefList
        Node* Def=DefList->child[0];
        //printTree(Def->child[0], 0);
        Type basictype=Specifier(Def->child[0]);
        Node *DecList=Def->child[1];
        while(DecList->childsum==3)
        {//DecList->Dec COMMA DecList
            FieldList field=VarDec(DecList->child[0]->child[0],basictype);
            if(findSymbol(field->name,0)!=NULL)
            {
                fprintf(stderr, "Error type 3 at Line %d: Redefined variable \"%s\".\n",DecList->child[0]->child[0]->lineno,field->name);
            }
            else 
            {
                insertSymbol(field);
            }
            DecList=DecList->child[2];
        }
        //printf("1\n");
        Node* Dec = DecList->child[0];
        FieldList field=VarDec(DecList->child[0]->child[0],basictype);
        if(findSymbol(field->name,0)!=NULL)
        {
            fprintf(stderr, "Error type 3 at Line %d: Redefined variable \"%s\".\n",DecList->child[0]->child[0]->lineno,field->name);
        }
        else
        {
            insertSymbol(field);
        }
        if (Dec->childsum==3)
        {//Dec->VarDec ASSIGNOP Exp
            Node* Expression = Dec->child[2];
            Type t = Exp(Expression);
            //先insertSymbol再判断Exp   (int a=a+1 不报错)
            if (t!=NULL)
            {
                if (TypeEqual(t, basictype)==0)
                {
                    fprintf(stderr, "Error type 5 at Line %d: Type mismatched for assignment.\n",Expression->lineno);
                }
            }
        }
        if(DefList->child[1]==NULL)//DefList->Def
        {
            return;
        }
        DefList=DefList->child[1];
    }
}

//处理Stmt
void Stmt(Node *root,Type returnType){
    Node *Stmt_=root;
    if(strcmp(Stmt_->child[0]->name,"RETURN")==0)
    {//Stmt->RETURN Exp SEMI
        Type returntype=Exp(Stmt_->child[1]);
        if (returntype!=NULL)
        {
            if(TypeEqual(returnType,returntype)==0)
            {
                fprintf(stderr, "Error type 8 at Line %d: Return type mismatch.\n",Stmt_->child[1]->lineno);
            }
        }
    }
    else if(strcmp(Stmt_->child[0]->name,"Exp")==0)
    {//Stmt->Exp SEMI
        Exp(Stmt_->child[0]);
    }
    else if(strcmp(Stmt_->child[0]->name,"CompSt")==0)
    {//Stmt->CompSt
        CompSt(Stmt_->child[0],returnType);
    }
    else if(strcmp(Stmt_->child[0]->name,"WHILE")==0)
    {//Stmt->WHILE LP Exp RP Stmt
        Type typ=Exp(Stmt_->child[2]);
        if (typ!=NULL)
        {
            if(!((typ->kind==BASIC)&&(typ->u.basic==INT_TYPE)))
            {
                //printf("typeEqual Error1\n");
                fprintf(stderr, "Error type 5 at Line %d: Only type INT could be used for judgement.\n",Stmt_->child[2]->lineno);
            }
        }
        Stmt(Stmt_->child[4],returnType);
    }
    else if(Stmt_->childsum<6)
    {//Stmt->IF LP Exp RP Stmt
        Type typ=Exp(Stmt_->child[2]);
        if(typ!=NULL)
        {
            if(!((typ->kind==BASIC)&&(typ->u.basic==INT_TYPE)))
            {
                //printf("typeEaual Error2\n");
                fprintf(stderr, "Error type 5 at Line %d: Only type INT could be used for judgement.\n",Stmt_->child[2]->lineno);
            }
        }
        Stmt(Stmt_->child[4],returnType);
    }
    else
    {//Stmt->IF LP Exp RP Stmt ELSE Stmt
        Type typ=Exp(Stmt_->child[2]);
        if (typ!=NULL)
        {
            if(!((typ->kind==BASIC)&&(typ->u.basic==INT_TYPE)))
            {
                //printf("typeEaual Error3\n");
                fprintf(stderr, "Error type 5 at Line %d: Only type INT could be used for judgement.\n",Stmt_->child[2]->lineno);
            }
        }
        Stmt(Stmt_->child[4],returnType);
        Stmt(Stmt_->child[6],returnType);
    }
}

//处理Exp，返回类型
Type Exp(Node* root){
    if(root==NULL)
    {
        return NULL;
    }
    else if((strcmp(root->child[0]->name,"ID")==0)&&(root->childsum==1)) 
    {//Exp->ID
        FieldList field=findSymbol(root->child[0]->text,0);
        if(field!=NULL)
        {
            return field->type;
        }
        else
        {
            fprintf(stderr, "Error type 1 at Line %d: Undefined variable \"%s\".\n",root->lineno,root->child[0]->text);
            return NULL;
        }
    }
    else if(strcmp(root->child[0]->name,"INT")==0)
    {//Exp->INT
        Type typ=(Type)malloc(sizeof(Type_));
        typ->kind=BASIC;
        typ->u.basic=INT_TYPE;
        return typ;
    }
    else if(strcmp(root->child[0]->name,"FLOAT")==0)
    {//Exp->FLOAT
        Type typ=(Type)malloc(sizeof(Type_));
        typ->kind=BASIC;
        typ->u.basic=FLOAT_TYPE;
        return typ;
    }
    else if((strcmp(root->child[0]->name,"LP")==0) ||
            (strcmp(root->child[0]->name,"MINUS")==0) ||
            (strcmp(root->child[0]->name,"NOT")==0))
    {//Exp->LP Exp RP
     //Exp->MINUS Exp
     //Exp->NOT Exp
        return Exp(root->child[1]);
    }
    else if((strcmp(root->child[1]->name,"PLUS")==0) ||
            (strcmp(root->child[1]->name,"MINUS")==0) ||
            (strcmp(root->child[1]->name,"STAR")==0) ||
            (strcmp(root->child[1]->name,"DIV")==0))
    {//Exp->Exp PLUS Exp
     //Exp->Exp MINUS Exp
     //Exp->Exp STAR Exp
     //Exp->Exp DIV Exp
     //数字运算，需要比较操作符两边的操作数类型是否匹配，返回的类型为其中一个的类型
        Type typ1=Exp(root->child[0]);
        Type typ2=Exp(root->child[2]);
        if(TypeEqual(typ1,typ2)==0)
        {
            if((typ1!=NULL)&&(typ2!=NULL))
            {
                fprintf(stderr, "Error type 7 at Line %d: Operand type mismatch.\n",root->lineno);
            }
            return NULL;
        }
        else return typ1;
    }
    else if((strcmp(root->child[1]->name,"AND")==0) ||
            (strcmp(root->child[1]->name,"OR")==0) ||
            (strcmp(root->child[1]->name,"RELOP")==0))
    {//Exp->Exp AND Exp
     //Exp->Exp OR Exp
     //Exp->Exp RELOP Exp
     //逻辑运算，需要比较操作符两边的操作数类型是否匹配，返回值为INT
        Type typ1=Exp(root->child[0]);
        Type typ2=Exp(root->child[2]);
        if(TypeEqual(typ1,typ2)==0)
        {
            if((typ1!=NULL)&&(typ2!=NULL))
            {
                fprintf(stderr, "Error type 7 at Line %d: Operand type mismatch.\n",root->lineno);
            }
            return NULL;
        }
        else
        {
            Type typ=(Type)malloc(sizeof(Type_));
            typ->kind=BASIC;
            typ->u.basic=INT_TYPE;
            return typ;
	    }
    }
    else if(strcmp(root->child[1]->name,"ASSIGNOP")==0)
    {//Exp->Exp ASSIGNOP Exp
        if(root->child[0]->childsum==1)
        {//Exp->ID
         //Exp->INT 报错
         //Exp->FLOAT 报错
            //printf("name:%s\n",root->child[0]->child[0]->name);
            if(!(strcmp(root->child[0]->child[0]->name,"ID")==0))
            {
                fprintf(stderr, "Error type 6 at Line %d: Assign a value to a right-hand-only expression.\n",root->lineno);
                return NULL;
            }
        }
        else if (root->child[0]->childsum==2)
        {//Exp->MINUS Exp 报错
         //Exp->NOT Exp 报错
            fprintf(stderr, "Error type 6 at Line %d: Assign a value to a right-hand-only expression.\n",root->lineno);
            return NULL;
        }
        else if(root->child[0]->childsum==3)
        {//Exp->Exp PLUS Exp
         //Exp->Exp MINUS Exp
         //Exp->Exp STAR Exp
         //Exp->Exp DIV Exp
         //Exp->Exp ASSIGNOP Exp
         //Exp->Exp AND Exp
         //Exp->Exp OR Exp
         //Exp->Exp RELOP Exp
         //Exp->LP Exp RP
         //Exp->ID LP RP
         //Exp->Exp DOT ID 不报错
            if(!((strcmp(root->child[0]->child[0]->name,"Exp")==0) &&
                 (strcmp(root->child[0]->child[1]->name,"DOT")==0) &&
                 (strcmp(root->child[0]->child[2]->name,"ID")==0)))
            {
                fprintf(stderr, "Error type 6 at Line %d: Assign a value to a right-hand-only expression.\n",root->lineno);
                return NULL;
            }
        }
        else if(root->child[0]->childsum==4)
        {//Exp->ID LP Args RP 报错
         //Exp->Exp LB Exp RB
            if(!((strcmp(root->child[0]->child[0]->name,"Exp")==0) &&
                 (strcmp(root->child[0]->child[1]->name,"LB")==0) &&
                 (strcmp(root->child[0]->child[2]->name,"Exp")==0) &&
                 (strcmp(root->child[0]->child[3]->name,"RB")==0)))
            {
                fprintf(stderr, "Error type 6 at Line %d: Assign a value to a right-hand-only expression.\n",root->lineno);
                return NULL;
            }
        }
        Type typ1=Exp(root->child[0]);
        if (typ1==NULL)
        {
            return NULL;
        }
        Type typ2=Exp(root->child[2]);
        // if (root->child[2]->childsum!=0)
        // {
        //     if (root->child[2]->child[0]->child[1]!=0)
        //     {
        //         printf("name:%s val:%d\n", root->child[2]->child[0]->child[1]->child[0]->name, root->child[2]->child[0]->child[1]->child[0]->text);
        //     }
        // }
        //printf("assignop lineno: %d\n", root->child[1]->lineno);
        //printf("exp1: %d   \n", typ1->u.array.size);
        //printf("exp2: %d   \n", typ2->u.array.size);
        // if (typ1->kind==1)
        // {
        //     printf("exp1: %d   \n", typ1->u.array.size);
        // }
        // if (typ2->kind==1)
        // {
        //     printf("exp2: %d   \n", typ2->u.array.size);
        // }
        
        if(TypeEqual(typ1,typ2)==0)
        {
            if((typ1!=NULL)&&(typ2!=NULL))
            {
                //printf("typeEaual Error4\n");
                fprintf(stderr, "Error type 5 at Line %d: Type mismatched for assignment.\n",root->lineno);
            }
            return NULL;
        }
        else 
        {
            return typ1;
        }
    }
    else if(strcmp(root->child[0]->name,"ID")==0)
    {//Exp->ID LP RP
     //Exp->ID LP Args RP
        FieldList field=findSymbol(root->child[0]->text,1);
        if(field==NULL)
        {
            FieldList field2=findSymbol(root->child[0]->text,0);
            if(field2!=NULL)
            {
                fprintf(stderr, "Error type 11 at Line %d: Illegal use of operator \"()\".\n",root->lineno);
            }
            else
            {
                fprintf(stderr, "Error type 2 at Line %d: Undefined function \"%s\".\n",root->lineno,root->child[0]->text);
            }
            return NULL;
        }
        Type definedType=field->type;

        Type typ=(Type)malloc(sizeof(Type_));
        typ->kind=FUNCTION;
        typ->u.function.paramNum=0;
        typ->u.function.params=NULL;
        FieldList *fl = &typ->u.function.params;
        if(strcmp(root->child[2]->name,"RP")!=0)
        {//Exp->ID LP Args RP
            Node* temp=root->child[2];  //Args
            while(temp->childsum==3)
            {//Args->Exp COMMA Args
                Type tempType=Exp(temp->child[0]);
                if (tempType==NULL)
                {
                    return NULL;
                }
                FieldList tempField=(FieldList)malloc(sizeof(FieldList_));
                tempField->name="argv";
		        tempField->type=tempType;
                //这里的tail是反过来的，想要实现正向tail，但是一直出现空指针，分配内存也不行，不知道为什么。
                //已解决
                typ->u.function.paramNum++;
                // tempField->tail=typ->u.function.params;
                // typ->u.function.params=tempField;
                tempField->tail = NULL;
                *fl = tempField;
                //printf("%s\n", (*fl)->name);
                fl = &((*fl)->tail);
                temp=temp->child[2];
            }//Args->Exp
            Type tempType=Exp(temp->child[0]);
            if (tempType==NULL)
            {
                return NULL;
            }
            FieldList tempField=(FieldList)malloc(sizeof(FieldList_));
            tempField->name="argv";
	        tempField->type=tempType;
            //这里的tail是反过来的，想要实现正向tail，但是一直出现空指针，分配内存也不行，不知道为什么。
            //已解决
            typ->u.function.paramNum++;
            // tempField->tail=typ->u.function.params;
            // typ->u.function.params=tempField;
            tempField->tail = NULL;
            *fl = tempField;
            //printf("%s\n", (*fl)->name);
            fl = &((*fl)->tail);
            
        }
        // FieldList flt  = typ->u.function.params;
        // while (flt!=NULL)
        // {
        //     printf("%s\n", flt->name);
        //     flt  = flt->tail;
        // }
        if(TypeEqual(typ,definedType)==0)
        {
            fprintf(stderr, "Error type 9 at Line %d: Arguments mismatch in function \"%s\".\n",root->lineno,root->child[0]->text);
            return NULL;
        }
        else
        {
            return definedType->u.function.returnType;
        }
    }
    else if(strcmp(root->child[1]->name,"DOT")==0)
    {//Exp->Exp DOT ID
        Type typ1=Exp(root->child[0]);
        if (typ1!=NULL)
        {
            if(typ1->kind!=STRUCTURE)
            {
                fprintf(stderr, "Error type 13 at Line %d: Illegal use of operator \".\".\n",root->lineno);
                return NULL;
            }
            else
            {
                char *s=root->child[2]->text;
                FieldList temp=typ1->u.structure;
                while(temp!=NULL)
                {
                    if(strcmp(temp->name,s)==0)
                    {
                        return temp->type;
                    }
                    temp=temp->tail;
                }
                fprintf(stderr, "Error type 14 at Line %d: Non-existent field \"%s\".\n",root->lineno,root->child[2]->text);
                return NULL;
            }
        }
        else
        {
            return NULL;
        }
    }
    else if(strcmp(root->child[1]->name,"LB")==0)
    {//Exp->Exp LB Exp RB
        Type typ1=Exp(root->child[0]);
        if (typ1==NULL)
        {
            //printf("null\n");
            return NULL;
        }
        if(typ1->kind!=ARRAY)
        {
            // printf("%d\n",typ1->kind);
            // printf("enter\n");
            // printf("root: name:%s   line:%d\n", root->name, root->lineno);
            //printTree(root, 0);
            fprintf(stderr, "Error type 10 at Line %d: Illegal use of of \"[]\" operator.\n",root->lineno);
            return NULL;
        }
        //printf("array:\n");
        Type temp=Exp(root->child[2]);
        if (temp==NULL)
        {
            return NULL;
        }
        
        if(temp->kind!=BASIC)
        {
            fprintf(stderr, "Error type 12 at Line %d: Array index is not an integer.\n",root->lineno);
            return NULL;
        }
        else if(temp->u.basic==FLOAT_TYPE)
        {
            fprintf(stderr, "Error type 12 at Line %d: Array index is not an integer.\n",root->lineno);
            return NULL;
        }
        // if (root->child[0]->lineno==3)
        // {
        //     printf("kind:%d\n", typ1->kind);
        //     printTree(root,0);
        // }
        
        return typ1->u.array.elem;
    }
    else
    {
        return NULL;
    }
}
