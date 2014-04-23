%{
/* header section.
 * declare terminal and nonterminal symbols
 */
#include <cstdio>
#include <iostream>
#include <string.h>
#include <stack>
#include <string>
#include <map>
#define YYDEBUG 1     /* enables compilation with trace facility  */
using namespace std;

int noerror=1;
int expr; //1->add,2->sub,3->mult,4->div.
int count;
int count2;
stack<int> if_s;
stack<int> while_s;
string t;
map<string,string> table;
stack<string> s;
// stuff from flex that bison needs to know about:
extern char yytext[];
extern int yylex(); /* type of yylex()  */
extern int  yylineno;
extern int yyparse();
extern  FILE *yyin;
 int count1;
void yyerror(const char *msg);
void itoa(int num,char *str);
string make_temporary();

bool find_id_in_table(string id);
%}
%union
 
{
 
char id[30];
 
int eval;
 
}
%token <id> Identifier
%token <eval> Constant
%token INT FLOAT
%token IF THEN WHILE DO
%token '+' '-' '(' ')' '*' '/' '=' ';'

%type <id> expr statement term factor body P

%start P
%%
P
	:dec Identifier		{//printf("P->dec Identifier\n");
				}
	|body P			{
				 strcpy($$,$1);
				 strcat($$,"\n");
				 strcat($$,$2);
				 //printf("%s\n",$$);
				 table.clear();
				 count=0;
				}
	|                       {//printf("$$ with e:%s\n",$$);
				}
	;
dec
	:INT			{//printf("dec->int\n");
				}
	|FLOAT			{//printf("dec->float\n");
				}
	;
body
	:IF expr THEN {
		printf("if not %s then Jump False_%d\n",$2,count1);
		if_s.push(count1++);
	} body	{//printf("body->if expr then body\n");
				 //printf("%s\n",$5);
				 printf("False_%d:\n",if_s.top());
				 if_s.pop();
		}
	|WHILE
	{printf("While_%d:\n",count2);
	  while_s.push(count2++);} 
	expr DO 
	{printf("if not %s then Jump While_False_%d:\n",$3,while_s.top());} 
	body	{//printf("%s\n",$6);
		 printf("JUMP While_%d\n",while_s.top());
				printf("While_False_%d:\n",while_s.top());
				while_s.pop();
				}
	|statement ';'		{//printf("body->statement\n");
				 memset($$,'\0',sizeof($$));
				 strcpy($$,$1);
				 //printf("body: %s\n",$$);
				}
	;
statement
	:			{//printf("epsilon prod\n");
				}
	|Identifier '=' expr	{//printf("statement->Id = expr\n");
				 char app[100];
				 if(find_id_in_table($1))
				 {
				 	//cout<<"In If"<<endl;
				 	strcpy(app,table[$1].c_str());
				 }
				 strcat(app,"=");
				 strcat(app,$3);
				 printf("%s\n",app);
				 memset(app,'\0',sizeof(app));
				 strcpy(app,$1);
				 strcat(app,"=");
				 strcat(app,table[$1].c_str());
				 strcpy($$,app);
				 printf("%s\n",$$);
				}
	;

expr
	:expr '+' term		{//printf("expr->expr + term\n");
				 char app[100];
				 strcpy($$,make_temporary().c_str());
				 strcpy(app,$$);
				 strcat(app,"=");
				 strcat(app,$1);
				 strcat(app,"+");
				 strcat(app,$3);
				 printf("%s\n",app);
				}
	|expr '-' term		{//printf("expr->expr - term\n");
				 char app[100];
				 strcpy($$,make_temporary().c_str());
				 strcpy(app,$$);
				 strcat(app,"=");
				 strcat(app,$1);
				 strcat(app,"-");
				 strcat(app,$3);
				 printf("%s\n",app);
				}
	|term			{//printf("expr->term\n");
				 strcpy($$,$1);
				}
	;

term
	:term '*' factor		{//printf("term->term * factor\n");
					 char app[100];
					 strcpy($$,make_temporary().c_str());
					 strcpy(app,$$);
					 strcat(app,"=");
					 strcat(app,$1);
					 strcat(app,"*");
					 strcat(app,$3);
					 printf("%s\n",app);
					}
	|term '/' factor		{//printf("term->term / factor\n");
					 char app[100];
					 strcpy($$,make_temporary().c_str());
					 strcpy(app,$$);
					 strcat(app,"=");
					 strcat(app,$1);
					 strcat(app,"/");
					 strcat(app,$3);
					 printf("%s\n",app);
					}
	|factor			{//printf("term->factor\n");
				 strcpy($$,$1);
				}
	;
factor
	:Identifier		{
					//printf("factor->Identifier\n");
					if(find_id_in_table($1))
					{
						strcpy($$,table[$1].c_str());
					}
				}
	|Constant		{//printf("factor->Constant\n");	
					//itoa($1,$$);
					strcpy($$,make_temporary().c_str());
					char app[100];
					strcpy(app,$$);
					strcat(app,"=");
					char a[100];
					itoa($1,a);
					strcat(app,a);
					printf("%s\n",app);
					//printf("Constant:%s\n",$$);
				}
	|'(' expr ')'		{//printf("factor->( expr )\n");
				 strcpy($$,$2);
				}
	;
%%
extern int column;

int main (int argc, char *argv[])
{
	++argv, --argc; /* skip over program name */
	if ( argc >0  )
	{
		if(argc >1)
    		{
    			 printf("Correct usage $./bison1 filename\n");
    exit(-2);
    		}
    		else
		{
			yyin = fopen( argv[0], "r" );
            	}
    	}       
    	else
             yyin = stdin;
 /* if (argc!=2)
  {
    printf("Correct usage $./bison1 filename\n");
    exit(-2);
  }
    	 
  yyin=fopen(argv[1],"r");*/
  yyparse ();
}
void yyerror(const char *s)
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}
void itoa(int num,char str[20])
{
	sprintf(str, "%d", num);
}
string make_temporary()
{
	string t;
	t.push_back('t');
	char append[11];
	itoa(count++,append);
	t.append(append);
	return t;
}

bool find_id_in_table(string id)
{
	//return (table.find(id)!=table.end());
	map<string,string> :: iterator it;
	for(it=table.begin(); it!=table.end(); it++)
	{
		//printf("%s--> %s\n",it->first,it->second);
		//cout<<it->first<<"->"<<it->second<<endl;
		if((it->first).compare(id)==0)
			return true;
	}
	return false;
}
