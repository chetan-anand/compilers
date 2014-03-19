#include <iostream>
#include<stdio.h>
#include<set>
#include<stack>
#include<algorithm>
#include<fstream>
#include<string.h>
#include"func.h"
using namespace std;
#define LPAR 0
#define RPAR 5
#define STAR 4
#define DOT 3
#define OR 2
#define ID 1
//char keyword_regex[2040];
//char operator_regex[2040];
//char identifier_regex[2040];
//char constant_regex[2040];
char postfix[2040];
char b[]={'(','a','|','.','*',')'};
int node_index = 0;
/* returns the operator.*/
//char tree[20400];
set<int> firstpos_set[2040];
set<int> lastpos_set[2040];
set<int> followpos_set[2040];
bool nullable[2040];
int position=0;
struct node
{
    char c;
    int pos;
    int node_index;
    struct node *left,*right;
};

struct dfa_states
{
    set<int> states;
    int state_no;
    bool is_final;
    bool marked;
    int transitions[256];
};
int is_operator(char c)
{
    switch(c)
    {
        case '(':
                return LPAR;
        case ')':
                return RPAR;
        case '*':
                return STAR;
        case '|':
                return OR;
        case '.':
                return DOT;
        default:
                return ID;
    }
}
struct node *root=NULL;
void infix_postfix(char a[2040])
{
    int len;
    len=strlen(a);
    stack<int> s;
    int index=0;
    //s.push(1);
   // postfix[index++]=a[0];
    int k=0;
    int priority;
    while(k!=len)
    {
        if((priority=is_operator(a[k]))!=1)
        {
            while(!s.empty() && s.top()>= priority && priority!=0)
            {
                postfix[index++]=b[s.top()];
                s.pop();
            }
            if(a[k]==')')
            {
                while(!(s.top()==LPAR))
                {
                    postfix[index++]=b[s.top()];
                    s.pop();
                }
                s.pop();
                k++;
                continue;
            }
            s.push(priority);
        }
        else
        {
            postfix[index++]=a[k];
            if(a[k] == '\\')
                postfix[index++] = a[++k];
        }
        k++;
    }
    while(!s.empty())
    {
        postfix[index++]=b[s.top()];
        s.pop();
    }
    postfix[index++]='#';
    postfix[index++]='.';
    postfix[index]='\0';
}
void make_tree(char postfix[2040])
{
    struct node *newnode;
    int k=0;
    int len=strlen(postfix);
    stack<struct node*> s;
    while(k!=len)
    {
        if(is_operator(postfix[k])!=1 && is_operator(postfix[k])!=4)
        {
            newnode=new(struct node);
            newnode->left=NULL;
            newnode->right=NULL;
            newnode->c=postfix[k];
            newnode->right=s.top();
            s.pop();
            newnode->left=s.top();
            s.pop();
            s.push(newnode);
            root=newnode;
        }
        else if(is_operator(postfix[k])==4)
        {
            newnode=new(struct node);
            newnode->left=NULL;
            newnode->right=NULL;
            newnode->c=postfix[k];
            newnode->left=s.top();
            s.pop();
            s.push(newnode);
            root=newnode;
        }
        else
        {
            newnode=new(struct node);
            newnode->left=NULL;
            newnode->right=NULL;
            if(postfix[k]!='\\')
                newnode->c=postfix[k];
            else
                newnode->c=postfix[++k];
            newnode->pos=position++;
            s.push(newnode);
            root=newnode;
        }
        k++;
    }
}
void make_firstpos(struct node *root)
{
    if(root==NULL)
        return;
    if(root->left==NULL && root->right==NULL)
    {
        if(root->c=='$')
        {
        }
        else if(root->left == NULL && root->right == NULL)
        {
            firstpos_set[root->node_index].insert(root->pos);
        }
    }
    if(root->left!=NULL)
    {
        make_firstpos(root->left);
        switch(root->c)
        {
            case '|':
                Union(firstpos_set[root->node_index],firstpos_set[root->left->node_index],firstpos_set[root->node_index]);
                break;
            case '*':
                Union(firstpos_set[root->node_index],firstpos_set[root->left->node_index],firstpos_set[root->node_index]);
                break;
            case '.':
                Union(firstpos_set[root->node_index],firstpos_set[root->left->node_index],firstpos_set[root->node_index]);
                break;
        }
    }
    if(root->right!=NULL)
    {
        make_firstpos(root->right);
        switch(root->c)
        {
            case '|':
                Union(firstpos_set[root->right->node_index],firstpos_set[root->node_index],firstpos_set[root->node_index]);
                break;
            case '.':
                if(nullable[root->left->node_index])
                    Union(firstpos_set[root->right->node_index],firstpos_set[root->node_index],firstpos_set[root->node_index]);
                break;
        }
    }
}

