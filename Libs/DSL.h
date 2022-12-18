#ifndef __SYM_DIFF_DSL__
#define __SYM_DIFF_DSL__

const double PRECISION = 1e-6;

#define RED           "\033[31m"
#define DEFAULT_COLOR "\033[0m"

#define CheckSyntaxError(cond, node, return_code)                                                       \
    if (!(cond))                                                                                        \
    {                                                                                                   \
        LogPrintf("(%d)Syntax error in line %ld symbol %ld: %s\n",                                      \
                                        __LINE__ ,                                                      \
                                        (node)->val.number_cmd_line_in_text,                            \
                                        (node)->val.number_cmd_in_text, #cond);                         \
        fprintf(stderr, RED "(%d)Syntax error in line %ld symbol %ld: %s\n" DEFAULT_COLOR,              \
                                              __LINE__ ,                                                \
                                              (node)->val.number_cmd_line_in_text,                      \
                                              (node)->val.number_cmd_in_text, #cond);                   \
        return return_code;                                                                             \
    }

#define ReturnAndTex                            \
    PrintRandBundleInLatex();                   \
    TexEqualityWithDesignations(node, new_node, \
             "\\begin{center}\n (", ")'\n "); \
                                                \
    PrintfInLatex("\\end{center}\n");           \
    return new_node;

#define BinaryConstConv(oper)           \
{                                       \
    double a = VAL_N(L(node));          \
    double b = VAL_N(R(node));          \
                                        \
    DelLR(node);                        \
                                        \
    TYPE(node)  = TYPE_NUM;             \
    VAL_N(node) = a oper b;             \
}

#define UnaryConstConv(func)            \
{                                       \
    double a = VAL_N(R(node));          \
                                        \
    DelLR(node);                        \
                                        \
    TYPE(node)  = TYPE_NUM;             \
    VAL_N(node) = func(a);              \
}

#define DelLR(node)                             \
    DeleteNode(L(node));                        \
    DeleteNode(R(node));                        \
    L(node) = nullptr;                          \
    R(node) = nullptr;

#define fprintOper              \
    switch(node->val.val.op)    \
    {                           \
        case OP_PLUS:           \
            fprintf(fp, "+");   \
            break;              \
        case OP_SUB:            \
            fprintf(fp, "-");   \
            break;              \
        case OP_MUL:            \
            fprintf(fp, "*");   \
            break;              \
        case OP_DIV:            \
            fprintf(fp, "/");   \
            break;              \
        case OP_POW:            \
            fprintf(fp, "^");   \
            break;              \
        case OP_IN:             \
            fprintf(fp, "in");  \
            break;              \
        case OP_OUT:            \
            fprintf(fp, "out"); \
            break;              \
        case OP_EQ:             \
            fprintf(fp, "=");   \
            break;              \
        case UNDEF_OPER_TYPE:   \
            fprintf(fp, "?");   \
            break;              \
        default:                \
            fprintf(fp, "#");   \
            break;              \
    }

#define L(node) (node)->left

#define R(node) (node)->right

#define LL(node) (node)->left->left

#define LR(node) (node)->left->right

#define RL(node) (node)->right->left

#define RR(node) (node)->right->right


#define IS_L_OP(node) (L(node) != nullptr && IS_OP(L(node)))

#define IS_L_NUM(node) (L(node) != nullptr && IS_NUM(L(node)))

#define IS_R_NUM(node) (R(node) != nullptr && IS_NUM(R(node)))

#define IS_R_OP(node) (R(node) != nullptr && IS_OP(R(node)))

#define IS_VAR(node) ((node)->val.type == TYPE_VAR)

#define IS_OP(node) ((node)->val.type == TYPE_OP)

#define IS_NUM(node) ((node)->val.type == TYPE_NUM)

#define IS_ZERO(node) (IS_NUM(node) && -PRECISION <= VAL_N(node) &&  VAL_N(node) <= PRECISION)

#define IS_ONE(node) (IS_NUM(node) && 1 - PRECISION <= VAL_N(node) &&  VAL_N(node) <= 1 + PRECISION)

#define IS_DOUBLE_EQ(x, y) ((-PRECISION <= (x - y)) && ((x - y) <= PRECISION))

#define IS_PLUS(node) (IS_OP(node) && VAL_OP(node) == OP_PLUS)

#define IS_SUB(node) (IS_OP(node) && VAL_OP(node) == OP_SUB)

#define IS_MUL(node) (IS_OP(node) && VAL_OP(node) == OP_MUL)

#define IS_DIV(node) (IS_OP(node) && VAL_OP(node) == OP_DIV)

#define IS_SYMB(node) (TYPE(node) == TYPE_SYMB)

#define IS_KEYWORD(node) (TYPE(node) == TYPE_KEYWORD)

#define IS_LOGICAL_OP(node) (IS_OP(node) && (VAL_OP(node) == OP_AND   || VAL_OP(node) == OP_IS_B  ||\
                                       VAL_OP(node) == OP_IS_BE || VAL_OP(node) == OP_IS_EQ       ||\
                                       VAL_OP(node) == OP_IS_NE || VAL_OP(node) == OP_IS_S        ||\
                                       VAL_OP(node) == OP_IS_SE || VAL_OP(node) == OP_OR))

#define VAL_N(node) (node)->val.val.dbl

#define VAL_OP(node) (node)->val.val.op

#define VAL_VAR(node) (node)->val.val.var

#define VAL_SYMB(node) (node)->val.val.symb

#define VAL_KEYWORD(node) (node)->val.val.keyword

#define TYPE(node) (node)->val.type


#define PUT_PLUS                    \
    case OP_PLUS:                   \
        fprintf(stream, "+");     \
        break;

#define PUT_SUB                     \
    case OP_SUB:                    \
        fprintf(stream, "-");     \
        break;
        
#define PUT_MUL                     \
    case OP_MUL:                    \
        fprintf(stream, "*");     \
        break;
        
#define PUT_DIV                     \
    case OP_DIV:                    \
        fprintf(stream, "/");     \
        break;

#define PUT_SIN                     \
    case OP_SIN:                    \
        fprintf(stream, "sin");   \
        break;

#define PUT_COS                     \
    case OP_COS:                    \
        fprintf(stream, "cos");   \
        break;

#define PUT_LOG                      \
    case OP_LOG:                     \
        fprintf(stream, "log");    \
        break;

#define PUT_POW                     \
    case OP_POW:                    \
        fprintf(stream, "**");    \
        break;
#endif