#include <assert.h>

#include "RecursiveDescent.h"
#include "../Logging/Logging.h"

static Node* last_comand = 0;

//----------------------------
/*!
Grammar   ::= CreateVar* Function* Scope
Scope     ::= "begin" CreateVar* { Return | While | If | Equal | {Logical ';'}}+ "end"
Return    ::= "return" Logical ';'
Function  ::= "function" Var '(' Var?{',' Var}* ')' Scope
While     ::= "while" Logical "do" Scope
If        ::= {"if" Logical "then"} Scope {"else" Scope}?
CreateVar ::= "var" Var ';'
Equal     ::= Var '=' Logical ';'

Logical   ::= PlusMinus {Logical_operator PlusMinus}?
PlusMinus ::= MulDiv{['+','-']MulDiv}*
MulDiv    ::= InOutCall{['*','/']InOutCall}*
InOutCall ::= "out" Logical | "in" | Call | Pow
Call      ::= "call" Var '(' Logical?{',' Logical}* ')'
Pow       ::= UnaryFunc {"^" Pow}*
UnaryFunc ::= {"!" Brackets} | {"sqrt(" Brackets ")"} | Brackets
Brackets  ::= '('Logical')' | Var | Num

Var       ::= ['a'-'z','0'-'9','_']+
Number    ::= ['0'-'9']+
*/
//----------------------------

#define IterIp(ip, return_val)                                  \
        (*ip)++;                                                \
        if ((*ip) >= last_comand) return return_val;            \

static Node* GetScope(Node** ip);

static Node* GetFunction(Node** ip);

static Node* GetCall(Node** ip);

static Node* GetReturn(Node** ip);

static Node* GetWhile(Node** ip);

static Node* GetIf(Node** ip);

static Node* GetCreateVar(Node** ip);

static Node* GetEqual(Node** ip);

static Node* GetLogical(Node** ip);

static Node* GetPlusMinus(Node** s);

static Node* GetMulDiv(Node** s);

static Node* GetInOutCall(Node** s);

static Node* GetPow(Node** s);

static Node* GetUnaryFunc(Node** s); 

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
    if (program == nullptr)
        return nullptr;
    #ifdef DEBUG
        printf("(Grammar)\n");
    #endif

    Node* new_node   = nullptr;
    Node* val        = NodeCtorFict();
    Node* start_node = val;

    L(val)= NodeCtorFict();
    val = L(val);

    while((new_node = GetCreateVar(&program)) != nullptr)         //!Get vars
    {
        L(val) = new_node;
        R(val) = NodeCtorFict();
        val = R(val);
    }
    val = start_node;

    while ((new_node = GetFunction(&program)) != nullptr)   //!Get functions
    {
        R(val)  = NodeCtorFict();
        RL(val) = new_node;
        val = R(val);
    }

    R(val) = GetScope(&program);                            //!Get main program

    #ifdef DEBUG
        printf("(End Grammar)\n");
    #endif
    return start_node;
}

static Node* GetReturn(Node** ip)
{
    if (ip == nullptr || *ip == nullptr || *ip >= last_comand)
        return nullptr;
    #ifdef DEBUG
        printf("(Return)\n");
    #endif

    Node* val = nullptr;

    if (IS_KW(*ip) && VAL_KW(*ip) == KEYWORD_RETURN)
    {
        val = NodeCtorReturn();
        (*ip)++;
        R(val) = GetLogical(ip);
        CheckSyntaxError(IS_SYMB(*ip) && VAL_SYMB(*ip) == ';', *ip, nullptr);
        (*ip)++;
    }

    #ifdef DEBUG
        printf("(end return)\n");
    #endif

    return val;
}

static Node* GetCall(Node** ip)
{
    if (ip == nullptr || *ip == nullptr || *ip >= last_comand)
        return nullptr;
    #ifdef DEBUG
        printf("(Call)\n");
    #endif

    Node* val = nullptr;

    if (IS_KW(*ip) && VAL_KW(*ip) == KEYWORD_CALL)
    {
        (*ip)++;
        CheckSyntaxError(IS_VAR(*ip), *ip, nullptr);                        
        val = NodeCtorCall(VAL_VAR(*ip));                                           //!Get funciton name
        val->val.number_cmd_in_text      = (*ip)->val.number_cmd_in_text;
        val->val.number_cmd_line_in_text = (*ip)->val.number_cmd_line_in_text;
        (*ip)++;
        CheckSyntaxError(IS_SYMB(*ip) && VAL_SYMB(*ip) == '(', *ip, nullptr);
        (*ip)++;

        Node* old_val = val;

        Node* new_node = nullptr;                                                   //!Get args
        bool more_arg = false;
        while ((new_node = GetLogical(ip)) != nullptr)
        {
            R(val)  = NodeCtorFict();
            RL(val) = new_node;
            val = R(val);
            if (TYPE(*ip) == TYPE_SYMB && VAL_SYMB(*ip) == ',')
            {
                more_arg = true;
                (*ip)++;
                continue;
            }
            more_arg = false;
            break;
        }
        CheckSyntaxError(!more_arg || new_node != nullptr, *ip, nullptr);        
        
        CheckSyntaxError(IS_SYMB(*ip) && VAL_SYMB(*ip) == ')', *ip, nullptr);
        (*ip)++;
        val = old_val;
    }

    #ifdef DEBUG
        printf("(end call)\n");
    #endif

    return val;
}