void make_lastpos(struct node *root)
{
    if(root==NULL)
        return;
    if(root->left==NULL && root->right==NULL)
    {
        if(root->c=='$')
        {
        }
        else if(root->left == NULL && root->right == NULL)
        {
            lastpos_set[root->node_index].insert(root->pos);
        }
    }
    if(root->left!=NULL)
    {
        make_lastpos(root->left);
        switch(root->c)
        {
            case '|':
                Union(lastpos_set[root->left->node_index],lastpos_set[root->node_index],lastpos_set[root->node_index]);
                break;
            case '*':
                Union(lastpos_set[root->left->node_index],lastpos_set[root->node_index],lastpos_set[root->node_index]);
                break;
            case '.':
                if(nullable[root->right->node_index])
                    Union(lastpos_set[root->left->node_index],lastpos_set[root->node_index],lastpos_set[root->node_index]);
                break;
        }
    }
    if(root->right!=NULL)
    {
        make_lastpos(root->right);
        switch(root->c)
        {
            case '|':
                Union(lastpos_set[root->right->node_index],lastpos_set[root->node_index],lastpos_set[root->node_index]);
                break;
            case '.':
                    Union(lastpos_set[root->right->node_index],lastpos_set[root->node_index],lastpos_set[root->node_index]);
                break;
        }
    }
}

void make_nullable(struct node *root)
{
    if(root==NULL)
        return;
    cout<<root->c<<endl;
    if(root->left==NULL && root->right==NULL)
    {
        if(root->c=='$')
        {
            nullable[root->node_index]=true;
        }
        else if(root->left == NULL && root->right == NULL)
        {
            nullable[root->node_index]=false;
        }
    }
    bool flag = false;
    if(root->left!=NULL)
    {
        make_nullable(root->left);
        flag = true;
        switch(root->c)
        {
            case '|':
                nullable[root->node_index] = false;
                nullable[root->node_index] = nullable[root->node_index] || nullable[root->left->node_index];
                break;
            case '*':
                nullable[root->node_index] = true;
                break;
            case '.':
                nullable[root->node_index] = true;
                nullable[root->node_index] = nullable[root->left->node_index] && nullable[root->node_index];
                break;
        }
    }
    if(root->right!=NULL)
    {
        make_nullable(root->right);
        {
            switch(root->c)
            {
                case '|':

                    nullable[root->node_index] = !flag?false:nullable[root->node_index];
                    nullable[root->node_index] = nullable[root->node_index] || nullable[root->right->node_index];
                    break;
                case '*':
                    nullable[root->node_index] = true;
                    break;
                case '.':
                    nullable[root->node_index] = !flag?true:nullable[root->node_index];
                    nullable[root->node_index] = nullable[root->right->node_index] && nullable[root->node_index];
                    break;
            }
        }
    }
}

