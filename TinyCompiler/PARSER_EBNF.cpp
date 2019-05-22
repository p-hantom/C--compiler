

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parser_ebnf.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode * declaration_list(void);
static TreeNode * declaration(void);
static TreeNode * compound_stmt();
static TreeNode * type_specifier();
static TreeNode * subscript_exp();
static TreeNode * params_exp();
static TreeNode * param_exp();
static TreeNode * if_stmt();
static TreeNode * while_stmt();
static TreeNode * assign_stmt(void);
static TreeNode * return_stmt();
static TreeNode * exp(void);
static TreeNode * simple_exp(void);
static TreeNode * term(void);
static TreeNode * factor(void);
static TreeNode * id();
static TreeNode * var_declaration();
static TreeNode * localDeclarations();
static TreeNode * statement_list();
static TreeNode * statement();
static TreeNode * args();

static void syntaxError(char * message)
{ fprintf(listing,"\n>>> ");
  fprintf(listing,"Syntax error at line %d: %s",lineno,message);
  Error = TRUE;
}

static void match(TokenType expected)
{ if (token == expected) token = getToken();
  else {
//  	if(expected==RPAREN||expected==LPAREN||expected==RCURL||
//	  expected==LCURL||expected==RSQ||expected==LSQ||expected==SEMI||expected==COMMA){
//	  	syntaxError("expected "+);
//	  }
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    fprintf(listing,"      ");
  }
}

TreeNode * declaration_list(void)
{ TreeNode * t = declaration();
  TreeNode * p = t;
//  while ((token!=ENDFILE) && (token!=END) &&
//         (token!=ELSE) && (token!=UNTIL))
		while ((token!=ENDFILE) && (token!=ELSE))
  { TreeNode * q;
    //match(SEMI);
    q = declaration();
    if (q!=NULL) {
      if (t==NULL) t = p = q;
      else /* now p cannot be NULL either */
      { p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

TreeNode * declaration(void){
	TreeNode * t = newStmtNode(DeFunK);
	if(t!=NULL)  t->child[0] = type_specifier();
	if(t!=NULL)  t->child[1] = id();
	if(t!=NULL && token == LPAREN){  //function-declaration
		match(LPAREN);
		t->child[2] = params_exp();
		match(RPAREN);
		t->child[3] = compound_stmt();
	}
	else if(t!=NULL){
		t->kind.stmt = DeVarK;
		if(token == LSQ){  //array declaration
			t->child[2] = subscript_exp();
		}
		else if(token == SEMI){ } //int declaration
		match(SEMI);
	}
	return t;
}

TreeNode * compound_stmt(){  //¸´ºÏÓï¾ä compound-stmt ¡ú { local-declarations statement-list }						 
	match(LCURL);  // {
	TreeNode * t = newStmtNode(CompoundK);
	t->child[0]=localDeclarations();
	t->child[1]=statement_list();
	match(RCURL);  // }
	return t;
}

TreeNode * statement_list(){ //statement-list ¡ú empty{statement}
	TreeNode * t = NULL;
  	TreeNode * p = t;
	while (token!=RCURL){ 
		if(token==ENDFILE || token==RPAREN){
			syntaxError("expected '}'");
			break;
		}
		TreeNode * q;
    	q = statement();
    	if (q!=NULL) {
      		if (t==NULL) t = p = q;
	        else { 
				p->sibling = q;
	          	p = q;
	        }
    	}
  	}
  	return t;
}

TreeNode * statement(){ //statement->expression-stmt|compound-stmt|if-stmt|while-stmt|return-stmt
	TreeNode * t = NULL;
    switch (token) {
    	case IF : t = if_stmt(); break;
    	case WHILE : t = while_stmt(); break;
    	case ID : t = assign_stmt(); break;   //assign / call a function
    	case LCURL: t = compound_stmt(); break;
    	case RETURN: t = return_stmt(); break;
    	default : syntaxError("unexpected token -> ");
              printToken(token,tokenString);
              token = getToken();
              break;
  	} 
  	return t;
}

TreeNode * if_stmt(void){ // if-stmt ¡ú if (expression) statement [ else statement]
	TreeNode * t = newStmtNode(IfK);
  	match(IF); match(LPAREN);  //if (
  	if (t!=NULL) t->child[0] = exp();
  	match(RPAREN);  // )
  	if (t!=NULL) t->child[1] = statement(); //deal with curly brackets
  	if (token==ELSE) {
    	match(ELSE);
    	if (t!=NULL) t->child[2] = statement();
 	}
  	return t;
}

TreeNode * while_stmt(void){ 
	TreeNode * t = newStmtNode(WhileK);
  	match(WHILE); match(LPAREN);
  	if (t!=NULL) t->child[0] = exp();
  	match(RPAREN);
  	if (t!=NULL) t->child[1] = statement();  //deal with curly brackets
  	return t;
}

TreeNode * return_stmt(){ //return-stmt ¡ú return [expression];
	TreeNode * t = newStmtNode(ReturnK);
	match(RETURN);
	if(token!=SEMI && t!=NULL) t->child[0] = exp();
	match(SEMI);
	return t;
}

TreeNode * localDeclarations(){  //local-declarations ¡ú empty{var-declaration}
	TreeNode * t = NULL;
	TreeNode * p = t;
	while (token==INT){
	    TreeNode * q;
    	q = var_declaration();
    	if (q!=NULL) {
      		if (t==NULL) t = p = q; //if no var-declarations at all
	        else { 
				p->sibling = q;
	        	p = q;
	      	}	
    	}	
  	}
  	return t;
}

TreeNode * params_exp(){
	TreeNode * t = param_exp();
    TreeNode * p = t;
	while ((token!=RPAREN)){
		if(token==ENDFILE || token==LCURL || token==RCURL || token==SEMI){
			syntaxError("expected ')'");
			break;
		}
	    TreeNode * q;
    	match(COMMA);
    	q = param_exp();
    	if (q!=NULL) {
      		if (t==NULL) t = p = q;
	        else /* now p cannot be NULL either */{ 
				p->sibling = q;
	        	p = q;
	      	}	
    	}	
  	}
  	return t;
}

TreeNode * var_declaration(){  //used param / local declaration
	TreeNode * t = newStmtNode(DeVarK);
	if(t!=NULL){
		t->child[0] = type_specifier();
		t->child[1] = id();
	}
	if(token==LSQ){  //if array
		if(t!=NULL) t->child[2] = subscript_exp();
	}
	match(SEMI);
	return t;
}

TreeNode * param_exp(){  //param ¡ú type-specifier ID {[]} | void
	TreeNode * t = newExpNode(ParamK);
	if(token==RPAREN) return NULL;  //if no params
	if(t!=NULL){
		if(token==VOID) t->child[0] = type_specifier();  //eg. main(void)
		else if(token==INT){  //eg. f(int x)
			t->child[0] = type_specifier();
			t->child[1] = id();
		}
	}
	if(token==LSQ){  //if array
		if(t!=NULL) t->child[2] = subscript_exp();
	}
	
	return t;
}

TreeNode * subscript_exp(){
	match(LSQ);
	TreeNode * t = newExpNode(SubscriptK);
	if(token!=RSQ) t->child[0]=simple_exp();
//	if(t!= NULL && token==NUM){
//		t->attr.val=atoi(tokenString);
//		match(NUM);
//	} 
//	else if(t!= NULL && token==ID){
//		t->attr.name=tokenString;
//		match(ID);
//	} 
	match(RSQ);
	return t;
}

TreeNode * assign_stmt(void){ //assign:child[0]:id,child[1]:exp / call a function:child[0]:id,child[1]:args
	TreeNode * t = newStmtNode(AssignK);
  	if ((t!=NULL) && (token==ID)) t->child[0]=id();
//    	t->attr.name = copyString(tokenString);
//  	match(ID);
  	if(token==LPAREN) { //call
 		match(LPAREN);
 		t->kind.exp = CallK;
      	t->child[1]=args();
      	t->nodekind = ExpK;
      	match(RPAREN);
	}
	else{ 			    //assign
		if(token==LSQ){ //array var
			t->child[0]->sibling=subscript_exp();
		}
		match(ASSIGN);
  		if (t!=NULL) t->child[1] = exp();
	}
  	match(SEMI);
  	return t;
}

TreeNode * type_specifier(){
	TreeNode * t = newExpNode(TypeK);
	if(t!=NULL){
		t->attr.name = copyString(tokenString);
		if(token==INT){
			match(INT);
		}
		else if(token==VOID){
			match(VOID);
		}
	}
	return t;
}

TreeNode * exp(void)
{ TreeNode * t = simple_exp();
  if (token==LT||token==EQ||token==GT||token==LE||token==GE||token==NOTEQ) {
    TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
    }
    match(token);
    if (t!=NULL)
      t->child[1] = simple_exp();
  }
  return t;
}

TreeNode * simple_exp(void)
{ TreeNode * t = term();
  while ((token==PLUS)||(token==MINUS))
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = term();
    }
  }
  return t;
}

TreeNode * term(void)
{ TreeNode * t = factor();
  while ((token==TIMES)||(token==OVER))
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = factor();
    }
  }
  return t;
}