static Node* GetFunction(Node** ip)
{
    if (ip == nullptr || *ip == nullptr || *ip >= last_comand)
        return nullptr;
    #ifdef DEBUG
        printf("(Function)\n");
    #endif

    Node* val = nullptr;
    if(IS_KW(*ip) && VAL_KW(*ip) == KEYWORD_FUNCTION)
    {
        (*ip)++;
        CheckSyntaxError(TYPE(*ip) == TYPE_VAR, *ip, nullptr);

        val = NodeCtorFunction(VAL_VAR(*ip));
        val->val.number_cmd_in_text      = (*ip)->val.number_cmd_in_text;
        val->val.number_cmd_line_in_text = (*ip)->val.number_cmd_line_in_text;
        (*ip)++;
        CheckSyntaxError(IS_SYMB(*ip) && VAL_SYMB(*ip) == '(', *ip, nullptr);
        (*ip)++;

        L(val) = NodeCtorFict();
        Node* arguments = L(val);
        
        Node* new_node = nullptr;
        while ((new_node = GetVar(ip)))
        {
            L(arguments)  = NodeCtorFict();
            R(arguments) = new_node;
            arguments     = L(arguments);
            if (TYPE(*ip) == TYPE_SYMB && VAL_SYMB(*ip) == ',')
            {
                (*ip)++;
                continue;
            }
            break;
        }
        
        CheckSyntaxError(IS_SYMB(*ip) && VAL_SYMB(*ip) == ')', *ip, nullptr);
        (*ip)++;

        R(val) = GetScope(ip);
    }

    #ifdef DEBUG
        printf("(End function)\n");
    #endif

    return val;
}

static Node* GetScope(Node** ip)
{
    #ifdef DEBUG
        printf("(Scope)\n");
    #endif
    if (ip == nullptr || *ip == nullptr || *ip >= last_comand)
        return nullptr;

    CheckSyntaxError(TYPE(*ip) == TYPE_KEYWORD && VAL_KW(*ip) == KEYWORD_BEGIN, *ip, nullptr);
    (*ip)++;

    Node* val = NodeCtorFict();

    Node* all_vars = NodeCtorFict();                        //!Get vars
    L(val) = all_vars;

    Node* new_var  = nullptr;

    while ((new_var = GetCreateVar(ip)) != nullptr)
    {
        L(all_vars) = new_var;
        R(all_vars) = NodeCtorFict();
        all_vars = R(all_vars);
    }

    Node* new_node = NodeCtorFict();                        //!Get code
    L(new_node) = val;
    val = new_node;

    while (true)
    {
        if (IS_KW(*ip) && VAL_KW(*ip) == KEYWORD_END)
            break;
        new_node = GetReturn(ip);
        if (new_node == nullptr)
        {
            new_node = GetWhile(ip);
            if (new_node == nullptr)
            {
                new_node = GetIf(ip);
                if (new_node == nullptr)
                {
                    new_node = GetEqual(ip);
                    if (new_node == nullptr)
                    {
                        new_node = GetLogical(ip);
                        if (new_node == nullptr)
                            break;
                        CheckSyntaxError(IS_SYMB(*ip) && VAL_SYMB(*ip) == ';', *ip, nullptr);
                        (*ip)++;
                    }
                }
            }
        }
        
        R(val) = new_node;
        new_node = NodeCtorFict();
        L(new_node) = val;
        R(new_node) = nullptr;
        val = new_node;
        
        if(TYPE(*ip) == TYPE_KEYWORD && VAL_KW(*ip) == KEYWORD_END)
        {
            (*ip)++;
            #ifdef DEBUG
                printf("(end scope)\n");
            #endif
            return val;
        }
    }
    
    #ifdef DEBUG
        printf("(end scope)\n");
    #endif

    return val;
}

