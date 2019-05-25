#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <stack>
#include <string>
#include <string.h>
#include <map>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 6
using namespace std;

typedef enum 
   {ENDFILE,ERROR,
    /* reserved words */
    IF,ELSE,INT,RETURN,WHILE,VOID,
    ID,
    ASSIGN,EQ,GT,LT,GE,LE,NOTEQ,
	PLUS,MINUS,TIMES,OVER,LPAREN,RPAREN,SEMI,
	LCURL,RCURL,LSQ,RSQ,COMMA,NUM,NULL_TK,PARSER_START 
   } TokenType;
   
typedef enum {Program,DeList,DeList1,De,De1,VarDe,FunDe,Type,
Type1,Params,ParamList,ParamList1,Param,Param1,CmpdStmt,LocalDes,	//15
LocalDes1,StmtList,StmtList1,Stmt,Stmt2,ExpStmt,SelecStmt,IterStmt,ReturnStmt,	//24
Exp,Exp1,SelecStmt1,ReturnStmt1,SimpleExp,SimpleExp1,Relop,AddExp,AddExp1,Addop,	
Mulop,Term,Term1,Factor,Factor1,Var,Call,Args,ArgList,ArgList1
 } VN;

typedef enum
{ START,INASSIGN,INSLASH,INCOMMENT_START,INCOMMENT_END,INNUM,
INGT,INLT,INGE,INLE,INNOTEQ,INID,DONE }
StateType;


extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum {StmtK,ExpK} NodeKind;
typedef enum {IfK,WhileK,ReturnK,AssignK,DeVarK,DeFunK,CompoundK} StmtKind;
typedef enum {OpK,ConstK,IdK,TypeK,SubscriptK,ParamK,CallK} ExpKind;

/* ExpType is used for type checking */
typedef enum {Void,Integer,Boolean} ExpType;

#define MAXCHILDREN 7

typedef struct treeNode
   { struct treeNode * child;
     struct treeNode * sibling;
     string pro;	//输出的产生式 
     TreeNode(){
     	child=NULL;
     	sibling=NULL;
	 } 
   } TreeNode;
   
extern stack<TreeNode*> treeStack;
extern TreeNode *t; 

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error; 
#endif