node *find_node_index(int pos,struct node *cur)
{
    node *i;
    if(cur!=NULL)
    {
        //cout<<"find node cur->c: "<<cur->c<<" "<<cur->pos<<endl;
        if(cur->left == NULL && cur->right == NULL)
            if(cur->pos==pos)
            {
                return cur;
            }
        if((i=find_node_index(pos,cur->left))!=NULL)
            return i;
        if((i=find_node_index(pos,cur->right))!=NULL)
            return i;
        return NULL;
    }
    return NULL;
}
void make_followpos(struct node *cur)
{
    if(cur==NULL)
        return;
    if(cur->left == NULL && cur->right == NULL)
        return;
    set<int> ::iterator it;
    switch(cur->c)
    {
        case '.':
            for(it = lastpos_set[cur->left->node_index].begin();it!=lastpos_set[cur->left->node_index].end();it++)
            {
                //cout<<"pos: "<<*it<<endl;
                node *n_index = find_node_index(*it,root);
                int index = n_index->node_index;
                //cout<<"index: "<<index<<endl;
                Union(followpos_set[index],firstpos_set[cur->right->node_index],followpos_set[index]);
            }
            break;
        case '*':
            for(it = lastpos_set[cur->node_index].begin();it!=lastpos_set[cur->node_index].end();it++)
            {
                //cout<<"pos: "<<*it<<endl;
                node *n_index = find_node_index(*it,root);
                int index = n_index->node_index;
                //cout<<"index: "<<index<<endl;
                Union(followpos_set[index],firstpos_set[cur->node_index],followpos_set[index]);
            }
            break;
    }
    make_followpos(cur->left);
    make_followpos(cur->right);
}
void init(struct node *root)
{
    if(root->left!=NULL)
    init(root->left);
    root->node_index = node_index++;
    if(root->right!=NULL)
    init(root->right);
}

void inorder(struct node *root)
{
    if(root->left!=NULL)
    inorder(root->left);
    cout<<"\t"<<root->node_index<<" "<<root->c<<endl;
    cout<<"Nullable: "<<nullable[root->node_index]<<endl;;
    set<int>::iterator it;
    cout<<"First pos: "<<endl;
    for(it = firstpos_set[root->node_index].begin();it!=firstpos_set[root->node_index].end();it++)
            cout<<*it<<endl;
    cout<<"Last pos: "<<endl;
    for(it = lastpos_set[root->node_index].begin();it!=lastpos_set[root->node_index].end();it++)
            cout<<*it<<endl;
    cout<<"Follow pos: "<<endl;
    for(it = followpos_set[root->node_index].begin();it!=followpos_set[root->node_index].end();it++)
                cout<<*it<<endl;
    if(root->right!=NULL)
    inorder(root->right);
}
int get_unmarked(vector<dfa_states> dfa)
{
    for(int i =0;i<dfa.size();i++)
    {
        if(!dfa[i].marked)
            return dfa[i].state_no;
    }
    return -1;
}
void print_set(set<int> u)
{
    cout<<"Set:"<<endl;
    for(set<int>::iterator it = u.begin();it!=u.end();it++)
        cout<<*it<<endl;
}
void construct_dfa(node *cur,node *root,vector<dfa_states> &dfa)
{
    dfa_states d;
    Union(d.states,firstpos_set[cur->node_index],d.states);
    int sn = 0;
    d.state_no = sn++;
    d.is_final = false;
    d.marked = false;
    for(int k = 0; k < 256; k++)
        d.transitions[k] = -1;
    dfa.push_back(d);
    set<int> :: iterator it;
    struct node *n_index;
    int r;
    while((r = get_unmarked(dfa))!=-1)
    {
        d = dfa[r];
        cout<<"state: "<<d.state_no<<endl;
        dfa[r].marked = true;
        for(int i = 0; i < 256; i++)
        {
            set<int> U;
            for(it = d.states.begin();it!=d.states.end();it++)
            {
                node *n_index= find_node_index(*it,root);
                int index= n_index->node_index;
                if(n_index->c==i)
                {
                    cout<<"char: "<<n_index->c<<" node index: "<<index<<endl;
                    Union( U,followpos_set[index],U);
                }
            }
            dfa_states temp;
            if(!U.empty())
                print_set(U);
            bool flag = false;
            int j;
            for(j = 0; j <dfa.size(); j++)
            {
                set<int> diff;
                Diff(dfa[j].states,U,diff);
                if((diff.empty() && dfa[j].states.size()==U.size()) || U.empty())
                {
                    flag = true;
                    break;
                }
            }
            if(!flag)
            {
                cout<<"change trans"<<endl;
                Union(temp.states, U, temp.states);
                temp.state_no = sn++;
                temp.marked = false;
                for(int k = 0; k < 256; k++)
                    temp.transitions[k] = -1;
                dfa[r].transitions[i] = temp.state_no;
                dfa.push_back(temp);
            }
            else if(!U.empty())
                dfa[r].transitions[i] = dfa[j].state_no;
        }
    }
}
int get_pos_of_hash(node *cur)
{
    int pos = 0;
    for(int i = 0; i < strlen(postfix);i++)
    {
        if(is_operator(postfix[i])==1)
        {
            if(postfix[i]=='#')
                return pos;
            pos++;
        }
    }
}
void init_final_states(vector<dfa_states> &dfa,node *root)
{
    int hash_pos = get_pos_of_hash(root);
    for(int i = 0; i < dfa.size(); i++)
    {
        dfa[i].is_final = false;
        for(set<int>::iterator it = dfa[i].states.begin(); it!=dfa[i].states.end(); it++)
        {
            if(*it==hash_pos)
            {
                dfa[i].is_final = true;
                break;
            }
        }
    }
}