static Node* GetWhile(Node** ip)
{
    #ifdef DEBUG
        printf("(While)\n");
    #endif

    Node* val = nullptr;
    if (IS_KW(*ip) && VAL_KW(*ip) == KEYWORD_WHILE)
    {
        val = NodeCtorKeyword(KEYWORD_WHILE);
        (*ip)++;
        L(val) = GetLogical(ip);
        CheckSyntaxError(IS_KW(*ip) && VAL_KW(*ip) == KEYWORD_DO, *ip, nullptr);
        (*ip)++;
        R(val) = GetScope(ip);
    }

    return val;
    #ifdef DEBUG
        printf("(end while)\n");
    #endif
}

static Node* GetIf(Node** ip)
{
    #ifdef DEBUG
        printf("(If)\n");
    #endif
    if (ip == nullptr || *ip == nullptr || *ip >= last_comand)
        return nullptr;

    Node* val = nullptr;
    if (IS_KW(*ip) && VAL_KW(*ip) == KEYWORD_IF)
    {
        val    = NodeCtorKeyword(KEYWORD_IF); 
        (*ip)++;
        L(val) = GetLogical(ip);     //!condition
        CheckSyntaxError(IS_KW(*ip) && (VAL_KW(*ip) == KEYWORD_THEN), *ip, nullptr);
        (*ip)++;

        R(val)  = NodeCtorKeyword(KEYWORD_ELSE);
        RL(val) = GetScope(ip);                                                         //!true branch

        if((*ip) < last_comand && IS_KW(*ip) && VAL_KW(*ip) == KEYWORD_ELSE)            //!else branch
        {
            (*ip)++;
            RR(val) = GetScope(ip);
        }
    }
    
    #ifdef DEBUG
        printf("(end if)\n");
    #endif    
    return val;
}

static Node* GetCreateVar(Node** ip)
{
    #ifdef DEBUG
        printf("(CreateVar)\n");
    #endif 
    if (ip == nullptr || *ip >= last_comand || *ip == nullptr)
        return nullptr;
    Node* new_node = nullptr;

    if (TYPE(*ip) == TYPE_KEYWORD)
    {
        switch (VAL_KW(*ip))
        {
            case KEYWORD_VAR:
            {
                (*ip)++;
                CheckSyntaxError(IS_VAR(*ip), *ip, nullptr);
                new_node = NodeCtorCreateVar(VAL_VAR(*ip));
                (*ip)++;
                CheckSyntaxError((IS_SYMB(*ip) && VAL_SYMB(*ip) == ';'), *ip, nullptr);
                (*ip)++;
                break;
            }
        }
    }
    
    #ifdef DEBUG
        printf("(end create var)\n");
    #endif

    return new_node;
}

static Node* GetEqual(Node** ip)
{
    #ifdef DEBUG
        printf("(Equal)\n");
    #endif

    if (ip == nullptr || *ip >= last_comand || *ip == nullptr) 
        return nullptr;

    Node* var = GetVar(ip);
    if (var == nullptr) return nullptr;

    CheckSyntaxError(IS_OP(*ip) && VAL_OP(*ip) == OP_EQ, *ip, nullptr)
    
    Node* new_node = CpyNode(*ip);
    L(new_node) = var;
    (*ip)++;
    R(new_node) = GetLogical(ip);
    CheckSyntaxError(IS_SYMB(*ip) && VAL_SYMB(*ip) == ';', *ip, nullptr);
    (*ip)++;

    if (R(new_node) == nullptr)
        return nullptr;

    #ifdef DEBUG
        printf("(end equal)\n");
    #endif

    return new_node;    
}

static Node* GetLogical(Node** ip)
{
    if (ip == nullptr || *ip >= last_comand || *ip == nullptr) 
        return nullptr;
    #ifdef DEBUG
        printf("(Logical)\n");
    #endif

    Node* val = nullptr;

    val = GetPlusMinus(ip);
    if (val == nullptr) return nullptr;
    
    while (*ip < last_comand && IS_LOGICAL_OP(*ip))
    {
        Node* op_node = NodeCtorOp(VAL_OP(*ip));
        (*ip)++;

        Node* right_node = GetPlusMinus(ip);
        if (right_node == nullptr) return nullptr;
        
        L(op_node) = val;
        R(op_node) = right_node;

        val = op_node;
    }
    
    #ifdef DEBUG
        printf("(end logical)\n");
    #endif
    return val;
}

