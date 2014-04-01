#include <iostream>
#include<map>
#include<string.h>
#include<string>
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include<climits>
#include "func.h"
#include<algorithm>
#include<set>
#include<fstream>
#include<cassert>

using namespace std;
multimap<string,string> rules,left_factored_rules,current_non_term_rules;
vector<string> nullable_recursed;
vector<string> first_recursed;
vector<string> follow_recursed;
vector<string> n_t,t;
map< string,set<string> > first_sets;
map< string,set<string> > follow_sets;
map< string,vector<string> > LL1_PT;
int next_nt;
vector<string> prefixes;
vector<string> left_rec_n_t;
vector<string> left_fact_n_t;
/******************************/
/* Not more than 10000 rules.*/
/******************************/
bool is_matched[1000][10000];
bool is_considered[10000];
void itoa(int num,string &str)
{
	char a[] = {'0','1','2','3','4','5','6','7','8','9'};
	int i =0;
	while(num/10!=0)
	{
		str.push_back(a[num/10]);
		num /= 10;
		i++;
	}
    str.push_back(a[num]);
	//str[i+1]='\0';
}

string next_nt_string()
{
    string num;
    string str="NON_TERMINAL";
    itoa(next_nt,num);
    str.append(num);
    next_nt++;
    return str;
}
/*********************************************/
/* Will add Non Terminals. */
/*********************************************/
void update_rules(bool is_considered[])
{
    /******************************/
    /* Delete all the rules which
    /* were considered by the
    /* left factoring.*/
    /******************************/
    multimap<string,string>::iterator it;
    for(int i=0;i<current_non_term_rules.size();i++)
    {
        if(!is_considered[i])
        {
            it=current_non_term_rules.begin();
            advance(it,i);
            left_factored_rules.insert(pair<string,string>((*it).first,(*it).second));
        }
    }
}
void left_factor()
{
    /*************************************/
    /* Algorithm
    /* Generate all the prefixes of
       all the rules.
    /* Take each prefix one by one
    /* Check if it is the common
    /* prefix of atleast two of the
    /* rules.
    /* Update the longest common prefix.*/
    /*************************************/
    bool non_terminal_used;
    string RHS;
    string pre;
    multimap<string,string>::iterator it;
    string new_left_rule;

//    for(int i=0;i<prefixes.size();i++)
//    {
//        cout<<"Prefix:"<<prefixes[i]<<endl;
//    }
    string prefix_of_rhs;
    int len_of_prefix;
    int len_of_longest_prefix;
    string longest_prefix; // keeps track of what is the longest prefix.
    int longest_prefix_index=-1;
    int count_of_matches; // keeps track of how many rules match for a given prefix.
    string left,remaining; // left holds the prefix. // remaining holds the remaining RHS.
    string new_non_terminal;
    for(int n=0;n<n_t.size();n++)
    {
        memset(is_considered,0,sizeof(is_considered));
        memset(is_matched,0,sizeof(is_matched));
        prefixes.clear();
        current_non_term_rules.clear();
        it=rules.find(n_t[n]);
        for(;it!=rules.end() && ((*it).first.compare(n_t[n])==0);it++)
        {
            current_non_term_rules.insert(pair<string,string>((*it).first,(*it).second));
        }
        for(it=current_non_term_rules.begin();it!=current_non_term_rules.end();it++)
        {
            pre.clear();
            RHS=(*it).second;
            //cout<<(*it).first<<"-->"<<(*it).second<<endl;
            //cout<<RHS.size();
            for(int i=0;i<RHS.size();i++)
            {
                pre.clear();
                for(int j=0;j<=i;j++)
                {
                    pre.push_back(RHS[j]);
                }
                if(find(prefixes.begin(),prefixes.end(),pre)==prefixes.end())
                    prefixes.push_back(pre);
            }
        }
//        for(it=current_non_term_rules.begin();it!=current_non_term_rules.end();it++)
//        {
//            cout<<(*it).first<<"-->"<<(*it).second;
//        }
        for(int m=0;m<current_non_term_rules.size();m++)
        {
            //cout<<"Round "<<m<<endl;
            longest_prefix.clear();
            for(int i=0;i<prefixes.size();i++)
            {
                //cout<<"Current Prefix:"<<prefixes[i]<<endl;
                len_of_prefix=prefixes[i].size();
                count_of_matches=0;
                for(it=current_non_term_rules.begin();it!=current_non_term_rules.end();it++)
                {
        //            if(is_matched[it-rules.begin()])
        //                continue;
                    prefix_of_rhs.clear();
                    RHS.clear();
                    RHS=(*it).second;
                    //cout<<(*it).first<<"-->"<<(*it).second<<endl;
                    if(RHS.size()<len_of_prefix|| is_considered[distance(current_non_term_rules.begin(),it)])
                        continue;
                    for(int j=0;j<len_of_prefix;j++)
                    {
                        prefix_of_rhs.push_back(RHS[j]);
                    }
                    //cout<<"Prefix of RHS:"<<prefix_of_rhs<<endl;
                    if(prefix_of_rhs.compare(prefixes[i])==0)
                    {
                        //cout<<prefix_of_rhs<<"matched with "<<prefixes[i]<<endl;
                        //cout<<"Rule No:"<<distance(rules.begin(),it)<<endl;
                        is_matched[i][distance(current_non_term_rules.begin(),it)]=true;
                        count_of_matches++;
                    }
                }
    //            cout<<"RUles after longest prefix"<<endl;
    //            for(it=rules.begin();it!=rules.end();it++)
    //            {
    //                cout<<"Rule No:"<<<<"-"<<(*it).first<<"-->"<<(*it).second<<endl;
    //            }
                //cout<<"Prefix is:"<<prefixes[i]<<" has count:"<<count_of_matches<<endl;
                if(count_of_matches>1 && prefixes[i].size()>=1)
                {
                    if(longest_prefix.size()<prefixes[i].size())
                    {
                        longest_prefix.clear();
                        longest_prefix=prefixes[i];
                        longest_prefix_index=i;
                    }
                }
            }
            //cout<<"Index:"<<longest_prefix_index<<"and longest prefix is :"<<longest_prefix<<"!"<<endl;
            if(longest_prefix_index!=-1 && longest_prefix.size()>=1)
            {
                /* Get rules which matched the longest prefix.*/
                non_terminal_used=false;
                new_non_terminal=next_nt_string();
                for(int j=0;j<current_non_term_rules.size();j++)
                {
                    if(is_matched[longest_prefix_index][j] && !is_considered[j])
                    {
                        non_terminal_used=true;
                        it=current_non_term_rules.begin();
                        advance(it,j);
                        RHS=(*it).second;
                        //cout<<"Rule No:"<<j<<"-"<<(*it).first<<"-->"<<(*it).second<<endl;
                        len_of_longest_prefix=longest_prefix.size();
                        left.clear();
                        remaining.clear();
                        for(int k=0;k<len_of_longest_prefix;k++)
                        {
                            left.push_back(RHS[k]);
                        }
                        if(RHS[len_of_longest_prefix]==' ')
                            len_of_longest_prefix++;
                        for(int k=len_of_longest_prefix;k<RHS.size();k++)
                        {
                            remaining.push_back(RHS[k]);
                        }
                        if(remaining.size()==0)
                            remaining="$"; //epsilon.
                        //cout<<"Remaining:"<<remaining<<" and new non terminal: "<<new_non_terminal<<endl;
                        /* Erase the current rule an push back the new rule in rules.*/
                        assert(left.compare(longest_prefix)==0);
                        //cout<<"Prefix:"<<left<<"and remaining: "<<remaining<<endl;
                        new_left_rule.clear();
                        if(longest_prefix[longest_prefix.size()-1]!=' ')
                            new_left_rule.append(longest_prefix).append(" ").append(new_non_terminal);
                        else
                            new_left_rule.append(longest_prefix).append(new_non_terminal);
                        left_factored_rules.insert(pair<string,string>(new_non_terminal,remaining));
                        is_considered[distance(current_non_term_rules.begin(),it)]=true;
                        //rules.erase(it);
                    }
                }
                if(non_terminal_used)
                {
                    left_factored_rules.insert(pair<string,string>((*it).first,new_left_rule));
                    left_fact_n_t.push_back(new_non_terminal);
                }
            }
        }
        update_rules(is_considered);
    }
    multimap<string,string>::iterator iter;
    for(iter=left_factored_rules.begin();iter!=left_factored_rules.end();iter++)
    {
        cout<<(*iter).first<<"-->"<<(*iter).second<<endl;
    }
}
void eliminate_immediate_left_recursion(multimap<string,string> &rules,int n_t_no)
{
    cout<<"Immediate Left Recurse"<<endl;
    vector<string> non_left_recursive_rules,left_recursive_rules;
    multimap<string,string>::iterator it;
    it=rules.find(n_t[n_t_no]);
    string non_term=(*it).first;
    cout<<"Non Terminal Being seen:"<<non_term<<endl;
    string RHS;
    string leftmost,remaining;
    const char *rhs;
    //string tok_rhs;
    char *tok_rhs;
    char r[1000];
    int i;
    for(;it!=rules.end() && (((*it).first.compare(non_term))==0);it++)
    {
        leftmost.clear();
        remaining.clear();
        RHS=(*it).second;

        //cout<<non_term<<"-->"<<RHS<<endl;
//        rhs=RHS.c_str();
//        cout<<"<1>"<<endl;
//        strcpy(r,rhs);
//        cout<<"r:"<<r<<endl;
//        tok_rhs=strtok(r," ");
//        cout<<"tok_rhs:"<<tok_rhs<<"and r:"<<r<<endl;
        for(i=0;i<RHS.size();i++)
        {
            if(RHS[i]!=' ')
            {
                leftmost.push_back(RHS[i]);
            }
            else
            {
                break;
            }
        }
        for(int j=i+1;j<RHS.size();j++)
        {
            remaining.push_back(RHS[j]);
        }
//        remaining.copy(r,strlen(r));
//        leftmost.copy(tok_rhs,strlen(tok_rhs));
        //cout<<"Remaining:"<<remaining<<"Leftmost:"<<leftmost<<endl;
        /****************/
        /* First Token */
        /****************/
        if(leftmost.compare(non_term)==0)
        {
            /*******************/
            /* Left Recursion */
            /*******************/
            left_recursive_rules.push_back(remaining);
        }
        else
        {
            non_left_recursive_rules.push_back(RHS);
        }
    }
    if(left_recursive_rules.size()==0)
        return;
    /***********************************/
    /* Delete all the old rules. */
    /***********************************/
    while(rules.find(n_t[n_t_no])!=rules.end())
    {
        rules.erase(rules.find(n_t[n_t_no]));
    }
    //cout<<"Intermediate Rules.."<<endl;
    for(it=rules.begin();it!=rules.end();it++);
        //cout<<(*it).first<<"-->"<<(*it).second<<endl;
    string new_non_terminal;
    new_non_terminal=next_nt_string();
    left_rec_n_t.push_back(new_non_terminal);
    //n_t.push_back(new_non_terminal);
    rules.insert(pair<string,string>(new_non_terminal,"$"));
    //cout<<new_non_terminal<<endl;
    string new_RHS;
    for(int i=0;i<non_left_recursive_rules.size();i++)
    {
        //<<non_left_recursive_rules[i]<<endl;
        new_RHS.append(non_left_recursive_rules[i]).append(" ").append(new_non_terminal);
        rules.insert(pair<string,string>(n_t[n_t_no],new_RHS));
        new_RHS.clear();
    }
    new_RHS.clear();
    for(int i=0;i<left_recursive_rules.size();i++)
    {
        //cout<<left_recursive_rules[i]<<endl;
        new_RHS.append(left_recursive_rules[i]).append(" ").append(new_non_terminal);
        rules.insert(pair<string,string>(new_non_terminal,new_RHS));
        new_RHS.clear();
    }
    for(it=rules.begin();it!=rules.end();it++);
        //cout<<(*it).first<<"-->"<<(*it).second<<endl;
}
/*************************************************/
/* Will only add new Non Terminals. */
/*************************************************/
void eliminate_left_recursion(multimap<string,string> &rules)
{
    //cout<<"Left Recurse"<<endl;
    multimap<string,string>::iterator it;
    multimap<string,string>::iterator it1;
    string non_term,non_term_1;
    string RHS,leftmost,RHS1,remaining;
    const char *rhs;
    //string tok_rhs;
    char *tok_rhs;
    char r[1000];
    string new_rhs;
    for(int i=0;i<n_t.size();i++)
    {
        for(int j=0;j<i;j++)
        {
            it=rules.find(n_t[i]);
            non_term=(*it).first;
            for(;it!=rules.end() && (((*it).first.compare(non_term))==0);it++)
            {
                RHS=(*it).second;
                rhs=RHS.c_str();
                strcpy(r,rhs);
                tok_rhs=strtok(r," ");
                remaining.copy(r,strlen(r));
                leftmost.copy(tok_rhs,strlen(tok_rhs));
                if(leftmost.compare(n_t[j])==0)
                {
                    new_rhs.clear();
                    it1=rules.find(n_t[j]);
                    non_term_1=(*it1).first;
                    for(;it1!=rules.end() && (((*it1).first.compare(non_term_1))==0);it1++)
                    {
                        RHS1=(*it1).second;
                        new_rhs.append(RHS1).append(remaining);
                        rules.erase(it);
                        rules.insert(pair<string,string>(non_term,new_rhs));
                    }

                }
            }
        }
        eliminate_immediate_left_recursion(rules,i);
    }
}
void update_non_terminals()
{
    for(int i=0;i<left_rec_n_t.size();i++)
    {
        if(find(n_t.begin(),n_t.end(),left_rec_n_t[i])==n_t.end())
        {
            n_t.push_back(left_rec_n_t[i]);
        }
    }
    for(int i=0;i<left_fact_n_t.size();i++)
    {
        if(find(n_t.begin(),n_t.end(),left_fact_n_t[i])==n_t.end())
        {
            n_t.push_back(left_fact_n_t[i]);
        }
    }
}

