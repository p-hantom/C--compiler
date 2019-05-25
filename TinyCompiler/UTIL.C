#include "globals.h"
#include "util.h"

void printToken( TokenType token, const char* tokenString )
{ switch (token)
  { case IF:
    case RETURN:
    case ELSE:
    case VOID:
    case WHILE:
    case INT:
      fprintf(listing,
         "reserved word: %s\n",tokenString);
      break;
    
    case NUM:
    	fprintf(listing,
         "NUM, val= %s\n",tokenString);
      break;
    case ASSIGN: fprintf(listing,"=\n"); break;
    case LT: fprintf(listing,"<\n"); break;
	case GT: fprintf(listing,">\n"); break;
	case GE: fprintf(listing,">=\n"); break;
	case LE: fprintf(listing,"<=\n"); break;
    case EQ: fprintf(listing,"==\n"); break;
    case NOTEQ: fprintf(listing,"!=\n"); break;
    case LPAREN: fprintf(listing,"(\n"); break;
    case RPAREN: fprintf(listing,")\n"); break;
    case LCURL: fprintf(listing,"{\n"); break;
    case RCURL: fprintf(listing,"}\n"); break;
    case LSQ: fprintf(listing,"[\n"); break;
    case RSQ: fprintf(listing,"]\n"); break;
    case COMMA: fprintf(listing,",\n"); break;
    case SEMI: fprintf(listing,";\n"); break;
    case PLUS: fprintf(listing,"+\n"); break;
    case MINUS: fprintf(listing,"-\n"); break;
    case TIMES: fprintf(listing,"*\n"); break;
    case OVER: fprintf(listing,"/\n"); break;
    case ENDFILE: fprintf(listing,"EOF\n"); break;
    case ID:
      fprintf(listing,
          "ID, name= %s\n",tokenString);
      break;
    case ERROR:
      fprintf(listing,
          "ERROR: %s\n",tokenString);
      break;
    default: /* should never happen */
      fprintf(listing,"Unknown token: %d\n",token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
//TreeNode * newStmtNode(StmtKind kind)
//{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
//  int i;
//  if (t==NULL)
//    fprintf(listing,"Out of memory error at line %d\n",lineno);
//  else {
//    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
//    t->sibling = NULL;
//    t->nodekind = StmtK;
//    t->kind.stmt = kind;
//    t->lineno = lineno;
//  }
//  return t;
//}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
//TreeNode * newExpNode(ExpKind kind)
//{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
//  int i;
//  if (t==NULL)
//    fprintf(listing,"Out of memory error at line %d\n",lineno);
//  else {
//    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
//    t->sibling = NULL;
//    t->nodekind = ExpK;
//    t->kind.exp = kind;
//    t->lineno = lineno;
//    t->type = Void;
//  }
//  return t;
//}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = (char *)malloc(n);
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}


//stack<TreeNode*> treeStack;
void printTree( TreeNode * tree ){
	INDENT;
	while(tree != NULL){
		printSpaces();
		if((tree->pro).length()==0){
			cout<<"null"<<endl;
		}
		else{
			fprintf(listing,"%s\n",(tree->pro).c_str());
		}
		
//		cout<<tree->pro<<endl;
		TreeNode *p=tree->child;
		if(p != NULL){
			printTree(p);
		}
		tree = tree->sibling;
	}
	UNINDENT;
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
//void printTree( TreeNode * tree )
//{ int i;
//  INDENT;
//  while (tree != NULL) {
//    printSpaces();
//    if (tree->nodekind==StmtK)
//    { switch (tree->kind.stmt) {
//        case IfK:
//          fprintf(listing,"If\n");
//          break;
//        case ReturnK:
//        	fprintf(listing,"Return\n");
//          	break;
//        case WhileK:
//          fprintf(listing,"While\n");
//          break;
//        case AssignK:
//          fprintf(listing,"Assign\n");
//          break;
//        case DeFunK:
//        	fprintf(listing,"Function declaration\n");
//          	break;
//        case DeVarK:
//          	fprintf(listing,"Var declaration\n");
//          	break;
//        case CompoundK:
//        	fprintf(listing,"Compound\n");
//          	break;
//        default:
//          fprintf(listing,"Unknown ExpNode kind\n");
//          break;
//      }
//    }
//    else if (tree->nodekind==ExpK)
//    { switch (tree->kind.exp) {
//        case OpK:
//          	fprintf(listing,"Op: ");
//          	printToken(tree->attr.op,"\0");
//          	break;
//        case ConstK:
//          	fprintf(listing,"Const: %d\n",tree->attr.val);
//          	break;
//        case IdK:
//          	fprintf(listing,"Id: %s\n",tree->attr.name);
//          	break;
//        case TypeK:
//          	fprintf(listing,"Type: %s\n",tree->attr.name);
//          	break;
//        case SubscriptK:
//        	fprintf(listing,"Subscript: ");
//        	if(tree->attr.val){
//        		fprintf(listing,"%d",tree->attr.val);
//			}
//			fprintf(listing,"\n");
//        	break;
//        case ParamK:
//        	fprintf(listing,"Param \n");
//        	break;
//        case CallK:
//        	fprintf(listing,"Call: ");
//        	if(tree->attr.name){
//        		fprintf(listing,"%s\n",tree->attr.name);
//			}
//			else fprintf(listing,"\n");
//        	break;
//        default:
//          	fprintf(listing,"Unknown ExpNode kind\n");
//          	break;
//      }
//    }
//    else fprintf(listing,"Unknown node kind\n");
//    for (i=0;i<MAXCHILDREN;i++)
//         printTree(tree->child[i]);
//    tree = tree->sibling;
//  }
//  UNINDENT;
//}
