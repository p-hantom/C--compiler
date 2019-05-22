%{
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
%}

DIGIT [0-9]
ID [a-z][a-z0-9]*
%%
{DIGIT}+                    {printf("整数:   %s(%d)\n",yytext,atoi(yytext));}
{DIGIT}+"."{DIGIT}+             {printf("实数:   %s(%g)\n",yytext,atof(yytext));}
if|else|int|return|void|while      {printf("关键字: %s\n",yytext);}
{ID}                        {printf("标识符: %s\n",yytext);}
"+"|"-"|"*"|"/"|"="                 {printf("运算符: %s\n",yytext);}
"=="							{printf("判断相等: %s\n",yytext);}
"!="							{printf("判断不相等: %s\n",yytext);}
">"							{printf("greater than: %s\n",yytext);}
"<"							{printf("less than: %s\n",yytext);}
">="							{printf("greater/equal: %s\n",yytext);}
"<="							{printf("less/equal: %s\n",yytext);}
"("|")"|"["|"]"|"{"|"}"|","|";"     {printf("%s\n",yytext);}
"/*"([^*]|[\r\n]|("*"+([^*/]|[\r\n])))*"*"+"/"			{printf("");}
" "+|[\t\n\x20]+;			{printf("");}
.                       {printf("不能识别的字符:%s\n",yytext);}
%%
int main(int argc,char **argv)
{
    ++argv;
    --argc;
    if(argc>0) yyin=fopen(argv[0],"r");
    else yyin=stdin;
    yylex();
    return 0;
}
int yywrap()
{
    return 1;
}
