#include <assert.h>

#include "RecursiveDescent.h"
#include "../Logging/Logging.h"

static Node* last_comand = 0;
#define DEBUG

//----------------------------
//Grammar   ::= CreateVar* {Equal | {PlusMinus ';'}}+
//CreateVar ::= "var"V ';'
//Equal     ::= V '=' PlusMinus ';'
//PlusMinus ::= MulDiv{['+','-']MulDiv}*
//MulDiv    ::= InOut{['*','/']InOut}*
//InOut     ::= "in" | "out" PlusMinus | Pow
//Pow       ::= Brackets {"^" Pow}*
//Brackets  ::= '('PlusMinus')' | Var | Num
//Var       ::= ['a'-'z','0'-'9','_']
//Number    ::= ['0'-'9']+
//----------------------------

//----------------------------
//+: x^2; x^x^x; y+sin(x^2); 5; 2934; 14+99; 5*x; x; 2 + x*(3 + 4542/2) - y; sin(sin(x)); y + sin(a * cos(log(1))) 
//-: -5; +7; -19*7; x + u15; 17l; x + y - ; kl; A
//----------------------------

Node* CreateNodeWithChild_Op(Node* left_node, Node* right_node, OPER_TYPES op);

static Node* GetCreateVar(Node** ip);

static Node* GetEqual(Node** ip);

static Node* GetPlusMinus(Node** s);

static Node* GetMulDiv(Node** s);

static Node* GetInOut(Node** s);

static Node* GetPow(Node** s);

static Node* GetBrackets(Node** s);

static Node* GetVar(Node** s);

static Node* GetNumber(Node** s);

Node* CreateNodeWithChild_Op(Node* left_node, Node* right_node, OPER_TYPES op)
{
    Node* new_node = NodeCtorOp(op);
    L(new_node)    = left_node;
    R(new_node)    = right_node;

    return new_node;
}

int  MakeTreeFromComands(Tree* tree, Node* program, size_t comand_num)
{
    assert(tree);
    assert(program);

    ReturnIfError(TreeCheck(tree));
    
    last_comand = program + comand_num;
    tree->root = GetNodeFromComands(program);

    return 0;
}

Node* GetNodeFromComands(Node* program)
{
    #ifdef DEBUG
        printf("(Grammar)\n");
    #endif

    Node* val     = nullptr;
    Node* new_var = GetCreateVar(&program);
    if (new_var != nullptr)
    {
        val = NodeCtorKeyword(KEYWORD_VAR);
        L(val) = new_var;
        while ((new_var = GetCreateVar(&program)) != nullptr)
        {
            Node* new_node  = NodeCtorFict();
            L(new_node)     = val;
            R(new_node)     = NodeCtorKeyword(KEYWORD_VAR);
            RR(new_node)    = nullptr;
            RL(new_node)    = new_var;

            val = new_node;
        }
    }

    Node* new_node = NodeCtorFict();
    L(new_node) = val;
    val = new_node;

    while (true)
    {
        new_node = GetEqual(&program);
        if (new_node == nullptr)
        {
            printf("-------------try get plus minus");
            new_node = GetPlusMinus(&program);
            if (new_node == nullptr)
                break;
            CheckSyntaxError(IS_SYMB(program) && VAL_SYMB(program) == ';', program, nullptr);
            program++;
        }
        
        R(val) = new_node;
        new_node = NodeCtorFict();
        L(new_node) = val;
        R(new_node) = nullptr;
        val = new_node;

        printf("---------------------------End while\n");
    }
    
    #ifdef DEBUG
        printf("val = %p\n", val);
    #endif

    return val;
}

static Node* GetCreateVar(Node** ip)
{
    #ifdef DEBUG
        printf("(CreateVar)\n");
    #endif 
    if (*ip == nullptr || *ip >= last_comand)
        return nullptr;
    Node* new_node = nullptr;

    if (TYPE(*ip) == TYPE_KEYWORD)
    {
        switch (VAL_KEYWORD(*ip))
        {
            case KEYWORD_VAR:
            {
                (*ip)++;
                new_node = GetVar(ip);
                CheckSyntaxError((IS_SYMB(*ip) && VAL_SYMB(*ip) == ';'), *ip, nullptr);
                (*ip)++;
                break;
            }

            case UNDEF_KEYWORD_TYPE:
                return nullptr;
            
            default:
                break;
        }
    }

    return new_node;
}

static Node* GetEqual(Node** ip)
{
    #ifdef DEBUG
        printf("(Equal)\n");
    #endif

    if (*ip == nullptr || *ip >= last_comand)
        return nullptr;
    Node* var = GetVar(ip);
    if (var == nullptr) return nullptr;

    CheckSyntaxError(IS_OP(*ip) && VAL_OP(*ip) == OP_EQ, *ip, nullptr)
    
    Node* new_node = CpyNode(*ip);
    L(new_node) = var;
    (*ip)++;
    R(new_node) = GetPlusMinus(ip);
    CheckSyntaxError(IS_SYMB(*ip) && VAL_SYMB(*ip) == ';', *ip, nullptr);
    (*ip)++;

    if (R(new_node) == nullptr)
        return nullptr;

    #ifdef DEBUG
        printf("(end equal)\n");
    #endif

    return new_node;    
}

