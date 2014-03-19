#include <iostream>
#include<fstream>
#include<vector>
#include<cstring>
#include<cstdlib>
using namespace std;
struct dfa
{
  int state_no;
  bool is_final;
  int transitions[256];
};
struct sym_table
{
    int id;
    int yylineno;
    char name[10240];
};
vector<dfa> keyword_dfa;
vector<dfa> operator_dfa;
vector<dfa> identifier_dfa;
vector<dfa> constant_dfa;
vector<sym_table> s_table;
int id = 0;
bool is_operator(char ch)
{
    //cout<<ch<<" "<<operator_dfa[0].transitions[ch]<<endl;
    return (operator_dfa[0].transitions[ch]!=-1)&&(operator_dfa[operator_dfa[0].transitions[ch]].is_final);
}
/* function to perform lexical analysis of
    file SOURCE.C */
void lex()
{
    FILE *fp=fopen("source.c","r");
    if(fp==NULL)
    {
        exit(1);
    }
    char ch;
    int yylineno=1;
    int yyleng=0;
    bool match_found;
    bool oper_found = false;
    char *yytext;
    char cur_lex[10240];
    char input[10240];
    /* read input from file */
    int k=0;
    while((ch=fgetc(fp))!=EOF)
    {
        input[k++]=ch;
    }
    input[k]='\0';
    printf("Input:%s\n",input);

    /*******************
    0->keyword
    1->opertor
    2->identifier
    3->constant
    *******************/
    int current_state[4];
    for(int i=0;i<4;i++)
    {
        current_state[i]=0;
    }
    k=0;
    int index=0;
    yytext=input;
    int mm = -1;

    /* loop to perform lexical analysis */
    while(1)
    {
        ch=input[k++];
        //printf("ch:%c\n",ch);

        if(ch==' ' || ch=='\t' || ch=='\n' || ch=='\0' || (is_operator(ch) && !oper_found))
        {
            //cout<<"if"<<endl;
            cur_lex[index]='\0';
            //cout<<current_state[1]<<endl;
            if(ch=='\n')
            {
                yylineno++;
            }
            if(current_state[0]!=-1 && keyword_dfa[current_state[0]].is_final)
            {
                printf("<Keyword,%s>\n",cur_lex);
            }
            else if(current_state[1]!=-1 && operator_dfa[current_state[1]].is_final)
            {
                printf("<Operator,%s>\n",cur_lex);
            }
            else if(current_state[2]!=-1 && identifier_dfa[current_state[2]].is_final)
            {
                //printf("Current_State:%d\n",current_state[2]);
                printf("<Identifier,%s>\n",cur_lex);
                sym_table st;
                st.id = id++;
                st.yylineno = yylineno;
                strcpy(st.name,cur_lex);
                s_table.push_back(st);
            }
            else if(current_state[3]!=-1 && constant_dfa[current_state[3]].is_final)
            {
                printf("<Constant,%s>\n",cur_lex);
            }
            else if(ch!='\0' && cur_lex[0] != 0)
            {
                cout<<"Cur_Lex: "<<cur_lex<<endl;
                printf("LEXICAL ERROR at line no:%d\n",yylineno);
            }
            if(ch=='\0')
            {
                break;
            }
            yytext=yytext+yyleng+1;
            index=0;
            mm = -1;
            yyleng=0;
            for(int i=0;i<4;i++)
            {
                current_state[i]=0;
            }

            if(is_operator(ch) && !oper_found)
            {
                //cout<<"oper found\n";
                yyleng++;
                //cur_lex[index++]=ch;
                if(current_state[0]!=-1)
                {
                    current_state[0]=keyword_dfa[current_state[0]].transitions[ch];
                    //printf("current_state:%d",current_state[0]);
                }
                if(current_state[1]!=-1)
                {
                    //printf("current_state:%d",current_state[1]);
                    current_state[1]=operator_dfa[current_state[1]].transitions[ch];
                    if(current_state[1]==-1)
                    {
                        if(mm!=-1)
                        {
                            cur_lex[index]='\0';
                            printf("<Operator,%s>\n",cur_lex);
                            index = 0;
                            for(int i =0;i<4;i++)
                                current_state[i] = 0;
                            yyleng = 0;
                            yytext=yytext+yyleng+1;
                            mm = -1;
                            current_state[0]=keyword_dfa[current_state[0]].transitions[ch];
                            current_state[1]=operator_dfa[current_state[1]].transitions[ch];
                        }
                    }
                    else
                        mm=current_state[1];
                }
                if(current_state[2]!=-1)
                {
                    //printf("state:%d\n",current_state[2]);
                    current_state[2]=identifier_dfa[current_state[2]].transitions[ch];
                    //printf("current_state:%d\n",current_state[2]);
                }
                if(current_state[3]!=-1)
                {
                    current_state[3]=constant_dfa[current_state[3]].transitions[ch];
                    //printf("current_state:%d",current_state[3]);
                }
                cur_lex[index++]=ch;
                oper_found = true;
            }
            continue;
        }
        else if(!oper_found)
        {
            //cout<<"else if"<<endl;
            yyleng++;
            //cout<<"C_STATE"<<current_state[2]<<endl;
            if(current_state[0]!=-1)
            {
                current_state[0]=keyword_dfa[current_state[0]].transitions[ch];
                //printf("current_state:%d",current_state[0]);
            }
            if(current_state[1]!=-1)
            {
                current_state[1]=operator_dfa[current_state[1]].transitions[ch];
                if(current_state[1]==-1)
                {
                    if(mm!=-1)
                    {
                        cur_lex[index]='\0';
                        printf("<Operator,%s>\n",cur_lex);
                        index = 0;
                        for(int i =0;i<4;i++)
                            current_state[i] = 0;
                        yyleng = 0;
                        yytext=yytext+yyleng+1;
                        mm = -1;
                        current_state[0]=keyword_dfa[current_state[0]].transitions[ch];
                            current_state[1]=operator_dfa[current_state[1]].transitions[ch];
                    }
                }
                //printf("current_state:%d",current_state[1]);
            }
            if(current_state[2]!=-1)
            {
                //printf("state:%d\n",current_state[2]);
                current_state[2]=identifier_dfa[current_state[2]].transitions[ch];
                //printf("current_state:%d\n",current_state[2]);
            }
            if(current_state[3]!=-1)
            {
                current_state[3]=constant_dfa[current_state[3]].transitions[ch];
                //printf("current_state:%d",current_state[3]);
            }
            cur_lex[index++]=ch;
        }
        else
        {
            //cout<<"else"<<endl;
            if(!is_operator(ch))
            {
                oper_found = false;
                cur_lex[index]='\0';

                if(current_state[0]!=-1 && keyword_dfa[current_state[0]].is_final)
                {
                    printf("<Keyword,%s>\n",cur_lex);
                }
                else if(current_state[1]!=-1 && operator_dfa[current_state[1]].is_final)
                {
                    printf("<Operator,%s>\n",cur_lex);
                }
                else if(current_state[2]!=-1 && identifier_dfa[current_state[2]].is_final)
                {
                    //printf("Current_State:%d\n",current_state[2]);
                    printf("<Identifier,%s>\n",cur_lex);
                    sym_table st;
                    st.id = id++;
                    st.yylineno = yylineno;
                    strcpy(st.name,cur_lex);
                    s_table.push_back(st);
                }
                else if(current_state[3]!=-1 && constant_dfa[current_state[3]].is_final)
                {
                    printf("<Constant,%s>\n",cur_lex);
                }
                else if(ch!='\0' && cur_lex[0] != 0)
                {
                    cout<<"Cur_Lex: "<<(int)cur_lex[0]<<endl;
                    printf("LEXICAL ERROR at line no:%d\n",yylineno);
                }
                if(ch=='\0')
                {
                    break;
                }
                yytext=yytext+yyleng+1;
                index=0;
                yyleng=0;
                mm = -1;
                for(int i=0;i<4;i++)
                {
                    current_state[i]=0;
                }
            }
            yyleng++;
            if(current_state[0]!=-1)
            {
                current_state[0]=keyword_dfa[current_state[0]].transitions[ch];
                //printf("current_state:%d",current_state[0]);
            }
            if(current_state[1]!=-1)
            {
                current_state[1]=operator_dfa[current_state[1]].transitions[ch];
                if(current_state[1]==-1)
                {
                    if(mm!=-1)
                    {
                        cur_lex[index]='\0';
                        printf("<Operator,%s>\n",cur_lex);
                        index = 0;
                        for(int i =0;i<4;i++)
                            current_state[i] = 0;
                        yyleng = 0;
                        yytext=yytext+yyleng+1;
                        mm=-1;
                        current_state[0]=keyword_dfa[current_state[0]].transitions[ch];
                            current_state[1]=operator_dfa[current_state[1]].transitions[ch];
                    }
                }
                //printf("current_state:%d",current_state[1]);
            }
            if(current_state[2]!=-1)
            {
                //printf("state:%d\n",current_state[2]);
                current_state[2]=identifier_dfa[current_state[2]].transitions[ch];
                //printf("current_state:%d\n",current_state[2]);
            }
            if(current_state[3]!=-1)
            {
                current_state[3]=constant_dfa[current_state[3]].transitions[ch];
                //printf("current_state:%d",current_state[3]);
            }
            cur_lex[index++]=ch;
        }
    }
    cout<<"Symbol Table: \n";
    for(int i = 0;i < s_table.size(); i++)
    {
        cout<<s_table[i].id<<"\t"<<s_table[i].name<<"\t"<<s_table[i].yylineno<<endl;
    }
}