TreeNode * id(){
	TreeNode * t = newExpNode(IdK);
	if ((t!=NULL) && (token==ID))
        t->attr.name = copyString(tokenString);
    match(ID);
    return t;
}

TreeNode * args(){  //args separated with commas
	TreeNode * t = NULL;
    TreeNode * p = t;
	while ((token!=RPAREN)){
	    TreeNode * q;
    	q = exp();
    	if(token!=RPAREN) match(COMMA);
    	if (q!=NULL) {
      		if (t==NULL) t = p = q;
	        else /* now p cannot be NULL either */{ 
				p->sibling = q;
	        	p = q;
	      	}	
    	}	
  	}
  	return t;
}

TreeNode * factor(void)
{ TreeNode * t = NULL;
  switch (token) {
    case NUM :
      t = newExpNode(ConstK);
      if ((t!=NULL) && (token==NUM))
        t->attr.val = atoi(tokenString);
      match(NUM);
      break;
    case ID :
      t = newExpNode(IdK);
      if ((t!=NULL) && (token==ID))
        t->attr.name = copyString(tokenString);
      match(ID);
      if(token==LPAREN){ //call a function
      	match(LPAREN);
      	t->child[0]=args();
      	t->kind.exp = CallK;
      	match(RPAREN);
	  }
	  else if(token==LSQ){ //array
	  	t->sibling=subscript_exp();
	  }
      break;
    case LPAREN :
      match(LPAREN);
      t = exp();
      match(RPAREN);
      break;
    default:
      syntaxError("unexpected token -> ");
      printToken(token,tokenString);
      token = getToken();
      break;
    }
  return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly 
 * constructed syntax tree
 */
TreeNode * parse(void)
{ TreeNode * t;
  token = getToken();
  t = declaration_list();
  if (token!=ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}
