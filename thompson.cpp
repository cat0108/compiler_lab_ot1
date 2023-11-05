#include "thompson.h"

int TOTAL_STATE_NUM=0;
string split_join_symbol(string input)
{
    int input_length = input.size();
    int return_length = 0;
    //最坏情况每个输入字符都加上“.”进行分割
    char *return_str=new char[2*input_length+1];
    char first,second;
    for(int i=0;i<input_length-1;i++)
    {
        first=input.at(i);
        second=input.at(i+1);
        return_str[return_length++]=first;
        //sencond是字母或者(，first不是(和|时需要分割
        if(first!='(' && first!='|' && (isalpha(second) || second=='('))
        {
            return_str[return_length++] = '.';
        }  
    }
    //写入最后一个字符
    return_str[return_length++]=second;
    return_str[return_length]='\0';
    //类型转换
    string return_string(return_str);
    cout<<"expr after split: "<<return_string<<endl;
    return return_string;
}

/*
定义优先级规则:
    (1)'*'='+'>'|'>'.'
    (2)有括号先算括号
    (3)相同优先级从左到右

    op      #   (   *,+     |   .   )
    isp     0   1    7      5   3   8
    icp     0   8    6      4   2   1
*/
int isp(char in)
{
    switch (in)
    {
    case '#':
        return 0;
    case '(':
        return 1;
    case '*':
        return 7;
    case '+':  
        return 7;
    case '|':
        return 5;
    case '.':
        return 3;
    case ')': 
        return 8;
    default:
        {
            cerr<<"isp operator error"<<endl;
            return -1;
        }
    }
}

int icp(char in)
{
    switch (in)
    {
    case '#':
        return 0;
    case '(':
        return 8;
    case '*':
        return 6;
    case '+':
        return 6;
    case '|':
        return 4;
    case '.':
        return 2;
    case ')':
        return 1; 
    default:
        {
            cerr<<"icp operator error"<<endl;
            return -1;
        }
    }
}


string infix_to_postfix(string input)
{
    //对于运算符栈，我们令栈底符号为'#',当栈外也读取到'#'匹配时结束
    input.append("#");
    stack<char> s;
    char input_iter,stack_iter;
    stack_iter='#';
    s.push(stack_iter);
    string output_str="";
    int read_location=0;
    input_iter=input.at(read_location++);
    /*
        运算顺序：
        栈内优先级>栈外优先级：入栈
        栈内优先级>栈外优先级：出栈且拼接到结果
        相等：说明遇到了（）或者#，出栈，但什么都不做
        以下代码将基于上述规则实现
    */
   while (!s.empty())
   {
        //遇到词素直接加入结果串中
        if(isalpha(input_iter))
        {
            output_str.push_back(input_iter);
            input_iter=input.at(read_location++);
        }
        else
        //是操作符
        {
            stack_iter=s.top();
            //栈内优先级低，入栈
            if(isp(stack_iter)<icp(input_iter))
            {
                s.push(input_iter);
                input_iter=input.at(read_location++);
            }
            //栈内优先级高，出栈并加入后缀串中
            else if(isp(stack_iter)>icp(input_iter))
            {
                output_str.push_back(stack_iter);
                s.pop();
            }
            else
            {
                if(stack_iter=='(')
                    input_iter=input.at(read_location++);
                s.pop();
            }
        }
   }
   cout<<"infix_to_postfix: "<<output_str<<endl;
   return output_str;
}

state create_state()
{
    state new_state;
    new_state.state_name=TOTAL_STATE_NUM;
    TOTAL_STATE_NUM++;
    return new_state;
}

edge create_edge(state start,state end,char symbol)
{
    edge new_edge;
    new_edge.start_state=start;
    new_edge.end_state=end;
    new_edge.symbol=symbol;
    return new_edge;
}

NFA_unit* single_rule(char in)
{
    NFA_unit* New_unit=new NFA_unit;
    New_unit->edge_count=0;
    state state1=create_state();
    state state2=create_state();
    edge new_edge=create_edge(state1,state2,in);
    //构建NFA_unit
    New_unit->egde_set[New_unit->edge_count++]=new_edge;
    New_unit->init_state=new_edge.start_state;
    New_unit->final_state=new_edge.end_state;
    return New_unit;
}