/* Function to find if a non terminal can generate epsilon
    $- for epsilon*/
bool is_nullable(string non_terminal)
{
    //cout<<"Token: "<<non_terminal<<endl;
    if(non_terminal.compare("$")==0)
    {
        //cout<<"epsilon production"<<endl;
        vector<string>::iterator iter=find(nullable_recursed.begin(),nullable_recursed.end(),non_terminal);
        if(iter!=nullable_recursed.end())
            nullable_recursed.erase(iter);
        return true;
    }
    else if(find(t.begin(),t.end(),non_terminal)!=t.end())
    {
        vector<string>::iterator iter=find(nullable_recursed.begin(),nullable_recursed.end(),non_terminal);
        if(iter!=nullable_recursed.end())
            nullable_recursed.erase(iter);
        return false;
    }
    multimap<string,string>::iterator it;
    it = rules.find(non_terminal);
    string token;
    bool flag = false;
    while(it!=rules.end() && (it->first).compare(non_terminal)==0)
    {
        flag = false;
        token.clear();
        //cout<<"RHS: "<<(it->second)<<endl;
        for(int i = 0; i < (it->second).size(); i++)
        {
            //cout<<"char: "<<(it->second)[i]<<endl;
            if((it->second)[i]!=' ' && i!=(it->second).size()-1)
            {
                token.push_back((it->second)[i]);
            }
            else
            {
                if(i==(it->second).size()-1)
                    token.push_back((it->second)[i]);
                vector<string>::iterator iter=find(nullable_recursed.begin(),nullable_recursed.end(),token);
                if(iter!=nullable_recursed.end())
                {
                    flag = false;
                    break;
                }
                nullable_recursed.push_back(token);
                if(is_nullable(token))
                {
                    //cout<<token<<" is nullable"<<endl;
                    flag = true;
                    token.clear();
                    continue;
                }
                else
                {
                    //cout<<token<<" is not nullable"<<endl;
                    flag = false;
                    break;
                }
            }
        }
        if(flag)
        {
            vector<string>::iterator iter=find(nullable_recursed.begin(),nullable_recursed.end(),non_terminal);
            if(iter!=nullable_recursed.end())
                nullable_recursed.erase(iter);
            return true;
        }
        it++;
    }
    //cout<<"finishing"<<endl;
    vector<string>::iterator iter=find(nullable_recursed.begin(),nullable_recursed.end(),non_terminal);
    if(iter!=nullable_recursed.end())
            nullable_recursed.erase(iter);
    return false;
}