static Node* GetPlusMinus(Node** s)
{
    if (*s == nullptr || *s >= last_comand)
        return nullptr;
    #ifdef DEBUG
        printf("(PlusMinus)\n" "[%d][%d]\n", (*s)->val.number_cmd_line_in_text,
                                             (*s)->val.number_cmd_in_text);
    #endif

    Node* val = GetMulDiv(s);
    if (val == nullptr) return nullptr;

    while (IS_PLUS(*s) || IS_SUB(*s))
    {
        Node* op = CpyNode(*s);
        (*s)++;

        Node* right_node = GetMulDiv(s);
        if (right_node == nullptr) return nullptr;

        op->left   = val;
        op->right = right_node;
        val = op;
    }

    #ifdef DEBUG
        printf("end PlusMinus\n");
    #endif
    return val;
}

static Node* GetMulDiv(Node** s)
{
    if (*s == nullptr || *s >= last_comand)
        return nullptr;
    #ifdef DEBUG
        printf("(MulDiv)\n" "[%d][%d]\n", (*s)->val.number_cmd_line_in_text,
                                          (*s)->val.number_cmd_in_text);
    #endif

    Node* val = GetInOut(s);
    if (val == nullptr) return nullptr;

    #ifdef DEBUG
        printf("(MulDiv)TYPE(*s) = %d\n", TYPE(*s));
        if (TYPE(*s) == TYPE_OP)
            printf("(MulDiv)OP(*s) = %d\n", VAL_OP(*s));
    #endif
    while (IS_MUL(*s) || IS_DIV(*s))
    {
        Node* op = CpyNode(*s);
        (*s)++;

        Node* right_node = GetInOut(s);
        if (right_node == nullptr) return nullptr;
        
        op->left  = val;
        op->right = right_node;
        val = op;
    }

    #ifdef DEBUG
        printf("end MulDiv\n");
    #endif

    return val;
}

static Node* GetInOut(Node** s)
{
    if (*s == nullptr || *s >= last_comand)
        return nullptr;
    #ifdef DEBUG
        printf("(InOut)\n" "[%d][%d]\n", (*s)->val.number_cmd_line_in_text,
                                         (*s)->val.number_cmd_in_text);
    #endif

    Node* new_node = nullptr;
    if (IS_OP(*s) && VAL_OP(*s) == OP_IN)
    {
        new_node = CpyNode(*s);
        (*s)++;
        CheckSyntaxError(IS_VAR(*s), *s, nullptr);
        R(new_node) = CpyNode(*s);
        (*s)++;
        return new_node;
    }
    else if (IS_OP(*s) && VAL_OP(*s) == OP_OUT)
    {
        printf("START OUT\n");
        new_node = CpyNode(*s);
        (*s)++;
        R(new_node) = GetPlusMinus(s);
        return new_node;
    }
    
    printf("There isnt in and out\n");
    new_node = GetPow(s);
    return new_node;
}

static Node* GetPow(Node** s)
{
    if (*s == nullptr || *s >= last_comand)
        return nullptr;
    #ifdef DEBUG
        printf("(POW)\n" "[%d][%d]\n", (*s)->val.number_cmd_line_in_text,
                                       (*s)->val.number_cmd_in_text);
    #endif
    Node* node = GetBrackets(s);
    if (node == nullptr) return nullptr;

    while (IS_OP(*s) && VAL_OP(*s) == OP_POW)
    {
        L(node) = CpyNode(*s);
        (*s)++;
        R(node) = GetPow(s);
        if (R(node) == nullptr) return nullptr;
    }

    #ifdef DEBUG
        printf("end POW\n");
    #endif
    
    return node;
}

static Node* GetBrackets(Node** s)
{
    if (*s == nullptr || *s >= last_comand)
        return nullptr;
    #ifdef DEBUG
        printf("(Breackets)\n" "[%d][%d]\n", (*s)->val.number_cmd_line_in_text,
                                             (*s)->val.number_cmd_in_text);
    #endif

    Node* val = 0;
    if (IS_SYMB(*s) && VAL_SYMB(*s) == '(')
    {
        (*s)++;
        val = GetPlusMinus(s);
        if (val == nullptr) return nullptr;

        CheckSyntaxError(IS_SYMB(*s) && VAL_SYMB(*s) == ')', *s, nullptr);
        (*s)++;
    }
    else if (IS_VAR(*s))
    {
        val = GetVar(s);
        if (val == nullptr) return nullptr;
    }
    else
    {
        val = GetNumber(s);
        if (val == nullptr) return nullptr;
    }

    #ifdef DEBUG
        printf("end Breackets\n");
    #endif

    return val;
}

static Node* GetVar(Node** s)
{
    if (*s == nullptr || *s >= last_comand)
        return nullptr;
    #ifdef DEBUG
        printf("(Var)\n" "[%d][%d]\n", (*s)->val.number_cmd_line_in_text,
                                       (*s)->val.number_cmd_in_text);
    #endif

    Node* node = nullptr;
    if (IS_VAR(*s))
    {
        #ifdef DEBUG
            printf("IT IS VAR\n");
        #endif
        node = CpyNode(*s);
        (*s)++;
    }
    else
    {
        #ifdef DEBUG
            printf("NOT VAR. TYPE *s = %d\n. VAR TYPE = %d\n", (*s)->val.type, TYPE_VAR);
        #endif
    }
    #ifdef DEBUG
        printf("end Var\n");
    #endif

    return node;
}

static Node* GetNumber(Node** s)
{
    if (*s == nullptr || *s >= last_comand)
        return nullptr;
    #ifdef DEBUG
        printf("(Number)\n" "[%d][%d]\n", (*s)->val.number_cmd_line_in_text,
                                          (*s)->val.number_cmd_in_text);
    #endif

    Node* new_node = CpyNode(*s);
    (*s)++;

    CheckSyntaxError(IS_NUM(new_node), new_node, nullptr);

    #ifdef DEBUG
        printf("end Number\n");
    #endif

    return new_node;
}