int main()
{
    ifstream kin;
    int cur_state = 0;
    kin.open("keyword_dfa.ap");
    struct dfa d;
    kin>>d.state_no>>d.is_final;
    memset(d.transitions,-1,sizeof(d.transitions));
    int c_state,d_state;
    char c;
    while(!kin.eof())
    {
        kin>>c_state;
        if(kin.eof())
            break;
        //cout<<c_state<<endl;
        if(c_state==cur_state)
        {
            kin>>c>>d_state;
            d.transitions[c] = d_state;
        }
        else
        {
            keyword_dfa.push_back(d);
            memset(d.transitions,-1,sizeof(d.transitions));
            cur_state = c_state;
            d.state_no = c_state;
            kin>>d.is_final;
        }
    }
    keyword_dfa.push_back(d);

    ifstream i_in;
    cur_state = 0;
    i_in.open("identifier_dfa.ap");
    struct dfa d1;
    i_in>>d1.state_no>>d1.is_final;
    memset(d1.transitions,-1,sizeof(d1.transitions));
    while(!i_in.eof())
    {
        i_in>>c_state;
        if(i_in.eof())
            break;
        //cout<<c_state<<endl;
        if(c_state==cur_state)
        {
            i_in>>c>>d_state;
            d1.transitions[c] = d_state;
        }
        else
        {
            identifier_dfa.push_back(d1);
            memset(d1.transitions,-1,sizeof(d1.transitions));
            cur_state = c_state;
            d1.state_no = c_state;
            i_in>>d1.is_final;
        }
    }
    identifier_dfa.push_back(d1);

    ifstream o_in;
    cur_state = 0;
    o_in.open("operator_dfa.ap");
    struct dfa d2;
    o_in>>d2.state_no>>d2.is_final;
    memset(d2.transitions,-1,sizeof(d2.transitions));
    while(!o_in.eof())
    {
        o_in>>c_state;
        if(o_in.eof())
            break;
        //cout<<c_state<<endl;
        if(c_state==cur_state)
        {
            o_in>>c>>d_state;
            d2.transitions[c] = d_state;
        }
        else
        {
            operator_dfa.push_back(d2);
            memset(d2.transitions,-1,sizeof(d2.transitions));
            cur_state = c_state;
            d2.state_no = c_state;
            o_in>>d2.is_final;
        }
    }
    operator_dfa.push_back(d2);

    ifstream c_in;
    cur_state = 0;
    c_in.open("constant_dfa.ap");
    struct dfa d3;
    c_in>>d3.state_no>>d3.is_final;
    memset(d3.transitions,-1,sizeof(d3.transitions));
    while(!c_in.eof())
    {
        c_in>>c_state;
        if(c_in.eof())
            break;
        //cout<<c_state<<endl;
        if(c_state==cur_state)
        {
            c_in>>c>>d_state;
            d3.transitions[c] = d_state;
        }
        else
        {
            constant_dfa.push_back(d3);
            memset(d3.transitions,-1,sizeof(d3.transitions));
            cur_state = c_state;
            d3.state_no = c_state;
            c_in>>d3.is_final;
        }
    }
    constant_dfa.push_back(d3);

    lex();
}