set<string> calc_first(string symbol)
{
    //cout<<"Symbol: "<<symbol<<endl;
    if(symbol.compare("$")==0)
    {
        set<string> ftemp;
        ftemp.insert("$");
        vector<string>::iterator ite=find(first_recursed.begin(),first_recursed.end(),symbol);
        if(ite!=first_recursed.end())
            first_recursed.erase(ite);
        return ftemp;
    }
    else if(find(t.begin(),t.end(),symbol)!=t.end())
    {
        set<string> ftemp;
        ftemp.insert(symbol);
        vector<string>::iterator ite=find(first_recursed.begin(),first_recursed.end(),symbol);
        if(ite!=first_recursed.end())
            first_recursed.erase(ite);
        return ftemp;
    }
    set<string> final_set;
    multimap<string,string>::iterator it;
    it = rules.find(symbol);
    string token;
    vector<string> tokens;
    while(it!=rules.end() && (it->first).compare(symbol)==0)
    {
        token.clear();
        tokens.clear();
        for(int i = 0; i < (it->second).size(); i++)
        {
            if((it->second)[i]!=' ' && i!=(it->second).size()-1)
            {
                token.push_back((it->second)[i]);
            }
            else
            {
                if(i==(it->second).size()-1)
                    token.push_back((it->second)[i]);
                tokens.push_back(token);
                token.clear();
            }
        }
        set<string> returned_fset;
        //cout<<"token-0: "<<tokens[0]<<" "<<tokens.size()<<endl;
        vector<string>::iterator ite=find(first_recursed.begin(),first_recursed.end(),tokens[0]);
        if(ite!=first_recursed.end())
        {

        }
        else
        {
            first_recursed.push_back(tokens[0]);
            returned_fset = calc_first(tokens[0]);
        }
        int j=0;
        int cnt=0;
        for(j = 1; j  < tokens.size(); j++)
        {
            //cout<<j<<" "<<tokens[j]<<endl;
            nullable_recursed.push_back(tokens[j-1]);
            if(is_nullable(tokens[j-1]))
            {
                cnt++;
                vector<string>::iterator ite=find(first_recursed.begin(),first_recursed.end(),tokens[j]);
                if(ite!=first_recursed.end())
                {

                }
                else
                {
                    first_recursed.push_back(tokens[j]);
                    Union(returned_fset,calc_first(tokens[j]),returned_fset);
                }
            }
            else
                break;
        }
        if(cnt==tokens.size())
        {
            returned_fset.insert("$");
        }
        Union(returned_fset,final_set,final_set);
        //cout<<"Inc it"<<endl;
        it++;
    }
    //cout<<"returning"<<endl;
    map< string,set<string> > :: iterator iter = first_sets.find(symbol);
    if(iter!=first_sets.end())
    {
        Union(final_set,first_sets[symbol],final_set);
        first_sets[symbol] = final_set;
    }
    else
    {
        first_sets[symbol] = final_set;
    }
    vector<string>::iterator ite=find(first_recursed.begin(),first_recursed.end(),symbol);
    if(ite!=first_recursed.end())
        first_recursed.erase(ite);
    return final_set;
}
/* Function to find first symbol of  non-terminals */
void first()
{
    set<string> sets;
    for(int i =0; i < n_t.size(); i++)
    {
        first_recursed.push_back(n_t[i]);
        sets = calc_first(n_t[i]);
    }
    set<string> term_fset;
    for(int i = 0; i < t.size(); i++)
    {
        term_fset.clear();
        term_fset.insert(t[i]);
        first_sets[t[i]] = term_fset;
    }
    term_fset.clear();
    term_fset.insert("$");
    first_sets["$"] = term_fset;
    map< string,set<string> > :: iterator iter;
    for(iter = first_sets.begin(); iter!=first_sets.end();iter++)
    {
        set<string>::iterator it;
        cout<<"First of: "<<iter->first<<endl;
        for(it = iter->second.begin();it!=iter->second.end();it++)
        {
            cout<<(*it)<<endl;
        }
    }
}
/* Function to calculate follow from the production rule */
set<string> calc_follow(string symbol)
{
    //cout<<"Symbol: "<<symbol<<endl;
    multimap<string,string>::iterator it;
    string token;
    vector<string> tokens;
    for(it = rules.begin();it!=rules.end(); it++)
    {
        token.clear();
        tokens.clear();
        bool flag = false;
        for(int i = 0; i < (it->second).size(); i++)
        {
            if((it->second)[i]!=' ' && i!=(it->second).size()-1)
            {
                token.push_back((it->second)[i]);
            }
            else
            {
                if(i==(it->second).size()-1)
                    token.push_back((it->second)[i]);
                if(token.compare(symbol)==0)
                    flag = true;
                tokens.push_back(token);
                token.clear();
            }
        }
        if(flag)
        {
            /*          A-> alpha B beta
               add every thing in First(beta) in Follow(B) */
            set<string>  foll_set;
            for(int i = 0; i< tokens.size(); i++)
            {
                //cout<<i<<"th rhs: "<<tokens[i]<<endl;
                foll_set.clear();
                if(find(t.begin(),t.end(),tokens[i])==t.end())
                {
                    if(i < tokens.size()-1)
                    {
                        Union(foll_set,first_sets[tokens[i+1]],foll_set);
                        for(int j = i+2; j < tokens.size(); j++)
                        {
                            if(is_nullable(tokens[j-1]))
                                Union(foll_set,first_sets[tokens[j]],foll_set);
                            else
                                break;
                        }
                    }
                    map< string,set<string> > :: iterator iter = follow_sets.find(tokens[i]);
                    if(iter!=follow_sets.end())
                    {
                        Union(foll_set,follow_sets[tokens[i]],foll_set);
                        follow_sets[tokens[i]] = foll_set;
                    }
                    else
                    {
                        follow_sets[tokens[i]] = foll_set;
                    }
                }
            }


            /*          A->alpha B beta
               add everything in FOLLOW(A) to FOLLOW(B) */
            for(int i = 0; i < tokens.size(); i++)
            {
                if(find(t.begin(),t.end(),tokens[i])==t.end())
                {
                    foll_set.clear();
                    bool flag1 = true;

                    for(int j = i+1; j < tokens.size(); j++)
                    {
                        if(is_nullable(tokens[j]))
                            flag1 = true;
                        else
                        {
                            flag1 = false;
                            break;
                        }
                    }
                    if(flag1)
                    {
                        set<string> foll_A;
                        vector<string>::iterator ite=find(follow_recursed.begin(),follow_recursed.end(),it->first);
                        if(ite!=follow_recursed.end())
                        {
                            break;
                        }
                        else
                        {
                            follow_recursed.push_back(it->first);
                        }
                        foll_A = calc_follow(it->first);
                        set<string>::iterator is = foll_A.begin();
                        //cout<<*is<<endl;
                        map< string,set<string> > :: iterator iter = follow_sets.find(tokens[i]);
                        //cout<<tokens[i]<<endl;
                        if(iter!=follow_sets.end())
                        {
                            //cout<<"here"<<endl;
                            Union(foll_A,follow_sets[tokens[i]],foll_A);
                            follow_sets[tokens[i]] = foll_A;
                        }
                        else
                        {
                            //cout<<"no, here"<<endl;
                            follow_sets[tokens[i]] = foll_A;
                        }
                    }
                }
            }
        }
    }
    vector<string>::iterator ite=find(follow_recursed.begin(),follow_recursed.end(),symbol);
    if(ite!=follow_recursed.end())
        follow_recursed.erase(ite);
    return follow_sets[symbol];
}
/* Function to calculate follow set of all the symbols */
void follow()
{
    set<string> init;
    init.insert("#");
    follow_sets[n_t[0]] = init;
    for(int i = 0;i < n_t.size();i++)
    {
        calc_follow(n_t[i]);
    }

    map< string,set<string> > :: iterator iter;
    for(iter = follow_sets.begin(); iter!=follow_sets.end();iter++)
    {
        set<string>::iterator it;
        cout<<"Follow of: "<<iter->first<<endl;
        for(it = iter->second.begin();it!=iter->second.end();it++)
        {
            cout<<(*it)<<endl;
        }
    }
}
/* Function to create LL1 parsging table for the give grammer */
void construct_LL1_pt()
{
    multimap<string,string>::iterator it;
    string token;
    vector<string> tokens;
    /* For each production A->alpha */
    for(it = rules.begin();it!=rules.end(); it++)
    {
        token.clear();
        tokens.clear();
        for(int i = 0; i < (it->second).size(); i++)
        {
            if((it->second)[i]!=' ' && i!=(it->second).size()-1)
            {
                token.push_back((it->second)[i]);
            }
            else
            {
                if(i==(it->second).size()-1)
                    token.push_back((it->second)[i]);
                tokens.push_back(token);
                token.clear();
            }
        }


        int j;
        set<string> first_alpha;
        for(j = 0; j < tokens.size(); j++)
        {
            Union(first_alpha,first_sets[tokens[j]],first_alpha);
            if(!is_nullable(tokens[j]))
                break;
        }
        set<string>::iterator iter;
        for( iter = first_alpha.begin(); iter != first_alpha.end(); iter++)
        {
            vector<string> :: iterator ip;
             /* For each terminal t in First(alpha)
                    LL1_PT[A,t] = alpha */
            if((ip = find(t.begin(),t.end(),*iter))!=t.end())
            {
                cout<<"LL1: "<<ip-t.begin()<<" "<<t[ip-t.begin()]<<" "<<*iter<<endl;
                if(LL1_PT[it->first][ip-t.begin()].compare("`") == 0)
                    LL1_PT[it->first][ip-t.begin()] = it->second;
                else
                    cout<<"Grammer is ambiguous "<<it->first<<" "<<it->second<<endl;
            }
            /* If epsilon is in First(alpha) */
            else
            {
                set<string> follow_A = follow_sets[it->first];
                set<string> :: iterator is;
                /* for each terminal b in Follow(A)
                        LL1_PT[A,b] = alpha */
                for(is = follow_A.begin();is != follow_A.end(); is++)
                {
                    if((ip = find(t.begin(),t.end(), *is))!=t.end())
                    {
                        cout<<"LL1: "<<ip-t.begin()<<" "<<t[ip-t.begin()]<<" "<<*is<<endl;
                        if(LL1_PT[it->first][ip-t.begin()].compare("`") == 0)
                            LL1_PT[it->first][ip-t.begin()] = it->second;
                        else
                            cout<<"Grammer is ambiguous "<<it->first<<" "<<it->second<<endl;

                    }
                    else if((*is).compare("#") == 0)
                    {
                        if(LL1_PT[it->first][t.size()].compare("`") == 0)
                            LL1_PT[it->first][t.size()] = it->second;
                        else
                            cout<<"Grammer is ambiguous "<<it->first<<" "<<it->second<<endl;
                    }
                }
            }
        }
    }
}
int main()
{
    int no_n_t,no_t;
    printf("No of Non Terminals\n");
    scanf("%d",&no_n_t);
    printf("Enter Non Terminals\n");
    string non_terminal,terminal;
    while(no_n_t--)
    {
        cin>>non_terminal;
        n_t.push_back(non_terminal);
    }
    printf("No of Terminals\n");
    scanf("%d",&no_t);
    printf("Enter Terminals\n");
    while(no_t--)
    {
        cin>>terminal;
        t.push_back(terminal);
    }
    printf("Enter Rules\n");
    char c;
    string rule,cur_rule;
    int len;
    multimap<string,string>::iterator it;
    fflush(stdin);
    for(unsigned int i=0;i<n_t.size();i++)
    {
        rule.clear();
        cout<<"For "<<n_t[i]<<":"<<endl;
        while((c=getc(stdin))!='\n')
        {
            rule.push_back(c);
        }
        //cout<<rule<<endl;
        /*********************/
        /* Rules Parser ('|')*/
        /*********************/
        len=rule.size();
        cur_rule.clear();
        for(int j=0;j<len;j++)
        {
            if(rule[j]=='|')
            {
                rules.insert(pair<string,string>(n_t[i],cur_rule));
                cur_rule.clear();
            }
            else
            {
                cur_rule.push_back(rule[j]);
            }
        }
        rules.insert(pair<string,string>(n_t[i],cur_rule));
        cur_rule.clear();
    }
    for(it=rules.begin();it!=rules.end();it++)
    {
        //cout<<(*it).first<<" "<<(*it).second<<endl;
    }
    //left_factor();

   /*********************************/
    /* Eliminating Left Recursion. */
    /*********************************/
    eliminate_left_recursion(rules);
    cout<<"New Rules"<<endl;
    update_non_terminals();
    for(it=rules.begin();it!=rules.end();it++)
    {
        cout<<(*it).first<<" "<<(*it).second<<endl;
    }
    cout<<"----------------------------------------------"<<endl;
    left_factor();
    update_non_terminals();
    rules.clear();
    for(it=left_factored_rules.begin();it!=left_factored_rules.end();it++)
    {
        rules.insert(pair<string,string>((*it).first,(*it).second));
    }
    cout<<"Rules after left factoring"<<endl;
    for(it=rules.begin();it!=rules.end();it++)
    {
        cout<<(*it).first<<" "<<(*it).second<<endl;
    }
    cout<<"Non Terminals..."<<endl;
    for(int i=0;i<n_t.size();i++)
    {
        cout<<n_t[i]<<endl;
    }

    first();
    follow();

    for(int i = 0; i < n_t.size(); i++)
    {
        vector<string> row(t.size()+1,"`");
        LL1_PT[n_t[i]] = row;
    }

    construct_LL1_pt();
    ofstream fout;
    fout.open("LL1_Table.ap");
    map< string,vector<string> > ::iterator ip;
//    fout<<"\t";
//    for(int i = 0; i < t.size(); i++)
//        fout<<t[i]<<"\t";
//    fout<<"#"<<endl;
    for(ip = LL1_PT.begin(); ip != LL1_PT.end(); ip++)
    {
        fout<<ip->first;
        for(int i = 0; i < t.size(); i++)
        {
            fout<<"\t"<<LL1_PT[ip->first][i];
        }
        fout<<"\t"<<LL1_PT[ip->first][t.size()]<<endl;
    }
    fout.close();

    fout.open("non_terminals.ap");
    for(int i = 0; i < n_t.size(); i++)
        fout<<n_t[i]<<"\t";
    fout.close();
    fout.open("terminals.ap");
    for(int i = 0; i < t.size(); i++)
        fout<<t[i]<<"\t";
    fout<<"#";
    fout.close();

    cout<<n_t[0]<<endl;
}
