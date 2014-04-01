%{
#include <stdio.h>
#include "input1.tab.h"
#include<map>
#include<string.h>
#include<iostream>
#include<string>
#include<stdlib.h>
using namespace std;

extern string make_temporary();
extern map<string,string> table;
string get_temp(string);
void insert_temp(string);
extern bool find_id_in_table(string);
int check_type();
%}

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%%


{D}+	{//printf("Constant %s\n",yytext);
		 yylval.eval=atoi(yytext);return Constant;}
"="	{return '=';}
"+"	{//printf("%s\n",yytext);
	return '+';}
"-"	{return '-';}
"*"	{return '*';}
"/"	{return '/';}
"("	{return '(';}
")"	{return ')';}
";"	{return ';';}
"if"	{//printf("IF\n");
	return IF;}
"then"	{//printf("THEN\n");
	return THEN;}
"while"	{//printf("while\n");
	return WHILE;}
"do"	{//printf("do\n");
	return DO;}
"int"	{//printf("int\n");
	return INT;}
"float"	{//printf("float\n");
	return FLOAT;}
([a-z]|_)([a-z]|[0-9]|_)*	{
					string id;
					//id.copy(yytext,0,strlen(yytext)-1);
					for(int i = 0; i < strlen(yytext);i++)
					{
						id.push_back(yytext[i]);
					}
					//cout<<"Id: "<<id<<endl;
					if(find_id_in_table(id))
					{
						//printf("%s=%s\n",get_temp(id),id);
						cout<<get_temp(id)<<"="<<id<<endl;
						//s.push(get_temp(id));
					}
					else
					{
						insert_temp(id);
						//printf("%s=%s\n",get_temp(id),id);
						cout<<get_temp(id)<<"="<<id<<endl;
						//s.push(get_temp(id));
					}
					//printf("Identifier\n");
					strcpy(yylval.id,yytext);
					return Identifier;
				}
[\t\n\r ]*	{}
.			{ printf("bad char\n");/* ignore bad characters */ }

%%
int yywrap()
{
	return(1);
}

int column = 0;

void count()
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
			column = 0;
		else if (yytext[i] == '\t')
			column += 8 - (column % 8);
		else
			column++;

	
}
string get_temp(string id)
{
	return table[id];
}
void insert_temp(string id)
{
	//table.insert(pair<string,string>(id,make_temporary()));
	table[id] = make_temporary();
}
int check_type()
{
/*
* pseudo code --- this is what it should check
*
*	if (yytext == type_name)
*		return(TYPE_NAME);
*
*	return(IDENTIFIER);
*/

/*
*	it actually will only return IDENTIFIER
*/

	
}