static Node* GetPlusMinus(Node** s)
{
    if (s == nullptr || *s == nullptr || *s >= last_comand)
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
    if (s == nullptr || *s == nullptr || *s >= last_comand)
        return nullptr;
    #ifdef DEBUG
        printf("(MulDiv)\n" "[%d][%d]\n", (*s)->val.number_cmd_line_in_text,
                                          (*s)->val.number_cmd_in_text);
    #endif

    Node* val = GetInOutCall(s);
    if (val == nullptr) 
        return nullptr;

    while (IS_MUL(*s) || IS_DIV(*s))
    {
        Node* op = CpyNode(*s);
        (*s)++;

        Node* right_node = GetInOutCall(s);
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

static Node* GetInOutCall(Node** ip)
{
    if (ip == nullptr || *ip >= last_comand || *ip == nullptr)
        return nullptr;

    #ifdef DEBUG
        printf("(InAndOut)\n" "[%d][%d]\n", (*ip)->val.number_cmd_line_in_text,
                                         (*ip)->val.number_cmd_in_text);
    #endif

    Node* new_node = nullptr;
    if (IS_OP(*ip) && VAL_OP(*ip) == OP_OUT)
    {
        new_node = CpyNode(*ip);
        (*ip)++;
        R(new_node) = GetLogical(ip);

        #ifdef DEBUG
            printf("(end InAndOut)\n");
        #endif
        return new_node;
    }
    else if (IS_OP(*ip) && VAL_OP(*ip) == OP_IN)
    {
        new_node = CpyNode(*ip);
        (*ip)++;

        #ifdef DEBUG
            printf("(end InAndOut)\n");
        #endif
        return new_node;
    }
    else if (IS_KW(*ip) && VAL_KW(*ip) == KEYWORD_CALL)
    {
        new_node = GetCall(ip);
        #ifdef DEBUG
            printf("(end InAndOut)\n");
        #endif
        return new_node;
    }
    
    #ifdef DEBUG
        printf("There isnt in and out\n");
    #endif
    
    new_node = GetPow(ip);
    #ifdef DEBUG
        printf("(end InAndOut)\n");
    #endif
    return new_node;
}

static Node* GetPow(Node** s)
{
    if (s == nullptr || *s == nullptr || *s >= last_comand)
        return nullptr;

    #ifdef DEBUG
        printf("(POW)\n" "[%d][%d]\n", (*s)->val.number_cmd_line_in_text,
                                       (*s)->val.number_cmd_in_text);
    #endif

    Node* node = GetUnaryFunc(s);
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

static Node* GetUnaryFunc(Node** s)
{
    if (s == nullptr || *s == nullptr || *s >= last_comand)
        return nullptr;
    #ifdef DEBUG
        printf("(UnaryFunc)\n");
    #endif

    Node* result = nullptr;

    if (IS_OP(*s) && VAL_OP(*s) == OP_NOT)
    {
        result = NodeCtorOp(OP_NOT);
        (*s)++;
        R(result) = GetBrackets(s);
        return result;
    }

    if (IS_OP(*s))
    {    
        switch (VAL_OP(*s))
        {
        case OP_SQRT:
        {
            result = NodeCtorOp(OP_SQRT);
            (*s)++;
            CheckSyntaxError(IS_SYMB(*s) && VAL_SYMB(*s) == '(', *s, nullptr);
            (*s)++;

            R(result) = GetBrackets(s);

            CheckSyntaxError(IS_SYMB(*s) && VAL_SYMB(*s) == ')', *s, nullptr);
            (*s)++;
            break;
        }
        default:
            break;
        }
        return result;
    }
    
    if (result == nullptr)
        return GetBrackets(s);

    #ifdef DEBUG
        printf("end UnaryFunc\n");
    #endif
}

static Node* GetBrackets(Node** s)
{
    if (s == nullptr || *s == nullptr || *s >= last_comand)
        return nullptr;
    #ifdef DEBUG
        printf("(Breackets)\n" "[%d][%d]\n", (*s)->val.number_cmd_line_in_text,
                                             (*s)->val.number_cmd_in_text);
    #endif

    Node* val = 0;
    if (IS_SYMB(*s) && VAL_SYMB(*s) == '(')
    {
        (*s)++;
        val = GetLogical(s);
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
    if (s == nullptr || *s == nullptr || *s >= last_comand)
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
            printf("NOT VAR. TYPE *s = %d\n", (*s)->val.type);
        #endif
    }
    #ifdef DEBUG
        printf("end Var\n");
    #endif

    return node;
}

static Node* GetNumber(Node** s)
{
    if (s == nullptr || *s == nullptr || *s >= last_comand)
        return nullptr;
    #ifdef DEBUG
        printf("(Number)\n" "[%d][%d]\n", (*s)->val.number_cmd_line_in_text,
                                          (*s)->val.number_cmd_in_text);
    #endif

    if (IS_NUM(*s))
    {
        Node* new_node = CpyNode(*s);
        (*s)++;
        #ifdef DEBUG
            printf("end Number\n");
        #endif
        return new_node;
    }
    else
    {
        #ifdef DEBUG
            printf("end Number\n");
        #endif
        return nullptr;
    }
}