NFA_unit* op_join(NFA_unit* front,NFA_unit* back)
{
    //增加一条连接边
    edge new_edge=create_edge(front->final_state,back->init_state,'#');
    front->egde_set[front->edge_count++]=new_edge;
    //最后返回的是front，将back的边复制到front中
    for(int i=0;i<back->edge_count;i++)
    {
        front->egde_set[front->edge_count++]=back->egde_set[i];
    }
    //更新final_state
    front->final_state=back->final_state;
    return front;

}

NFA_unit* op_or(NFA_unit* a,NFA_unit* b)
{
    state new_init_state=create_state();
    state new_final_state=create_state();
    //新增四条连接边
    edge new_edge[4];
    new_edge[0]=create_edge(new_init_state,a->init_state,'#');
    new_edge[1]=create_edge(new_init_state,b->init_state,'#');
    new_edge[2]=create_edge(a->final_state,new_final_state,'#');
    new_edge[3]=create_edge(b->final_state,new_final_state,'#');
    //同样结果保存在a中，先复制b的边
    for(int i=0;i<b->edge_count;i++)
    {
        a->egde_set[a->edge_count++]=b->egde_set[i];
    }
    for(int i=0;i<4;i++)
        a->egde_set[a->edge_count++]=new_edge[i];
    //修改初态和终态
    a->init_state=new_init_state;
    a->final_state=new_final_state;
    return a;
}

NFA_unit* op_closure(NFA_unit* a)
{
    state new_init_state=create_state();
    state new_final_state=create_state();
    //同样新增四条边
    edge new_edge[4];
    new_edge[0]=create_edge(new_init_state,a->init_state,'#');
    new_edge[1]=create_edge(a->final_state,new_final_state,'#');
    new_edge[2]=create_edge(new_init_state,new_final_state,'#');
    new_edge[3]=create_edge(a->final_state,a->init_state,'#');
    for(int i=0;i<4;i++)
    {
        a->egde_set[a->edge_count++]=new_edge[i];
    }
    a->init_state=new_init_state;
    a->final_state=new_final_state;
    return a;
}

NFA_unit *op_Rclosure(NFA_unit *a)
{
    //和普通闭包不同，少了一条边而已
    state new_init_state=create_state();
    state new_final_state=create_state();
    //三条边，少了一条#
    edge new_edge[3];
    new_edge[0]=create_edge(new_init_state,a->init_state,'#');
    new_edge[1]=create_edge(a->final_state,new_final_state,'#');
    new_edge[2]=create_edge(a->final_state,a->init_state,'#');
    for(int i=0;i<3;i++)
    {
        a->egde_set[a->edge_count++]=new_edge[i];
    }
    a->init_state=new_init_state;
    a->final_state=new_final_state;
    return a;
}

NFA_unit* expr_to_NFA(string input)
{
    int length=input.size();
    char input_iter;
    stack<NFA_unit*> s;
    NFA_unit* Right,*Left;
    for(int i=0;i<length;i++)
    {
        input_iter=input.at(i);
        switch (input_iter)
        {
            case '|':
            {
                Right=s.top();
                s.pop();
                Left=s.top();
                s.pop();
                NFA_unit* new_unit=op_or(Left,Right);
                s.push(new_unit);
                break;
            }
            case '*':
            {
                Left=s.top();
                s.pop();
                NFA_unit* new_unit=op_closure(Left);
                s.push(new_unit);
                break;            
            }
            case '.':
            {
                Right=s.top();
                s.pop();
                Left=s.top();
                s.pop();
                NFA_unit* new_unit=op_join(Left,Right);
                s.push(new_unit);
                break;      
            }
            case '+':
            {
                Left=s.top();
                s.pop();
                NFA_unit* new_unit=op_Rclosure(Left);
                s.push(new_unit);
                break;                       
            }
            default:
            {
                NFA_unit* new_unit=single_rule(input_iter);
                s.push(new_unit);
                break;
            }
        }
    }
    NFA_unit* res=s.top();
    s.pop();
    return res;
}



void printNFA(NFA_unit *a)
{
    cout<<"total edge = :"<<a->edge_count<<endl;
    for(int i=0;i<a->edge_count;i++)
    {
     cout<<" from: "<<a->egde_set[i].start_state.state_name<<" to: "<<a->egde_set[i].end_state.state_name<<" edge is "<<a->egde_set[i].symbol<<endl;
    }
}