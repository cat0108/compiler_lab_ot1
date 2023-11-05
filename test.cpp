#include "thompson.h"

int main()
{
    string input("(a|b)*ab+b");
    input=split_join_symbol(input);
    input=infix_to_postfix(input);
    NFA_unit* tempb;
    tempb=expr_to_NFA(input);
    printNFA(tempb);
}