void dispose_tree(node *cur)
{
    if(cur!=NULL)
    {
        dispose_tree(cur->left);
        dispose_tree(cur->right);
        delete(cur);
    }
}

void clean(set<int> s[])
{
    for(int i =0;i<2040;i++)
    {
        s[i].erase(s[i].begin(),s[i].end());
    }
}
void handle_range(char **input)
{
    int len = strlen(*input);
    char *new_in = new char[1024];
    int st,end;
    int index = 0;
    for(int i = 0; i < len; i++)
    {
        if((*input)[i] != '[')
        {
            new_in[index++] = (*input)[i];
        }
        else
        {
            st = (*input)[++i];
            ++i;
            end = (*input)[++i];
            ++i;
            new_in[index++] = '(';
            for(int j = st; j<end; j++)
            {
                new_in[index++] = (char)j;
                new_in[index++] = '|';
            }
            new_in[index++] = (char)end;
            new_in[index++] = ')';
        }
    }
    new_in[index] = '\0';
    *input = new_in;
    cout<<"expanded regexp "<<*input<<endl;
}
int main()
{
    char *input = new char[2040];
    cout<<"Enter Regex for Keywords"<<endl;
    cin>>input;
    handle_range(&input);
    cout<<"input: " <<input<<endl;
    int len=strlen(input);
    infix_postfix(input);
    printf("%s\n",postfix);
    make_tree(postfix);
    init(root);

    make_nullable(root);
    make_firstpos(root);
    make_lastpos(root);
    make_followpos(root);
//    cout<<endl;
//
//
    ofstream kout;
    kout.open("keyword_dfa.ap");
    inorder(root);
    vector<dfa_states> keyword_dfa;
    construct_dfa(root,root,keyword_dfa);
    init_final_states(keyword_dfa,root);
    for(int i = 0; i < keyword_dfa.size(); i++)
    {
        kout<<keyword_dfa[i].state_no<<"\t"<<keyword_dfa[i].is_final<<endl;
        for(int j = 0; j < 256; j++)
        {
            if(keyword_dfa[i].transitions[j]!=-1)
            {
                kout<<keyword_dfa[i].state_no<<"\t"<<(char)j<<"\t"<<keyword_dfa[i].transitions[j]<<endl;
            }
        }
    }
    dispose_tree(root);


    cout<<"Enter regex for Identifiers: "<<endl;
    cin>>input;
    handle_range(&input);
    cout<<"input: " <<input<<endl;
    root = NULL;
    len=strlen(input);
    memset(postfix,'\0',sizeof(postfix));
    position = 0;
    node_index = 0;
    infix_postfix(input);
    printf("%s\n",postfix);
    make_tree(postfix);
    init(root);

    clean(followpos_set);
    clean(firstpos_set);
    clean(lastpos_set);
    memset(nullable,0,2040);

    make_nullable(root);
    make_firstpos(root);
    make_lastpos(root);
    make_followpos(root);

    ofstream iout;

    iout.open("identifier_dfa.ap");
    inorder(root);
    vector<dfa_states> identifier_dfa;
    construct_dfa(root,root,identifier_dfa);
    init_final_states(identifier_dfa,root);
    for(int i = 0; i < identifier_dfa.size(); i++)
    {
        iout<<identifier_dfa[i].state_no<<"\t"<<identifier_dfa[i].is_final<<endl;
        for(int j = 0; j < 256; j++)
        {
            if(identifier_dfa[i].transitions[j]!=-1)
            {
                iout<<identifier_dfa[i].state_no<<"\t"<<(char)j<<"\t"<<identifier_dfa[i].transitions[j]<<endl;
            }
        }
    }
    dispose_tree(root);

    cout<<"Enter regex for Operators: "<<endl;
    cin>>input;
    handle_range(&input);
    cout<<"input: " <<input<<endl;
    root = NULL;
    len=strlen(input);
    memset(postfix,'\0',sizeof(postfix));
    position = 0;
    node_index = 0;
    infix_postfix(input);
    printf("%s\n",postfix);
    make_tree(postfix);
    init(root);

    clean(followpos_set);
    clean(firstpos_set);
    clean(lastpos_set);
    memset(nullable,0,2040);

    make_nullable(root);
    make_firstpos(root);
    make_lastpos(root);
    make_followpos(root);

    ofstream o_out;

    o_out.open("operator_dfa.ap");
    inorder(root);
    handle_range(&input);
    cout<<"input: " <<input<<endl;
    vector<dfa_states> operator_dfa;
    construct_dfa(root,root,operator_dfa);
    init_final_states(operator_dfa,root);
    for(int i = 0; i < operator_dfa.size(); i++)
    {
        o_out<<operator_dfa[i].state_no<<"\t"<<operator_dfa[i].is_final<<endl;
        for(int j = 0; j < 256; j++)
        {
            if(operator_dfa[i].transitions[j]!=-1)
            {
                o_out<<operator_dfa[i].state_no<<"\t"<<(char)j<<"\t"<<operator_dfa[i].transitions[j]<<endl;
            }
        }
    }
    dispose_tree(root);

    cout<<"Enter regex for Constant: "<<endl;
    cin>>input;
    handle_range(&input);
    cout<<"input: " <<input<<endl;
    root = NULL;
    len=strlen(input);
    memset(postfix,'\0',sizeof(postfix));
    position = 0;
    node_index = 0;
    infix_postfix(input);
    printf("%s\n",postfix);
    make_tree(postfix);
    init(root);

    clean(followpos_set);
    clean(firstpos_set);
    clean(lastpos_set);
    memset(nullable,0,2040);

    make_nullable(root);
    make_firstpos(root);
    make_lastpos(root);
    make_followpos(root);

    ofstream c_out;

    c_out.open("constant_dfa.ap");
    inorder(root);
    handle_range(&input);
    cout<<"input: " <<input<<endl;
    vector<dfa_states> constant_dfa;
    construct_dfa(root,root,constant_dfa);
    init_final_states(constant_dfa,root);
    for(int i = 0; i < constant_dfa.size(); i++)
    {
        c_out<<constant_dfa[i].state_no<<"\t"<<constant_dfa[i].is_final<<endl;
        for(int j = 0; j < 256; j++)
        {
            if(constant_dfa[i].transitions[j]!=-1)
            {
                c_out<<constant_dfa[i].state_no<<"\t"<<(char)j<<"\t"<<constant_dfa[i].transitions[j]<<endl;
            }
        }
    }
    dispose_tree(root);
    return 0;
}
