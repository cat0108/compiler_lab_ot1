//thompson构造法实现正规式->NFA
#ifndef THOMPSON_H
#define THOMPSON_H

#include <iostream>
#include <stdio.h>
#include <stack>
#include <cctype>
#include <string>

using namespace std;
#define MAX_EDGE_NUM 100

//便于后续处理，为状态定义结构体
struct state
{
    int state_name;
};



//由于是NFA，所以不在状态中定义边
struct edge
{
    state start_state;
    state end_state;
    //边所对应的词素
    char symbol;
};

//定义NFA单元
struct NFA_unit
{
    edge egde_set[MAX_EDGE_NUM];
    int edge_count;
    //整个NFA的初态和终态
    state init_state;
    state final_state;
};

//创建新状态
state create_state();

//创建新边
edge create_edge(state start,state end,char symbol);

//为了便于读取正规式，我们将连接运算使用'.'分割开来
string split_join_symbol(string input);

//中缀转后缀，便于后续处理
string infix_to_postfix(string input);

//栈内优先级
int isp(char in);

//栈外优先级
int icp(char in);

//计算后缀表达式的NFA
NFA_unit* expr_to_NFA(string input);

//处理单个符号
NFA_unit* single_rule(char in);

//处理连接“.”运算
NFA_unit* op_join(NFA_unit* front,NFA_unit* back);

//处理或"|"运算
NFA_unit* op_or(NFA_unit* a,NFA_unit* b);

//处理"*"运算
NFA_unit* op_closure(NFA_unit* a);

//处理"+"运算

NFA_unit* op_Rclosure(NFA_unit* a);
//打印相关数据
void printNFA(NFA_unit* a);
#endif