#include "../Setup.h"
#include "../BinaryTree/BinaryTree.h"

struct Function_t
{
    const char*      name = "";
    const OPER_TYPES code = UNDEF_OPER_TYPE; 
};

int  MakeTreeFromComands(Tree* tree, Node* program, size_t comand_num);

Node* GetNodeFromComands(Node* program);